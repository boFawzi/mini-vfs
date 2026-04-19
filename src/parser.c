#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

/* Tokenise a raw input line into a ParsedCmd.
   Supports:
     - simple whitespace-delimited tokens
     - double-quoted strings (spaces preserved inside quotes)
   Returns 0 on success, -1 if the line is empty/whitespace-only.
*/
int parse_input(const char *line, ParsedCmd *out) {
    if (!line || !out) return -1;
    memset(out, 0, sizeof(*out));

    static char buf[MAX_INPUT];
    strncpy(buf, line, MAX_INPUT - 1);
    buf[MAX_INPUT - 1] = '\0';

    /* strip trailing newline / carriage return */
    int len = (int)strlen(buf);
    while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r'))
        buf[--len] = '\0';

    /* iterate and tokenise */
    char *p = buf;
    int   argc = 0;
    char *tokens[MAX_ARGS + 1];   /* +1 for the command itself */

    while (*p) {
        /* skip whitespace */
        while (*p && isspace((unsigned char)*p)) p++;
        if (*p == '\0') break;

        if (*p == '"') {
            /* quoted token — skip opening quote */
            p++;
            tokens[argc++] = p;
            while (*p && *p != '"') p++;
            if (*p == '"') *p++ = '\0';    /* null-terminate, skip closing */
        } else {
            tokens[argc++] = p;
            while (*p && !isspace((unsigned char)*p)) p++;
            if (*p) *p++ = '\0';           /* null-terminate */
        }
        if (argc >= MAX_ARGS + 1) break;
    }

    if (argc == 0) return -1;   /* empty line */

    /* first token → command name */
    strncpy(out->cmd, tokens[0], MAX_NAME_LEN - 1);
    out->argc = argc - 1;
    for (int i = 0; i < out->argc; i++)
        out->args[i] = tokens[i + 1];

    return 0;
}

void free_parsed(ParsedCmd *p) {
    (void)p;    /* nothing heap-allocated currently */
}
