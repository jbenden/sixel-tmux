> sixel-tmux : a terminal multiplexer that display graphics one way or another!

## Live examples

![mintty running sixel-tmux displaying sixels inside](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/sixel-tmux_mintty.gif)

![mintty running sixel-tmux with 2 vertical tabs](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/sixel-tmux_2-vertical-tabs.gif)

## What is sixel-tmux?

sixel-tmux is a fork of tmux, focusing on proper graphic (sixels) and text
attributes preservation and representation (bold, underline...).

While tmux now has a better support of text attributes, it still eats these
yummy sixels!

The initial description of sixel-tmux said it could display graphics simply
"because it does not eat escape sequences" that constitutes the sixels.

This new version still does that, but now with with a more ambitious 2nd mode:

- not only it does not eat escape sequences containing the sixels, which is
  the "sixel passthrough" mode, to pass them to any connected terminal that
  supports sixels,

- but it if detects it's connected to a terminal that can't display sixels at
  all (like most Linux terminal emulators based on VTE, or hum uhm, a certain
  Windows Terminal...), the sixel images are converted into a text rendition
  thanks to https://github.com/csdvrx/derasterize which is the ambitious
  "fallback mode"

This is sixel-tmux running inside https://github.com/csdvrx/cutexterm in "sixel passthrough" mode, as you can see from the XTerm right-click menu:
![cutexterm running sixel-tmux and displaying sixels inside](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/cutexterm_sixel-tmux.png)

This is another sixel-tmux running inside another cutexterm in "sixel fallback" mode, with all the sixel graphics converted into what is often called "ascii-art" by derasterize
![cutexterm running sixel-tmux with derasterized sixels](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/cutexterm_sixel-tmux_derasterized.png)

Since cutexterm uses xterm, which features proper sixel support, and both cutexterm and mintty examples consist of running the client and the server the same terminal, you may say it's easy, and still not convinced sixel-tmux is cool.

Here's a regular off-the-shelf (off the Windows store?) Windows Terminal Preview using sixel-tmux to become sixel-aware and display derasterized pictures to replace the sixels sequence:
![sixel-tmux offering derasterized sixels from mintty to Windows Terminal Preview](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/sixel-tmux-inside-windows-terminal.gif)

Here's another regular Windows Terminal Preview connecting to a sixel-tmux session that has been opened in mintty, with both terminals side-by-side on Windows 10, and Windows Terminal automatically benefitting from the desrasterized fallback mode of the previously viewed sixels from within mintty: notice how sixel-tmux shows pure sixel content for as long as possible, here until Windows Terminal Preview connects to the session
![sixel-tmux offering derasterized sixels from mintty to Windows Terminal Preview](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/sixel-tmux-inside-both-mintty-and-windows-terminal.gif)

Now, is that cool or what?

## It's cool, but explain me like I'm 5

You can now see cute pictures inside your terminal, even if you shouldn't be
able to do that, say if your terminal is like Windows Terminal and doesn't
support the sixel format!! (at least, not yet)

This is why this second mode is very ambitious: it is like "putting magical
glasses" on your existing terminal that is currently blind to sixels graphics.

Thanks to having sixel-tmux magical glasses on, your terminal can now see
sixels without changing anything else: while before it showed you nothing at all,
or just an empty square (due to your terminal not supporting sixels), your
terminal now shows you the best it can do with the sixels! 

Of course, the glasses are not really magical, therefore they are imperfect:
the picture will be blocky due to the technical limitations of derasterizing in
character sized block. Also, the picture will be made with a possibly limited
color palette: if your terminal only supports 16 or 256 colors, sixel-tmux
can't invent the colors you'll be missing.

Derasterize is simply the best it can do, but still, I believe it is far better
than the alternative of having no picture shown at all!

## Limitations: the sixels can be replaced by derasterize

It is a work in progress: for the first mode (pass through) ideally sixel-tmux
should keep a sixel scrollback buffer to store actual sixels sequences instead
of their derasterized versions, to "clip them" as necessary and let the
terminal display them.

At the moment, a scrollback will often cause the sixels to be replaced by blocks
coming from derasterize, unless you have very specific conditions, like using
your terminal native scrollback buffer with just one tmux window (or not
switching to the others). Also, you must take the precaution of moving tmux
menu bar to the botton of the screen: this is just so you can scrollback
without the textual menu bar messing up your pretty pictures!

It may sound quite a constraining, but I provide here known good configurations
files, along with some help to configure recommended terminal emulators.

