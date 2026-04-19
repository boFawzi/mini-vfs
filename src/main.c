#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs_core.h"
#include "parser.h"
#include "utils.h"

/* ── Print the shell prompt ─────────────────────── */
static void print_prompt(void) {
    char buf[MAX_PATH_DEPTH * MAX_NAME_LEN];
    printf("%s> ", get_full_path(fs.cwd, buf, sizeof(buf)));
    fflush(stdout);
}

/* ── Dispatch a parsed command ──────────────────── */
static void dispatch(ParsedCmd *cmd) {
    const char *c = cmd->cmd;

    /* ── pwd ─────────────────────────────────────── */
    if (strcmp(c, "pwd") == 0) {
        if (cmd->argc != 0) { vfs_error("usage: pwd"); return; }
        fs_pwd();

    /* ── ls ──────────────────────────────────────── */
    } else if (strcmp(c, "ls") == 0) {
        if (cmd->argc != 0) { vfs_error("usage: ls"); return; }
        fs_ls();

    /* ── mkdir ───────────────────────────────────── */
    } else if (strcmp(c, "mkdir") == 0) {
        if (cmd->argc != 1) { vfs_error("usage: mkdir <dirname>"); return; }
        fs_mkdir(cmd->args[0]);

    /* ── rmdir ───────────────────────────────────── */
    } else if (strcmp(c, "rmdir") == 0) {
        if (cmd->argc != 1) { vfs_error("usage: rmdir <dirname>"); return; }
        fs_rmdir(cmd->args[0]);

    /* ── cd ──────────────────────────────────────── */
    } else if (strcmp(c, "cd") == 0) {
        if (cmd->argc != 1) { vfs_error("usage: cd <path>"); return; }
        fs_cd(cmd->args[0]);

    /* ── touch ───────────────────────────────────── */
    } else if (strcmp(c, "touch") == 0) {
        if (cmd->argc != 1) { vfs_error("usage: touch <filename>"); return; }
        fs_touch(cmd->args[0]);

    /* ── write ───────────────────────────────────── */
    } else if (strcmp(c, "write") == 0) {
        /* write <filename> <text>
           write --append <filename> <text>  */
        int append = 0;
        int base   = 0;
        if (cmd->argc >= 1 && strcmp(cmd->args[0], "--append") == 0) {
            append = 1;
            base   = 1;
        }
        if (cmd->argc < base + 2) {
            vfs_error("usage: write [--append] <filename> <text>");
            return;
        }
        /* Join remaining args (after filename) as the text */
        char text[4096] = {0};
        for (int i = base + 1; i < cmd->argc; i++) {
            if (i > base + 1) strncat(text, " ", sizeof(text) - strlen(text) - 1);
            strncat(text, cmd->args[i], sizeof(text) - strlen(text) - 1);
        }
        fs_write(cmd->args[base], text, append);

    /* ── cat ─────────────────────────────────────── */
    } else if (strcmp(c, "cat") == 0) {
        if (cmd->argc != 1) { vfs_error("usage: cat <filename>"); return; }
        fs_cat(cmd->args[0]);

    /* ── rm ──────────────────────────────────────── */
    } else if (strcmp(c, "rm") == 0) {
        if (cmd->argc != 1) { vfs_error("usage: rm <filename>"); return; }
        fs_rm(cmd->args[0]);

    /* ── stat ────────────────────────────────────── */
    } else if (strcmp(c, "stat") == 0) {
        if (cmd->argc != 1) { vfs_error("usage: stat <name>"); return; }
        fs_stat(cmd->args[0]);

    /* ── help ────────────────────────────────────── */
    } else if (strcmp(c, "help") == 0) {
        printf("Available commands:\n");
        printf("  mkdir <dir>               Create a directory\n");
        printf("  rmdir <dir>               Remove an empty directory\n");
        printf("  cd <path>                 Change directory (supports .., /, relative)\n");
        printf("  pwd                       Print working directory\n");
        printf("  ls                        List directory contents\n");
        printf("  touch <file>              Create an empty file\n");
        printf("  write [--append] <f> <t>  Write text to file\n");
        printf("  cat <file>                Show file content\n");
        printf("  rm <file>                 Delete a file\n");
        printf("  stat <name>               Show file/dir metadata\n");
        printf("  exit                      Quit the simulator\n");

    /* ── unknown ─────────────────────────────────── */
    } else {
        fprintf(stderr, "error: command not recognized: '%s'  (type 'help' for list)\n", c);
    }
}

/* ── Main ───────────────────────────────────────── */
int main(void) {
    fs_init();

    printf("Mini Virtual File System Simulator\n");
    printf("Type 'help' for a list of commands, 'exit' to quit.\n\n");

    char line[MAX_INPUT];
    ParsedCmd cmd;

    while (1) {
        print_prompt();

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;    /* EOF (Ctrl-D) */
        }

        if (parse_input(line, &cmd) != 0) continue;   /* blank line */

        if (strcmp(cmd.cmd, "exit") == 0) {
            printf("Goodbye.\n");
            break;
        }

        dispatch(&cmd);
        free_parsed(&cmd);
    }

    fs_destroy();
    return 0;
}
