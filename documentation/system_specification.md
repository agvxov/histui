# Used technologies
+ C style C++ with pottential future "back" porting
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
### Readline
+ we are not trying to replace it in anyways; it only makes sense

# Project structure

```
source/
    version
    main
    tui     : frontend
    db      : import/export, append functionality
    config  : parse and initialize configuration
```
