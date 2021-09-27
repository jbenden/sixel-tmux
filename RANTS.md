> A long rant on why I believe sixel support has been blocked deliberately and unfairly

I'm sick and tired of the current sixel situation. This may not gain me friends, but
I've decided to speak up.

## Some maintainers are holding users hostage to favor their preferred formats

Sorry guys, but I don't want you to hold Linux terminal users hostage for your
petty concerns over what is the "right" way to do something like sixels.

It's something that has been done successfully for over 30 years. It's 2021,
and we should be able to do litterate programming in the console, with full
graphical support. Currently, it is way too hard, due to artificial hurdles,
such as the lack of sixel support. This situation is totally artificial, since
various patches have been released for a long time on other platforms.

Just because of your pettiness, I had to use a graphical RStudio in uni, which
put my old computer on its knees. That's not cool, as I could totally have done
everything from a commandline, if only Linux has supported back then what has been
supported by Windows from a long time.

It's not like it's impossible: the patches had been written.

Still, you could successfully block sixel support by having control over the
terminal emulators or the libraries. Ok, but now, try to prevent your users
from running sixel-tmux!

See, I learned from what you guys often say: I know free software maintainers
owe their users nothing, and apparently not even the basic concern of not
torpedoing ongoing efforts to enhance their software and support more formats.

So I owe you nothing either, and I will release the software I wish had existed.

Also, you guys like to say that the only voices that matters are from those who
can code? Hmm, ok for that too. Let's see how you like the code I release to
show the pointlessness of your petty fights, and free your users from your
questionable decisions.

## Technical reasons can not justify this

Now, if you have tolerated my rants so far, please cool down with me, and read
for example this enlightening discussion about supporting the various font
styles (bold, bright, faint...) to understand the complexity of the issues we
have to deal with in a terminal: https://github.com/microsoft/terminal/issues/2916

You will see that ultimately, it comes to a question of preferences:

- Windows in general has sanctified the principle of backwards compatibility;
  a typical example is that you can run unmodified old binaries from say 20
  years ago on a modern Windows installation with everything working just as
  before,

- Linux maintainers often prefer to sacrifice backwards compatibility in the
  hope of achieving better results in the long run. 

After all, why not? In the absolute, each approach has its pros and cons, but when it
comes to the cons-ole (pun intended) and terminal emulators, the Linux approach
seems very misguided.

They try to dress it up with positive terms, like "brave" and "hope", see for
example egmontkob comment in
https://github.com/microsoft/terminal/issues/2916#issuecomment-544999801: "But
again it's mostly a question of project vision: where does WT want to go? Is it
legacy and backwards compatibility, or being a decent modern future environment
that is more important? Does it want to play safe, or be brave?"

Dude, in the word of terminals, playing safe is being brave, as backwards
compatibility requires tremendous creativity and complexity, so that we can all
keep enjoying the programs we love instead of breaking them every now and then
on a whim.

## Ranting again on the poor state of terminal emulation on Linux

You may not be convinced, but if you care about pretty text support in the console,
compare the situation on Windows with the situation on Linux: given the bad
results achieved by terminal emulators on Linux as described in my other long rant on
https://github.com/csdvrx/cutexterm#why-did-you-make-cutexterm-warning-long-rant
I have a lot of reasons to believe the Linux approach is wrong.

Why is there not a single free software terminal having feature parity with mintty?

Why in 2021 is a pimped up XTerm the best that Linux has to offer?

What will happen as Wayland replaces X?

Linux terminal emulators always fail, but in a quite consistent fashion, let's
grant them that much: they strive to achieve only the bare minimum, they show
an utter disrespect for the established standards, and a lack of concern for
achieving excellency. In a way, I understand why they don't care about backwards
compatibility: they have much bigger problems to deal with before addressing that!

And it's not just because I want working working text ligatures, or low latency
in my terminal: that may be my pet concern, but just it's just a canary in the
mine for many more issues. Still, if you need more data, look at the latency
test from https://danluu.com/term-latency/

