#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// board information
#define BOARD_SIZE 12
#define EMPTY 0
#define BLACK 1
#define WHITE 2

// bool
typedef int BOOL;
#define TRUE 1
#define FALSE 0

// option
typedef int OPTION;
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define UP_LEFT 4
#define UP_RIGHT 5
#define DOWN_LEFT 6
#define DOWN_RIGHT 7

#define MAX_BYTE 10000
#define Neg_Inf -999999
#define Pos_Inf 999999

#define START "START"
#define PLACE "PLACE"
#define TURN  "TURN"
#define END   "END"

struct Command
{
	int x;
	int y;
	OPTION option;
};

char buffer[MAX_BYTE] = { 0 };
char board[BOARD_SIZE][BOARD_SIZE] = { 0 };
int me_flag;
int other_flag;

const int DIR[8][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

void debug(const char *str) {
	printf("DEBUG %s\n", str);
	fflush(stdout);
}

void printBoard() {
	char visual_board[BOARD_SIZE][BOARD_SIZE] = { 0 };
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == EMPTY) {
				visual_board[i][j] = '.';
			}
			else if (board[i][j] == BLACK) {
				visual_board[i][j] = 'O';
			}
			else if (board[i][j] == WHITE) {
				visual_board[i][j] = 'X';
			}
		}
		printf("%s\n", visual_board[i]);
	}
}

