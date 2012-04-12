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

Latest version: 1.0.0

[Homepage](https://github.com/gabrielg/kqfm)

`kqfm` takes a list of paths on `stdin` and monitors them for changes using `kqueue(1)`, outputting those changes to `stdout`.

## Dependencies

`kqfm` has no dependencies. It only works on platforms where `kqueue(1)` is available, such as Mac OS X, FreeBSD, NetBSD, OpenBSD, and DragonflyBSD.

## Install

### OS X via Homebrew

`brew install https://raw.github.com/gabrielg/kqfm/master/brew/kqfm.rb`

If you like `kqfm`, please consider submitting [the formula](https://github.com/gabrielg/kqfm/blob/master/brew/kqfm.rb) to [Homebrew](https://github.com/mxcl/homebrew/wiki/Formula-Cookbook).

I've already made the formula, but they frown on authors submitting their own work. I can't blame them.

### OS X and other supported platforms, manually

Get [the source](https://github.com/gabrielg/kqfm/tags) and run `make install`.

## Examples

Watch a directory of `.md` files for writes, and output a listing when a file is written to.

	find doc -name '*.md' | kqfm -e write | while read path changes; do ls -la $path; done

Watch a single file and run a command when it changes in any way.

	echo "file.c" | kqfm | while read changes; do make; done

[The man page](https://github.com/gabrielg/kqfm/blob/master/man/kqfm.md) contains some more usage information.

## Similar projects

[guard](https://github.com/guard/guard) and [wach](https://github.com/quackingduck/wach) both already exist, and may suit your needs better than this. Check them out too. 

I wrote this to scratch my own itch, which was for something with no dependencies that operates with plain old text, and leaves things like executing commands to other programs.

## Contributing

Patches appreciated. Fork, make changes, and send over a pull request.

[Ronn](https://github.com/rtomayko/ronn) is used to generate the man page. I suggest using it to preview your work if you're changing the man page, but please don't commit the generated output from `ronn`, just commit the original Markdown file.

## Author / Copyright

Gabriel Gironda, 2012. See LICENSE for license details.