Everybody has an excuse for ligatures: they are new, or not everybody likes
them. For latency, it's the fault of gnome, or X, or wayland... but how come
the gigantic XTerm is now the faster in the block? Because it has had the
benefit of time and stable specs?

Ok then, what about sixels? They are over 30 years old, and you can implement
them from scratch, so there can be no such excuse!

## Sixel Sabotage in VTE

Do you really wonder why there's no sixel support in VTE in 2021, given that a
first patch implenting sixel support in VTE was released over 5 years ago?

Check https://bugzilla.gnome.org/show_bug.cgi?id=729204#c3 and you will
understand that's because sixels are akin to blasphemy to some people: sixels
are too technically impure, or not fancy, or something more elusive because
when faced with technical arguments, they can't explain their irrational dislikes!

Egmontkob has expressed multiple times his dislike of sixels like
https://github.com/thestinger/termite/issues/539#issuecomment-345259308 "Alas
no standard seems to emerge that doesn't suck big time (sixel is so last
millennium that it hurts)"

You know, terminals are last milleniums, and so is Unix, but it works great!

For the longest time, Egmontkob plan was to veto sixel support in VTE, for the
machiavellian reason of affording more time to other formats to try to catch up
https://bugzilla.gnome.org/show_bug.cgi?id=729204#c26 "Having sixel support
surely looks like a great win in the short term: more apps can display
pictures, users are happier.  I'd argue however that in the long term, adding
sixel support is straight harmful, for three main reasons. One is that it
pushes the entire ecosystem in the wrong direction. If VTE, which has about 50%
of market share within Linux, add this, it will encourage more terminals and
more apps to add this absolutely broken format, or happily stick to this if
it's already added, rather than looking for a better solution. It makes it
harder to argue and cooperate across terminals for a better one."

I couldn't disagree more with the whole thing, and I wish I had something,
anything, working 5 years ago: compared to being stuck with RStudio, even 256
colors sixels would have been a blessing!

And what do you have to show for these 5 years of delays? Is there a new
technically superior and more adopted format? If not, what do you gain from
stalling even more? Don't you think we would be in a much better place if the
original patch had been integrated? It would certainly have allowed more tools
to take advantage of graphics in the console, like iTerm2 did for the Macs!

So what about we try to move forward now? Because I think trying the same
approach again and again while failing to learn from the valuable lessons of
failure is very close to the definition of stupidy: you have tried to hold
sixel support back, you have failed. When are you going to concede victory?
Do you need 5 more years? 10 maybe?

If you need a funny example, watch Bart Simpson vs the Hamster, experiment 2,
the electrified cupcake on https://www.youtube.com/watch?v=5pxG4yd8U3U and you
will understand my issues, or why I recommend mintty in particular, and Windows
in general: the VTE situation about sixels is just an emerged part of the
iceberg: the issues run much deeper, and are much more serious.

## Technical arguments again

They may even be resolved someday, as quite a few people seem to understand the
situation on both technical and non-technical aspects, like Bastien Nocera in
https://bugzilla.gnome.org/show_bug.cgi?id=729204#c7 : "Sixel support seems
interesting because it's an existing protocol, not something supported by few
terminals that likely won't find actual buy-in in those very old protocols."

See also Hans Petter Jansson in https://bugzilla.gnome.org/show_bug.cgi?id=729204#c22
"I think sixels is an ok choice for a graphics protocol, mostly because it's
the most widely supported one, but also because I've managed to convince myself
the quantization it requires on the application side can be fairly efficient.
The palettized and RLE-encoded data strikes a reasonable balance between
bandwidth and CPU usage with a fairly simple encoder. Palette size also
provides a size/quality knob, much like it does in PNG. On top of that, ssh
allows for compression, which can mitigate the ASCII redundancy if you've got
the CPU headroom for zlib and don't mind the cryptanalysis implications."

That is a valid technical argument.

