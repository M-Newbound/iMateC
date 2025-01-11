/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"

void quit_command(const CommandParams params) {
    *params.engine_is_running = false;
}
