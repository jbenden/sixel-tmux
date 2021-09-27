/* $OpenBSD$ */

/*
 * Copyright (c) 2019 Nicholas Marriott <nicholas.marriott@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
//#include <uchar.h>

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "tmux.h"

#define SIXEL_COLOUR_REGISTERS 1024
#define SIXEL_WIDTH_LIMIT 2016
#define SIXEL_HEIGHT_LIMIT 2016

#define SIXEL_FLAG_HSL 0x02000000
#define SIXEL_FLAG_RGB 0x04000000

#define CN 3
#define DIST_THRESHOLD 0.00001f

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

#define GLYPH_COUNT 35
static const wchar_t kRunes[] = {
    u' ', /* 0020 empty block */
    u'█', /* 2588 full block */
    u'▄', /* 2584 lower half block */
    u'▀', /* 2580 upper half block */
    u'▐', /* 2590 right half block */
    u'▌', /* 258C left half block */
    u'▝', /* 259D quadrant upper right */
    u'▙', /* 2599 quadrant upper left and lower left and lower right */
    u'▗', /* 2597 quadrant lower right */
    u'▛', /* 259B quadrant upper left and upper right and lower left */
    u'▖', /* 2596 quadrant lower left */
    u'▜', /* 259C quadrant upper left and upper right and lower right */
    u'▘', /* 2598 quadrant upper left */
    u'▟', /* 259F quadrant upper right and lower left and lower right */
    u'▞', /* 259E quadrant upper right and lower left */
    u'▚', /* 259A quadrant upper left and lower right */
    u'▔', /* 2594 upper one eighth block */
    u'▁', /* 2581 lower one eighth block */
    u'▂', /* 2582 lower one quarter block */
    u'▃', /* 2583 lower three eighths block */
    u'▅', /* 2585 lower five eighths block */
    u'▆', /* 2586 lower three quarters block */
    u'▇', /* 2587 lower seven eighths block */
    u'▕', /* 2595 right one eight block */
    u'▏', /* 258F left one eight block */
    u'▎', /* 258E left one quarter block */
    u'▍', /* 258D left three eigths block */
    u'▋', /* 258B left five eigths block */
    u'▊', /* 258A left three quarters block */
    u'━', /* 2501 box drawings heavy horizontal */
    u'┉', /* 2509 box drawings heavy quadruple dash horizontal */
    u'┃', /* 2503 box drawings heavy vertical */
    u'╋', /* 254B box drawings heavy vertical & horiz. */
    u'╹', /* 2579 box drawings heavy up */
    u'╺', /* 257A box drawings heavy right */
    u'╻', /* 257B box drawings heavy down */
    u'╸', /* 2578 box drawings heavy left */
    u'┏', /* 250F box drawings heavy down and right */
    u'┛', /* 251B box drawings heavy up and left */
    u'┓', /* 2513 box drawings heavy down and left */
    u'┗', /* 2517 box drawings heavy up and right */
    u'◢', /* 25E2 black lower right triangle */
    u'◣', /* 25E3 black lower left triangle */
    u'◥', /* 25E4 black upper right triangle */
    u'◤', /* 25E5 black upper left triangle */
    // FIXME: add diagonals lines ,‘` /\ and less angleed: ╱╲ cf /╱╲\
    u'═', /* 2550 box drawings double horizontal */
    u'⎻', /* 23BB horizontal scan line 3 */
    u'⎼', /* 23BD horizontal scan line 9 */
};


#define RP4(B) ((((B) & 0xf0) >> 4 ) | ((B & 0x0f) << 4))
#define RP2(B) ((((B) & 0xcc) >> 2 ) | ((B & 0x33) << 2))
#define RP1(B) ((((B) & 0xaa) >> 1 ) | ((B & 0x55) << 1))
#define R(B) RP1(RP2(RP4(B)))

#define RRBB128( \
                A,B,C,D,E,F,G,H \
                , \
                I,J,K,L,M,N,O,P \
                ) ( \
  ((uint128_t)R(P)<<120) \
+ ((uint128_t)R(O)<<112) \
+ ((uint128_t)R(N)<<104) \
+ ((uint128_t)R(M)<<96) \
+ ((uint128_t)R(L)<<88) \
+ ((uint128_t)R(K)<<80) \
+ ((uint128_t)R(J)<<72) \
+ ((uint128_t)R(I)<<64) \
+ (( uint64_t)R(H)<<56) \
+ (( uint64_t)R(G)<<48) \
+ (( uint64_t)R(F)<<40) \
+ (( uint64_t)R(E)<<32) \
+ (( uint64_t)R(D)<<24) \
+ (( uint64_t)R(C)<<16) \
+ (( uint64_t)R(B)<<8) \
+ (  uint64_t)R(A))



