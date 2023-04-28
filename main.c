#include "conio.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define white 0
#define black 1
#define defaultBoard "board_tester.txt"
#define debug 1

int getTeam(char testChar);
void loadBoard(char boardState[8][8], int *moveNumber, int *turn,
               char filetoread[]);
void saveBoard(char boardState[][8], int moveNumber, int turn, char filename[]);
void validateMove(char boardState[8][8], int turn, char inputMove[8],
                  char outputMove[]);
void printboard(char boardState[8][8]);
void copyBoard(char destBoard[8][8], char srcBoard[8][8]);
void flattenPieces(char inputPieces[8][8], char flattenedPieces[8][8]);
void makeMove(char boardState[8][8], char inputMove[8]);
int isInCheck(char boardState[8][8], int color);
int drctnKngChck(char boardState[8][8], int coord1, int coord2, int x_offset,
                 int y_offset, int movelimit, int color);
int hasValidMoves(char boardState[8][8], int color);
void printTitle(void);


int main(void) {
  int moveNumber = 1, turn = white; // 0 = white, 1 = black
  int gameState = -1;
  /*-1 = New Game, 0 = In Menu, 1 = In Progress, 2+ = Game Over*/
  char boardState[8][8] = {0}, doExit = 'n', selection,
       displayMessage[100] = {0};
  char inputmove[8], validmove[8] = {0}, boardToLoad[100];
  FILE *testptr;

  while (1) {
  menu:
    switch (gameState) {
    case -1:
    case 0:
      system("clear");
      printTitle();
      printf("\n%s\n\n", displayMessage);
      printf("Select an Option: \n");
      printf("1 - New Game\n");
      printf("2 - Continue Game\n");
      printf("3 - Save Game\n");
      printf("4 - Load Game\n");
      printf("5 - Exit\n\n");

      selection = getch();
      switch (selection) {
      case '1':
        loadBoard(boardState, &moveNumber, &turn, defaultBoard);
        gameState = 1;
        break;
      case '2':
        loadBoard(boardState, &moveNumber, &turn, "quicksave.txt");
        gameState = 1;
        break;
      case '3':
        if (gameState != -1) {
          printf("Name the Saved Game: ");
          scanf("%90s", boardToLoad);
          strcat(boardToLoad, "_save.txt");
          saveBoard(boardState, moveNumber, turn, boardToLoad);
        } else {
          strcpy(displayMessage,
                 "There is no Board to Save.\n Try Starting a New Game!");
        }
        break;
      case '4':
        printf("Name the Game to Load: ");
        scanf("%90s", boardToLoad);
        strcat(boardToLoad, "_save.txt");
        if ((testptr = fopen(boardToLoad, "r")) == NULL) {
          strcpy(displayMessage, "Save File was not Found.");
        } else {
          loadBoard(boardState, &moveNumber, &turn, boardToLoad);
          gameState = 1;
        }
        break;
      case '5':
        printf("Are you sure you want to exit TextChess? (Y to Confirm): ");
        scanf("%c", &doExit);
        if (doExit == 'Y')
          goto exit_program;
        break;
      case '\n':
        break;
      default:
        strcpy(displayMessage, "");
      } // Main Menu Options
      break;

    case 1:

      while (gameState == 1) { // Main Game Loop
        system("clear");
        // printf("%d",isInCheck(boardState,turn));
        if (debug) {
          printf("Processed Move: %s\n", validmove);
          printf("Is in Check?: %d\n", isInCheck(boardState, turn));
        }
        printf("Move #%d\n\n", moveNumber);
        printboard(boardState);
        if (turn == white) {
          printf("\nYour turn, white!\n");
        } else {
          printf("\nYour turn, black!\n");
        }

        do {
          printf("Enter a move: ");
          scanf("%7s", inputmove);
          if (strcmp(inputmove, "menu") == 0) {
            gameState = 0;
            saveBoard(boardState, moveNumber, turn, "quicksave.txt");
            goto menu;
          }
          validateMove(boardState, turn, inputmove, validmove);
          if (validmove[0] < 10) {
            if (validmove[0] == 3) {
              printf("You are in Check!\n\n");
            } else {
              printf("Invalid move detected!\n\n");
            }
          }
        } while (validmove[0] < 10);

        makeMove(boardState, validmove);
        if (turn == black)
          moveNumber++;
        turn = (turn + 1) % 2;

        saveBoard(boardState, moveNumber, turn, "quicksave.txt");
      }
      break;
    }
  }
exit_program:
  system("clear");
  printf("Thank you for Playing TextChess!\n\n");
  printf("EEL 2880 Final Project\n");
  printf("Florida International University\n\n");
  printf("Knight Ascii Art by Brent James Benton \n");
  printf("     (ASCII Art Archive)\n");
  printf("Text to ASCII Art Generator by patorjk\n");
  printf("     http://patorjk.com/");

  return 0;
}

