function _histui_run() {
    COMMANDFILE="${XDG_CACHE_HOME}/histui_command.txt"
    histui tui 3> "${COMMANDFILE}"
    READLINE_LINE=$(cat "${COMMANDFILE}")
    READLINE_POINT=${#READLINE_LINE}
}

bind -x '"\e[A": _histui_run'
bind -x '"\C-r": _histui_run'
