﻿#pragma once
#include "board.h"
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "ai.h"
#include "menu.h"
// Class for game board

struct Board* newBoard() {
    struct Board* board = malloc(sizeof(struct Board));
    memset(board->board, 0, sizeof(board->board));
    board->currentPlayer = 1;
    board->gameStatus    = 1;
    board->movesCount    = 0;
    board->currentCordY  = 0;
    board->currentCordX  = 0;
    return board;
}

int placePieceBoard(struct Board* board, int cordx, int cordy) {
    if (board->board[cordx][cordy] != 0) return 0;
    board->board[cordx][cordy]             = board->currentPlayer;
    board->moves[board->movesCount].cordx  = cordx;
    board->moves[board->movesCount].cordy  = cordy;
    board->moves[board->movesCount].player = board->currentPlayer;
    board->movesCount++;
    board->gameStatus    = checkStatusBoard(board);
    board->currentPlayer = 1 + (board->currentPlayer & 1);
    displayBoard(board, 1);
    return board->gameStatus;
}

struct CharPair encodeMove(struct Move move) {
    struct CharPair ret;
    int             value = (move.cordx * 16 + move.cordy) + (move.player - 1) * 300;
    ret.fst               = 'a' + value / 26;
    ret.snd               = 'a' + value % 26;
    return ret;
}

struct Move decodeMove(struct CharPair code) {
    struct Move ret;
    int         value = (code.fst - 'a') * 26 + (code.snd - 'a');
    ret.player        = value / 300 + 1;
    ret.cordx         = (value % 300) / 16;
    ret.cordy         = (value % 300) % 16;
    return ret;
}

// Remember to free the memory afterwards!!!!!!!!!
int saveReplayBoard(struct Board* board) {
    char code[512];
    memset(code, 0, sizeof(code));
    for (int index = 0; index < board->movesCount; index++) {
        struct CharPair encodedMove = encodeMove(board->moves[index]);
        code[index * 2]             = encodedMove.fst;
        code[index * 2 + 1]         = encodedMove.snd;
    }
    printf("\n%s\n", code);
    fflush(stdout);
    return 0;
}

int playReplayBoard(struct Board* board, char* replayData) {
    clearBoard(board);
    int length = strlen(replayData);
    for (int index = 0; index < length; index += 2) {
        struct CharPair code     = {replayData[index], replayData[index + 1]};
        struct Move     moveTemp = decodeMove(code);
        int             ret      = placePieceBoard(board, moveTemp.cordx, moveTemp.cordy);
        if (ret != 1 && ret != 2) return 1;
        Sleep(1000);
        displayBoard(board, 1);
    }
    return 0;
}
int startGameBoard(struct Board* board, int type) {
    char data[512];
    int  aiPlayer = 0;
    board->type   = type;
    if (type == 0) {
        printf("Are you going first?(y/n)");
        fflush(stdout);
        int op = 0;
        while (op != 'y' && op != 'n') op = _getch();
        aiPlayer = 1 + (op == 'y');
    }
    displayBoard(board, 0);
    while (1) {
        if (board->currentPlayer == aiPlayer && (board->gameStatus == 1 || board->gameStatus == 2)) {
            Sleep(500);
            struct Move move   = aiConsiderBoard(board, 0);
            int         status = placePieceBoard(board, move.cordx, move.cordy);
            if (status < 0) printf("Player %d wins!\n", -status);
            if (status == 3) printf("It's a draw.");
            fflush(stdout);
        } else {
            int op = _getch();
            switch (op) {
                case 13: // enter
                    if (board->gameStatus == 1 || board->gameStatus == 2) {
                        int status = placePieceBoard(board, board->currentCordX, board->currentCordY);
                        displayBoard(board, 1);
                        if (status == 0) printf("Invalid Move.\n");
                        if (status < 0) printf("Player %d wins!\n", -status);
                        if (status == 3) printf("It's a draw.");
                        if (status == 1 || status == 2) printf("Next move: Player %d\n", status);
                    } else {
                        displayBoard(board, 1);
                        printf("The Game has ended.\n");
                    }
                    fflush(stdout);
                    break;
                case 224: // arrows id
                    op = _getch();
                    switch (op) {
                        case 75:
                            board->currentCordX = board->currentCordX > 0 ? board->currentCordX - 1 : 0; // left
                            break;
                        case 72: // up
                            board->currentCordY = board->currentCordY > 0 ? board->currentCordY - 1 : 0;
                            break;
                        case 80: // down
                            board->currentCordY = board->currentCordY < 14 ? board->currentCordY + 1 : 14;
                            break;
                        case 77: // right
                            board->currentCordX = board->currentCordX < 14 ? board->currentCordX + 1 : 14;
                            break;
                    }
                    displayBoard(board, 0);
                    break;
                case 58: // :
                    printf("Waiting for the second key...\n");
                    fflush(stdout);
                    op = _getch();
                    switch (op) {
                        case 113: saveBoard(board); break; // q
                        case 119:                          // w
                            scanf_s("%s", data, 512);
                            loadBoard(board, data);
                            break;
                        case 97: saveReplayBoard(board); break; // a
                        case 115:                               // s
                            scanf_s("%s", data, 512);
                            playReplayBoard(board, data);
                            break;
                        case 114: aiConsiderBoard(board, 1); break;
                        case 117: undoBoard(board); break; // u
                        case 122: return 0;                // z
                        default: displayBoard(board, 0);
                    }
                    break;
            }
        }
    }
}

