# Bash history infodump

Bash stores its history at `$HISTFILE`.

It's reasonable to assume that we will only need to search `HISTSIZE`
number of entries

`shopt -s lithist` promises to store multiline commands with embeded newlines,
otherwise newlines are removed and the command is stored like that.
However, when a new session is initialized, bash totally forgets
which commands were the same 1 command with newlines

Time stamps configured with `HISTTIMEFORMAT`.
If its set bash will save command timestamps along with the commands
in your histfile as "comments".
Comments entered in bash are also stored as comments.
`HISTTIMEFORMAT` also only influences the output of the `history` builtin,
timestamps are always stored in POSIX format.

## tl;dr on how bash history parsing works in practice
+ `#<int>` is always interpreted as a POSIX timestamp
+ a timestamp applies to all commands bellow it
+ bash always reads timestamps
+ invalid timestamps are silently ignored as long as you dont query them,
in which case an error is printed

## Warcrimes
Yes, all the above means that .bash\_history is fucking cursed.
The bellow script RAPES bash in a number of ways:
```sh
# below me is not a comment
#1700000000
echo test
HISTTIMEFORMAT='%y/%m/%d %T: '
#1700000000
echo test
for i in *; do
    echo "o"
done
for i in *; do # harder daddy
echo "ah"
done
shopt -s lithist
for i in *; do
echo "ah"
done
for i in *; do
#1700000000
echo "ah"
done
```

+ comment on the second line is lost as it maches the regex of a time stamp
+ the 2nd line comment *should* insert a fake timestamp, which works
in a true interactive shell, but if you run this whole comment it itself
gets a forged timestamp made by bash
+ the 3th comment can never fake a timestamp because its always timestamped
+ the 1st for loop is stored as 1 line
+ the 2nd for loop is stored as 2 lines because newlines are always preserved in comments
+ the 3th for loop is 3 lines becaise of lithist, they share the timestamp
+ the 4th for loop has an always working forged timestamp, inserted in the middle
