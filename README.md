# atrautorun
atrautorun - Utility to make autorunning ATR disk images

## USAGE

<pre>atrautorun.exe [options]</pre>

By omitting the options, you get online help.

## OPTIONS (mandatory)

`-i <filename>` input filename of source atr disk image

This option allows you to indicate the disk to be scanned, to make the executable self-starting.

`-o <filename>` output filename of modified atr disk image

This option allows you to indicate the disk where the modified disk has to be written to.

`-f <filename>` executable file to autorun

This option allows you to select the file to be elected as `AUTORUN.SYS`.

## OPTIONS

`-v`            make execution verbose

Activates the display of all essential information.