int clearBoard(struct Board* board) {
    int i;
    for (i = 0; i < board->movesCount; i++) {
        board->board[board->moves[i].cordx][board->moves[i].cordy] = 0;
        board->moves[i].cordx                                      = 0;
        board->moves[i].cordy                                      = 0;
        board->moves[i].player                                     = 0;
    }
	memset(board->board, 0, sizeof(board->board));
	board->currentPlayer = 1;
    board->gameStatus    = 1;
    board->movesCount    = 0;
    board->currentCordY  = 0;
    board->currentCordX  = 0;
    return 0;
}
int undoBoard(struct Board* board) {
    if (board->movesCount == 0) {
    } else if (board->movesCount == 1) {
        board->board[board->moves[board->movesCount - 1].cordx][board->moves[board->movesCount - 1].cordy] = 0;
        board->moves[board->movesCount - 1].cordx                                                          = 0;
        board->moves[board->movesCount - 1].cordy                                                          = 0;
        board->moves[board->movesCount - 1].player                                                         = 0;
        board->currentPlayer                                                                               = 1;
        board->movesCount                                                                                  = 0;
    } else {
        board->board[board->moves[board->movesCount - 1].cordx][board->moves[board->movesCount - 1].cordy] = 0;
        board->board[board->moves[board->movesCount - 2].cordx][board->moves[board->movesCount - 2].cordy] = 0;
        board->moves[board->movesCount - 1].cordx                                                          = 0;
        board->moves[board->movesCount - 1].cordy                                                          = 0;
        board->moves[board->movesCount - 1].player                                                         = 0;
        board->moves[board->movesCount - 2].cordx                                                          = 0;
        board->moves[board->movesCount - 2].cordy                                                          = 0;
        board->moves[board->movesCount - 2].player                                                         = 0;
        board->movesCount -= 2;
    }
    displayBoard(board, 1);
}

int displayBoard(struct Board* board, int type) {
    clear();
    const int LT = 0, TOP = 1, RT = 2;
    const int LEFT = 3, CENTER = 4, RIGHT = 5;
    const int LF = 6, FOOT = 7, RF = 8;
    const int SPACE = 9;
    const int BLACK = 10, WHITE = 11, POS = 12;
    const int DIMENSION = 15;
    int       initialboard[15][15];
    for (int k = 0; k < DIMENSION; k++)
        for (int j = 0; j < DIMENSION; j++) initialboard[j][k] = SPACE;

    initialboard[0][0]                         = LT;
    initialboard[0][DIMENSION - 1]             = RT;
    initialboard[DIMENSION - 1][0]             = LF;
    initialboard[DIMENSION - 1][DIMENSION - 1] = RF;
    for (int i = 1; i < DIMENSION - 1; i += 1) {
        initialboard[0][i]             = TOP;
        initialboard[DIMENSION - 1][i] = FOOT;
        initialboard[i][0]             = LEFT;
        initialboard[i][DIMENSION - 1] = RIGHT;
    }
    for (int j = 1; j < DIMENSION - 1; j += 1) {
        for (int k = 1; k < DIMENSION - 1; k += 1) { initialboard[j][k] = CENTER; }
    }
    for (int j = 0; j < DIMENSION; j += 1) {
        for (int k = 0; k < DIMENSION; k += 1) {
            if (board->board[j][k] != 0) { initialboard[j][k] = board->board[j][k] + 9; }
        }
    }
    if (type == 0) initialboard[board->currentCordX][board->currentCordY] = POS;

    printf("\n");                         /// BUG 2 boards   no"reset"
    for (int k = 0; k < DIMENSION; k++) { /// columns
        for (int l = 0; l < DIMENSION; l++) {
            /// rows
            switch (initialboard[l][k]) {
                case 0: printf("┌ "); break;
                case 1: printf("├ "); break;
                case 2: printf("└ "); break;
                case 3: printf("┬ "); break;
                case 4: printf("┼ "); break;
                case 5: printf("┴ "); break;
                case 6: printf("┐ "); break;
                case 7: printf("┤ "); break;
                case 8: printf("┘ "); break;
                case 9: printf(""); break;
                case 10: printf("○"); break;
                case 11: printf("●"); break;
                case 12:
                    if (board->currentPlayer == 1) {
                        printf("①");
                    } else {
                        printf("②");
                    }
                    break;
            }
        }
        putchar('\n');
    }
    if (board->currentPlayer == 1)
        printf("    Player 1 turn\n");
    else if (board->currentPlayer == 2)
        printf("    Player 2 turn\n");
    printf("    POSITION(%d,%d)\n", board->currentCordX, board->currentCordY);
    printf("\n");
    printf("    \033[31;35m\":z\"  :Return to menu\n\033[0m");
    printf("    \033[31;35m\":u\"  :Retract a false move\n\033[0m");
    printf("    \033[31;35m\":q\"  :Save the current game\n\033[0m");
    printf("    \033[31;35m\":a\"  :Save the replay\n\033[0m");
    printf("    \033[31;35m\":s\"  :Play the replay video of the game\n\033[0m");
    printf("    \033[31;35m\":w\"  :Load saved games\n\033[0m");
    fflush(stdout);
    return 0;
}