And HPA has done many great things, like a decent competitor to derasterize with
https://github.com/hpjansson/chafa ; also he also seem to be able to see beyond
technologies, and towards the users concerns and needs: see how chafa has added
support for both Kitty and iTerm2 on top of its own derasterize and sixel:
https://hpjansson.org/blag/2021/09/16/chafa-1-8-terminal-graphics-with-a-side-of-everything/

This is the *right* way to do: compatibility does not mean competition. If
anyone takes this sixel-tmux and add the ability to ingest Kitty/iTerm2
graphics (or pass them through, or convert between them and sixels...) I will
welcome it, and call that both a progress and a good thing! Ultimately, I do
not care that much about sixels: I care about doing things the right way, and
preserving the users freedoms!

But with the Linux mentality of sacrificing backwards compatibility whenever
convenient, I don't think it's a safe bet to hope the nice people like HPA will
win in the end.

This is why I released sixel-tmux. That's also why I use Windows besides the
wonderful mintty being the number 1 choice for terminal afficionados, it gives
more options in general: I like that because I don't like depending on people
who seem stuck in a desire to be lord-of-the-flies. Unfortunately, there
seem to be quite a few in the free software world...

### Ok but why release sixel-tmux now?

If you have noticed the current sixel-tmux is based on a tmux branch from 2019,
and concluded from it that sixel-tmux must have been finalized in early 2020,
you have a keen eye and a sharp mind!

Or maybe you have followed my posts, like this one from december 2020 on
https://github.com/microsoft/terminal/issues/448#issuecomment-744192801 where I
did include screenshots and a description of the features.

sixel-tmux hasn't changed much, because I've been quite busy in 2021.

Initially, I didn't even want to release all of sixel-tmux: it was made for a
custom order by a large client, and I kept it for my own personal use, as I
don't like sharing my toys.

Also, they are more like elaborate hack for very specific problems that most
people do not have, so I'm afraid they will not understand how or why to use
them instead of better alternatives (like, mintty if you want to gnuplot!)

On top of that, given the recent discussions and progress on
https://gitlab.gnome.org/GNOME/vte/-/issues/253 like Autumn Lamonte I had some
hope gnome-terminal would feature sixel support by default in a major linux
distribution.

However, it's not the case, and you will still need for example to "build from git
master using -Dsixel=true meson option" which is too complicated for most
users, in a world of flatpaks and docker images, especially with the multiple
dependancies (and the risk of breaking gnome)

Just yesterday, the VTE milestone was moved yet again - now sixels are planned
for vte 0.68.

I do not think it's acceptable to keep delaying: the VTE maintainers had 5
years to get the ball rolling with Yatli patches.

Personally, I gave them almost a full year, thinking the situation had improved.

It hasn't. I do not wish to tolerate that one more day. So sixel-tmux is out now, as incomplete and imperfect as it may be. Deal with it, VTE!

Am I trying to force your hand and make VTE include sixel support? You bet!

## Sixel support: sixel-tmux to the rescue!

If some people think adding sixel support by default to VTE is beyond them,
the VTE-based terminal users will now be able to simply use sixel-tmux as a
"gateway drug" to get into what these maintainers may believe to be the worse
format. Maybe I can get them into worse and worse stuff, say cutexterm to get
native sixels, then maybe mintty on Windows to get perfect font support?

Then maybe these users will say "Bye Linux", and they will move to Windows
Terminal when Sixel support is finally shipping? Recently, there have been
many positive signs this may eventually happen, like PR 11152 adding support
for DECRQSS cf https://github.com/microsoft/terminal/pull/11152

If it doesn't happen, mintty is still great, and who knows, when Windows
Terminal reaches feature parity, maybe I'll be tempted to fork it and add sixel
support? It must be possible, as there are several private branches adding
sixel support already. A little polishing could go a long way. I could even put
the end result on the Windows Store and let users decide what they like best...

Yes, for free software fanatics, by encouraging Linux terminal users to move to
Windows, I'm a bad person. But you know what? I'm proud of it :) Because
Windows Terminal is at least trying to escape from the global mediocrity of Linux
terminal emulators, and starting from scratch has achieved tremendous progress
in just a few years.

