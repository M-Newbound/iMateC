/**
 * iMate -- Copyright (C) 2024 Martin Newbound
 * 
 * @file main.c
 * @version 1.0.0
 * @author Martin Newbound
 * @date 2024
 * 
 * @note
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "IMate.h"
#include <stdio.h>

int main() {
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
