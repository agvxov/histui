function _histui_run;
    set COMMANDFILE "$XDG_CACHE_HOME/histui_command.txt";

    if not set -q HISTUICMD;
        set -f HISTUICMD "histui" "tui";
    end;

    if not set -q HISTFILE;
        set HISTFILE "$HOME/.local/share/fish/fish_history";
    end;

    env HISTFILE=$HISTFILE $HISTUICMD --input (commandline) 3> $COMMANDFILE;

    commandline --replace -- (cat "$COMMANDFILE");
end;

bind \e\[A _histui_run;
bind \cr   _histui_run;
