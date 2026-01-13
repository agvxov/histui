function _histui_run() {
    COMMANDFILE="${XDG_CACHE_HOME}/histui_command.txt"
    if ! [ -v HISTUICMD ]; then
        HISTUICMD="histui tui"
    fi
    HISTFILE=$HISTFILE ${HISTUICMD} --input "${READLINE_LINE}" 3> "${COMMANDFILE}
    READLINE_LINE=$(cat "${COMMANDFILE}")
    READLINE_POINT=${#READLINE_LINE}\n"
}

bind -x '\"\\e[A\": _histui_run'
bind -x '\"\\C-r\": _histui_run'
