#include "conio.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define white 0
#define black 1
#define no_color 2
#define defaultBoard "board_default.txt"
#define debug 0

// for charType command:
#define none 0
#define piece 1
#define file 2
#define rank 3

// for ValidateMove command
#define wrongFormat 1
#define ambiguousMove 2
#define impossibleMove 3
#define InCheck 4
#define wrongTeam 5
#define noPiece 6
#define invalidInput 7

#define x 0
#define y 1

//for GameStates
#define Stalemate 2
#define Whitemate 3
#define Blackmate 4
#define Draw 5

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
int drctnKngChck(char boardState[8][8], int coord_x, int coord_y, int x_offset,
                 int y_offset, int movelimit, int color);
int hasValidMoves(char boardState[8][8], int color);
void printTitle(void);
int charType(char testChar);
char deTeam(char testChar);
int matchesCoords(char boardState[8][8], int i1, int i2, int team,
                  int coords_end[2]);
int drctnCrdChck(char boardState[8][8], int coord_x, int coord_y, int x_offset,
                 int y_offset, int movelimit, int color, int coords[2]);

void printdraw(void);
void printstalemate(void);
void printcheckmate(void);

// Start of Main Function
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
        if((testptr=fopen("quicksave.txt","r"))!=NULL){
        loadBoard(boardState, &moveNumber, &turn, "quicksave.txt");}
        else{
          loadBoard(boardState, &moveNumber, &turn, defaultBoard);
            }
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
            fclose(testptr);
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
        if(!hasValidMoves(boardState,turn)){
          if (isInCheck(boardState,turn)){
            if(turn==black){
              gameState = Whitemate;
              goto menu;
            } else{
              gameState = Blackmate;
              goto menu;
            }
          }else{
            gameState = Stalemate;
            goto menu;
          }
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
          else if(strcmp(inputmove, "draw")==0){
            printf("Are you sure you want to draw? (Y to confirm)");
            scanf(" %c",&selection);
              if (selection=='Y'){
                gameState = Draw;
                strcpy(displayMessage,"by agreement");
                goto menu;
              }
            else{
              goto menu;
            }
          }
          validateMove(boardState, turn, inputmove, validmove);
          if (validmove[0] < 10) {
            switch (validmove[0]) {
            case wrongFormat:
              printf("This format is incorrect!\n\n");
              break;

            case ambiguousMove:
              printf("This command is ambiguous. Be more Specific!\n\n");
              break;

            case impossibleMove:
              printf("That piece cannot make that move!\n\n");
              break;

            case wrongTeam:
              printf("Move your own pieces!\n\n");
              break;

            case InCheck:
              printf("Your King is in Check!\n\n");
              break;

            case noPiece:
              printf("That's not a piece!\n\n");
              break;

            default:
              printf("That move is invalid!\n\n");
            } // Displays reason for invalid
          }
        } while (validmove[0] < 10);

        makeMove(boardState, validmove);
        if (turn == black)
          moveNumber++;
        turn = (turn + 1) % 2;

        saveBoard(boardState, moveNumber, turn, "quicksave.txt");
      }
      break;

      case Stalemate:
        system("clear");
        printstalemate();
        printf("\n This game reached STALEMATE on move %d .\n",moveNumber);
        
        printboard(boardState);

        printf("\n\nSelect an Option:\n\n");
        printf("1. Return to Main Menu\n");
        printf("2. Exit TextChess\n");

        while(1){
        selection = getch();
        switch(selection){
          case '1':
          gameState=0;
          goto menu;
          break;

          case '2':
          goto exit_program;
          break;

            }
          }
        break;

      case Blackmate:
      case Whitemate:
        system("clear");
        printcheckmate();
        if(gameState == Whitemate)
          printf("\nWhite delivered CHECKMATE on move %d !:\n",moveNumber);
        if(gameState ==Blackmate)
          printf("\nBlack delivered CHECKMATE on move %d !:\n",moveNumber-1);
        printboard(boardState);

        printf("\n\nSelect an Option:\n\n");
        printf("1. Return to Main Menu\n");
        printf("2. Exit TextChess\n");

        while(1){
        selection = getch();
        switch(selection){
          case '1':
          gameState=0;
          goto menu;
          break;

          case '2':
          goto exit_program;
          break;

            }
          }
        break;
      case Draw:
      system("clear");
      printdraw();
      printf("\nThis game DREW %s on move %d .\n",displayMessage,moveNumber);
        printf("\n\nSelect an Option:\n\n");
        printf("1. Return to Main Menu\n");
        printf("2. Exit TextChess\n");

        while(1){
        selection = getch();
        switch(selection){
          case '1':
          gameState=0;
          goto menu;
          break;

          case '2':
          goto exit_program;
          break;

            }
          }
      
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
// End of Main Function

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
  char movedPiece, transformPiece = none;
  int cmdLngth, strtX_Knwn = 0, strtY_Knwn = 0, numvalid = 0;
  int coords_start[2], coords_end[2];
  copyBoard(tempBoard, boardState);
