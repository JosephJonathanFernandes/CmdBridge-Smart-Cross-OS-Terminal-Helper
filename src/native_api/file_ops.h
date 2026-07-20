#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "../intent.h"

// Cross-platform filesystem operations
int api_create_folder(const char *target);
int api_delete_folder(const char *target);
int api_copy_file(const char *source, const char *dest);
int api_move_file(const char *source, const char *dest);

// Utility for validation
int api_file_exists(const char *path);

// Central dispatcher
int execute_native_api(const Intent *intent);

#endif // FILE_OPS_H