### Ok, but why a fork?

To support slow terminal with little bandwith, tmux started implementing
some controversial features after version 2.3. For example, when a lot of text
arrives, tmux start silently discarding the output until a redraw can be done.

Bug reports <https://github.com/tmux/tmux/issues/1019#issuecomment-318284445>
<https://github.com/tmux/tmux/issues/1502#issuecomment-429710887> mention
serious issues, such as being unable to cat a long text file without some parts
being cut off, so this questionable feature for slow terminals impacts many usecases. 

Among other things, it breaks sixel support. Even when not using sixels, it
seems very wrong for tmux to be taking the initiative of silently discarding
parts of the outputs, without providing any command line flags to override that
"feature".

As it can't be disabled, this anti-feature is not just a questionable default,
but a perverse feature: I believe breaking the integrity of the output of
whatever command was run, with no recourse, is not acceptable for a terminal multiplexer.

Also, refusing to even provide a command-line toggle to opt-out is not
respectful of the end user freedom: no one should have to track patches to
allow basic functionality like properly displaying long text files!

## Sixel sabotage again?

The maintainer of tmux explicitely said several times that even if a patch was provided
to support sixels, the functionality would never be added to tmux, and it would
have to be a fork
<https://github.com/tmux/tmux/issues/44#issuecomment-119755304>

By becoming a permanent fork of tmux, sixel-tmux does just what was asked so nicely!

Once again, free software maintainers love saying "my way or the highway", so
personally I take them to heart, and take the high way by doing exactly what
they askeed for :)

## Is this fork just for sixels?

sixel-tmux is not just made for sixels but for all graphical things that can no
longer be properly displayed inside tmux, including long text files.

It would have been better if the changes had had at least a fighting chance to
be upstreamed.

Unfortunately, a permanent fork seems necessary as again, the problem seems to
be more general, and spreading: tmux intercept and improperly rewrites various
escapes codes, breaking other things
<https://github.com/tmux/tmux/issues/1391#issuecomment-403267557>

At this point, it is very unlikely that tmux will be fixed: all these
questionable choices start piling up, and make the correct display of what is
not just simple plain text inside tmux harder and harder.

This is a sad situation as tmux was one of the best terminal multiplexer, and
supported what GNU screen couldn't when too many questionable choices had been
accumulated and frozen in over 32 years of spaghetti code.

Therefore, all patches adding functionality to sixel-tmux are welcome-
including cleanups, and backports of important tmux features.

Even the features that may affect the proper display of graphics and text are
welcome, as long as they are off by default, since obscure usecases like limited
bandwidth are now as rare as serial ports.

### History

Hayaki Saito <https://github.com/saitoha/tmux-SIXEL> wrote a patch and released
a tmux-sixel fork in 2015.  As tmux keep adding features, Chris Steinbach
<https://github.com/ChrisSteinbach/tmux-sixel> tried to integrate them in
tmux-sixel.  Patches were released by others to add to the version of tmux
officialy packaged by distributions to have tmux-sixel features, like Mehdi
Abaakouk <https://gist.github.com/sileht/8759089d5620da763bb456a0ae82e4d0>

Yatao Li <https://github.com/yatli/tmux> had a branch that was the closet to upstream
tmux, but the most recent changes dated from 2017 and some sixel isues remained.

When I tried them, none had full support for ANSI SGR parameters. In 2019, as
there was no recent activity on any of these, I forked the most recent branch,
fixed the remaining issues to have good sixel support at least in the current
buffer, added the missing SGR parameters I needed, and fixed issues brough by
the inclusion of unavoidable anti-features in mainline tmux.

By late 2019, given some general progress of few important issues, I rebased on tmux
main. Derasterize support was added on top of that in 2020.

After a year of hesitations on what to do, the new sixel-tmux code was released
in 2021, with the ambition of introducing more people to sixels without asking
them to change their terminal emulators, thanks to the derasterize fallback
mode.

