/* iMate -- Copyright (C) 2024 Martin Newbound */

#ifndef I_MATE_H
#define I_MATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Starts the main engine loop. Reads commands line-by-line from stdin,
 * dispatches them to the appropriate handler, and writes responses to stdout.
 * Blocks until a "quit" command is recieved.
 */
void engine_loop(void);

#ifdef __cplusplus
}
#endif

#endif /* I_MATE_H */