static const uint128_t kGlyphs128[] = /* clang-format off */ {
    /* U+0020 ' ' empty block [ascii;200cp437;20] */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
      /* U+2588 '█' full block [cp437] */
RRBB128(0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111),
      /* U+2584 '▄' lower half block [cp437,dc] */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111),
      /* U+2580 '▀' upper half block [cp437] */
RRBB128(0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
      // Mode B
      /* U+2590 '▐' right half block [cp437,de] */
RRBB128(0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111),
      /* U+258C '▌' left half block [cp437] */
RRBB128(0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000),
      /* U+259D '▝' quadrant upper right */
RRBB128(0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
      /* U+2599 '▙' quadrant upper left and lower left and lower right */
RRBB128(0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111),
      /* U+2597 '▗' quadrant lower right */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111),
      /* U+259B '▛' quadrant upper left and upper right and lower left */
RRBB128(0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000),
      /* U+2596 '▖' quadrant lower left */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000),
      /* U+259C '▜' quadrant upper left and upper right and lower right */
RRBB128(0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111),
      /* U+2598 '▘' quadrant upper left */
RRBB128(0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
      /* U+259F '▟' quadrant upper right and lower left and lower right */
RRBB128(0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111),
      /* U+259E '▞' quadrant upper right and lower left */
RRBB128(0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000),
      /* U+259A '▚' quadrant upper left and lower right */
RRBB128(0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001100,
        0b00001100),
      // Mode C
      /* U+2594 '▔' upper one eighth block */
RRBB128(0b11111111,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
      /* U+2581 '▁' lower one eighth block */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111111),
      /* U+2582 '▂' lower one quarter block */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111),
      /* U+2583 '▃' lower three eighths block */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111),
      /* U+2585 '▃' lower five eighths block */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111),
      /* U+2586 '▆' lower three quarters block */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111),
      /* U+2587 '▇' lower seven eighths block */
RRBB128(0b00000000,
        0b00000000,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111),
      /* U+2595 '▕' right one eight block */
RRBB128(0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001),
      /* U+258F '▏' left one eight block */
RRBB128(0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000),
      /* U+258E '▎' left one quarter block */
RRBB128(0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000),
      /* U+258D '▍' left three eigths block */
RRBB128(0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000,
        0b11100000),
      /* U+258B '▋' left five eigths block */
RRBB128(0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000,
        0b11111000),
      /* U+258A '▊' left three quarter block */
RRBB128(0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100),
      /* U+2589 '▉' left seven eights block */
RRBB128(0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110,
        0b11111110),
        /* ▁ *\
      2501▕━▎box drawings heavy horizontal
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
     25019▕┉▎box drawings heavy quadruple dash horizontal
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b01010101,
        0b01010101,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      2503▕┃▎box drawings heavy vertical
        \* ▔ */
        // FIXME, X=9 would work better for that
RRBB128(0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000),
        /* ▁ *\
      254b▕╋▎box drawings heavy vertical and horizontal
        \* ▔ */
RRBB128(0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000),
        /* ▁ *\
      2579▕╹▎box drawings heavy up
        \* ▔ */
RRBB128(0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      257a▕╺▎box drawings heavy right
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00001111,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      257b▕╻▎box drawings heavy down
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000),
        /* ▁ *\
      2578▕╸▎box drawings heavy left
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b11110000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      250f▕┏▎box drawings heavy down and right
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00111111,
        0b00111111,
        0b00111111,
        0b00111111,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000),
        /* ▁ *\
      251b▕┛▎box drawings heavy up and left
        \* ▔ */
RRBB128(0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      2513▕┓▎box drawings heavy down and left
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111100,
        0b11111100,
        0b11111100,
        0b11111100,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000),
        /* ▁ *\
      2517▕┗▎box drawings heavy up and right
        \* ▔ */
