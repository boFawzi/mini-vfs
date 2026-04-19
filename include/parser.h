#ifndef PARSER_H
#define PARSER_H

#include "fs_core.h"   /* for MAX_NAME_LEN */

#define MAX_ARGS 16
#define MAX_INPUT 1024

typedef struct {
    char  cmd[MAX_NAME_LEN];
    char *args[MAX_ARGS];   /* pointers into the static buffer */
    int   argc;
} ParsedCmd;

/* Parse a raw input line into a ParsedCmd.
   Returns 0 on success, -1 on empty/comment input. */
int parse_input(const char *line, ParsedCmd *out);

/* Free any heap memory in a ParsedCmd (none currently, but good practice). */
void free_parsed(ParsedCmd *p);

#endif /* PARSER_H */
