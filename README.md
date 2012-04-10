             _.-----._
           .'.-'''''-.'._                          
          //`.:#:'    `\\\                          __           ___           
         ;; '           ;;'.__.===============,    |  |--.-----.'  _|.--------.
         ||             ||  __                 )   |    <|  _  |   _||        |
         ;;             ;;.'  '==============='    |__|__|__   |__|  |__|__|__|
          \\           ///                                  |__|   
      jgs  ':.._____..:'~
             `'-----'`

# KQFM: A utility to monitor files for changes

[Homepage](https://github.com/gabrielg/kqfm)

`kqfm` takes a list of paths on `stdin` and monitors them for changes using `kqueue(1)`, outputting those changes to `stdout`.

## Dependencies

`kqfm` has no dependencies. It only works on platforms where `kqueue(1)` is available, such as Mac OS X, FreeBSD, NetBSD, OpenBSD, and DragonflyBSD.

## Install

Get the source and run `make install`. A Homebrew recipe for OS X is forthcoming.

## Examples

Watch a directory of `.md` files for writes, and output a listing when a file is written to.

	find doc -name '*.md' | kqfm -e write | while read path changes; do ls -la $path; done

Watch a single file and run a command when it changes in any way.

	echo "file.c" | kqfm | while read changes; do make; done

## Others

I wrote this to scratch my own itch, which was for something with no dependencies that operates with plain old text, and leaves things like executing commands to other programs.

[guard](https://github.com/guard/guard) and [wach](https://github.com/quackingduck/wach) both already exist, and may suit your needs better than this. Check them out too. 

## Author / Copyright

Gabriel Gironda, 2012. See LICENSE for license details.