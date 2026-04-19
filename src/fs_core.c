#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fs_core.h"
#include "utils.h"

/* ── Global state ───────────────────────────────── */
FSContext fs;

/* ── Internal helpers ───────────────────────────── */

static DirEntry *alloc_dir(const char *name, DirEntry *parent) {
    DirEntry *d = calloc(1, sizeof(DirEntry));
    if (!d) { vfs_error("out of memory"); exit(1); }
    strncpy(d->name, name, MAX_NAME_LEN - 1);
    d->parent = parent;
    return d;
}

static void free_dir_tree(DirEntry *d) {
    if (!d) return;
    for (int i = 0; i < d->file_count; i++) {
        free(d->files[i]->content);
        free(d->files[i]);
    }
    for (int i = 0; i < d->subdir_count; i++)
        free_dir_tree(d->subdirs[i]);
    free(d);
}

static FileEntry *find_file(DirEntry *dir, const char *name) {
    for (int i = 0; i < dir->file_count; i++)
        if (strcmp(dir->files[i]->name, name) == 0)
            return dir->files[i];
    return NULL;
}

static DirEntry *find_subdir(DirEntry *dir, const char *name) {
    for (int i = 0; i < dir->subdir_count; i++)
        if (strcmp(dir->subdirs[i]->name, name) == 0)
            return dir->subdirs[i];
    return NULL;
}

/* ── Lifecycle ──────────────────────────────────── */

void fs_init(void) {
    fs.root = alloc_dir("/", NULL);
    fs.cwd  = fs.root;
}

void fs_destroy(void) {
    free_dir_tree(fs.root);
    fs.root = fs.cwd = NULL;
}

/* ── Directory commands ─────────────────────────── */

int fs_mkdir(const char *name) {
    if (!valid_name(name)) {
        vfs_error("invalid directory name");
        return -1;
    }
    if (find_subdir(fs.cwd, name)) {
        vfs_error("directory already exists");
        return -1;
    }
    if (find_file(fs.cwd, name)) {
        vfs_error("a file with that name already exists");
        return -1;
    }
    if (fs.cwd->subdir_count >= MAX_SUBDIRS_PER_DIR) {
        vfs_error("directory is full (max subdirectories reached)");
        return -1;
    }
    DirEntry *newdir = alloc_dir(name, fs.cwd);
    fs.cwd->subdirs[fs.cwd->subdir_count++] = newdir;
    return 0;
}

int fs_rmdir(const char *name) {
    if (!valid_name(name)) { vfs_error("invalid name"); return -1; }

    DirEntry *target = find_subdir(fs.cwd, name);
    if (!target) { vfs_error("no such directory"); return -1; }
    if (target->file_count > 0 || target->subdir_count > 0) {
        vfs_error("directory is not empty");
        return -1;
    }
    /* remove from parent's array */
    int idx = -1;
    for (int i = 0; i < fs.cwd->subdir_count; i++) {
        if (fs.cwd->subdirs[i] == target) { idx = i; break; }
    }
    for (int i = idx; i < fs.cwd->subdir_count - 1; i++)
        fs.cwd->subdirs[i] = fs.cwd->subdirs[i + 1];
    fs.cwd->subdirs[--fs.cwd->subdir_count] = NULL;

    free(target);
    return 0;
}

int fs_cd(const char *path) {
    if (!path || path[0] == '\0') { vfs_error("path required"); return -1; }

    DirEntry *target = resolve_path(path);
    if (!target) { vfs_error("no such directory"); return -1; }

    fs.cwd = target;
    return 0;
}

void fs_pwd(void) {
    char buf[MAX_PATH_DEPTH * MAX_NAME_LEN];
    printf("%s\n", get_full_path(fs.cwd, buf, sizeof(buf)));
}