RRBB128(0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00011100,
        0b00111000,
        0b00111111,
        0b00111111,
        0b00111111,
        0b00111111,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      25E2▕◢▎black lower right triangle
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000001,
        0b00000011,
        0b00001111,
        0b00111111,
        0b01111111,
        0b11111111,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      25E3▕◣▎black lower left triangle
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b10000000,
        0b11000000,
        0b11110000,
        0b11111100,
        0b11111110,
        0b11111111,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      25E4▕◥▎black upper right triangle
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111111,
        0b01111111,
        0b00111111,
        0b00001111,
        0b00000011,
        0b00000001,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      25E5▕◤▎black upper left triangle
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111111,
        0b11111110,
        0b11111100,
        0b11110000,
        0b11000000,
        0b10000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      2500▕═▎box drawings double horizontal
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11111111,
        0b00000000,
        0b00000000,
        0b11111111,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      TODO▕⎻▎horizontal scan line 3
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11100000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000),
        /* ▁ *\
      TODO▕⎼▎horizontal scan line 9
        \* ▔ */
RRBB128(0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b11100000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000)
} /* clang-format on */;


static unsigned bsr(unsigned x) {
#if -__STRICT_ANSI__ + !!(__GNUC__ + 0) && (__i386__ + __x86_64__ + 0)
  asm("bsr\t%1,%0" : "=r"(x) : "r"(x) : "cc");
#else
  static const unsigned char kDebruijn[32] = {
      0, 9,  1,  10, 13, 21, 2,  29, 11, 14, 16, 18, 22, 25, 3, 30,
      8, 12, 20, 28, 15, 17, 24, 7,  19, 27, 23, 6,  26, 5,  4, 31};
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x = kDebruijn[(x * 0x07c4acddu) >> 27];
#endif
  return x;
}

static unsigned long tpenc(wchar_t x) {
  if (0x00 <= x && x <= 0x7f) {
    return x;
  } else {
    static const struct ThomPike {
      unsigned char len, mark;
    } kThomPike[32 - 7] = {
        {1, 0xc0}, {1, 0xc0}, {1, 0xc0}, {1, 0xc0}, {2, 0xe0},
        {2, 0xe0}, {2, 0xe0}, {2, 0xe0}, {2, 0xe0}, {3, 0xf0},
        {3, 0xf0}, {3, 0xf0}, {3, 0xf0}, {3, 0xf0}, {4, 0xf8},
        {4, 0xf8}, {4, 0xf8}, {4, 0xf8}, {4, 0xf8}, {5, 0xfc},
        {5, 0xfc}, {5, 0xfc}, {5, 0xfc}, {5, 0xfc}, {5, 0xfc},
    };
    wchar_t wc;
    unsigned long ec;
    struct ThomPike op;
    ec = 0;
    wc = x;
    op = kThomPike[bsr(wc) - 7];
    do {
      ec |= 0x3f & wc;
      ec |= 0x80;
      ec <<= 010;
      wc >>= 006;
    } while (--op.len);
    return ec | wc | op.mark;
  }
}


static char *tptoa(char *p, wchar_t x) {
  unsigned long w;
  for (w = tpenc(x); w; w >>= 010) *p++ = w & 0xff;
  return p;
}


struct sixel_line {
	u_int		 x;
	uint16_t	*data;
};

struct sixel_image {
	u_int			 x;
	u_int			 y;
	u_int			 xpixel;
	u_int			 ypixel;

	u_int			*colours;
	u_int			 ncolours;

	u_int			 dx;
	u_int			 dy;
	u_int			 dc;

	struct sixel_line	*lines;
};

static int
sixel_parse_expand_lines(struct sixel_image *si, u_int y)
{
	if (y <= si->y)
		return (0);
	if (y > SIXEL_HEIGHT_LIMIT)
		return (1);
	si->lines = xrecallocarray(si->lines, si->y, y, sizeof *si->lines);
	si->y = y;
	return (0);
}

static int
sixel_parse_expand_line(struct sixel_image *si, struct sixel_line *sl, u_int x)
{
	if (x <= sl->x)
		return (0);
	if (x > SIXEL_WIDTH_LIMIT)
		return (1);
	if (x > si->x)
		si->x = x;
	sl->data = xrecallocarray(sl->data, sl->x, si->x, sizeof *sl->data);
	sl->x = si->x;
	return (0);
}

static u_int
sixel_get_pixel(struct sixel_image *si, u_int x, u_int y)
{
	struct sixel_line	*sl;

	if (y >= si->y)
		return (0);
	sl = &si->lines[y];
	if (x >= sl->x)
		return (0);
	return (sl->data[x]);
}

static int
sixel_set_pixel(struct sixel_image *si, u_int x, u_int y, u_int c)
{
	struct sixel_line	*sl;

	if (sixel_parse_expand_lines(si, y + 1) != 0)
		return (1);
	sl = &si->lines[y];
	if (sixel_parse_expand_line(si, sl, x + 1) != 0)
		return (1);
	sl->data[x] = c;
	return (0);
}

