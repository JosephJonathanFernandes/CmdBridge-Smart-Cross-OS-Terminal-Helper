#ifndef _WIN32

#include "file_ops.h"
#include "../logger.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

int api_file_exists(const char *path) {
    if (!path) return 0;
    return access(path, F_OK) == 0;
}

int api_create_folder(const char *target) {
    if (!target) return 0;
    // 0777 sets permissions (umask will apply)
    if (mkdir(target, 0777) == 0) {
        log_msg(LOG_INFO, "API: Successfully created folder %s", target);
        return 1;
    }
    log_msg(LOG_ERROR, "API: Failed to create folder %s", target);
    return 0;
}

int api_move_file(const char *source, const char *dest) {
    if (!source || !dest) return 0;
    
    // Naive implementation: handle rename if same filesystem, otherwise manual copy & delete
    if (rename(source, dest) == 0) {
        log_msg(LOG_INFO, "API: Successfully moved %s to %s", source, dest);
        return 1;
    }
    
    // Fallback: cross-device move
    if (api_copy_file(source, dest)) {
        if (unlink(source) == 0) {
            log_msg(LOG_INFO, "API: Successfully moved across devices %s to %s", source, dest);
            return 1;
        }
    }
    
    log_msg(LOG_ERROR, "API: Failed to move %s to %s", source, dest);
    return 0;
}

int api_copy_file(const char *source, const char *dest) {
    if (!source || !dest) return 0;

    FILE *src = fopen(source, "rb");
    if (!src) {
        log_msg(LOG_ERROR, "API: Copy failed to open source %s", source);
        return 0;
    }

    FILE *dst = fopen(dest, "wb");
    if (!dst) {
        fclose(src);
        log_msg(LOG_ERROR, "API: Copy failed to open destination %s", dest);
        return 0;
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            fclose(src);
            fclose(dst);
            log_msg(LOG_ERROR, "API: Copy failed during write to %s", dest);
            return 0;
        }
    }

    fclose(src);
    fclose(dst);
    log_msg(LOG_INFO, "API: Successfully copied %s to %s", source, dest);
    return 1;
}

// Recursive delete helper
static int remove_dir_recursive(const char *path) {
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;
        r = 0;
        while (!r && (p=readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;
            
            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);
            if (buf) {
                struct stat statbuf;
                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode)) {
                        r2 = remove_dir_recursive(buf);
                    } else {
                        r2 = unlink(buf);
                        if (r2 == 0) r2 = 0; else r2 = -1;
                    }
                }
                free(buf);
            }
            if (r2 != 0) {
                r = -1;
                break;
            }
        }
        closedir(d);
    }

    if (r == 0) {
        r = rmdir(path);
        if (r == 0) r = 0; else r = -1;
    }
    return r == 0; 
}

int api_delete_folder(const char *target) {
    if (!target) return 0;
    
    struct stat statbuf;
    if (stat(target, &statbuf) != 0) return 0;
    
    if (S_ISDIR(statbuf.st_mode)) {
        if (remove_dir_recursive(target)) {
            log_msg(LOG_INFO, "API: Successfully deleted folder %s", target);
            return 1;
        }
    } else {
        if (unlink(target) == 0) {
            log_msg(LOG_INFO, "API: Successfully deleted file %s", target);
            return 1;
        }
    }
    
    log_msg(LOG_ERROR, "API: Failed to delete %s", target);
    return 0;
}

#endif // !_WIN32
