# Histui 1 manual

## DESCRIPTION
*Histui* is an interactive terminal interface for history entries.
It integrates with bash.
Do note that it does not replace the original history management.

## SYNOPSIS
**histui** [ *GLOBAL-OPTIONS* ] *VERB* [ *OPTIONS*+ ]

--version
: print version and quit.

--help
: print help and quit.

### enable
Dump a Bash script to stdout which is meant to be sourced from a bashrc.

It rebinds the default history keys, such as `UP` and `CTRL + r`.

### tui
Start the interactive interface.

--execute
: do not copy the selected entry to the Bash cli, but execute it upon selection.
*NOTE:* this requires TIOCSTI support from the terminal.

--fuzzy
: make the input be considered a fuzzy search.
Fuzzy searching should not be noticeably slower than regular searches,
but significantly, more convenient.
In contrast,
the default behaviour is to emulate the built in history search of Readline.
*NOTE:* In the current implementation this means that whitespace separated
substrings may appear in any order.

--caseless
: ignore letter case in searches.

--group
: group equal commands together, in effect only showing the most recent invocation.

## CONTROLS

UP
DOWN
CTRL + j
CTRL + K
: scroll the entry cursor by one.

PAGEUP
PAGEDOWN
CTRL + u
CTRL + d
: scroll the entry cursor by half a screen.

ENTER
: select the entry under the cursor, making histui terminate and returning the entry.

TAB
: like ENTER, but not affected by --execute.

DEFAULT
: edit the query line.

## VARIABLES

### HISTUICMD
The script output by enable will use this variable to invoke histui.
For example, if you define it in your bashrc before histui is enable as: `histui --caseless`;
`CRTL + r` will always be case insensitive.

### HISTFILE
As defined in the Bash manual.
This variable must be exported so histui can figure out where you keep your history file.

[//]: # (@BAKE kramdown-man $@ @STOP)