## Ambitions: having sixel use become mainstream thanks to sixel-tmux (short rant)

If you've followed on the "magical glasses" analogy, you can see how
derasterize is much more ambitious than just a "fallback" mode for sixel-tmux:
I basically want to force sixels down every (willing) Linux user throats
(because consent is a thing!), to stop the current deadlock situation of
competing formats that has set back the progress of console graphics for decades.

In case it's not clear enough, I will be very blunt: releasing sixel-tmux with
derasterize support baked-in is my "activist action" to democratize sixels, by
weaponizing my own fork of tmux against all those who have decided that running out
the clock (my apologies for this football metaphor) was a fair move to ensure
the victory for their favorite non-sixel terminal graphic format.

You don't believe me? They have been blocking patches adding sixels support for
spurious reasons, causing over 5 years of delays for most Linux users.

Skip if you don't care about the politics, but you can get more details on the
situation if you can stomach my long https://github.com/csdvrx/sixel-tmux/blob/main/RANTS.md

### Testing sixel-tmux

sixel-tmux is a fork of tmux, with just one goal: having the most reliable
support of graphics.

In a console, this means:
 1. ANSI blocks and box-drawing characters <https://en.wikipedia.org/wiki/Box-drawing_character> to display ASCII and ANSI art <https://en.wikipedia.org/wiki/ANSI_art>
 2. ANSI escape codes, with a focus on SGR parameters <https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_parameters> to render text attributes
 3. and the best of all: sixel graphics <https://en.wikipedia.org/wiki/Sixel> to display graphs <https://github.com/csdvrx/sixel-gnuplot>

This seems very simple requirements, yet a regular tmux fails on all 3:
 1. ANSI art is corrupted,
 2. some SGR parameters are supported, but the more exotic features like overline do not work, and some sequences may be intercepted and improperly changed
 3. sixels are not supported.

For a proper support of all these features, sixel-tmux is necessary but
not sufficient: the terminal must be able to decode and render the sequences
passed by sixel-tmux.

Therefore, a separate test suite <https://github.com/csdvrx/sixel-testsuite>
is provided to first check if a terminal can achieve these goals.

If you are dissatisfied by your current terminal, please consider mlterm
<http://mlterm.sourceforge.net/> or mintty <https://mintty.github.io/> that are
tested and known to work well.

Microsoft Window Terminal is very promising, but does not support sixels yet.
Please upvote the feature request <https://github.com/microsoft/Terminal/issues/448>

You can find a longer list of alternative terminals and terminal multiplexers through
related works such as <https://github.com/gizak/termui/pull/233>

Below, we'll be using the test suite to configure your terminal. If that seems
too complicated, install msys2 and use mintty. If you like tabs and regret
their absence in mintty, try Windows Terminal Preview instead.

## Configuration

In an ideal world, each piece of software would seemlessly integrate with the
others, and work flawlessly right after installation.

We do not live in such a world, so you may need to configure your terminal to
make sure sixel-tmux will work fine.

The configuration step is optional, but recommended for best results - after
all, if you care to install sixel-tmux instead of sticking to tmux, you may
care about graphics!

While I may recommend some defaults and test for them, you may have to make
some choices, as terminal issues are extremely complicated: it is sometimes
impossible to achieve perfect results without tweaking applications, since
they often have contradictory requirements.

#### Step 1. Deploying then using terminfo files with the TERM variable

Step 1 is to deploy then use a new terminfo that will accurately describe the
capabilities of sixel-tmux, and do the same if necessary for the terminal into
which you will run sixel-tmux: this is achieved by referring to this terminfo
in the TERM variable prior to running sixel-tmux, then referring to another
termifo from within sixel-tmux.

The terminfo list the capabilities of your terminal. Some prodding and guessing
is possible, but for best results, you want a terminfo that: 

- accurately describes your terminal capabilities, without minimizing them or
  overestimating them, (more on that below)
 
- uses a name that is common enough to avoid confusing software but not so
  common that it tries to make clever guesses based on the name alone, for the
  sake of compability with software that has been dead or ununsed for tens of
  years

