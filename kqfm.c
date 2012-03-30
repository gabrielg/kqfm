#include <sys/types.h>
#include <sys/event.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <libgen.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <search.h>
#include <signal.h>

#ifdef O_EVTONLY
#define OPEN_MODE O_EVTONLY
#else
#define OPEN_MODE O_RDONLY
#endif

sig_atomic_t signal_caught = 0;

const char *program_name;

const struct option longopts[] = {
    {"help", no_argument, NULL, 'h'},
    {NULL,   0,           NULL, 0}
};

struct path_entry {
    void *next;
    void *prev;
    char *path;
};

struct path_entry *paths_tail = NULL;

struct {
    int flag;
    char *name;
} flag_descs[] = {
    { NOTE_DELETE, "DELETE" },
    { NOTE_WRITE,  "WRITE" },
    { NOTE_EXTEND, "EXTEND" },
    { NOTE_ATTRIB, "ATTRIB" },
    { NOTE_LINK,   "LINK" },
    { NOTE_RENAME, "RENAME" },
    { NOTE_REVOKE, "REVOKE" },
    { 0, 0 }
};


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
    int fd = open(path, OPEN_MODE);

    if (fd == -1) { err(errno, "couldn't open %s", path); }

    EV_SET(&k_fchange, fd, EVFILT_VNODE, EV_ADD|EV_CLEAR, UINT32_MAX, 0, path);

    if (kevent(kq, &k_fchange, 1, NULL, 0, NULL) == -1) {
        err(1, "couldn't monitor %s", path);
    }
}

void register_paths(int kq, FILE *in, int bytes_available, int eof_signaled)
{
    char *line;
    size_t len, bytes_read = 0;
    int pathlen;
    static struct path_entry *paths_head;
    struct path_entry *new_path;

    while ((bytes_read < bytes_available) || (eof_signaled && !feof(in))) {
        if ((line = fgetln(in, &len)) == NULL) {
            if (feof(in)) {
                break;
            } else {
                err(1, "couldn't read input");
            }
        }

        bytes_read += len;

        if (!(new_path = malloc(sizeof(struct path_entry)))) {
            err(1, "couldn't allocate memory for path entry");
        }

        pathlen = len + (line[len - 1] == '\n' ? -1 : 0);
        if (!(new_path->path = malloc(pathlen + 1))) {
            err(1, "couldn't allocate memory for path");
        }

        strncpy(new_path->path, line, pathlen);
        new_path->path[pathlen] = '\0';

        insque(new_path, paths_head);

        if (!paths_tail) { paths_tail = new_path; }
        paths_head = new_path;

        register_path(kq, new_path->path);
    }
}

void change_flags_to_msg(int flags, char **buf)
{
    int flagged = 0, i;
    size_t buflen;

    for (i = 0; flag_descs[i].flag; i++) {
        if (flag_descs[i].flag & flags) {
            buflen = strlen(*buf) + strlen(flag_descs[i].name) + flagged + 1;
            *buf = realloc(*buf, buflen);
            if (flagged) { strcat(*buf, ","); }
            strcat(*buf, flag_descs[i].name);
            flagged = 1;
        }
    }
}

void handle_event(struct kevent event, FILE * out)
{
    char *changes = malloc(1);
    changes[0] = '\0';
    change_flags_to_msg(event.fflags, &changes);
    fprintf(out, "%s\t%s\n", (char *)event.udata, changes);
    free(changes);
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
        signal_caught = 0;
        if (kevent(kq, NULL, 0, &k_event, 1, NULL) == -1) {
            if (errno != EINTR) { err(1, "error checking kqueue"); }
        }

        if (signal_caught) { continue; }

        if (k_event.ident == in_fno && k_event.filter == EVFILT_READ) {
            register_paths(kq, in, k_event.data, k_event.flags & EV_EOF);
        } else {
            handle_event(k_event, out);
        }
    }
}

void dump_paths(int sig)
{
    struct path_entry *p_entry = paths_tail;
    signal_caught = 1;

    if (!p_entry) { return; }

    do {
        fprintf(stderr, "%s\n", p_entry->path);
        p_entry = p_entry->next;
    } while (p_entry);
}

int main(int argc, char *argv[])
{
    program_name = basename(argv[0]);
    parse_options(argc, argv);
    signal(SIGUSR1, dump_paths);
    watcher_loop(stdin, stdout);
    return 1;
}