static int
sixel_parse_write(struct sixel_image *si, u_int ch)
{
	struct sixel_line	*sl;
	u_int			 i;

	if (sixel_parse_expand_lines(si, si->dy + 6) != 0)
		return (1);
	sl = &si->lines[si->dy];

	for (i = 0; i < 6; i++) {
		if (sixel_parse_expand_line(si, sl, si->dx + 1) != 0)
			return (1);
		if (ch & (1 << i))
			sl->data[si->dx] = si->dc;
		sl++;
	}
	return (0);
}

static const char *
sixel_parse_attributes(struct sixel_image *si, const char *cp, const char *end)
{
	const char	*last;
	char		*endptr;
	u_int		 d, x, y;

	last = cp;
	while (last != end) {
		if (*last != ';' && (*last < '0' || *last > '9'))
			break;
		last++;
	}
	d = strtoul(cp, &endptr, 10);
	if (endptr == last || *endptr != ';')
		return (last);
	d = strtoul(endptr + 1, &endptr, 10);
	if (endptr == last || *endptr != ';')
		return (NULL);

	x = strtoul(endptr + 1, &endptr, 10);
	if (endptr == last || *endptr != ';')
		return (NULL);
	if (x > SIXEL_WIDTH_LIMIT)
		return (NULL);
	y = strtoul(endptr + 1, &endptr, 10);
	if (endptr != last)
		return (NULL);
	if (y > SIXEL_HEIGHT_LIMIT)
		return (NULL);

	si->x = x;
	sixel_parse_expand_lines(si, y);

	return (last);
}

static const char *
sixel_parse_colour(struct sixel_image *si, const char *cp, const char *end)
{
	const char	*last;
	char		*endptr;
	u_int		 c, type, r, g, b;

	last = cp;
	while (last != end) {
		if (*last != ';' && (*last < '0' || *last > '9'))
			break;
		last++;
	}

	c = strtoul(cp, &endptr, 10);
	if (c > SIXEL_COLOUR_REGISTERS)
		return (NULL);
	si->dc = c + 1;
	if (endptr == last || *endptr != ';')
		return (last);

	type = strtoul(endptr + 1, &endptr, 10);
	if (endptr == last || *endptr != ';')
		return (NULL);
	r = strtoul(endptr + 1, &endptr, 10);
	if (endptr == last || *endptr != ';')
		return (NULL);
	g = strtoul(endptr + 1, &endptr, 10);
	if (endptr == last || *endptr != ';')
		return (NULL);
	b = strtoul(endptr + 1, &endptr, 10);
	if (endptr != last)
		return (NULL);

	if (type != 1 && type != 2)
		return (NULL);
	if (c + 1 > si->ncolours) {
		si->colours = xrecallocarray(si->colours, si->ncolours, c + 1,
		    sizeof *si->colours);
		si->ncolours = c + 1;
	}

	/* In HLS, the hue is in [0;360[ so it is stored on 9 bits */
	si->colours[c] = (type << 25) | (r << 16) | (g << 8) | b;
	return (last);
}

static const char *
sixel_parse_repeat(struct sixel_image *si, const char *cp, const char *end)
{
	const char	*last;
	char		 tmp[32], ch;
	u_int		 n = 0, i;
	const char	*errstr = NULL;

	last = cp;
	while (last != end) {
		if (*last < '0' || *last > '9')
			break;
		tmp[n++] = *last++;
		if (n == (sizeof tmp) - 1)
			return (NULL);
	}
	if (n == 0 || last == end)
		return (NULL);
	tmp[n] = '\0';

	n = strtonum(tmp, 1, SIXEL_WIDTH_LIMIT, &errstr);
	if (n == 0 || errstr != NULL)
		return (NULL);

	ch = (*last++) - 0x3f;
	for (i = 0; i < n; i++) {
		if (sixel_parse_write(si, ch) != 0)
			return (NULL);
		si->dx++;
	}
	return (last);
}

