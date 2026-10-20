#ifndef COMMANDS_H
#define COMMANDS_H

/* Returns 1 when a command was handled, or 0 when it was not found.
 * The input line may be modified while parsing the command name.
 */
int commands_execute(char *line);

#endif
