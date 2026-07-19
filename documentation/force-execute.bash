#bind '"\C-r": accept-line'

# ---

#function bar() {
#    echo "--"
#}
#
#function foo() {
#    READLINE_LINE="echo force execute"
#}
#
#
#bind -x '"\C-k": foo'
#bind '"\C-r": "\C-k\n"'

# ---


function foo() {
    bind -x '"\C-k": bar'
    if [[ $DO_EXECUTE ]]; then
        READLINE_LINE="echo force execute"
        return
    fi

    SHADOW_READLINE_LINE="echo force execute"
}

function bar() {
    bind -x '"\C-k": foo'

    if [ -v SHADOW_READLINE_LINE ]; then
        READLINE_LINE="echo force execute"
        unset SHADOW_READLINE_LINE
    fi
}

bind -x '"\C-k": foo'
bind '"\C-r": "\C-k\n\C-k"'