void loadBoard(char boardState[8][8], int *moveNumber, int *turn,
               char filetoread[]) {
  int i, j;

  FILE *freader;

  if ((freader = fopen(filetoread, "r")) == NULL) {
    printf("No File present");

  } else {
    for (i = 0; i < 8; i++) {
      for (j = 0; j < 8; j++) {
        fscanf(freader, "%c%*c", &boardState[i][j]);
      }
    }
    fscanf(freader, "%*s%d%*s%d", moveNumber, turn);
  }
}

void printboard(char boardState[8][8]) {

  char piecesToPrint[8][8];
  flattenPieces(boardState, piecesToPrint);

  printf("+-------------------------------+\n");
  for (int i = 0; i < 8; i++) {

    for (int j = 0; j < 8; j++) {
      printf("| %c ", piecesToPrint[i][j]);
    }
    printf("| %d\n+---+---+---+---+---+---+---+---+\n", 8 - i);
  }

  for (int i = 65; i < 73; i++) {
    printf("  %c ", i);
  }
  printf("\n");
}

void printTitle(void) {
  printf(" _______  _______  __   __  _______   |\\_\n");
  printf("|       ||       ||  |_|  ||       | /  .\\_\n");
  printf("|_     _||    ___||       ||_     _||   ___)\n");
  printf("  |   |  |   |___ |       |  |   |  |    \\\n");
  printf("  |   |  |    ___| |     |   |   |  |  =  |\n");
  printf("  |   |  |   |___ |   _   |  |   |  /_____\\\n");
  printf("  |___|  |_______||__| |__|  |___| [_______]\n");
  printf(" _______  __   __  _______  _______  _______\n");
  printf("|       ||  | |  ||       ||       ||       |\n");
  printf("|       ||  |_|  ||    ___||  _____||  _____|\n");
  printf("|       ||       ||   |___ | |_____ | |_____ \n");
  printf("|   ____||       ||    ___||_____  ||_____  |\n");
  printf("|  |____||   _   ||   |___  _____| | _____| |\n");
  printf("|_______||__| |__||_______||_______||_______|\n");
}

void validateMove(char boardState[8][8], int turn, char inputMove[8],
                  char outputMove[]) {
  char tempBoard[8][8];
  int i, j, validStatus;
  copyBoard(tempBoard, boardState);

  makeMove(tempBoard, inputMove);
  if (isInCheck(tempBoard, turn)) {
    outputMove[0] = 3;
    return;
  }
  // TEMPORARY SOLUTION BEFORE VALIDATION ADDED
  strcpy(outputMove, inputMove);
}

void flattenPieces(char inputPieces[8][8], char flattenedPieces[8][8]) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      switch (inputPieces[i][j]) {
      case '6':
        flattenedPieces[i][j] = 'p';
        break;

      case '9':
        flattenedPieces[i][j] = 'P';
        break;

      case '+':
        flattenedPieces[i][j] = 'r';
        break;

      case '#':
        flattenedPieces[i][j] = 'R';
        break;

      case '_':
        flattenedPieces[i][j] = ' ';
        break;

      default:
        flattenedPieces[i][j] = inputPieces[i][j];
      }
    }
  }
}