A gleaming example of such issues is MC, which treats screen-256color as xterm
then proceeds to ignore the kmous entry of the terminfo unless DISPLAY is set, cf
https://unix.stackexchange.com/questions/304960/midnight-commander-force-xterm-permanently
"Midnight Commander treats that as 'xterm' (and decides it supports a mouse)
only if DISPLAY also is set. Ignore the comment (it is wrong), and just read
the source code. The function return value is used in one place, from main.c
(again, ignore the comment...).  Midnight Commander ignores the actual contents
of the terminal description, which happens to say that this configuration
supports xterm-style mouse (i.e., the existence of kmous=\E[M, in the
description). It does this to work around its problems using slang, which also
ignores the terminal description, looking only at TERM. This is an old bug,
dating back to the 1990s"

The main tmux branch has had similar problems with terminfo entries in the
past; ultimately patches have been added to support the core features such as
24-bit color: if you are curious, check
https://sunaku.github.io/tmux-24bit-color.html#usage which describes the
history of the process and explain the basics of what we'll be doing.

As many applications hardcode workarounds, I have decided to use the "sixel-"
prefix: if they expect tmux or xterm, we should be able to catch them by
surprise!

### Step 1.1 compiling the provided terminfo

1. install sixel-tmux.terminfo: `tic sixel-tmux.terminfo; tic -o ~/.terminfo sixel-tmux.terminfo`
2. start sixel-tmux: `sixel-tmux`
3. select sixel-tmux: `export TERM=sixel-tmux`
4. verify sixel-tmux is used: `tput smglr|base64` should return GzcbWz82OWgbWyVpJXAxJWQ7JXAyJWRzGzg= : most sixel aware applications will check for smglr and other sequences before outputting long strings of sixels.

Once you are done with these 4 steps, you will have a valid sixel-tmux to use
inside sixel-tmux (!!)

However, before starting sixel-tmux, you must export the appropriate TERM for your
terminal: otherwise, sixel-tmux will know know what your terminal is capable of
(say, how many colors it may support) and could give you ugly graphics.

#### Step 1.2 exporting TERM to say which terminfo to use

If you are using mintty, mlterm or the cutexterm configuration of XTerm, you
can safely skip ahead and just use xterm-256color with: `export
TERM=xterm-256color` : all 3 terminals supports the best possible set of features:
most of the fancy text attributes, 256 colors, and truecolors on top!

If you are using Windows Terminal Preview, you are mostly in the good, but
there are different versions and you may be stuck with an old version or there
may have been regression, so please follow the next 2 steps.

#### Step 2: Configuring the msys2 features

I am not a huge fan of WSL2: WSL1 was technically more ambitious. WSL2 is just
a glorified VM.

I like my processes to interact with eachother, so I'm sticking to MSYS2, and
if you want a great command-line experience on Windows, I recommend you do the
same (especially if you do not have a large amount of RAM!)

So install msys2 from https://www.msys2.org/, which comes with mintty which you
may want to use as it's currently better than Windows Terminal (unless you need
tabs more than sixels!)
 
If you are using msys2 within Windows Terminal, I recommend a commandline of
`C:/msys64/usr/bin/env.exe  MSYS='enable_pcon winsymlinks:nativestrict'
MSYSTEM=MSYS /bin/bash.exe --login -i`

If you want to immediately start sixel-tmux, you will need to use script to create a pseudotty: for example replace the call to bash with `/usr/bin/script -c '/usr/bin/tmux c' /dev/null` to create a new session, or `/usr/bin/script -c '/usr/bin/tmux a' /dev/null` to attach to any existing sixel-tmux session.

To run from within bash inside Windows Terminal, you do the same, due to the way the console is handled; see https://cygwin.com/pipermail/cygwin/2020-May/244878.html for more details.

### Step 3: Configuring your 16 colors palette

You should also use a color palette (called "color scheme" in Windows Terminal)
separating the light colors from the regular colors: Windows Terminal knows the
difference, as you can test yourself from Powershell, for example with
https://gist.githubusercontent.com/mlocati/fdabcaeb8071d5c75a2d51712db24011/raw/b710612d6320df7e146508094e84b92b34c77d48/win10colors.cmd 

If you want to check if you can do the same in bash, the simplest is comparing
red to bright-red (as red is kept unaffected by Solarized which uses 6 of the 8
colors for shades of grey) with:

```
printf "\e[31mTest red \e[91mTest bright red\e[39
\e[35mTest purple \e[95mTest bright purple\e"
```

### Step 3.1: Defining your 16 colors palette

Here is for example a custom Solarized Light palette, inverting the highlight and regular colors compared to Windows Terminal default Solarized Light theme: 
![Windows Terminal Solarized Light custom palette](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/windows-terminal-solarized-custom-palette.jpg)