BOOL isInBound(int x, int y) {
	return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

/**
 * YOUR CODE BEGIN
 * 你的代码开始
 */

 /**
  * You can define your own struct and variable here
  * 你可以在这里定义你自己的结构体和变量
  */
char chessboard[5][12][12] = { 0 }; // 剪枝使用的棋盘
int val = 0;
struct Command command = { 0,0,0 };
struct Command tmp_command = { 0,0,0 };
/**
 * 你可以在这里初始化你的AI
 */
void initAI(int mycolor) {

}

/*struct Command findValidPos(const char board[BOARD_SIZE][BOARD_SIZE], int flag) {
  struct Command command = {0, 0, 0};
  for (int k = 0; k < 8; k++) {
	const int* delta = DIR[k];
	for (int x = 0; x < BOARD_SIZE; x++) {
	  for (int y = 0; y < BOARD_SIZE; y++) {
		if (board[x][y] != flag) {
		  continue;
		}
		int new_x = x + delta[0];
		int new_y = y + delta[1];
		if (isInBound(new_x, new_y) && board[new_x][new_y] == EMPTY) {
		  command.x = x;
		  command.y = y;
		  command.option = k;
		  return command;
		}
	  }
	}
  }
  return command;
}
*/

bool MakeNextMove(int mycolor,int depth) {
	int new_x = command.x + DIR[command.option][0];
	int new_y = command.y + DIR[command.option][1];
	chessboard[depth][command.x][command.y] = EMPTY;
	chessboard[depth][new_x][new_y] = mycolor;
	int other_color = 3 - mycolor;

	// 挑
	int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
	for (int i = 0; i < 4; i++) {
		int x1 = new_x + intervention_dir[i][0];
		int y1 = new_y + intervention_dir[i][1];
		int x2 = new_x - intervention_dir[i][0];
		int y2 = new_y - intervention_dir[i][1];
		if (isInBound(x1, y1) && isInBound(x2, y2) && chessboard[depth][x1][y1] == other_color && chessboard[depth][x2][y2] == other_color) 
		{
			chessboard[depth][x1][y1] = mycolor;
			chessboard[depth][x2][y2] = mycolor;
		}
	}

	// 夹
	int custodian_dir[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
	for (int i = 0; i < 8; i++) {
		int x1 = new_x + custodian_dir[i][0];
		int y1 = new_y + custodian_dir[i][1];
		int x2 = new_x + custodian_dir[i][0] * 2;
		int y2 = new_y + custodian_dir[i][1] * 2;
		if (isInBound(x1, y1) && isInBound(x2, y2) && chessboard[depth][x2][y2] == mycolor && chessboard[depth][x1][y1] == other_color) 
		{
			chessboard[depth][x1][y1] = mycolor;
		}
	}
	return TRUE;
}

/*void UnMakeMove(int mycolor,int depth) {
	int new_x = command.x + DIR[command.option][0];
	int new_y = command.y + DIR[command.option][1];
	chessboard[depth][command.x][command.y] = mycolor;
	chessboard[depth][new_x][new_y] = EMPTY;
	return;
}*/

bool chessboard_offset(int depth) {
	for (int x = 0; x < BOARD_SIZE; x++) {
		for (int y = 0; y < BOARD_SIZE; y++) {
			chessboard[depth][x][y] = chessboard[depth+1][x][y];
		}
	}
	return TRUE;
}

int Evaluate() {

}

int alphabeta(int alpha, int beta, int depth, int mycolor) {
	if (depth == 0) return Evaluate();
	const int* delta = NULL;
	for (int x = 0; x < BOARD_SIZE; x++) {  //寻找可行着法
		for (int y = 0; y < BOARD_SIZE; y++) {
			if (board[x][y] != mycolor) continue;
			for (int k = 0; k < BOARD_SIZE; k++) {
				delta = DIR[k];
				int new_x = x + delta[0];
				int new_y = y + delta[1];
				if (isInBound(new_x, new_y) && board[new_x][new_y] == EMPTY) {
					command.x = x;
					command.y = y;
					command.option = k;
					MakeNextMove(mycolor, depth);  //操作棋盘
					chessboard_offset(depth); //调整下一层的棋盘
					val = alphabeta(depth - 1, -beta, -alpha, 3 - mycolor); //递归调用 进入下一层
					chessboard_offset(depth); //重置棋盘
				}
				if (val >= beta) return beta;
				if (val > alpha) {
					alpha = val;
				}
			}
		}
	}
}
/**
 * 轮到你落子。
 * 棋盘上0表示空白，1表示黑棋，2表示白旗
 * mycolor表示你所代表的棋子(1或2)
 * 你需要返回一个结构体Command，在x属性和y属性填上你想要移动的棋子的位置，option填上你想要移动的方向。
 */
struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int mycolor) {
	for (int i = 0; i < BOARD_SIZE; i++) { //初始化剪枝用的棋盘
		for (int j = 0; j < BOARD_SIZE; j++) {
			chessboard[5][i][j] = board[i][j];
		}
	}
	alphabeta(Pos_Inf, Neg_Inf, 4, mycolor);

		return preferedPos;
}

/**
 * 你的代码结束
 */


BOOL place(int x, int y, OPTION option, int cur_flag) {
	// 移动之前的位置没有我方棋子
	if (board[x][y] != cur_flag) {
		return FALSE;
	}

	int new_x = x + DIR[option][0];
	int new_y = y + DIR[option][1];
	// 移动之后的位置超出边界, 或者不是空地
	if (!isInBound(new_x, new_y) || board[new_x][new_y] != EMPTY) {
		return FALSE;
	}

	board[x][y] = EMPTY;
	board[new_x][new_y] = cur_flag;

	int cur_other_flag = 3 - cur_flag;

	// 挑
	int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
	for (int i = 0; i < 4; i++) {
		int x1 = new_x + intervention_dir[i][0];
		int y1 = new_y + intervention_dir[i][1];
		int x2 = new_x - intervention_dir[i][0];
		int y2 = new_y - intervention_dir[i][1];
		if (isInBound(x1, y1) && isInBound(x2, y2) && board[x1][y1] == cur_other_flag && board[x2][y2] == cur_other_flag) {
			board[x1][y1] = cur_flag;
			board[x2][y2] = cur_flag;
		}
	}

	// 夹
	int custodian_dir[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
	for (int i = 0; i < 8; i++) {
		int x1 = new_x + custodian_dir[i][0];
		int y1 = new_y + custodian_dir[i][1];
		int x2 = new_x + custodian_dir[i][0] * 2;
		int y2 = new_y + custodian_dir[i][1] * 2;
		if (isInBound(x1, y1) && isInBound(x2, y2) && board[x2][y2] == cur_flag && board[x1][y1] == cur_other_flag) {
			board[x1][y1] = cur_flag;
		}
	}

	return TRUE;
}


//............
//............
//..OOO....X..
//.........X..
//............
//......OOO...
//...XXX......
//............
//..O.........
//..O....XXX..
//............
//............
void start(int flag) {
	memset(board, 0, sizeof(board));

	for (int i = 0; i < 3; i++) {
		board[2][2 + i] = WHITE;
		board[6][6 + i] = WHITE;
		board[5][3 + i] = BLACK;
		board[9][7 + i] = BLACK;
	}

	for (int i = 0; i < 2; i++) {
		board[8 + i][2] = WHITE;
		board[2 + i][9] = BLACK;
	}

	initAI(flag);
}

void turn() {
	// AI
	struct Command command = aiTurn((const char(*)[BOARD_SIZE])board, me_flag);
	place(command.x, command.y, command.option, me_flag);
	printf("%d %d %d\n", command.x, command.y, command.option);
	fflush(stdout);
}

void end(int x) {

}

void loop() {
	//  freopen("../input", "r", stdin);
	while (TRUE)
	{
		memset(buffer, 0, sizeof(buffer));
		gets(buffer);

		if (strstr(buffer, START))
		{
			char tmp[MAX_BYTE] = { 0 };
			sscanf(buffer, "%s %d", tmp, &me_flag);
			other_flag = 3 - me_flag;
			start(me_flag);
			printf("OK\n");
			fflush(stdout);
		}
		else if (strstr(buffer, PLACE))
		{
			char tmp[MAX_BYTE] = { 0 };
			int x, y;
			OPTION option;
			sscanf(buffer, "%s %d %d %d", tmp, &x, &y, &option);
			place(x, y, option, other_flag);
		}
		else if (strstr(buffer, TURN))
		{
			turn();
		}
		else if (strstr(buffer, END))
		{
			char tmp[MAX_BYTE] = { 0 };
			int x;
			sscanf(buffer, "%s %d", tmp, &x);
			end(x);
		}
		//    printBoard();
	}
}


int main(int argc, char *argv[]) {
	loop();
	return 0;
}
