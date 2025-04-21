#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#define COLOR_DIR   "\x1b[34m"
#define COLOR_EXEC  "\x1b[32m"
#define COLOR_RESET "\x1b[0m"

#define CHECK_ALLOC(ptr) \
    if (!(ptr)) { \
        perror("Memory allocation failed"); \
        exit(EXIT_FAILURE); \
    }

typedef struct {
    char *name;
    struct stat st;
} Entry;

void time_fm(time_t t, char *buf) {
    strftime(buf, 32, "%Y-%m-%d %H:%M:%S", localtime(&t));
}

void print_info(const char *path, Entry *e) {
    char full[1024];
    snprintf(full, sizeof(full), "%s/%s", path, e->name);

    const char *color = COLOR_RESET;
    if (S_ISDIR(e->st.st_mode)) color = COLOR_DIR;
    else if (e->st.st_mode & S_IXUSR) color = COLOR_EXEC;

    char time_buf[32];
    time_fm(e->st.st_atime, time_buf);

    printf("%s%s%s\n", color, e->name, COLOR_RESET);
    printf("Size: %ld bytes\n", e->st.st_size);
    printf("Accessed: %s\n\n", time_buf);
}

int filter(const struct dirent *d, int mode) {
    if (mode == 2 && d->d_name[0] == '.') return 0; // skip hidden
    if (mode == 3 && d->d_name[0] != '.') return 0; // only hidden
    return 1;
}

void show_file_info(const char *filepath) {
    struct stat st;
    if (stat(filepath, &st) == -1) {
        perror("stat failed");
        return;
    }

    char time_buf[32];
    time_fm(st.st_atime, time_buf);

    printf("File: %s\n", filepath);
    printf("Size: %ld bytes\n", st.st_size);
    printf("Accessed: %s\n", time_buf);
    printf("Permissions: %o\n\n", st.st_mode & 0777);
}

void copy_file(const char *src, const char *dst) {
    int in = open(src, O_RDONLY);
    if (in < 0) {
        perror("open src failed");
        return;
    }

    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) {
        perror("open dst failed");
        close(in);
        return;
    }

    char buf[4096];
    ssize_t n;
    while ((n = read(in, buf, sizeof(buf))) > 0) {
        if (write(out, buf, n) != n) {
            perror("write failed");
            break;
        }
    }
    close(in);
    close(out);
    printf("Copied '%s' to '%s'\n", src, dst);
}

void delete_file(const char *path) {
    if (remove(path) == 0) {
        printf("Deleted '%s'\n", path);
    } else {
        perror("delete failed");
    }
}

int main(int argc, char **argv) {
    int opt;
    int sort = 0, rev = 0, mode = 1;

    while ((opt = getopt(argc, argv, "sra123")) != -1) {
        switch (opt) {
            case 's': sort = 1; break;
            case 'r': rev = 1; break;
            case '1': mode = 1; break;
            case '2': mode = 2; break;
            case '3': mode = 3; break;
            case 'a':
                printf("Author: Anton\nGroup: FEI STU IKT\nYear: 2025\n\n");
                break;
            default:
                fprintf(stderr, "Usage: %s [-s] [-r] [-1|2|3] [-a] [dir]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *dir = (optind < argc ? argv[optind] : ".");

    while (1) {
        DIR *dp = opendir(dir);
        if (!dp) {
            fprintf(stderr, "Error opening directory '%s': %s\n", dir, strerror(errno));
            exit(EXIT_FAILURE);
        }

        struct dirent *d;
        Entry *list = (Entry *)malloc(32 * sizeof(Entry));
        CHECK_ALLOC(list);
        size_t count = 0, cap = 32;

        while ((d = readdir(dp)) != NULL) {
            if (!filter(d, mode)) continue;

            if (count == cap) {
                cap *= 2;
                Entry *tmp = (Entry *)realloc(list, cap * sizeof(Entry));
                CHECK_ALLOC(tmp);
                list = tmp;
            }

            list[count].name = strdup(d->d_name);
            CHECK_ALLOC(list[count].name);

            char full[1024];
            snprintf(full, sizeof(full), "%s/%s", dir, d->d_name);
            if (stat(full, &list[count].st) == -1) {
                perror("stat failed");
                free(list[count].name);
                continue;
            }
            count++;
        }
        closedir(dp);

        if (sort) {
            for (size_t i = 0; i < count - 1; ++i) {
                for (size_t j = i + 1; j < count; ++j) {
                    if ((rev && strcmp(list[i].name, list[j].name) < 0) ||
                        (!rev && strcmp(list[i].name, list[j].name) > 0)) {
                        Entry tmp = list[i];
                        list[i] = list[j];
                        list[j] = tmp;
                    }
                }
            }
        }

        for (size_t i = 0; i < count; ++i) {
            print_info(dir, &list[i]);
            free(list[i].name);
        }
        free(list);

        char command[256];
        printf("> ");
        if (!fgets(command, sizeof(command), stdin)) break;

        command[strcspn(command, "\n")] = '\0';
        if (strcmp(command, "exit") == 0) break;
        else if (strcmp(command, "list") == 0) continue;
        else if (strncmp(command, "info ", 5) == 0) {
            show_file_info(command + 5);
        } else if (strncmp(command, "delete ", 7) == 0) {
            delete_file(command + 7);
        } else if (strncmp(command, "copy ", 5) == 0) {
            char src[128], dst[128];
            if (sscanf(command + 5, "%127s %127s", src, dst) == 2) {
                copy_file(src, dst);
            } else {
                printf("Usage: copy <src> <dst>\n");
            }
        } else {
            printf("Unknown command\n");
        }
    }

    return 0;
}
