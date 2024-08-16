# Histui 1 manual

## DESCRIPTION
*Histui* is an interactive terminal interface for history entries.
It integrates with bash.
Do note that it does not replace the original history management.

## SYNOPSIS
**histui** [ *GLOBAL-OPTIONS* ] *VERB* [ *OPTIONS*+ ]

--version
: print version and quit

--help
: print help and quit

### enable
Dump a Bash script to stdout which is meant to be sourced from a bashrc.

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
*NOTE:* In the current implementation this

--caseless
: ignore letter case in searches.

--group
: group equal commands together, in effect only showing the most recent invocation.

## Controls

`UP`
`DOWN`
`CTRL + j`
`CTRL + K`
: scroll the entry cursor by one

`PAGEUP`
`PAGEDOWN`
`CTRL + u`
`CTRL + d`
: scroll the entry cursor by half a screen

`ENTER`
: select the entry under the cursor, making histui terminate and returning the entry

`CTRL + q`
: like ENTER, but not affected by --execute

`DEFAULT`
: edit the query line

[//]: # (@BAKE kramdown-man $@ @STOP)
