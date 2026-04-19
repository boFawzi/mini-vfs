#ifndef UTILS_H
#define UTILS_H

#include "fs_core.h"

/* Validate a name: no '/', no whitespace, length 1–63.
   Returns 1 if valid, 0 otherwise. */
int  valid_name(const char *name);

/* Walk the parent chain and write the full path into buf (size len).
   Returns buf. */
char *get_full_path(const DirEntry *dir, char *buf, int len);

/* Resolve an absolute or relative path string from cwd.
   Returns the target DirEntry*, or NULL if not found. */
DirEntry *resolve_path(const char *path);

/* Print an error message to stderr with a consistent prefix. */
void vfs_error(const char *msg);

#endif /* UTILS_H */
