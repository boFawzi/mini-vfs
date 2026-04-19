#ifndef FS_CORE_H
#define FS_CORE_H

#include <time.h>

/* ── Capacity limits ─────────────────────────────── */
#define MAX_FILES_PER_DIR  64
#define MAX_SUBDIRS_PER_DIR 32
#define MAX_NAME_LEN       64   /* 63 chars + '\0' */
#define MAX_PATH_DEPTH     16

/* ── File node ───────────────────────────────────── */
typedef struct FileEntry {
    char    name[MAX_NAME_LEN];
    char   *content;        /* heap-allocated, may be NULL */
    int     size;           /* bytes currently stored      */
    time_t  created_at;
} FileEntry;

/* ── Directory node ──────────────────────────────── */
typedef struct DirEntry {
    char    name[MAX_NAME_LEN];
    FileEntry  *files[MAX_FILES_PER_DIR];
    int         file_count;
    struct DirEntry *subdirs[MAX_SUBDIRS_PER_DIR];
    int         subdir_count;
    struct DirEntry *parent;    /* NULL for root */
} DirEntry;

/* ── Global FS context ───────────────────────────── */
typedef struct {
    DirEntry *root;
    DirEntry *cwd;
} FSContext;

extern FSContext fs;

/* ── Lifecycle ───────────────────────────────────── */
void fs_init(void);
void fs_destroy(void);

/* ── Directory commands ──────────────────────────── */
int  fs_mkdir(const char *name);
int  fs_rmdir(const char *name);
int  fs_cd(const char *path);
void fs_pwd(void);
void fs_ls(void);

/* ── File commands ───────────────────────────────── */
int  fs_touch(const char *name);
int  fs_write(const char *name, const char *text, int append);
int  fs_cat(const char *name);
int  fs_rm(const char *name);
int  fs_stat(const char *name);

#endif /* FS_CORE_H */
