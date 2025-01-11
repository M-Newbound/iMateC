/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "IMate.h"
#include "Moves/MagicBitboards.h"
#include <stdio.h>

int main(void) {
    magic_init();
    printf(
        "______________________________________________________________________\n"
        "                                                                      \n"
        "              ,--. ,--.   ,--.            ,--.                        \n"
        "              `--' |   `.'   |  ,--,--. ,-'  '-.  ,---.               \n"
        "              ,--. |  |'.'|  | ' ,-.  | '-.  .-' | .-. :              \n"
        "              |  | |  |   |  | ' '-'  |   |  |   :   --.              \n"
        "              `--' `--'   `--'  `--`--'   `--'    `----'              \n"
        "                                                                      \n"
        "                  Copyright (C) 2024 Martin Newbound                  \n"
        "                                                                      \n"
        "                                                                      \n"
        " This program is free software: you can redistribute it and/or modify \n"
        " it under the terms of the GNU General Public License as published by \n"
        "  the Free Software Foundation, either version 3 of the License, or   \n"
        "                (at your option) any later version.                   \n"
        "                                                                      \n"
        "    This program is distributed in the hope that it will be useful,   \n"
        "    but WITHOUT ANY WARRANTY; without even the implied warranty of    \n"
        "     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the     \n"
        "             GNU General Public License for more details.             \n"
        "                                                                      \n"
        "  You should have received a copy of the GNU General Public License   \n"
        "  along with this program. If not, see https://www.gnu.org/licenses/  \n"
        "                                                                      \n"   
        "______________________________________________________________________\n"
    );
    
    engine_loop();
    return 0;
}
