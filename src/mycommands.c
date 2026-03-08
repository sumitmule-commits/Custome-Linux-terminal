#include "my_shell.h"
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

// Helper to print permissions
void print_permissions(mode_t mode)
{
    printf((S_ISDIR(mode)) ? "d" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
    printf(" ");
}

// myls implementation
int command_myls(char** args)
{
    int show_hidden = 0;  // -a flag
    int long_format = 0;  // -l flag
    char* dir_path = "."; // default current directory

    // Parse flags
    for (int i = 1; args[i]; i++) {
        if (my_strcmp(args[i], "-a") == 0) {
            show_hidden = 1;
        } else if (my_strcmp(args[i], "-l") == 0) {
            long_format = 1;
        } else if (my_strcmp(args[i], "-la") == 0 || my_strcmp(args[i], "-al") == 0) {
            show_hidden = 1;
            long_format = 1;
        } else {
            dir_path = args[i]; // treat as directory path
        }
    }

    // Open directory
    DIR* dir = opendir(dir_path);
    if (!dir) {
        perror("myls");
        return 1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip hidden files unless -a flag
        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }

        if (long_format) {
            // Build full path for stat
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

            struct stat file_stat;
            if (stat(full_path, &file_stat) == -1) {
                perror("stat");
                continue;
            }

            // Print permissions
            print_permissions(file_stat.st_mode);

            // Print link count
            printf("%ld ", (long)file_stat.st_nlink);

            // Print owner
            struct passwd* pw = getpwuid(file_stat.st_uid);
            if (pw) printf("%s ", pw->pw_name);

            // Print group
            struct group* gr = getgrgid(file_stat.st_gid);
            if (gr) printf("%s ", gr->gr_name);

            // Print size
            printf("%6ld ", (long)file_stat.st_size);

            // Print time
            char time_buf[20];
            struct tm* tm_info = localtime(&file_stat.st_mtime);
            strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", tm_info);
            printf("%s ", time_buf);

            // Print filename
            printf("%s\n", entry->d_name);
        } else {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
    return 0;
}

// mygrep implementation
int command_mygrep(char** args)
{
    if (args[1] == NULL || args[2] == NULL) {
        printf("Usage: mygrep [OPTIONS] PATTERN FILE\n");
        printf("Options:\n");
        printf("  -n    Show line numbers\n");
        printf("  -c    Show count of matching lines\n");
        return 1;
    }

    int show_lines  = 0; // -n flag
    int count_only  = 0; // -c flag
    char* pattern   = NULL;
    char* filename  = NULL;

    // Parse flags
    int i = 1;
    while (args[i] && args[i][0] == '-') {
        if (my_strcmp(args[i], "-n") == 0) {
            show_lines = 1;
        } else if (my_strcmp(args[i], "-c") == 0) {
            count_only = 1;
        }
        i++;
    }
    pattern  = args[i++];
    filename = args[i];

    if (!pattern || !filename) {
        printf("Usage: mygrep [OPTIONS] PATTERN FILE\n");
        return 1;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("mygrep");
        return 1;
    }

    char line[4096];
    int line_num = 0;
    int match_count = 0;

    while (fgets(line, sizeof(line), file))
    {
        line_num++;
        // Search for pattern in line
        int found = 0;
        for (int j = 0; line[j]; j++) {
            int match = 1;
            for (int k = 0; pattern[k]; k++) {
                if (line[j + k] != pattern[k]) {
                    match = 0;
                    break;
                }
            }
            if (match) { found = 1; break; }
        }

        if (found) {
            match_count++;
            if (!count_only) {
                if (show_lines) printf("%d: %s", line_num, line);
                else printf("%s", line);
            }
        }
    }

    if (count_only) printf("%d\n", match_count);

    fclose(file);
    return 0;
}

// mycp implementation
int command_mycp(char** args)
{
    if (args[1] == NULL || args[2] == NULL) {
        printf("Usage: mycp [SOURCE] [DESTINATION]\n");
        return 1;
    }

    int verbose = 0;
    char* src   = NULL;
    char* dest  = NULL;

    int i = 1;
    while (args[i] && args[i][0] == '-') {
        if (my_strcmp(args[i], "-v") == 0) verbose = 1;
        i++;
    }
    src  = args[i++];
    dest = args[i];

    if (!src || !dest) {
        printf("Usage: mycp [SOURCE] [DESTINATION]\n");
        return 1;
    }

    FILE* source = fopen(src, "r");
    if (!source) { perror("mycp"); return 1; }

    FILE* destination = fopen(dest, "w");
    if (!destination) { perror("mycp"); fclose(source); return 1; }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, destination);
    }

    fclose(source);
    fclose(destination);

    if (verbose) printf("'%s' -> '%s'\n", src, dest);
    return 0;
}

// mymv implementation
int command_mymv(char** args)
{
    if (args[1] == NULL || args[2] == NULL) {
        printf("Usage: mymv [SOURCE] [DESTINATION]\n");
        return 1;
    }

    int verbose = 0;
    char* src   = NULL;
    char* dest  = NULL;

    int i = 1;
    while (args[i] && args[i][0] == '-') {
        if (my_strcmp(args[i], "-v") == 0) verbose = 1;
        i++;
    }
    src  = args[i++];
    dest = args[i];

    if (!src || !dest) {
        printf("Usage: mymv [SOURCE] [DESTINATION]\n");
        return 1;
    }

    // Try rename first (works on same filesystem)
    if (rename(src, dest) == 0) {
        if (verbose) printf("'%s' -> '%s'\n", src, dest);
        return 0;
    }

    // If rename fails, copy then delete
    char* cp_args[] = {"mycp", src, dest, NULL};
    command_mycp(cp_args);
    remove(src);

    if (verbose) printf("'%s' -> '%s'\n", src, dest);
    return 0;
}