struct sixel_image *
sixel_parse(const char *buf, size_t len, u_int xpixel, u_int ypixel)
{
	struct sixel_image	*si;
	const char		*cp = buf, *end = buf + len;
	char			 ch;

	if (len == 0 || len == 1 || *cp++ != 'q')
		return (NULL);

	si = xcalloc (1, sizeof *si);
	si->xpixel = xpixel;
	si->ypixel = ypixel;

	while (cp != end) {
		ch = *cp++;
		switch (ch) {
		case '"':
			cp = sixel_parse_attributes(si, cp, end);
			if (cp == NULL)
				goto bad;
			break;
		case '#':
			cp = sixel_parse_colour(si, cp, end);
			if (cp == NULL)
				goto bad;
			break;
		case '!':
			cp = sixel_parse_repeat(si, cp, end);
			if (cp == NULL)
				goto bad;
			break;
		case '-':
			si->dx = 0;
			si->dy += 6;
			break;
		case '$':
			si->dx = 0;
			break;
		default:
			if (ch < 0x20)
				break;
			if (ch < 0x3f || ch > 0x7e)
				goto bad;
			if (sixel_parse_write(si, ch - 0x3f) != 0)
				goto bad;
			si->dx++;
			break;
		}
	}

	if (si->x == 0 || si->y == 0)
		goto bad;
	return (si);

bad:
	free(si);
	return (NULL);
}

void
sixel_free(struct sixel_image *si)
{
	u_int	y;

	for (y = 0; y < si->y; y++)
		free(si->lines[y].data);
	free(si->lines);

	free(si->colours);
	free(si);
}

void
sixel_log(struct sixel_image *si)
{
	struct sixel_line	*sl;
	char			 s[SIXEL_WIDTH_LIMIT + 1];
	u_int			 i, x, y, cx, cy;

	sixel_size_in_cells(si, &cx, &cy);
	log_debug("%s: image %ux%u (%ux%u)", __func__, si->x, si->y, cx, cy);
	for (i = 0; i < si->ncolours; i++)
		log_debug("%s: colour %u is %07x", __func__, i, si->colours[i]);
	for (y = 0; y < si->y; y++) {
		sl = &si->lines[y];
		for (x = 0; x < si->x; x++) {
			if (x >= sl->x)
				s[x] = '_';
			else if (sl->data[x] != 0)
				s[x] = '0' + (sl->data[x] - 1) % 10;
			else
				s[x] = '.';
			}
		s[x] = '\0';
		log_debug("%s: %4u: %s", __func__, y, s);
	}
}

void
sixel_size_in_cells(struct sixel_image *si, u_int *x, u_int *y)
{
	if ((si->x % si->xpixel) == 0)
		*x = (si->x / si->xpixel);
	else
		*x = 1 + (si->x / si->xpixel);
	if ((si->y % si->ypixel) == 0)
		*y = (si->y / si->ypixel);
	else
		*y = 1 + (si->y / si->ypixel);
}

struct sixel_image *
sixel_scale(struct sixel_image *si, u_int xpixel, u_int ypixel, u_int ox,
    u_int oy, u_int sx, u_int sy)
{
	struct sixel_image	*new;
	u_int			 cx, cy, pox, poy, psx, psy, tsx, tsy, px, py;
	u_int			 x, y;

	/*
	 * We want to get the section of the image at ox,oy in image cells and
	 * map it onto the same size in terminal cells, remembering that we
	 * can only draw vertical sections of six pixels.
	 */

	sixel_size_in_cells(si, &cx, &cy);
	if (ox >= cx)
		return (NULL);
	if (oy >= cy)
		return (NULL);
	if (ox + sx >= cx)
		sx = cx - ox;
	if (oy + sy >= cy)
		sy = cy - oy;

	pox = ox * si->xpixel;
	poy = oy * si->ypixel;
	psx = sx * si->xpixel;
	psy = sy * si->ypixel;

	tsx = sx * xpixel;
	tsy = ((sy * ypixel) / 6) * 6;

	new = xcalloc (1, sizeof *si);
	new->xpixel = xpixel;
	new->ypixel = ypixel;

	for (y = 0; y < tsy; y++) {
		py = poy + ((double)y * psy / tsy);
		for (x = 0; x < tsx; x++) {
			px = pox + ((double)x * psx / tsx);
			sixel_set_pixel(new, x, y, sixel_get_pixel(si, px, py));
		}
	}
	return (new);
}

static void
sixel_print_add(char **buf, size_t *len, size_t *used, const char *s,
    size_t slen)
{
	if (*used + slen >= *len + 1) {
		(*len) *= 2;
		*buf = xrealloc(*buf, *len);
	}
	memcpy(*buf + *used, s, slen);
	(*used) += slen;
}

