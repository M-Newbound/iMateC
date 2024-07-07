/* iMate -- Copyright (C) 2024 Martin Newbound */

#include "../Commands.h"
#include <stdio.h>

#define CMD_WIDTH 14
#define DESC_WIDTH 32

#define LENGTH_OF_COMMANDS sizeof(CMD_DESCRIPTIONS) / sizeof(CMD_DESCRIPTIONS[0])


/**
 * @brief A list of command descriptions.
 *
 * Each element is a 2-element array where the first element is the command and the second element is the description.
 */
const char* CMD_DESCRIPTIONS[][2] = {
    {"help",                                            "Shows this help message"},
    {"position startpos|<fen> [<move> <move>...]",      "Sets the state of the engine's internal game board"},
    {"go",                                              "Start searching for the best move"},
    {"print board",                                     "Print the current board state"},
    {"print moves <from_square>",                       "Print all possible moves from a square"},
    {"move <from_square> <to_square>",                  "Make a move on the board"},
    {"status",                                          "Prints the current status of the game"},
    {"quit",                                            "Quit the engine"}
};


/**
 * @brief Prints a separator line for the help message.
 *
 * @param cmd_width The width of the command column.
 * @param desc_width The width of the description column.
 */
void print_separator(int cmd_width, int desc_width) {
    printf("+");
    for (size_t i = 0; i < cmd_width; i++) printf("-");
    printf("+");
    for (size_t i = 0; i < desc_width; i++) printf("-");
    printf("+\n");
}


/**
 * @brief Executes the 'help' command.
 *
 * This function prints a list of all available commands along with their descriptions.
 * 
 * @param params The command parameters. This parameter is not used in this function.
 */
void help_command(const CommandParams params) {

    print_separator(CMD_WIDTH, DESC_WIDTH);
    printf("| Command%*s | Description%*s |\n", CMD_WIDTH - 7, "", DESC_WIDTH - 11, "");
    print_separator(CMD_WIDTH, DESC_WIDTH);

    for (size_t i = 0; i < LENGTH_OF_COMMANDS; i++) {
        printf("| %-14s | %-32s |\n", CMD_DESCRIPTIONS[i][0], CMD_DESCRIPTIONS[i][1]);
        print_separator(CMD_WIDTH, DESC_WIDTH);
    }
}
