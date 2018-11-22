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
#define INFINITY 999999

#define START "START"
#define PLACE "PLACE"
#define TURN  "TURN"
#define END   "END"

#define MAXDEPTH 5

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
		if (i == 0) printf(" 012345678901\n");
		printf("%d", i%10);
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
			printf("%c", visual_board[i][j]);
		}
		printf("\n");
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
char My_Board[7][12][12] = { 0 }; // 剪枝使用的棋盘
int step = 0;
struct Command Tmp_Command = { 0,0,0 };
struct Command Best_Command = { 0,0,0 };
/**
 * 你可以在这里初始化你的AI
 */
void initAI(int mycolor) {

}

BOOL MakeNextMove(int mycolor, int depth) {
	int new_x = Tmp_Command.x + DIR[Tmp_Command.option][0];
	int new_y = Tmp_Command.y + DIR[Tmp_Command.option][1];
	My_Board[depth][Tmp_Command.x][Tmp_Command.y] = EMPTY;
	My_Board[depth][new_x][new_y] = mycolor;
	int other_color = 3 - mycolor;

	// 挑
	int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
	for (int i = 0; i < 4; i++) {
		int x1 = new_x + intervention_dir[i][0];
		int y1 = new_y + intervention_dir[i][1];
		int x2 = new_x - intervention_dir[i][0];
		int y2 = new_y - intervention_dir[i][1];
		if (isInBound(x1, y1) && isInBound(x2, y2) && My_Board[depth][x1][y1] == other_color && My_Board[depth][x2][y2] == other_color)
		{
			My_Board[depth][x1][y1] = mycolor;
			My_Board[depth][x2][y2] = mycolor;
		}
	}

	// 夹
	int custodian_dir[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
	for (int i = 0; i < 8; i++) {
		int x1 = new_x + custodian_dir[i][0];
		int y1 = new_y + custodian_dir[i][1];
		int x2 = new_x + custodian_dir[i][0] * 2;
		int y2 = new_y + custodian_dir[i][1] * 2;
		if (isInBound(x1, y1) && isInBound(x2, y2) && My_Board[depth][x2][y2] == mycolor && My_Board[depth][x1][y1] == other_color)
		{
			My_Board[depth][x1][y1] = mycolor;
		}
	}
	return TRUE;
}

BOOL chessboard_offset(int depth) {
	for (int x = 0; x < BOARD_SIZE; x++) {
		for (int y = 0; y < BOARD_SIZE; y++) {
			My_Board[depth][x][y] = My_Board[depth + 1][x][y];
		}
	}
	return TRUE;
}

int Evaluate(int depth, int mycolor) {
	int value = 0;
	int other_color = 3 - mycolor;
	const int* delta = NULL;
	for (int x = 0; x < BOARD_SIZE; x++) {
		for (int y = 0; y < BOARD_SIZE; y++) {
			if (My_Board[depth][x][y] != EMPTY) {
				if (My_Board[depth][x][y] == mycolor) {
					value += 1000;
					for (int k = 0; k < 8; k++) {
						delta = DIR[k];
						int near_x = x + delta[0];
						int near_y = y + delta[1];
						if (isInBound(near_x, near_y) && My_Board[depth][near_x][near_y] == EMPTY) { 
							//判断有无可能被挑
							if(isInBound(near_x+delta[0], near_y+delta[1]) && My_Board[depth][near_x+delta[0]][near_y+delta[1]] == mycolor){
								const int* delta2 = NULL;
								for (int i = 0; i < 8; i++) {
									delta2 = DIR[i];
									int Enm_Pos_x = near_x + delta[0];
									int Enm_Pos_y = near_y + delta[1];
									if (My_Board[depth][Enm_Pos_x][Enm_Pos_y] == other_color) {
										value -= 1600;
									}
								}
              }
						}
						else if (My_Board[depth][near_x][near_y] == other_color) { //判断有无可能被夹
							int sym_near_x = x - delta[0];
							int sym_near_y = y - delta[1];
							if (isInBound(sym_near_x, sym_near_y) && My_Board[depth][sym_near_x][sym_near_y] == EMPTY) {
								for (int i = 0; i < 8; i++) {
									int Enm_Pos_x = sym_near_x + delta[0];
									int Enm_Pos_y = sym_near_y + delta[1];
									if (My_Board[depth][Enm_Pos_x][Enm_Pos_y] == other_color) {
										value -= 800;
									}
								}
							}
						}
					}
				}
				else value -= 1000;
			}
		}
	}
	return value;
}

int AlphaBeta(int depth, int alpha, int beta, int mycolor) {
	chessboard_offset(depth); //调整棋盘
	if (depth == 0) return Evaluate(depth, mycolor);
	const int* delta = NULL;
	for (int x = 0; x < BOARD_SIZE; x++) {  //寻找可行着法
		for (int y = 0; y < BOARD_SIZE; y++) {
			if (My_Board[depth][x][y] != mycolor) continue;
			for (int k = 0; k < 8; k++) {
				int val = 0;
				delta = DIR[k];
				int new_x = x + delta[0];
				int new_y = y + delta[1];
				if (isInBound(new_x, new_y) && My_Board[depth][new_x][new_y] == EMPTY) {
					if (depth == MAXDEPTH) {
						Tmp_Command.x = x;
						Tmp_Command.y = y;
						Tmp_Command.option = k;
					}
					MakeNextMove(mycolor, depth);  //操作棋盘
					val = -AlphaBeta(depth - 1, -beta, -alpha, 3 - mycolor); //递归调用 进入下一层，将beta,alpha置为负，转换执棋方
					chessboard_offset(depth);
					if (val >= beta) {
						return beta;
					}
					if (val > alpha) {
						alpha = val;
						if (depth == MAXDEPTH) {
							Best_Command.x = Tmp_Command.x;
							Best_Command.y = Tmp_Command.y;
							Best_Command.option = Tmp_Command.option;
						}
					}
				}
			}
		}
	}
	return alpha;
}
/**
 * 轮到你落子。
 * 棋盘上0表示空白，1表示黑棋，2表示白旗
 * mycolor表示你所代表的棋子(1或2)
 * 你需要返回一个结构体Command，在x属性和y属性填上你想要移动的棋子的位置，option填上你想要移动的方向。
 */
BOOL aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int mycolor) {
	if (step == 0) {
		Best_Command.x = 9;
		Best_Command.y = 7;
		Best_Command.option = 0;
		return TRUE;
	}
	if (step == 1) {
		Best_Command.x = 2;
		Best_Command.y = 4;
		Best_Command.option = 1;
		return TRUE;
	}
	for (int i = 0; i < BOARD_SIZE; i++) { //初始化剪枝用的棋盘
		for (int j = 0; j < BOARD_SIZE; j++) {
			My_Board[6][i][j] = board[i][j];
		}
	}
	AlphaBeta(MAXDEPTH, -INFINITY, INFINITY, mycolor);
	return TRUE;
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
	aiTurn((const char(*)[BOARD_SIZE])board, me_flag);
	struct Command command = Best_Command;
	place(command.x, command.y, command.option, me_flag);
	printf("%d %d %d\n", command.x, command.y, command.option);
	fflush(stdout);
}

void end(int x) {

}

int main(int argc, char *argv[]) {
	while (TRUE)
	{
		memset(buffer, 0, sizeof(buffer));
		gets_s(buffer);

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
			step++;
		}
		else if (strstr(buffer, TURN))
		{
			turn();
			step++;
		}
		else if (strstr(buffer, END))
		{
			char tmp[MAX_BYTE] = { 0 };
			int x;
			sscanf(buffer, "%s %d", tmp, &x);
			end(x);
		}
		printBoard();
	}
	return 0;
}