static void
sixel_print_repeat(char **buf, size_t *len, size_t *used, u_int count, char ch)
{
	char	tmp[16];
	size_t	tmplen;

	if (count == 1)
		sixel_print_add(buf, len, used, &ch, 1);
	else if (count == 2) {
		sixel_print_add(buf, len, used, &ch, 1);
		sixel_print_add(buf, len, used, &ch, 1);
	} else if (count == 3) {
		sixel_print_add(buf, len, used, &ch, 1);
		sixel_print_add(buf, len, used, &ch, 1);
		sixel_print_add(buf, len, used, &ch, 1);
	} else if (count != 0) {
		tmplen = xsnprintf(tmp, sizeof tmp, "!%u%c", count, ch);
		sixel_print_add(buf, len, used, tmp, tmplen);
	}
}

char *
sixel_print(struct sixel_image *si, struct sixel_image *map, size_t *size)
{
	char			*buf, tmp[64], *contains, data, last;
	size_t			 len, used = 0, tmplen;
	u_int			*colours, ncolours, i, c, x, y, count;
	struct sixel_line	*sl;

	if (map != NULL) {
		colours = map->colours;
		ncolours = map->ncolours;
	} else {
		colours = si->colours;
		ncolours = si->ncolours;
	}
	contains = xcalloc(1, ncolours);

	len = 8192;
	buf = xmalloc(len);

	sixel_print_add(&buf, &len, &used, "\033Pq", 3);

	tmplen = xsnprintf(tmp, sizeof tmp, "\"1;1;%u;%u", si->x, si->y);
	sixel_print_add(&buf, &len, &used, tmp, tmplen);

	for (i = 0; i < ncolours; i++) {
		c = colours[i];
		tmplen = xsnprintf(tmp, sizeof tmp, "#%u;%u;%u;%u;%u",
		    i, c >> 25, (c >> 16) & 0x1ff, (c >> 8) & 0xff, c & 0xff);
		sixel_print_add(&buf, &len, &used, tmp, tmplen);
	}

	for (y = 0; y < si->y; y += 6) {
		memset(contains, 0, ncolours);
		for (x = 0; x < si->x; x++) {
			for (i = 0; i < 6; i++) {
				if (y + i >= si->y)
					break;
				sl = &si->lines[y + i];
				if (x < sl->x && sl->data[x] != 0)
					contains[sl->data[x] - 1] = 1;
			}
		}

		for (c = 0; c < ncolours; c++) {
			if (!contains[c])
				continue;
			tmplen = xsnprintf(tmp, sizeof tmp, "#%u", c);
			sixel_print_add(&buf, &len, &used, tmp, tmplen);

			count = 0;
			for (x = 0; x < si->x; x++) {
				data = 0;
				for (i = 0; i < 6; i++) {
					if (y + i >= si->y)
						break;
					sl = &si->lines[y + i];
					if (x < sl->x && sl->data[x] == c + 1)
						data |= (1 << i);
				}
				data += 0x3f;
				if (data != last) {
					sixel_print_repeat(&buf, &len, &used,
					    count, last);
					last = data;
					count = 1;
				} else
					count++;
			}
			sixel_print_repeat(&buf, &len, &used, count, data);
			sixel_print_add(&buf, &len, &used, "$", 1);
		}

		if (buf[used - 1] == '$')
			used--;
		if (buf[used - 1] != '-')
			sixel_print_add(&buf, &len, &used, "-", 1);
	}
	if (buf[used - 1] == '$' || buf[used - 1] == '-')
		used--;

	sixel_print_add(&buf, &len, &used, "\033\\", 2);

	buf[used] = '\0';
	if (size != NULL)
		*size = used;

	free(contains);
	return (buf);
}

#define FLOAT_C(X) X##f

static float pow24(float x) {
  float x2, x3, x4;
  x2 = x * x;
  x3 = x * x * x;
  x4 = x * x * x * x;
  return FLOAT_C(0.0985766365536824) + FLOAT_C(0.839474952656502) * x2 +
         FLOAT_C(0.363287814061725) * x3 -
         FLOAT_C(0.0125559718896615) /
             (FLOAT_C(0.12758338921578) + FLOAT_C(0.290283465468235) * x) -
         FLOAT_C(0.231757513261358) * x - FLOAT_C(0.0395365717969074) * x4;
}

static float frgb2linl(float x) 
{
  float r1, r2;
  r1 = x / FLOAT_C(12.92);
  r2 = pow24((x + FLOAT_C(0.055)) / (FLOAT_C(1.0) + FLOAT_C(0.055)));
  return x < FLOAT_C(0.04045) ? r1 : r2;
}

