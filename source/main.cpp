#include <locale.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include "cli.hpp"
#include "bash_history.yy.hpp"
#include "storage.hpp"
#include "tui.hpp"

bool do_run = true;
bool do_execute = false;

void init(void);
void deinit(void);

void init(void) {
    setlocale(LC_TIME, "C");
    init_storage();

    char * history_file_path = getenv("HISTFILE");
    if (!history_file_path) {
        fputs("$HISTFILE is not set, try exporting it.\n", stderr);
        deinit();
        exit(1);
    }

    bash_history_in = fopen(history_file_path, "r");
    if (!bash_history_in) {
        fputs("Failed to open history file.\n", stderr);
        deinit();
        exit(1);
    }

    bash_history_lex();

    init_tui();
}

void deinit(void) {
    deinit_tui();
    deinit_storage();
}

void export_result(const char * const result) {
    if (do_execute) {
        /* Inject the command and a newline to STDIN directly.
         * Some systems could theoretically be configured to disallow it.
         *  Not my problem.
         */
        for (size_t i = 0; i < strlen(result); i++) {
            if (ioctl(STDIN_FILENO, TIOCSTI, &result[i]) == -1) {
                perror("ioctl TIOCSTI");
            }
        }
        const char newline = '\n';
        ioctl(STDIN_FILENO, TIOCSTI, &newline);
    } else {
        /* Copy to a 3th pipe file descriptor which we can
         *  scoop up from a file and copy with READLINE_LINE.
         * XXX: if anyone knows a better method, please tell me
         */
        int fd[2];
        pipe(fd);
        dprintf(3, result);
        close(fd[0]);
        close(fd[1]);
    }
}

void * async(void * arg) {
    entry_t entry;

    while (true) {
        if (is_input_changed) {
            query(rl_line_buffer, entry_lines, selection_offset);
            is_input_changed = false;
        } else {
            requery();
        }
        while (entry = get_entry(), entry.command != NULL) {
            tui_append_back(entry);
        }
    }

    return NULL;
}

signed main(const int argc, const char * const * const argv) {
    extern void testtest(void);
    // NOTE: never returns on error
    parse_arguments(argc, argv);

    init();

    tui_refresh();

    pthread_t query_thread;
    pthread_create(&query_thread, NULL, async, NULL);
    while (do_run) {
        tui_take_input();
        if (is_input_changed) {
            testtest();
        }
        tui_refresh();
    }

    query(rl_line_buffer, 1, selection_offset + selection_relative);
    export_result(get_entry().command);

    deinit();

    return 0;
}
