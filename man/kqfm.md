kqfm(1) -- monitor a file or a set of files for changes
=======================================================

## SYNOPSIS

`kqfm`<br>
`kqfm` `-e` *event_flag* | `--event=`*event_flag*<br>

## DESCRIPTION

**kqfm** takes in a newline delimited set of paths on `stdin` and watches them for changes. When a change occurs (any by default, can be filtered with `-e` - see **EVENT FLAGS**), `kqfm` outputs a line describing the changes. The output line is tab delimited - the field in the first position is the path the change happened on, the second is a comma delimited string representing the change(s) that occurred.

## OPTIONS

  * `-e` *event_flag*, `--event=`*event_flag*:
	The event flag to watch. This option can be specified more than once, for example: 	`kqfm -e write -e attrib`

## EVENT FLAGS
  
Available flags are:

  * **DELETE**<br>
    The unlink() system call was called on the file referenced by the descriptor. 
  * **WRITE**<br>
	A write occurred on the file referenced by the descriptor.
  * **EXTEND**<br>
	The file referenced by the descriptor was extended.
  * **ATTRIB**<br>
	The file referenced by the descriptor had its attributes changed.
  * **LINK**<br>
	The link count on the file changed.
  * **RENAME**<br>
	The file referenced by the descriptor was renamed.
  * **REVOKE**<br>
	Access to the file was revoked via revoke(2) or the underlying fileystem was unmounted.

See `EVFILT_VNODE` under `kqueue(2)` for more details on these flags.

## EXAMPLES

Watch a single file for all changes:

	$ echo /tmp/example_path | kqfm &
	[1] 577
	$ touch /tmp/example_path 
	/tmp/example_path	ATTRIB

Watch a single file for writes only:

	$ echo /tmp/example_path | kqfm -e write &
	[1] 580
	$ echo "what" > /tmp/example_path 
	/tmp/example_path	WRITE

Watch all CSS files in a directory for writes:
	
	$ find . -name '*.css' | kqfm -e write
	
## BUGS

The formatting of this man page is fairly poor.

## AUTHOR

Gabriel Gironda <gabriel@gironda.org>

## SEE ALSO

kqueue(2), [kqfm home page](https://github.com/gabrielg/kqfm)