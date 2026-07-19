# Bash is a bit special and as such we must pursue her in strange ways to comply with us.
#
# Bash exposes no direct way to accept a line, unlike say Fish does with:
#  commandline --function execute
#
# She however uses readline.
#
# One problematic way to make readline obey would be to write a builtin plugin.
# This strategy is discussed in detail at 'documentation/my-bash-plugin'.
#
# Bash provides you with the builtin `bind` which is your intended way
#  to communicate with readline.
#
# `bind` does in fact expose all readline function including `accept-line`.
# However, you can only *bind* `accept-line` and there is no way
#  to add anything else to said bind, making the function useless,
#  not just to us, but in general.
#
# `bind` can simulate key presses, including newlines.
# This means that we can simulate another bound key combination,
#  which invokes the actual function we want,
#  modify the users input line
#  and have it be automatically be executed on return.
#
# Histui sometimes want to execute the command right away,
#  but other times it only wants to write the users input line.
# Yet we know that the newline is coming.
# The trick presented below is to mitigate the above.
function _histui_run() {
    bind -x '"\e[8697~": _histui_patch_up'

    COMMANDFILE="${XDG_CACHE_HOME}/histui_command.txt"
    if ! [ -v HISTUICMD ]; then
        HISTUICMD="histui tui"
    fi
    HISTFILE=$HISTFILE ${HISTUICMD} --input "${READLINE_LINE}" 3> "${COMMANDFILE}"
    HISTUI_STATUS=$?
    if [ $HISTUI_STATUS -eq 14 ]; then
        READLINE_LINE=$(cat "${COMMANDFILE}")
        READLINE_POINT=${#READLINE_LINE}
    else
        _HISTUI_SHADOW_READLINE_LINE=$(cat "${COMMANDFILE}")
    fi
}

function _histui_patch_up() {
    bind -x '"\e[8697~": _histui_run'

    if [ -v _HISTUI_SHADOW_READLINE_LINE ]; then
        READLINE_LINE=$_HISTUI_SHADOW_READLINE_LINE
        READLINE_POINT=${#READLINE_LINE}
        unset _HISTUI_SHADOW_READLINE_LINE
    fi
}

bind -x '"\e[8697~": _histui_run'   # meaningless escape code; if anyone enters this by accident, i owe them a beer
bind '"\e[A": "\e[8697~\n\e[8697~"' # up arrow
bind '"\C-r": "\e[8697~\n\e[8697~"' # ctrl + r