The inversion is deliberate: most command-line tools know how to use colors, but very few know how to use SGR attributes, so it makes sense to tweak the rare tools that will take advantage of SGR or the Solarized theme, while keeping everything else on its default settings

If you want to do the same and add this customized Solarized to your Windows
Terminal, here is the relevant JSON:

```json
        {
            "background": "#FFFDE7",
            "black": "#073642",
            "blue": "#839496",
            "brightBlack": "#002B36",
            "brightBlue": "#268BD2",
            "brightCyan": "#2AA198",
            "brightGreen": "#859900",
            "brightPurple": "#D33682",
            "brightRed": "#DC322F",
            "brightWhite": "#EEE8D5",
            "brightYellow": "#B58900",
            "cursorColor": "#002B36",
            "cyan": "#93A1A1",
            "foreground": "#58584D",
            "green": "#586E75",
            "name": "Solarized Light Custom",
            "purple": "#6C71C4",
            "red": "#CB4B16",
            "selectionBackground": "#FFFFFF",
            "white": "#FDF6E3",
            "yellow": "#657B83"
        },
        {
            "background": "#002B36",
            "black": "#002B36",
            "blue": "#839496",
            "brightBlack": "#073642",
            "brightBlue": "#268BD2",
            "brightCyan": "#2AA198",
            "brightGreen": "#859900",
            "brightPurple": "#D33682",
            "brightRed": "#DC322F",
            "brightWhite": "#EEE8D5",
            "brightYellow": "#B58900",
            "cursorColor": "#FFFFFF",
            "cyan": "#93A1A1",
            "foreground": "#839496",
            "green": "#586E75",
            "name": "Solarized Dark Custom",
            "purple": "#6C71C4",
            "red": "#CB4B16",
            "selectionBackground": "#FFFFFF",
            "white": "#FDF6E3",
            "yellow": "#657B83"
        },
```

You can also use command line scripts to dynamically change your palette,
here's for example
https://github.com/csdvrx/sixel-testsuite/toggle-solarized-light.sh: (there is a similar file for solarized-dark)

```
#!/bin/sh

# With 24 bit colors support, dynamically change your current palette to solarized-light
echo "attempting toggle of solarized-light"
echo -ne   '\eP\e]10;#657B83\a'  # Foreground   -> base00
echo -ne   '\eP\e]11;#FDF6E3\a'  # Background   -> base3
echo -ne   '\eP\e]12;#DC322F\a'  # Cursor       -> red
echo -ne  '\eP\e]4;0;#073642\a'  # black        -> Base02
echo -ne  '\eP\e]4;8;#002B36\a'  # bold black   -> Base03
echo -ne  '\eP\e]4;1;#DC322F\a'  # red          -> red
echo -ne  '\eP\e]4;9;#CB4B16\a'  # bold red     -> orange
echo -ne  '\eP\e]4;2;#859900\a'  # green        -> green
echo -ne '\eP\e]4;10;#586E75\a'  # bold green   -> base01 *
echo -ne  '\eP\e]4;3;#B58900\a'  # yellow       -> yellow
echo -ne '\eP\e]4;11;#657B83\a'  # bold yellow  -> base00 *
echo -ne  '\eP\e]4;4;#268BD2\a'  # blue         -> blue
echo -ne '\eP\e]4;12;#839496\a'  # bold blue    -> base0 *
echo -ne  '\eP\e]4;5;#D33682\a'  # magenta      -> magenta
echo -ne '\eP\e]4;13;#6C71C4\a'  # bold magenta -> violet
echo -ne  '\eP\e]4;6;#2AA198\a'  # cyan         -> cyan
echo -ne '\eP\e]4;14;#93A1A1\a'  # bold cyan    -> base1 *
echo -ne  '\eP\e]4;7;#EEE8D5\a'  # white        -> Base2
echo -ne '\eP\e]4;15;#FDF6E3\a'  # bold white   -> Base3
```

### Step 3.2: Testing both your 16 colors palette and accessing it through AIXTerm SGR support

Supporting AIXTerm SGR allow to properly control brightness separately from
bold, using AIXTerm SGR 90 to 96 and 100 to 107 for a brightness control
independant of the SGR 1 attribute that often just means "bold" on Linux.

In Windows Terminal, AIXTERM SGR codes nicely complement the setting
`"intenseTextStyle": "bold"` for SGR1, allowing Linux users to still access
brightness control through different SGR attributes.

