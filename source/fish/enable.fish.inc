function _histui_run;
    set COMMANDFILE "$XDG_CACHE_HOME/histui_command.txt";

    if not set -q HISTUICMD;
        set -f HISTUICMD "histui" "tui";
    end;

    if not set -q HISTFILE;
        set HISTFILE "$HOME/.local/share/fish/fish_history";
    end;

    set INPUT (commandline);
    commandline --replace "";

    env HISTFILE=$HISTFILE $HISTUICMD --input "$INPUT" 3> $COMMANDFILE;

    commandline --replace -- (cat "$COMMANDFILE");
    commandline --function repaint;
end;

bind \e\[A _histui_run;
bind \cr   _histui_run;
