#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "fs_core.h"

/* ── valid_name ──────────────────────────────────── */
int valid_name(const char *name) {
    if (!name || name[0] == '\0') return 0;
    int len = 0;
    for (const char *p = name; *p; p++, len++) {
        if (*p == '/' || isspace((unsigned char)*p)) return 0;
    }
    return (len > 0 && len < MAX_NAME_LEN);
}

/* ── get_full_path ───────────────────────────────── */
/*  Walk from 'dir' to root, collecting names, then
    reverse them into a '/' separated string.          */
char *get_full_path(const DirEntry *dir, char *buf, int len) {
    if (!dir) { snprintf(buf, len, "?"); return buf; }

    /* collect segments from leaf to root */
    const char *segs[MAX_PATH_DEPTH];
    int depth = 0;
    const DirEntry *cur = dir;
    while (cur) {
        if (depth >= MAX_PATH_DEPTH) break;
        segs[depth++] = cur->name;
        cur = cur->parent;
    }

    /* root is segs[depth-1] which is "/" */
    buf[0] = '\0';
    int pos = 0;
    for (int i = depth - 1; i >= 0; i--) {
        /* root segment is "/" — handle leading slash */
        if (strcmp(segs[i], "/") == 0) {
            if (depth == 1) {
                /* we ARE root */
                snprintf(buf + pos, len - pos, "/");
                pos++;
            } else {
                /* root prefix already accounted for by leading '/' */
                /* we'll prepend '/' when we see parent == NULL */
            }
            continue;
        }
        /* add separator */
        if (pos == 0 || buf[pos-1] != '/') {
            snprintf(buf + pos, len - pos, "/");
            pos = (int)strlen(buf);
        }
        snprintf(buf + pos, len - pos, "%s", segs[i]);
        pos = (int)strlen(buf);
    }
    if (buf[0] == '\0') { buf[0] = '/'; buf[1] = '\0'; }
    return buf;
}

/* ── resolve_path ────────────────────────────────── */
DirEntry *resolve_path(const char *path) {
    if (!path || path[0] == '\0') return NULL;

    DirEntry *cur = (path[0] == '/') ? fs.root : fs.cwd;

    /* make a mutable copy */
    char tmp[1024];
    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    char *tok = strtok(tmp, "/");
    while (tok) {
        if (strcmp(tok, ".") == 0) {
            /* stay */
        } else if (strcmp(tok, "..") == 0) {
            if (cur->parent) cur = cur->parent;
            /* else stay at root */
        } else {
            /* find matching subdir */
            int found = 0;
            for (int i = 0; i < cur->subdir_count; i++) {
                if (strcmp(cur->subdirs[i]->name, tok) == 0) {
                    cur = cur->subdirs[i];
                    found = 1;
                    break;
                }
            }
            if (!found) return NULL;
        }
        tok = strtok(NULL, "/");
    }
    return cur;
}

/* ── vfs_error ───────────────────────────────────── */
void vfs_error(const char *msg) {
    fprintf(stderr, "error: %s\n", msg);
}