void makeMove(char boardState[8][8], char inputMove[8]) {
  // Assumes Valid Move, makes move from modified UCI Standard Input

  /* Special Inputs (Checked For)
o-o-o = Black Castle Queenside
o-o = Black Bastle Kingside
O-O-O = White Castle Queenside
O-O = White Castle Kingside

XYxyep = Black takes with En Passant
XYxyEP = White takes with En Passant
XYxyz = Transform into Piece Z (For Pawns advancing)
XYxykm = Black King Move (Depower Black Rooks)
XYxyKM = White King Move (Depower White Rooks)

*/
  int coords_start[2] = {inputMove[0] - 97, 8 - (inputMove[1] - 48)};
  int coords_end[2] = {inputMove[2] - 97, 8 - (inputMove[3] - 48)};
  int i, flag;

  if (strcmp(inputMove, "o-o-o") == 0) {
    if (debug)
      printf("Castling Detected!");
    for (i = 0, flag = 0; i < 8; i++) {
      if ((boardState[0][i] == '+') && flag == 0) {
        boardState[0][i] = '_';
        flag++;
      }
      if (boardState[0][i] == 'k') {
        boardState[0][i] = '_';
      }
      if ((boardState[0][i] == '+') && flag == 1) {
        boardState[0][i] = 'r';
      }
    }
    boardState[0][2] = 'k';
    boardState[0][3] = 'r';
  } // Black Queenside Castling
  else if (strcmp(inputMove, "o-o") == 0) {
    if (debug)
      printf("Castling Detected!");
    for (i = 0, flag = 0; i < 8; i++) {
      if ((boardState[0][i] == '+') && flag == 0) {
        boardState[0][i] = 'r';
        flag++;
      }
      if (boardState[0][i] == 'k') {
        boardState[0][i] = '_';
      }
      if ((boardState[0][i] == '+') && flag == 1) {
        boardState[0][i] = '_';
      }
    }
    boardState[0][6] = 'k';
    boardState[0][5] = 'r';
  } // Black Kingside Castling
  else if (strcmp(inputMove, "O-O-O") == 0) {
    if (debug)
      printf("Castling Detected!");
    for (i = 0, flag = 0; i < 8; i++) {
      if ((boardState[7][i] == '#') && flag == 0) {
        boardState[7][i] = '_';
        flag++;
      }
      if (boardState[7][i] == 'K') {
        boardState[7][i] = '_';
      }
      if ((boardState[7][i] == '#') && flag == 1) {
        boardState[7][i] = 'R';
      }
    }
    boardState[7][2] = 'K';
    boardState[7][3] = 'R';
  } // White Queenside Castling
  else if (strcmp(inputMove, "O-O") == 0) {
    if (debug)
      printf("Castling Detected!");
    for (i = 0, flag = 0; i < 8; i++) {
      if ((boardState[7][i] == '#') && flag == 0) {
        boardState[7][i] = 'R';
        flag++;
      }
      if (boardState[7][i] == 'K') {
        boardState[7][i] = '_';
      }
      if ((boardState[7][i] == '#') && flag == 1) {
        boardState[7][i] = '_';
      }
    }
    boardState[7][6] = 'K';
    boardState[7][5] = 'R';
  } // White Kingside Castling

  else {

    if (inputMove[5] == 'm') {
      for (i = 0; i < 8; i++) {
        if (boardState[0][i] == '+')
          boardState[0][i] = 'r';
      }
    } // Black King Move
    if (inputMove[5] == 'M') {
      for (i = 0; i < 8; i++) {
        if (boardState[7][i] == '#')
          boardState[7][i] = 'R';
      }
    } // White King Move

    if ((inputMove[4] != NULL) && toupper(inputMove[4]) != 'E') {
      boardState[coords_end[1]][coords_end[0]] = inputMove[4];
    } // If Transform Piece Provided
    else {
      boardState[coords_end[1]][coords_end[0]] =
          boardState[coords_start[1]][coords_start[0]];
    }

    if (inputMove[4] == 'E') {
      boardState[coords_end[1] + 1][coords_end[0]] = '_';
    } // White En-Passant
    if (inputMove[4] == 'e') {
      boardState[coords_end[1] - 1][coords_end[0]] = '_';
    } // Black En-Passant

    boardState[coords_start[1]][coords_start[0]] = '_';
  } // Normal Piece Movement
}

void saveBoard(char boardState[][8], int moveNumber, int turn,
               char filename[]) {
  FILE *fpt;
  int j, i;
  fpt = fopen(filename, "w");
  for (j = 0; j < 8; j++) {
    for (i = 0; i < 7; i++) {
      fprintf(fpt, "%c ", boardState[j][i]);
    }
    fprintf(fpt, "%c\n", boardState[j][i]);
  }
  fprintf(fpt, "Move: %d\n", moveNumber);
  fprintf(fpt, "Turn: %d\n", turn);
  fprintf(fpt, "File name: %s", filename);
  fclose(fpt);
}

