/**
 * iMate -- Copyright (C) 2024 Martin Newbound
 * 
 * @file IMate.h
 * @brief Entry point for the iMate chess engine.
 * 
 * @details
 * This file contains the entry point for the iMate chess engine.
 * The engine_loop function is called from the main function in main.c
 * and is responsible for running the main engine loop.
 * 
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

#ifndef I_MATE_H
#define I_MATE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Starts the main engine loop for the iMate chess engine.
 * 
 * @details
 * This function initiates the main loop of the iMate chess engine. It is responsible for managing the game flow,
 * processing input from the user or from a chess GUI, and sending the engine's moves to the user or the GUI.
 * 
 * The function continues to run until the game is over till an exit command is received.
 * 
 * @note this function interacts with standard input and output streams.
 * @warning this function is blocking and will not return until the game is over or an exit command is received.
 */
void engine_loop(void);

#ifdef __cplusplus
}
#endif

#endif // I_MATE_H