int kingpos,rookpos;

  if(strcmp(inputMove, "O-O")==0){
    if(turn==black){
      int flag = 0;
      for(int i=7;i>=0;i--){
        if(flag==0){
          if(boardState[0][i]=='+'){
            flag++;
            tempBoard[0][i]='_';
            rookpos =i;
          }
        }
        if(flag==1){
          if(boardState[0][i]=='k'){
            tempBoard[0][i]='_';
            flag++;
            kingpos =i;
          }
        }
      }
      if(flag==2){
        for(int i=kingpos;i<rookpos;i++){
          if((tempBoard[0][i])!='_'){
            outputMove[0] = invalidInput;
            return;
          }
        }
      }
      else{
        outputMove[0]=invalidInput;
        return;
      }
      for(int i=kingpos;i<=6;i++){
        tempBoard[0][i]='k';
        if (isInCheck(tempBoard, black)){
          outputMove[0] = InCheck;
            return;
        }
      }
      strcpy(outputMove,"o-o");
      return;
      }
    if(turn==white){
      int flag = 0;
      for(int i=7;i>=0;i--){
        if(flag==0){
          if(boardState[7][i]=='#'){
            flag++;
            tempBoard[7][i]='_';
            rookpos =i;
          }
        }
        if(flag==1){
          if(boardState[7][i]=='K'){
            tempBoard[7][i]='_';
            flag++;
            kingpos =i;
          }
        }
      }
      if(flag==2){
        for(int i=kingpos;i<rookpos;i++){
          if((tempBoard[7][i])!='_'){
            outputMove[0] = invalidInput;
            return;
          }
        }
      }
      else{
        outputMove[0]=invalidInput;
        return;
      }
      for(int i=kingpos;i<=6;i++){
        tempBoard[7][i]='K';
        if (isInCheck(boardState, black)){
          outputMove[0] = InCheck;
            return;
        }
      }
      strcpy(outputMove,"O-O");
      return;
      }
    outputMove[0]=invalidInput;
    return;
    }
  if(strcmp(inputMove, "O-O-O")==0){
    if(turn==black){
      int flag = 0;
      for(int i=0;i<8;i++){
        if(flag==0){
          if(boardState[0][i]=='+'){
            flag++;
            tempBoard[0][i]='_';
            rookpos =i;
          }
        }
        if(flag==1){
          if(boardState[0][i]=='k'){
            tempBoard[0][i]='_';
            flag++;
            kingpos =i;
          }
        }
      }
      if(flag==2){
        for(int i=kingpos;i>rookpos;i--){
          if((tempBoard[0][i])!='_'){
            outputMove[0] = invalidInput;
            return;
          }
        }
      }else{
        outputMove[0]=invalidInput;
        return;
      }
      if(kingpos>=2){
        for(int i=kingpos;i>=2;i--){
          if (isInCheck(tempBoard, black)){
          outputMove[0] = InCheck;
            return;
        }
        }
      }
      if(kingpos<2){
        for(int i=kingpos;i<=2;i++){
          if (isInCheck(tempBoard, black)){
          outputMove[0] = InCheck;
            return;
          }
        }
      }
      strcpy(outputMove,"o-o-o");
      return;
    }
    if(turn==white){
      int flag = 0;
      for(int i=0;i<8;i++){
        if(flag==0){
          if(boardState[7][i]=='#'){
            flag++;
            tempBoard[7][i]='_';
            rookpos =i;
          }
        }
        if(flag==1){
          if(boardState[7][i]=='K'){
            tempBoard[7][i]='_';
            flag++;
            kingpos =i;
          }
        }
      }
      if(flag==2){
        for(int i=kingpos;i>rookpos;i--){
          if((tempBoard[7][i])!='_'){
            outputMove[0] = invalidInput;
            return;
          }
        }
      }else{
        outputMove[0]=invalidInput;
        return;
      }
      if(kingpos>=2){
        for(int i=kingpos;i>=2;i--){
          if (isInCheck(tempBoard, white)){
          outputMove[0] = InCheck;
            return;
        }
        }
      }
      if(kingpos<2){
        for(int i=kingpos;i<=2;i++){
          if (isInCheck(tempBoard, black)){
          outputMove[0] = InCheck;
            return;
          }
        }
      }
      strcpy(outputMove,"O-O-O");
      return;
    }
    outputMove[0]=invalidInput;
    return;
  }
    
  
  copyBoard(tempBoard, boardState);
  
  cmdLngth = strlen(inputMove);
  if (charType(inputMove[cmdLngth - 1]) == piece) {
    if (debug) {
      printf("Promotion Detected!");
      scanf("%*d");
    }
    transformPiece = inputMove[cmdLngth - 1];
    cmdLngth--;
  }
  coords_end[x] = inputMove[cmdLngth - 2] - 97;
  coords_end[y] = 8 - (inputMove[cmdLngth - 1] - 48);

  if ((charType(inputMove[cmdLngth - 1]) != rank) ||
      (charType(inputMove[cmdLngth - 2]) != file)) {
    outputMove[0] = wrongFormat;
    return;
  }

  if ((charType(inputMove[0]) != file) && (charType(inputMove[0]) != piece)) {
    outputMove[0] = wrongFormat;
    return;
  }
  if (cmdLngth == 2) {
    if (turn == white) {
      if (coords_end[y] != 4) {
        coords_start[y] = coords_end[y] + 1;
      } else if (boardState[6][coords_end[x]] == 'P') {
        coords_start[y] = 6;
        transformPiece = '9';
      } else {
        coords_start[y] = 5;
      }
    }
    if (turn == black) {
      if (coords_end[y] != 3) {
        coords_start[y] = coords_end[y] - 1;
      } else if (boardState[1][coords_end[x]] == 'p') {
        coords_start[y] = 1;
        transformPiece = '6';
      } else {
        coords_start[y] = 2;
      }
    }
    coords_start[x] = coords_end[x];
    movedPiece = 'P';
    strtX_Knwn = 1;
    strtY_Knwn = 1;
  }
  if (cmdLngth == 3) {
    if ((charType(inputMove[0])) == piece) {
      movedPiece = deTeam(inputMove[0]);
    } else if (charType(inputMove[0]) == file) {
      if (turn == black)
        coords_start[y] = coords_end[y] - 1;
      else
        coords_start[y] = coords_end[y] + 1;
      movedPiece = 'P';
      coords_start[x] = inputMove[0] - 97;
      strtX_Knwn = 1;
      strtY_Knwn = 1;
    } else {
      outputMove[0] = wrongFormat;
      return;
    }
  }
  if (cmdLngth == 4) {
    if (charType(inputMove[0]) == file) {
      if (charType(inputMove[1]) != rank) {
        outputMove[0] = wrongFormat;
        return;
      } else {
        coords_start[x] = inputMove[0] - 97;
        coords_start[y] = 8 - (inputMove[1] - 48);
        movedPiece = boardState[coords_start[y]][coords_start[x]];
        if (movedPiece == 'P') {
          if ((coords_start[y] - coords_end[y]) == 2)
            transformPiece = '9';
        }
        if (movedPiece == 'p') {
          if ((coords_start[y] - coords_end[y]) == -2)
            transformPiece = '6';
        }
        movedPiece = deTeam(movedPiece);
        if (charType(movedPiece) != piece) {
          outputMove[0] = noPiece;
          return;
        }
        strtX_Knwn = 1;
        strtY_Knwn = 1;
      }
    } else if (charType(inputMove[0]) == piece) {
      movedPiece = inputMove[0];
      if ((charType(inputMove[1]) == rank)) {
        coords_start[y] = 8 - (inputMove[1] - 48);
        strtY_Knwn = 1;
      } else if (charType(inputMove[1]) == file) {
        coords_start[x] = inputMove[1] - 97;
        strtX_Knwn = 1;
      } else {
        outputMove[0] = wrongFormat;
        return;
      }
    } else {
      outputMove[0] = wrongFormat;
      return;
    }
  }
  if (cmdLngth == 5) {
    if ((charType(inputMove[0]) != piece) || (charType(inputMove[1]) != file) ||
        (charType(inputMove[2]) != rank)) {
      outputMove[0] = wrongFormat;
      return;
    } else {
      movedPiece = inputMove[0];
      coords_start[x] = inputMove[1] - 97;
      coords_start[y] = 8 - (inputMove[2] - 48);
      strtX_Knwn = 1;
      strtY_Knwn = 1;
    }
  }
  if (cmdLngth > 5) {
    outputMove[0] = wrongFormat;
    return;
  }

  if (strtX_Knwn && strtY_Knwn) {
    if (movedPiece != deTeam(boardState[coords_start[y]][coords_start[x]])) {
      outputMove[0] = invalidInput;
      return;
    } else if (turn != getTeam(boardState[coords_start[y]][coords_start[x]])) {
      outputMove[0] = wrongTeam;
      return;
    }
  }

  if (strtX_Knwn && strtY_Knwn) {
    if (matchesCoords(boardState, coords_start[y], coords_start[x], turn,
                      coords_end)) {
      numvalid++;
    }
  } 
  else if (strtX_Knwn) {
    for (int i = 0; i < 8; i++) {
      if ((movedPiece == deTeam(boardState[i][coords_start[x]])) &&
          (getTeam(boardState[i][coords_start[x]])) == turn) {
        if (matchesCoords(boardState, i, coords_start[x], turn, coords_end)) {
          numvalid++;
          coords_start[y]=i;
        }
      }
    }
  } 
  else if (strtY_Knwn) {
    for (int i = 0; i < 8; i++) {
      if ((movedPiece == deTeam(boardState[coords_start[y]][i])) &&
          (getTeam(boardState[coords_start[y]][i])) == turn) {
        if (matchesCoords(boardState, coords_start[y], i, turn, coords_end)) {
          numvalid++;
          coords_start[x]=i;
        }
      }
    }
  } 
  else {
    for (int i1 = 0; i1 < 8; i1++) {
      for (int i2 = 0; i2 < 8; i2++) {
        if((movedPiece == deTeam(boardState[i1][i2])) &&
          (getTeam(boardState[i1][i2])) == turn){
          if(matchesCoords(boardState, i1, i2, turn, coords_end)){
            numvalid++;
            coords_start[x]=i2;
            coords_start[y]=i1;
          }
        }
      }
    }
  }

  if (numvalid == 0) {
    outputMove[0] = invalidInput;
    return;
  }
  if (numvalid > 1) {
    outputMove[0] = ambiguousMove;
    return;
  }

  outputMove[0] = coords_start[x] + 97;
  outputMove[1] = 56 - coords_start[y];
  outputMove[2] = coords_end[x] + 97;
  outputMove[3] = 56 - coords_end[y];
  outputMove[4]='\0';
  if (transformPiece != none) {
    outputMove[4] = transformPiece;
    outputMove[5] = '\0';
  } else if(movedPiece=='K'){
    if(turn==black){
      outputMove[4]='k';
      outputMove[5]='m';
    }
    else{
      outputMove[4]='K';
      outputMove[5]='M';
    }
    outputMove[6]='\0';
  }
  else if(movedPiece=='P'){
    if(turn==white){
      if(boardState[coords_end[y]+1][coords_end[x]]=='6'){
        outputMove[4]='E';
        outputMove[5]='P';
        outputMove[6]='\0';
      }
    }else{
      if(boardState[coords_end[y]-1][coords_end[x]]=='9'){
        outputMove[4]='e';
        outputMove[5]='p';
        outputMove[6]='\0';
      }
    }
  }

  makeMove(tempBoard, outputMove);
  if(isInCheck(tempBoard, turn)) {
    outputMove[0] = InCheck;
    return;
  }
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

  for (int i1 = 0; i1 < 8; i1++) {
    for (int i2 = 0; i2 < 8; i2++) {
      if (boardState[i1][i2] == '9')
        boardState[i1][i2] = 'P';
      if (boardState[i1][i2] == '6')
        boardState[i1][i2] = 'p';
    }
  }
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
    for (i = 0, flag = 0; i < 8; i++){
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
        case 'K':
        case 'k':
          if (getTeam(chChar) == !color){
            if (drctnKngChck(boardState, i2, i1, 1, 1, 1, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, 1, 0, 1, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, 1, -1, 1, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, 0, 1, 1, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, 0, -1, 1, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, -1, 1, 1, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, -1, 0, 1, color))
              return 1;
            if (drctnKngChck(boardState, i2, i1, -1, -1, 1, color))
              return 1;
          }

        
        break;
      default:
        if (deTeam(chChar)=='Q'||deTeam(chChar)=='R') {
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
        if (deTeam(chChar)=='Q'||deTeam(chChar)=='B') {
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

int drctnKngChck(char boardState[8][8], int coord_x, int coord_y, int x_offset,
                 int y_offset, int movelimit, int color) {
  int i1 = coord_y;
  int i2 = coord_x;
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
int charType(char testChar) {
  switch (testChar) {
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
    return file;
    break;
  case 'K':
  case 'Q':
  case 'R':
  case 'B':
  case 'N':
  case 'P':
    return piece;
    break;
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
    return rank;
    break;
  default:
    return none;
  }
}
char deTeam(char testChar) {
  switch (testChar) {
  case 'r':
  case '+':
  case '#':
    return 'R';
    break;
  case '9':
  case '6':
  case 'p':
    return 'P';
  case 'k':
    return 'K';
    break;
  case 'n':
    return 'N';
    break;
  case 'b':
    return 'B';
    break;
  case 'q':
    return 'Q';
    break;
  default:
    return testChar;
  }
}
int matchesCoords(char boardState[8][8], int i1, int i2, int team,
                  int coords_end[2]) {
  char testChar = deTeam(boardState[i1][i2]);
  switch (testChar) {
  case 'P':
    if (team == black) {
      if (getTeam((boardState[i1 + 1][i2 - 1]) == white) ||
          boardState[i1][i2 - 1] == '9') {
        if (drctnCrdChck(boardState, i2, i1, -1, 1, 1, black, coords_end))
          return 1;
      }
      if ((getTeam(boardState[i1 + 1][i2 + 1]) == white) ||
          boardState[i1][i2 + 1] == '9') {
        if (drctnCrdChck(boardState, i2, i1, 1, 1, 1, black, coords_end))
          return 1;
      }
      if (boardState[i1 + 1][i2] == '_') {
        if (drctnCrdChck(boardState, i2, i1, 0, 1, 1, black, coords_end))
          return 1;
        if ((i1 == 1) && (boardState[i1 + 2][i2] == '_'))
          if (drctnCrdChck(boardState, i2, i1, 0, 1, 2, black, coords_end))
            return 1;
      }
    }
    if (team == white) {
      if ((getTeam(boardState[i1 - 1][i2 - 1]) == black) ||
          boardState[i1][i2 - 1] == '6')
        if (drctnCrdChck(boardState, i2, i1, -1, -1, 1, white, coords_end))
          return 1;
      if ((getTeam(boardState[i1 - 1][i2 + 1]) == black) ||
          boardState[i1][i2 + 1] == '6')
        if (drctnCrdChck(boardState, i2, i1, 1, -1, 1, white, coords_end))
          return 1;
      if (boardState[i1 - 1][i2] == '_') {
        if (drctnCrdChck(boardState, i2, i1, 0, -1, 1, white, coords_end))
          return 1;
        if ((i1 == 6) && (boardState[i1 - 2][i2] == '_'))
          if (drctnCrdChck(boardState, i2, i1, 0, -1, 2, white, coords_end))
            return 1;
      }
    }
    break;
  case 'N':
    if (drctnCrdChck(boardState, i2, i1, 1, 2, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, -1, 2, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, 1, -2, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, -1, -2, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, 2, 1, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, -2, 1, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, 2, -1, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, -2, -1, 1, team, coords_end))
      return 1;
    break;
  case 'K':
    if (drctnCrdChck(boardState, i2, i1, 1, 1, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, 1, 0, 1, team, coords_end))
     return 1;
    if (drctnCrdChck(boardState, i2, i1, 1, -1, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, 0, 1, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, 0, -1, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, -1, 1, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, -1, 0, 1, team, coords_end))
      return 1;
    if (drctnCrdChck(boardState, i2, i1, -1, -1, 1, team, coords_end))
      return 1;
    break;

  default:
    if (testChar == 'R' || testChar == 'Q') {
      if (drctnCrdChck(boardState, i2, i1, 1, 0, 8, team, coords_end))
        return 1;
      if (drctnCrdChck(boardState, i2, i1, -1, 0, 8, team, coords_end))
        return 1;
      if (drctnCrdChck(boardState, i2, i1, 0, 1, 8, team, coords_end))
        return 1;
      if (drctnCrdChck(boardState, i2, i1, 0, -1, 8, team, coords_end))
        return 1;
    }
    if (testChar == 'B' || testChar == 'Q') {
      if (drctnCrdChck(boardState, i2, i1, 1, 1, 8, team, coords_end))
        return 1;
      if (drctnCrdChck(boardState, i2, i1, -1, 1, 8, team, coords_end))
        return 1;
      if (drctnCrdChck(boardState, i2, i1, 1, -1, 8, team, coords_end))
        return 1;
      if (drctnCrdChck(boardState, i2, i1, -1, -1, 8, team, coords_end))
        return 1;
    }
    return 0;
  }
  return 0;
}
int drctnCrdChck(char boardState[8][8], int coord_x, int coord_y, int x_offset,
                 int y_offset, int movelimit, int color, int chk_coords[2]) {
  int hasCapturedPiece = 0;
  int i1 = coord_y, i2 = coord_x;

  for (int i = 0; (i < movelimit) && (hasCapturedPiece == 0); i++) {
    i1 += y_offset;
    i2 += x_offset;
    if (i1 > 7 || i1 < 0 || i2 > 7 || i2 < 0)
      return 0;
    if (getTeam(boardState[i1][i2]) == color)
      return 0;
    else if (getTeam(boardState[i1][i2]) == !color)
      hasCapturedPiece = 1;
    if ((chk_coords[x] == i2) && chk_coords[y] == i1) {
      if (debug)
        printf("We got a Match! Yippee!\n");
      return 1;
    }
  }
  return 0;
}

void printdraw(void){
  printf(" ______   ______    _______  _     _ \n");
  printf("|      | |    _ |  |   _   || | _ | |\n");
  printf("|  _    ||   | ||  |  |_|  || || || |\n");
  printf("| | |   ||   |_||_ |       ||       |\n");
  printf("| |_|   ||    __  ||       ||       |\n");
  printf("|       ||   |  | ||   _   ||   _   |\n");
  printf("|______| |___|  |_||__| |__||__| |__|\n");
}
void printstalemate(void){
  printf(" _______  _______  _______  ___      _______ \n");
  printf("|       ||       ||   _   ||   |    |       |\n");
  printf("|  _____||_     _||  |_|  ||   |    |    ___|\n");
  printf("| |_____   |   |  |       ||   |    |   |___\n");
  printf("|_____  |  |   |  |       ||   |___ |    ___|\n");
  printf(" _____| |  |   |  |   _   ||       ||   |___ \n");
  printf("|_______|  |___|  |__| |__||_______||_______|\n");
  printf("     __   __  _______  _______  _______\n");
  printf("    |  |_|  ||   _   ||       ||       |\n");
  printf("    |       ||  |_|  ||_     _||    ___|\n");
  printf("    |       ||       |  |   |  |   |___\n");
  printf("    |       ||       |  |   |  |    ___|\n");
  printf("    | ||_|| ||   _   |  |   |  |   |___\n");
  printf("    |_|   |_||__| |__|  |___|  |_______|\n");
  
}
void printcheckmate(void){
  printf(" _______  __   __  _______  _______  ___   _ \n");
  printf("|       ||  | |  ||       ||       ||   | | |\n");
  printf("|       ||  |_|  ||    ___||       ||   |_| |\n");
  printf("|       ||       ||   |___ |       ||      _|\n");
  printf("|      _||       ||    ___||      _||     |_\n");
  printf("|     |_ |   _   ||   |___ |     |_ |    _  \n");
  printf("|_______||__| |__||_______||_______||___| |_|\n");
  printf("     __   __  _______  _______  _______\n");
  printf("    |  |_|  ||   _   ||       ||       |\n");
  printf("    |       ||  |_|  ||_     _||    ___|\n");
  printf("    |       ||       |  |   |  |   |___\n");
  printf("    |       ||       |  |   |  |    ___|\n");
  printf("    | ||_|| ||   _   |  |   |  |   |___\n");
  printf("    |_|   |_||__| |__|  |___|  |_______|\n");
}