int isInCheck(char boardState[8][8], int color) {
  int i1, i2, j1, j2, k1, k2;
  char chChar;

  for (i1 = 0; i1 < 8; i1++) {
    for (i2 = 0; i2 < 8; i2++) {
      chChar = boardState[i1][i2];
      switch (chChar) {
      case '_':
        continue;
      case 'p':
      case '6':
        if (getTeam(chChar) == !color) {
          if (drctnKngChck(boardState, i2, i1, 1, 1, 1, color))
            return 1;
          if (drctnKngChck(boardState, i2, i1, -1, 1, 1, color))
            return 1;
        }
        break;
      case 'P':
      case '9':
        if (getTeam(chChar) == !color) {
          if (drctnKngChck(boardState, i2, i1, 1, -1, 1, color))
            return 1;
          if (drctnKngChck(boardState, i2, i1, -1, -1, 1, color))
            return 1;
        }
        break;
      case 'n':
      case 'N':
        if (getTeam(chChar) == !color) {
          if (drctnKngChck(boardState, i2, i1, 1, 2, 1, color))
            return 1;
          if (drctnKngChck(boardState, i2, i1, -1, 2, 1, color))
            return 1;
          if (drctnKngChck(boardState, i2, i1, 1, -2, 1, color))
            return 1;
          if (drctnKngChck(boardState, i2, i1, -1, -2, 1, color))
            return 1;
          if (drctnKngChck(boardState, i2, i1, 2, 1, 1, color))
            return 1;
          if (drctnKngChck(boardState, i2, i1, -2, 1, 1, color))
            return 1;
          if (drctnKngChck(boardState, i2, i1, 2, -1, 1, color))
            return 1;
          if (drctnKngChck(boardState, i2, i1, -2, -1, 1, color))
            return 1;
        }
        break;

      default:
        if (chChar == 'q' || chChar == 'r' || chChar == '+' || chChar == 'Q' ||
            chChar == 'R' || chChar == '#') {
          if (getTeam(chChar) == !color) {
            if (drctnKngChck(boardState, i2, i1, 1, 0, 8, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, -1, 0, 8, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, 0, 1, 8, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, 0, -1, 8, color))
              return 1;
          }
        }
        if (chChar == 'q' || chChar == 'b' || chChar == 'Q' || chChar == 'B') {
          if (getTeam(chChar) == !color) {
            if (drctnKngChck(boardState, i2, i1, 1, 1, 8, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, 1, -1, 8, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, -1, 1, 8, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, -1, -1, 8, color))
              return 1;
          }
        }
      }
    }
  }
  return 0;
}

int hasValidMoves(char boardState[8][8], int color) {
  int i1, i2, j1, j2;
  int checkFlag = 0;
  char inputMove[8], outputMove[8];

  for (i1 = 0; i1 < 8; i1++) {
    for (i2 = 0; i2 < 8; i2++) {
      for (j1 = 0; j1 < 8; j1++) {
        for (j2 = 0; j2 < 8; j2++) {

          inputMove[0] = 97 + i1;
          inputMove[1] = 49 + i2;
          inputMove[2] = 97 + j1;
          inputMove[3] = 49 + j2;
          inputMove[4] = '\0';

          validateMove(boardState, color, inputMove, outputMove);
          if (outputMove[0] > 10)
            return 1;
        }
      }
    }
  } // Runs through all possible move combos
    // runs through castling options (Only applies to Fischer Random)
  validateMove(boardState, color, "o-o-o", outputMove);
  if (outputMove[0] > 10) {
    return 1;
  }
  validateMove(boardState, color, "O-O-O", outputMove);
  if (outputMove[0] > 10) {
    return 1;
  }
  validateMove(boardState, color, "o-o", outputMove);
  if (outputMove[0] > 10) {
    return 1;
  }
  validateMove(boardState, color, "O-O", outputMove);
  if (outputMove[0] > 10) {
    return 1;
  }
  return 0;
}

void copyBoard(char destBoard[8][8], char srcBoard[8][8]) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      destBoard[i][j] = srcBoard[i][j];
    }
  }
}
int drctnKngChck(char boardState[8][8], int coord1, int coord2, int x_offset,
                 int y_offset, int movelimit, int color) {
  int i1 = coord2;
  int i2 = coord1;
  for (int i = 0; i < movelimit; i++) {
    i1 += y_offset;
    i2 += x_offset;
    if (i1 > 7 || i1 < 0 || i2 > 7 || i2 < 0)
      return 0;
    if (boardState[i1][i2] == 'k') {
      if (color == black) {
        return 1;
      } else {
        return 0;
      }
    }
    if (boardState[i1][i2] == 'K') {
      if (color == white) {
        return 1;
      } else {
        return 0;
      }
    }
    if (boardState[i1][i2] == '_')
      continue;
    return 0;
  }
  return 0;
}
int getTeam(char testChar) {
  switch (testChar) {
  case 'K':
  case 'B':
  case 'R':
  case 'Q':
  case 'N':
  case '#':
  case 'P':
  case '9':
    return white;
    break;
  case 'k':
  case 'b':
  case 'r':
  case 'q':
  case 'n':
  case '+':
  case 'p':
  case '6':
    return black;
  default:
    return -1;
  }
}