static void rgb2lin(float *f, const unsigned char *u, u_int size)
{
  unsigned i;
  for (i = 0; i < size; ++i) f[i] = u[i];
  for (i = 0; i < size; ++i) f[i] /= FLOAT_C(255.0);
  for (i = 0; i < size; ++i) f[i] = frgb2linl(f[i]);
}

static float adjudicate(u_int glyph_idx, float bg[CN], float fg[CN], const float *lb, u_int gs, float bestSoFar) {
	uint128_t 	glyph_data = kGlyphs128[glyph_idx];
	float 		dist_sq = 0.0f;

	for (u_int c = 0; c < CN; c++) {
		float fg_chan = fg[c];
		float bg_chan = bg[c];
		float *this_chan = lb + c * gs;

		for (u_int i = 0; i < gs; i++) {
			uint128_t one = 1;
			uint128_t is_fg = (glyph_data & (one << i)) ? 1 : 0;
			float diff = (is_fg ? fg_chan : bg_chan) - this_chan[i];
			dist_sq += diff*diff;
		}
		if (dist_sq >= bestSoFar)
			return dist_sq;
	}
	return dist_sq;
}

u_int sixel_derasterize_block(unsigned char *rgb_block, u_int gs, unsigned char *picked_fg, unsigned char *picked_bg) {
	float 	best, lin_block[CN * gs];
	u_int 	picked_glyph;
	u_int 	temp_b[CN], temp_f[CN];
	unsigned char 	b[CN], f[CN];
	float 	bf[CN], ff[CN];

	best = -1u;
	rgb2lin(lin_block, rgb_block, CN * gs);

	for (u_int glyph_idx = 0; glyph_idx < GLYPH_COUNT; glyph_idx++) {
		memset(temp_b, 0, sizeof(temp_b));
		memset(temp_f, 0, sizeof(temp_f));
		memset(b, 0, sizeof(b));
		memset(f, 0, sizeof(f));

		 
		/* Compute the best background/foreground color to use with the currently tried glyph */
		int ones = 0;
		for (u_int i = 0; i < gs; i++) {
			uint128_t one = 1;
			if (kGlyphs128[glyph_idx] & (one << i)) {
				for (u_int c = 0; c < CN; c++) {
					temp_f[c] += rgb_block[c * gs + i];
				}
				ones++;
			} else {
				for (u_int c = 0; c < CN; c++) {
					temp_b[c] += rgb_block[c * gs + i];
				}
			}
		}

		if (ones) {
			for (u_int c = 0; c < CN; c++) {
				f[c] = temp_f[c] / ones;
			} 
		}

		if (ones < gs) {
			for (u_int c = 0; c < CN; c++) {
				b[c] = temp_b[c] / (gs - ones);
			}
		}

		rgb2lin(bf, b, CN);
		rgb2lin(ff, f, CN);

		float r = adjudicate(glyph_idx, bf, ff, lin_block, gs, best);
		if (r < best) {
			best = r;
			picked_glyph = glyph_idx;
			memcpy(picked_fg, f, CN);
			memcpy(picked_bg, b, CN);
//			if (best < DIST_THRESHOLD) return picked_glyph;
		}
	}
	return picked_glyph;
}

