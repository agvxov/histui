#include <time.h>
#include <string>
#include <vector>

struct Shell {
    size_t id;
    std::string name;
};

struct Entry {
    std::string line;
    time_t timestamp;
    Shell * shell;
};

extern const char * db_file;

extern void append_entry(const Entry * const entry);
extern std::vector<Entry> search(const char * const query);
extern std::vector<Entry> inspect(const Entry * const entry);

extern signed export_main(int argc, char * * argv);
extern signed import_main(int argc, char * * argv);