int checkStatusBoard(struct Board* board) {
    const int DIMENSION = 15;
    if (board->movesCount == 225) return 3;
    int w = 1, x = 1, y = 1, z = 1, i; //累计四个方向的连续相同棋子数目
    for (i = 1; i < 5; i++) {
        if (board->moves[board->movesCount - 1].cordy + i < DIMENSION &&
            board->board[board->moves[board->movesCount - 1].cordx][board->moves[board->movesCount - 1].cordy + i] ==
                board->currentPlayer)
            w++;
        else
            break; //向下检查
    }
    for (i = 1; i < 5; i++) {
        if (board->moves[board->movesCount - 1].cordy - i >= 0 &&
            board->board[board->moves[board->movesCount - 1].cordx][board->moves[board->movesCount - 1].cordy - i] ==
                board->currentPlayer)
            w++;
        else
            break; //向上检查
    }
    if (w >= 5) return -board->currentPlayer; //若果达到5个则判断当前走子玩家为赢家

    for (i = 1; i < 5; i++) {
        if (board->moves[board->movesCount - 1].cordx + i < DIMENSION &&
            board->board[board->moves[board->movesCount - 1].cordx + i][board->moves[board->movesCount - 1].cordy] ==
                board->currentPlayer)
            x++;
        else
            break; //向右检查
    }
    for (i = 1; i < 5; i++) {
        if (board->moves[board->movesCount - 1].cordx - i >= 0 &&
            board->board[board->moves[board->movesCount - 1].cordx - i][board->moves[board->movesCount - 1].cordy] ==
                board->currentPlayer)
            x++;
        else
            break; //向左检查
    }
    if (x >= 5) return -board->currentPlayer; //若果达到5个则判断当前走子玩家为赢家

    for (i = 1; i < 5; i++) {
        if (board->moves[board->movesCount - 1].cordx + i < DIMENSION &&
            board->moves[board->movesCount - 1].cordy + i < DIMENSION &&
            board->board[board->moves[board->movesCount - 1].cordx + i]
                        [board->moves[board->movesCount - 1].cordy + i] == board->currentPlayer)
            y++;
        else
            break; //向右下检查
    }
    for (i = 1; i < 5; i++) {
        if (board->moves[board->movesCount - 1].cordx - i >= 0 && board->moves[board->movesCount - 1].cordy - i >= 0 &&
            board->board[board->moves[board->movesCount - 1].cordx - i]
                        [board->moves[board->movesCount - 1].cordy - i] == board->currentPlayer)
            y++;
        else
            break; //向左上检查
    }
    if (y >= 5) return -board->currentPlayer; //若果达到5个则判断当前走子玩家为赢家

    for (i = 1; i < 5; i++) {
        if (board->moves[board->movesCount - 1].cordx + i < DIMENSION &&
            board->moves[board->movesCount - 1].cordy - i >= 0 &&
            board->board[board->moves[board->movesCount - 1].cordx + i]
                        [board->moves[board->movesCount - 1].cordy - i] == board->currentPlayer)
            z++;
        else
            break; //向右上检查
    }
    for (i = 1; i < 5; i++) {
        if (board->moves[board->movesCount - 1].cordx - i >= 0 &&
            board->moves[board->movesCount - 1].cordy + i < DIMENSION &&
            board->board[board->moves[board->movesCount - 1].cordx - i]
                        [board->moves[board->movesCount - 1].cordy + i] == board->currentPlayer)
            z++;
        else
            break; //向左下检查
    }
    if (z >= 5) return -board->currentPlayer;
    // printf("%d %d %d %d\n", w, x, y, z);
    return 1 + (board->currentPlayer & 1);
}

int saveBoard(struct Board* board) {
    int  i = 0, j = 0, m = 15, k = 0, data[500], n;
    char saveData[500];
    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) { saveData[k++] = board->board[i][j] + '0'; }
    }
    saveData[225] = board->type + '0';
    saveData[226] = '\0';
    printf("\n%s\n", saveData);
    fflush(stdout);
    return 0;
}
int loadBoard(struct Board* board, char* saveData) {
    int i = 0, j = 0, m = 15, k = 0, a = 0, b = 0, n, c;
    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) board->board[i][j] = saveData[k++] - '0';
    }
    for (n = 0; n < 225; n++) {
        if (saveData[n] == 1)
            a++;
        else if (saveData[n] == 2)
            b++;
    }
    if (a == b)
        board->gameStatus = board->currentPlayer = 1;
    else
        board->gameStatus = board->currentPlayer = 2;
    board->type = saveData[225] - '0';
    displayBoard(board, 1);
    return 0;
}