void fs_ls(void) {
    int any = 0;
    /* list subdirectories first */
    for (int i = 0; i < fs.cwd->subdir_count; i++) {
        printf("[DIR]  %s\n", fs.cwd->subdirs[i]->name);
        any = 1;
    }
    /* then files */
    for (int i = 0; i < fs.cwd->file_count; i++) {
        printf("[FILE] %s  (%d bytes)\n",
               fs.cwd->files[i]->name,
               fs.cwd->files[i]->size);
        any = 1;
    }
    if (!any) printf("(empty)\n");
}

/* ── File commands ──────────────────────────────── */

int fs_touch(const char *name) {
    if (!valid_name(name)) { vfs_error("invalid file name"); return -1; }
    if (find_file(fs.cwd, name)) { vfs_error("file already exists"); return -1; }
    if (find_subdir(fs.cwd, name)) {
        vfs_error("a directory with that name exists");
        return -1;
    }
    if (fs.cwd->file_count >= MAX_FILES_PER_DIR) {
        vfs_error("directory is full (max files reached)");
        return -1;
    }
    FileEntry *f = calloc(1, sizeof(FileEntry));
    if (!f) { vfs_error("out of memory"); exit(1); }
    strncpy(f->name, name, MAX_NAME_LEN - 1);
    f->content    = NULL;
    f->size       = 0;
    f->created_at = time(NULL);
    fs.cwd->files[fs.cwd->file_count++] = f;
    return 0;
}

int fs_write(const char *name, const char *text, int append) {
    FileEntry *f = find_file(fs.cwd, name);
    if (!f) { vfs_error("no such file"); return -1; }

    int text_len = (int)strlen(text);

    if (append && f->content) {
        /* grow and append */
        int new_size = f->size + text_len + 1;   /* +1 for space separator */
        char *buf = realloc(f->content, new_size + 1);
        if (!buf) { vfs_error("out of memory"); exit(1); }
        f->content = buf;
        f->content[f->size] = ' ';
        memcpy(f->content + f->size + 1, text, text_len);
        f->size += text_len + 1;
        f->content[f->size] = '\0';
    } else {
        /* overwrite */
        free(f->content);
        f->content = malloc(text_len + 1);
        if (!f->content) { vfs_error("out of memory"); exit(1); }
        memcpy(f->content, text, text_len);
        f->content[text_len] = '\0';
        f->size = text_len;
    }
    return 0;
}

int fs_cat(const char *name) {
    FileEntry *f = find_file(fs.cwd, name);
    if (!f) { vfs_error("no such file"); return -1; }
    if (!f->content || f->size == 0)
        printf("(empty file)\n");
    else
        printf("%s\n", f->content);
    return 0;
}

int fs_rm(const char *name) {
    if (!valid_name(name)) { vfs_error("invalid name"); return -1; }
    int idx = -1;
    for (int i = 0; i < fs.cwd->file_count; i++) {
        if (strcmp(fs.cwd->files[i]->name, name) == 0) { idx = i; break; }
    }
    if (idx == -1) { vfs_error("no such file"); return -1; }

    free(fs.cwd->files[idx]->content);
    free(fs.cwd->files[idx]);

    for (int i = idx; i < fs.cwd->file_count - 1; i++)
        fs.cwd->files[i] = fs.cwd->files[i + 1];
    fs.cwd->files[--fs.cwd->file_count] = NULL;
    return 0;
}

int fs_stat(const char *name) {
    FileEntry *f = find_file(fs.cwd, name);
    if (!f) {
        /* maybe it's a directory? */
        DirEntry *d = find_subdir(fs.cwd, name);
        if (d) {
            printf("  Name   : %s\n", d->name);
            printf("  Type   : directory\n");
            printf("  Files  : %d\n", d->file_count);
            printf("  Subdirs: %d\n", d->subdir_count);
            return 0;
        }
        vfs_error("no such file or directory");
        return -1;
    }
    char timebuf[64];
    struct tm *tm_info = localtime(&f->created_at);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);

    printf("  Name   : %s\n", f->name);
    printf("  Type   : file\n");
    printf("  Size   : %d bytes\n", f->size);
    printf("  Created: %s\n", timebuf);
    return 0;
}
