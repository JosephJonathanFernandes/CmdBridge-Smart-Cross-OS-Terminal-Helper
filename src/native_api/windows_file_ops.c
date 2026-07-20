#ifdef _WIN32

#include "file_ops.h"
#include "../logger.h"
#include <windows.h>
#include <stdio.h>

int api_file_exists(const char *path) {
    if (!path) return 0;
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES);
}

int api_create_folder(const char *target) {
    if (!target) return 0;
    if (CreateDirectoryA(target, NULL)) {
        log_msg(LOG_INFO, "API: Successfully created folder %s", target);
        return 1;
    }
    log_msg(LOG_ERROR, "API: Failed to create folder %s", target);
    return 0;
}

int api_move_file(const char *source, const char *dest) {
    if (!source || !dest) return 0;
    // MOVEFILE_COPY_ALLOWED allows moving across drives
    if (MoveFileExA(source, dest, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
        log_msg(LOG_INFO, "API: Successfully moved %s to %s", source, dest);
        return 1;
    }
    log_msg(LOG_ERROR, "API: Failed to move %s to %s", source, dest);
    return 0;
}

int api_copy_file(const char *source, const char *dest) {
    if (!source || !dest) return 0;
    if (CopyFileA(source, dest, FALSE)) { // FALSE = overwrite existing
        log_msg(LOG_INFO, "API: Successfully copied %s to %s", source, dest);
        return 1;
    }
    log_msg(LOG_ERROR, "API: Failed to copy %s to %s", source, dest);
    return 0;
}

static int remove_dir_recursive(const char *path) {
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*.*", path);

    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA(search_path, &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }

    int success = 1;
    do {
        if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0) {
            char sub_path[MAX_PATH];
            snprintf(sub_path, sizeof(sub_path), "%s\\%s", path, fd.cFileName);

            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (!remove_dir_recursive(sub_path)) {
                    success = 0;
                    break;
                }
            } else {
                if (!DeleteFileA(sub_path)) {
                    success = 0;
                    break;
                }
            }
        }
    } while (FindNextFileA(hFind, &fd));

    FindClose(hFind);

    if (success) {
        if (!RemoveDirectoryA(path)) {
            success = 0;
        }
    }
    return success;
}

int api_delete_folder(const char *target) {
    if (!target) return 0;
    
    DWORD attrs = GetFileAttributesA(target);
    if (attrs == INVALID_FILE_ATTRIBUTES) return 0;
    
    if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
        if (remove_dir_recursive(target)) {
            log_msg(LOG_INFO, "API: Successfully deleted folder %s", target);
            return 1;
        }
    } else {
        if (DeleteFileA(target)) {
            log_msg(LOG_INFO, "API: Successfully deleted file %s", target);
            return 1;
        }
    }
    
    log_msg(LOG_ERROR, "API: Failed to delete %s", target);
    return 0;
}

#endif // _WIN32