This is nothing new: it's an approach that has been long known and documented,
cf this example from 7 years ago in
https://unix.stackexchange.com/questions/93814/cant-apply-brightness-to-terminals-background-color

Given the compatibility matrix of existing terminals from
https://unix-junkie.github.io/christmas/Comparison%20of%20Terminal%20Emulators%20-%20Colour%20Support.html
the support of AIXTerm sequences for brightness control seem highly desirable
to match the behavior of most terminal emulators: if you are not using mintty or Windows Terminal, you should really check if this feature is suported!

Therefore, another script provides a more complete test: here is what both
the minimal red-purple-brightred-brightpurple script from
https://github.com/csdvrx/sixel-testsuite/aixterm-minimal.sh and the more
complete 16 colors script from
https://github.com/csdvrx/sixel-testsuite/16-colors.sh look like in a Windows
Terminal with a custom Solarized Light palette
![Windows Terminal with Solarized custom running tests](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/windows-terminal-solarized-custom.jpg)

If you did not do the previous step, the colors may look identical: Windows
Terminal supports AIXTerm SGR attributes for brightness, so it just means you
have a bad palette, for example:

![Windows Terminal with a bad Solarized custom palette next to mintty](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/windows-terminal-wrong-palette.jpg)

### Step 4: Configuring 256 colors and 24 bit colors

Most terminals now support 256 colors, and a few support 24 bit colors, but
ideally, you want both. Fortunately, this is far less complicated than proper
16 color support.

However, Solarized users do not like 256 color mode, because it often means
approximations of the RGB values of the precise Solarize colors are used,
instead of keeping the 16 precise true color sequences.

Still, it's a nice thing to have a 256 color mode, as many tools don't know how
to call colors from a 24 bit palette (the setaf, setab capabilities from
terminfo) let alone juggle with the 3 incompatible variants if they don't know
what a terminfo capability is (check
https://github.com/alacritty/alacritty/issues/1485 for an clear presentation of
why SGR 38/48 in ECMA-48 is vague)

So start with ``tput colors`` which should return 256, if not, you need to go
back to step 1 and export a TERM that will fully represent your terminal, yet
give you the expected answer of 256 when asked with tput.

For 24-bit or "truecolor" mode, try ``tput setab`` (or tput setaf for the
foreground) if with either you get a string containing e48 of about 63
characters (use ``tput setab|grep e48|wc`` after the export TERM), you should
be good.

If you need more explanation, check in sixel-tmux.terminfo source from
config/.terminfo.src: it's meant to be didactic, so it doesn't include any
other terminfo but defines paramters from scratch, and instead fully define
each and every capability, with a few comments for the crunchy parts!

You will see that Tc is another option, but setaf/setab should be preferred,
and that colors should not return the true value because until 2018 the
terminfo format was constrained to signed 16 bit ; see also
https://stackoverflow.com/questions/36158093/terminal-color-using-terminfo/36163656#36163656 

```
# 16m color was Tc, now obsoleted: use setaf, setab
# https://github.com/jwilm/alacritty/issues/1485#issue-346291738
#
# 256 colors was: colors#0x100, but colors#16777216 is not signed 16 bit so use colors#0x100
# along with setaf setbg to get 256 as fallback for apps not knowing setaf/setab
```

In 2021, the terminfo format change is relatively recent (3 years) ; to play it
safe I have decided to keep using the same colors string: apps are likely to
keep using the alternative ways for a while (like checking for setab).

If you are interested in more details, including clever ways to detect if
truecolor is supported (try setting the color to a specific truecolor value
then ask the terminal what color it's using and compare the value to what's
expected if it could comply) check https://gist.github.com/XVilka/8346728

### Step 5: testing colors

Now, try sixel-testsuite/256-colors.pl to confirm the 256 colors are working,
then see if you are lucky and can indeed use a 24 bits palette with
sixel-testsuite/24-bit-color.sh which does not use anything form terminfo

It's a new awk-based script that is both faster and nicer that the previous version, by taking into account the width of your terminal to scale

Here's what it looks like in Windows Terminal:

![Windows Terminal in restricted width running the truecolor test](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/truecolor-test1-small.jpg)

![Windows Terminal in full width running the truecolor test](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/truecolor-test2-fullwidth.jpg)

### Step 6: font test

