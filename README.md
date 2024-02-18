# Head over Heels

The free and open source remake of the Jon Ritman and Bernie Drummond’s game

## The Story

This repository originated with version 1.0.1 by *Jorge Rodríguez Santos* aka *helmantika*
which I found on http://www.headoverheels2.com/

*Jorge Rodríguez Santos* wrote :

> In summer 2001 I decided to achieve the goal of programming a remake of Head over Heels. The first stage arrived two years later, when I published a beta version of it. That experience was a really extraordinary one; above all, because of all the support I received from so many people that, in one way or another, collaborated to make possible that the project saw finally the light. Although the game credits are a good proof of it, I would specifically mention in these pages the participation, as the graphic designer, of Davit Masiá. Without his work, I could never complete my own one.

> Unfortunately, I couldn't develop that beta into an stable version because, for almost three years, I went through personal circumstances I prefer to forget. However, once I was able to recover my free time and feeling, as I felt, that I had a thorn in my side for not having finished the «remake», I decided to return to the project. Ignacio Pérez Gil had released the 0.3 version of his isometric engine Isomot, and I took it as the basis from which I reconsidered the work. In 2007, with most of the code already finished, I decided to reopen the forum.

> These last two years have been ones of hard work, but a very gratifying one for the same reason the first part was: the unconditional support from several persons that have always paid attention to the evolution of the game. I'd like to thank, from my heart, to Santiago Acha, Xavi Colomé, Paco Santoyo and Hendrik Bezuidenhout their support and the good moments they have given me.

I found it in the fall of 2016 and saw that the sources lack a (sup)port for Mac OS X at all, and in particular for OS X on PowerPC,
therefore I began this project on GitHub which eventually evolved into what it is now.

## The Present

Since then, I reworked almost every piece of the Jorge's code, dealed around many gotchas and added many new features as like
* animated movin'n‘binkin dudes in the menus
* transitions between the user interface menus
* the new black & white set of graphics
* ... with the speccy-like coloring of rooms
* the room entry tunes
* room miniatures
* the camera optionally follows the active character
* the support for various screen sizes not only 640 x 480
* the support for both the allegro 5 and allegro 4 libraries
* the many easy-to-enable cheats :O

However, this project went (and still continues to go) almost unnoticed.
Neither Jorge nor anyone else from the original team was ever participated in any further development and never wrote a single line about what's goin' on or was done here.
https://osgameclones.com/ was perhaps the only lone site on which a link to this repo ever appeared.
In the spring of 2019, I lost interest in this project and almost forgot about it.
François Bissy's ( @kiwifb ) contribution in the fall of 2023 seems to have awakened my care

And then it happened. *Finalmente* https://github.com/dougmencken/HeadOverHeels/commit/1f5cbbc4723150ccf5b74a1387330a126d0efeb9#commitcomment-131827424 Jorge came to this project. At the end of 2023 we were sharing various stories via email

## The Future

But yep, all this is still quite “raw” for now and really needs some more love.
Any playing, testing, issues found, spreading a word about this project somewhere, and other contributions are welcome.
Especially patches, these are **very much** welcome ;)

## Building

Since I’m currently using the GNU/Linux operating system, I’ll describe how to build and run this game on GNU/Linux with the “apt” package manager (Debian, Ubuntu and others)

For sure, you already know what the *console* aka *terminal* is, already did ``sudo apt-get install git`` for any deals with GitHub, and cloned the game’s repository somewhere on your local storage. For the case when there’s a completely fresh system in which nothing was compiled before, to get all the things needed for building at once, just type

```
sudo apt-get install build-essential binutils make cmake autoconf automake libtool
```

And yep, don’t forget ``libx11-dev`` for the “allegro” library (https://github.com/dougmencken/HeadOverHeels/issues/37#issuecomment-1743796681)

```
sudo apt install libx11-dev
```

Then building the “Head over Heels” game on GNU/Linux is pretty easy with the ``linux-build.sh`` script I provided, which also builds all the used dependences as well

```
./linux-build.sh
```

Unedited ``linux-build.sh`` script installs the game inside your build directory at *where-the-build-dir-is/_rootdir*, thus type

```
cd _rootdir
bin/headoverheels
```

just after finishing the build to get the game running! 😲🥹😌

## If something ’s wrong there

Don’t hear any music and sounds? Try

```
padsp bin/headoverheels
```

And if that doesn’t work, maybe it will the more complicated way

```
LD_PRELOAD=$(find /usr/lib/ -name libpulsedsp.so) bin/headoverheels --linux-audio=oss
```

Any other gotchas? Feel free to write and ask 😚
