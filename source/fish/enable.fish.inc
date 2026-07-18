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
    set histui_status $status;

    commandline --replace -- (cat "$COMMANDFILE");

    if test $histui_status -eq 14;
        commandline --function execute;
    else;
        commandline --function repaint;
    end;
end;

bind \e\[A _histui_run; echo 'up arrow' &> /dev/null;
bind \cr   _histui_run; echo 'ctrl + r' &> /dev/null;
