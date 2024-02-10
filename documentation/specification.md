# CRITICAL NOTES
+ flex might be total overkill as scanf might entirely suffice

# Used technologies
+ C style C++ with pottential future "back" porting
+ flex history parsers
+ SQLite
+ ncurses
+ readline

# Rationale
### C/C++
+ fuck pust
### NCurses
+ i know nothing better that is:
- portable
- reliable
- C compatible
- anyway better than ncurses
### SQLite
+ sharing history between shell types could be desirable, this is the simplest approach
+ having our own storage means that we could support meta informations globally
which are not standard for every application
### Flex
+ its *actually* reusable
### Readline
+ we are not trying to replace it in anyways; it only makes sense

# CLI

```
histui <verb>
<import> <file-1>:<file-2> [format]
<export> <file>            [format]
<tui>    <file>            [options]
<init>   <shell>
```

## Import
Read history entries from \<file-1\> into
histuidb \<file-2\>.

Unless all lines are pure entry values,
a format will have to be specified.

If no files are specified,
precomposed formats shall be printed.
E.g. format for "Bash with timestamps".

## Export
Dump the contents of histuidb \<file\>
to stdout using [format].

## Init
Used for printing reasonable default configuration
for using histui with the selected shell.

The default configuration,
if possible should have the following features:
+ override history search with invoking histui
+ sync further commands a histuidb

If no shell is specified,
the available options shall be printed.

# TUI

```
$ histui tui [options]
--command   : start in search mode
--normal    : start in normal mode
```

```
+---+---------------+
| p |               |
| o |   listing     |
| s |               |
+---+---------------+
|       ruler       |
+-------------------+
|       input       |
+-------------------+
```

#### Input

**<select>**

Echo entry value and exit with success.

**<inspect>**

Select an entry to load the context for.
Without filtering this is effectless,
however if the output was filtered,
the search results are overwritten
by the commands that were executed
in the same shell as this one.

**<clear>**

Clear any filtering or inspection.

**<pop>**

Pop result frame.

### Result stack
Everytime the user issues a search,
or inspects an item
the result stack is pushed.

The top search frame can/will be
overwritten.

The user may pop or clear by hand.

The result stack is NOT persistent between sessions.

## Format

```
%[flags][width]<specifier>
```

#### Flags

```
-   : left-justify within [width]
#  : relative (has effect on time specifiers)
```

#### Width
Number of characters to align to.

#### Specifiers
These specifiers root mainly from the C stdlib.

```
s   : history entry
n   : entry number
p   : shell id
a   : short weekday name
A   : full weekday name
u   : ISO 8601 decimal week day weekday (1-7)
b   : short month name
B   : full month name
C   : year (integer) devided by 100
d   : day of the month; 0 padded {"09"}
D   : short date ("MM/DD/YY")
H   : hour in 24h format {18}
I   : hour in 12h format {06}; 0 padded
r   : 12h time {12:38:21 PM}
R   : 24h time ("HH:MM")
S   : seconds; 0 padded {04}
M   : minutes; 0 padded {03}
T   : ISO 8601 time format ("HH:MM:SS")
```

# Environment
HISTUIFILE  : histuidb to be used