struct screen *
sixel_to_screen(struct sixel_image *si)
{
	u_int gx = 8, gy = 16; /* TODO */ 
	struct screen		*s;
	struct screen_write_ctx	 ctx;
	u_int			 cell_, cell_y, sx, sy;
	unsigned char 			*rgbdata = NULL;
	u_int 			rgbdata_size;

	sixel_size_in_cells(si, &sx, &sy);

	s = xmalloc(sizeof *s);
	screen_init(s, sx, sy, 0);


	screen_write_start(&ctx, NULL, s);

   	rgbdata_size = CN * sx * sy * gx * gy;
	rgbdata = xmalloc(rgbdata_size);
	memset(rgbdata, 0, rgbdata_size);

	u_int *colours = xmalloc(si->ncolours * sizeof(u_int));
	/* Convert HSL to RGB if necessary */
	for (u_int k = 0; k < si->ncolours; k++) {
		u_int R = 0;
		u_int G = 0;
		u_int B = 0;
		u_int flag = SIXEL_FLAG_RGB;
		if (si->colours[k] & SIXEL_FLAG_HSL) {

			/* sixel HSL format is in fact 0xHHLLSS and H=0 is blue */ 
			float H = ((si->colours[k] >> 16) & 0x1FF);
			H -= 120.0;
			if (H < 0.0)
				H += 360.0;
			float L = ((si->colours[k] >> 8) & 0xFF) / 100.0;
			float S = (si->colours[k] & 0xFF) / 100.0;
			
			float C = (1.0 - fabs(2*L - 1.0)) * S; /* range [0;10000] */
			float X = C * (1.0 - fabs(fmodf((H / 60.0), 2.0) - 1));
			float m = L - C/2;

			float _R, _G, _B;
			if (H < 60) {
				_R = C;
				_G = X;
				_B = 0;
			} else if (H < 120) {
				_R = X;
				_G = C;
				_B = 0;
			} else if (H < 180) {
				_R = 0;
				_G = C;
				_B = X;
			} else if (H < 240) {
				_R = 0;
				_G = X;
				_B = C;
			} else if (H < 300) {
				_R = X;
				_G = 0;
				_B = C;
			} else { /* H < 360 */
				_R = C;
				_G = 0;
				_B = X;
			}

			R = (_R + m) * 255;
			G = (_G + m) * 255;
			B = (_B + m) * 255;
			flag = SIXEL_FLAG_HSL;
		} else if (si->colours[k] & SIXEL_FLAG_RGB) {
			flag = SIXEL_FLAG_RGB;
			R = (((si->colours[k] >> 16) & 0xFF) * 255) / 100;
			G = (((si->colours[k] >> 8) & 0xFF) * 255) / 100;
			B = ((si->colours[k] & 0xFF) * 255) / 100;
		}
		colours[k] = flag | (R << 16) | (G << 8) | B;
	}

	/* resample sixel rgbdata (nearest-neighbor) */
	for (u_int c = 0; c < CN; c++) {
		for (u_int x = 0; x < sx * gx; x++) {
			for (u_int y = 0; y < sy * gy; y++) {
				u_int source_x = (x * si->xpixel) / gx;
				u_int source_y = (y * si->ypixel) / gy;

				if ((source_x >= si->x) || (source_y >= si->y)) continue;
				u_int source_colour = si->lines[source_y].data[source_x];

				if ((source_colour <= 0) || (source_colour > si->ncolours)) continue;

				rgbdata[c * sx * sy * gx * gy + y * sx * gx + x] = (colours[source_colour - 1] >> (c * 8)) & 0xFF;
			}
		}
	}

	free(colours);

	/* derasterize */
	unsigned char *rgb_block = xmalloc(CN * gx * gy);
	for (u_int cell_x = 0; cell_x < sx; cell_x++) {
		for (u_int cell_y = 0; cell_y < sy; cell_y++) {

			/* extract gx x gy block from rgbdata */
			for (u_int c = 0; c < CN; c++) {
				for (u_int y = 0; y < gy; y++) {
					memcpy(rgb_block + c * gx * gy + y * gx, rgbdata + c * sx * sy * gx * gy + (cell_y * gy + y)* sx * gx + cell_x * gx, gx);
				}
			}

			/* pick glyph and bg/fg color closest to rgb block */
			unsigned char fg[CN];
			unsigned char bg[CN];
			u_int glyph_idx = sixel_derasterize_block(rgb_block, gx * gy, fg, bg);

			/* build grid cell corresponding to glyph and colors */
			struct grid_cell gc;
			memcpy(&gc, &grid_default_cell, sizeof gc);
			int utf8_size;
			char utf8[8]; /* even though utf8 max size is 4, tptoa documentation requires a 8 bytes buffer */ 
			char *end = tptoa(utf8, kRunes[glyph_idx]);
			for (char *ptr = utf8; ptr != end; ptr++) {
				if (ptr == utf8) {
					if (ptr + 1 == end) {
						utf8_set(&gc.data, *ptr);
					} else {
						utf8_open(&gc.data, *ptr);
					}
				} else {
					utf8_append(&gc.data, *ptr);
				}
			}
			gc.fg = COLOUR_FLAG_RGB;
			gc.bg = COLOUR_FLAG_RGB;
			for (u_int c = 0; c < CN; c++) {
				gc.fg |= fg[c] << (c * 8);
				gc.bg |= bg[c] << (c * 8);
			}
			grid_view_set_cell(s->grid, cell_x, cell_y, &gc);
		}
	}

	screen_write_stop(&ctx);

	free(rgb_block);
	free(rgbdata);

	return (s);
}
