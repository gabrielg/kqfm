#include <sys/types.h>
#include <sys/event.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <libgen.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <search.h>

const char *program_name;

const struct option longopts[] = {
    {"help", no_argument, NULL, 'h'},
    {NULL,   0,           NULL, 0}
};

struct path_entry {
    uintptr_t   *next;
    uintptr_t   *prev;
    char        *path;
};

struct path_entry *paths_head = NULL;

void print_usage(FILE * out)
{
    fprintf(out, "%s: takes newline delimited filenames to watch on stdin "
                    "and reports changes on stdout\n", program_name);
    fprintf(out, "usage: %s options \n", program_name);
    fprintf(out, "  -h  --help             Display this usage information.\n");
}

void parse_options(int argc, char *argv[])
{
    int optchar;

    while ((optchar = getopt_long(argc, argv, "h", longopts, NULL)) != -1) {
        switch(optchar) {
            case '?':
            case 'h':
                print_usage(stdout);
                exit(0);
        }
    }
}

void register_path(int kq, char *path)
{
    struct kevent k_fchange;
    int fd = open(path, O_EVTONLY);

    if (fd == -1) { err(errno, "couldn't open %s", path); }

    EV_SET(&k_fchange, fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, 
        NOTE_DELETE | NOTE_WRITE, 0, path);
    
    if (kevent(kq, &k_fchange, 1, NULL, 0, NULL) == -1) {
        err(1, "couldn't monitor %s", path);
    }
}

void register_paths(int kq, FILE * in)
{
    char *line;
    size_t len;
    int lineoffset;
    struct path_entry *new_path;

    while ((line = fgetln(in, &len)) != NULL)  {
        if (!(new_path = malloc(sizeof(struct path_entry)))) {
            err(1, "couldn't allocate memory for path entry");
        }

        lineoffset = line[len - 1] == '\n' ? -1 : 0;
        if (!(new_path->path = malloc(len + lineoffset + 1))) {
            err(1, "couldn't allocate memory for path");
        }
        strncpy(new_path->path, line, len + lineoffset);

        insque(new_path, paths_head);
        paths_head = new_path;

        register_path(kq, new_path->path);
    }
}

void handle_event(struct kevent event, FILE * out)
{
    // TODO: show what kind of changes happened
    fprintf(out, "changed: %s\n", (char *)event.udata);
}

void watcher_loop(FILE * in, FILE * out)
{
    struct kevent k_input;
    struct kevent k_event;
    int kq;
    int in_fno = fileno(in);

    kq = kqueue();
    if (kq == -1) { err(1, "couldn't get kqueue"); }

    EV_SET(&k_input, in_fno, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
    if (kevent(kq, &k_input, 1, NULL, 0, NULL) == -1) {
        err(1, "couldn't set input event");
    }

    while (1) {
        if (kevent(kq, NULL, 0, &k_event, 1, NULL) == -1) {
            err(1, "error checking kqueue");
        }

        if (k_event.ident == in_fno && k_event.filter == EVFILT_READ) {
            register_paths(kq, in);
        } else {
            handle_event(k_event, out);
        }
    }
}

int main(int argc, char *argv[])
{
    program_name = basename(argv[0]);
    parse_options(argc, argv);
    watcher_loop(stdin, stdout);
    return 1;
}