In this final test, we will check if the font you've selected supports the various drawing characters used by terminal tools:
1. start with `cat font-ansi-blocks.txt` to see block drawing characters and a test bear
2. run `font-vt100.sh` to see the basic box drawing characters
2. `cat font-ansi-box.txt` to see the full set of box drawing characters
3. `cat font-dec.txt` to check DEC VT220 extra characters
4. `cat font-test-all.txt` to check unicode support and overall support of the drawing characters
5. Type in the console echo followed by the sign less than, then a space, then the sign equal, then another space, then bring back this line and remove the spaces.

For the first test, you should see a bear without lines insides its head:
![Font test 1](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/font-test1.jpg)

For the second test, you should see thin lines:
![Font test 2](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/font-test2.jpg)

For the third test, you should see lines:
![Font test 3](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/font-test3.jpg)

For the fourth test, you should see text in various languages, followed by the most important:  grids of various patterns:
![Font test 4](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/font-test4.jpg)

In this test, boxes should be properly aligned, and lines should intersect cleanly.

In the fifth and final test, if you see a double arrow, congratulations! Your font even supports the programming ligatures!

![Windows Terminal with Iosevka SS04 supports programming ligatures](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/font-programming-ligatures.jpg)

If the font test fails, you may need to change your font as it may not support every character; I recommend using Iosevka which supports about everything: I personally like SS04 best, but if you are into fine tuning, check the alternatives options!

Alternatively, when using mlterm you can tell it to extend your preferred font with one or more other fonts, for example:

```
        ISO8859_1 = Iosevka SS04 18
        ISO8859_15 = Iosevka SS04 18
        ISO10646_UCS2_1 =Iosevka SS04 18
        ISO10646_UCS2_1_BOLD = Iosevka SS04 18
        U+2500-25ff=Segoe UI Symbol
        U+25C6 = Tera Special # Diamond                  ◆
        U+2409 = Tera Special # Horizontal tab           ␉
        U+240C = Tera Special # Form feed                ␌
        U+240D = Tera Special # Carrier return           ␍
        U+240A = Tera Special # Linefeed                 ␊
        U+2424 = Tera Special # Newline                  ␤
        U+240B = Tera Special # Vertical tab             ␋
        U+23BA = Tera Special # Horizontal line 1        ⎺
        U+23BB = Tera Special # Horizontal line  3       ⎻
        U+2500 = Tera Special # Horizontal line  5       ─
        U+23BC = Tera Special # Horizontal line  7       ⎼
        U+23BD = Tera Special # Horizontal line  9       ⎽
```

When everything works to your satisfaction, compile sixel-tmux, or run the provided msys2 binary

### Compiling sixel-tmux

To compile, first install libevent2 and ncurses along with the C development packages (gcc, make etc)

If you are using msys2 use pacman -S to install the relevant development packages after checking their version with pacman -sS:
```
pacman -sS libevent
pacman -sS ncurses
pacman -S msys/libevent-devel
pacman -S msys/ncurses-devel
```

After installing them, here is what you should get:

![pacman from msys2 inside Windows Terminal](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/msys2-ncurses-libevent-installed.jpg)

You can then compile with: `./configure && make && make -j8 install`

if you want to run it from Windows Terminal, please use script to create a pty instead of /dev/consN, for example `/usr/bin/script -c '/usr/bin/tmux c' /dev/null` to create a new session or `/usr/bin/script -c '/usr/bin/tmux a' /dev/null` to attach to an existing session.

You can then run gain the sixel test-suite from <https://github.com/csdvrx/sixel-testsuite> to check if what was working in your terminal before sixel-tmux is still working.

If it isn't, oops: it means sixel-tmux broke something. Patches are welcome!
(this outcome is extremely unlikely, but you know, prepare for the worst, and hope for the best!)

### Current status and tests failed

Sixels fully work, but images are not redrawn when returning from another buffer (snake.six)

vertical split does not plays well with ansi and sixel in passthrough mode

Overlines and double underlines are not working yet (ansi-vte52.sh) ; the situation is slightly better in mintty:
![ansi-vt52 inside sixel-tmux inside mintty](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/ansi-vt52-inside-sixel-tmux-inside-mintty.jpg)

than inside Windows Terminal:
![ansi-vt52 inside sixel-tmux inside Windows Terminal](https://raw.githubusercontent.com/csdvrx/sixel-tmux/main/examples/ansi-vt52-inside-sixel-tmux-inside-windows-terminal.jpg)

dynamic switching of the 16 colors palette is not working (toggle-solarized-dark.sh/toggle-solarized-light.sh)
