# Histui
> TUI for your Bash history

## Demo
![demo](documentation/histui_demo.gif)

Histui that simply provides a fast TUI for your history.
It does not provide favourites ([alternative solution](placeholder)),
it does not attempt to manage your dotfiles
 ([alternative solution](https://www.gnu.org/software/stow/))
and it sure as hell does not replace your history storage while continuously syncing to a server
 (no recommendations; please don't do that).
Histui fulfills its UNIX-y purpose and fuck-all else.

## Documentation
For usage details consult the [manual](documentation/histui.1.md).

## Installation
After copying the project and cd-ing to it's root,
```
make
```
should yield you a `histui` executable.
Compile errors on Linux are a symptom of missing requirements.

### Requirements to compile
+ C compiler
+ Bison parser generator
+ ncurses C library
+ sqlite3 C library
+ readline C library
+ (optional for man page) kramdown-man
