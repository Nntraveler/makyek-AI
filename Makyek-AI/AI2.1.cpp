#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
#define INFINITY_VALUE 9999999

#define START "START"
#define PLACE "PLACE"
#define TURN  "TURN"
#define END   "END"

#define MAXDEPTH 4

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
const int clockDir[8][2] = { {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1} };
//从向上的方向顺时针到向左上的方向
const int sunShapeDir[8][2] = 
{ 
	{-1,-2}, {-2,-1},
  {-2,1}, {-1,2}, 
  {1,2}, {2,1}, 
  {2,-1}, {1,-2} 
};

void debug(const char *str) {
	printf("DEBUG %s\n", str);
	fflush(stdout);
}

void printBoard() {
	char visual_board[BOARD_SIZE][BOARD_SIZE] = { 0 };
	printf("UP=0 DOWN=1 LEFT=2 RIGHT=3 UP_LEFT=4 UP_RIGHT=5 DOWN_LEFT=6 DOWN_RIGHT=7\n");
	for (int i = 0; i < BOARD_SIZE; i++) {
		if (i == 0) printf(" 012345678901\n");
		printf("%d", i % 10);
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
char myBoard[7][12][12] = { 0 }; // 剪枝使用的棋盘
int step = 0;
int lowTotalMoves = 0;
struct Command tmpCommand = { 0,0,0 };
struct Command bestCommand = { 0,0,0 };
const int* priorDir[8] = { 0 };
struct Command legalMoves[MAXDEPTH+1][128] = { 0 };
BOOL gameOver = FALSE;
/**
 * 你可以在这里初始化你的AI
 */
void initAI(int mycolor) {

}
int generateLegalMoves(int depth, int myColor) {
	const int* delta = NULL;
	int count = 0;
	for (int x = 0; x < BOARD_SIZE; x++) {  //寻找可行着法
		for (int y = 0; y < BOARD_SIZE; y++) {
			if (myBoard[depth][x][y] != myColor) continue;
			for (int k = 0; k < 8; k++) {
				delta = DIR[k];
				int new_x = x + delta[0];
				int new_y = y + delta[1];
				if (isInBound(new_x, new_y) && myBoard[depth][new_x][new_y] == EMPTY)
				{
					legalMoves[depth][count].x = x;
					legalMoves[depth][count].y = y;
					legalMoves[depth][count].option = k;
					count++;
				}
			}
		}
	}
	if (count == 0) gameOver = TRUE;
	return count;
}

BOOL makeNextMove(int myColor, int count, int depth) {
	int new_x = legalMoves[depth][count].x + DIR[legalMoves[depth][count].option][0];
	int new_y = legalMoves[depth][count].y + DIR[legalMoves[depth][count].option][1];
	myBoard[depth][legalMoves[depth][count].x][legalMoves[depth][count].y] = EMPTY;
	myBoard[depth][new_x][new_y] = myColor;
	int other_color = 3 - myColor;

	// 挑
	int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
	for (int i = 0; i < 4; i++) {
		int x1 = new_x + intervention_dir[i][0];
		int y1 = new_y + intervention_dir[i][1];
		int x2 = new_x - intervention_dir[i][0];
		int y2 = new_y - intervention_dir[i][1];
		if (isInBound(x1, y1) && isInBound(x2, y2) && myBoard[depth][x1][y1] == other_color && myBoard[depth][x2][y2] == other_color)
		{
			myBoard[depth][x1][y1] = myColor;
			myBoard[depth][x2][y2] = myColor;
		}
	}

	// 夹
	int custodian_dir[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
	for (int i = 0; i < 8; i++) {
		int x1 = new_x + custodian_dir[i][0];
		int y1 = new_y + custodian_dir[i][1];
		int x2 = new_x + custodian_dir[i][0] * 2;
		int y2 = new_y + custodian_dir[i][1] * 2;
		if (isInBound(x1, y1) && isInBound(x2, y2) && myBoard[depth][x2][y2] == myColor && myBoard[depth][x1][y1] == other_color)
		{
			myBoard[depth][x1][y1] = myColor;
		}
	}
	return TRUE;
}

BOOL chessboard_offset(int depth) {
	memcpy(myBoard[depth], myBoard[depth + 1], sizeof(myBoard[depth]));
	return TRUE;
}

// 012345678901
//0............
//1............
//2..OOO....X..
//3.........X..
//4............
//5......OOO...
//6...XXX......
//7............
//8..O.........
//9..O....XXX..
//0............
//1............
int Evaluate(int depth, int mycolor) {
	int value = 0;
	int other_color = 3 - mycolor;
	const int* delta = NULL;
	const int* sunShapeDelta = NULL;
	if (tmpCommand.option > 3) value += 1;//优先斜走
	for (int x = 0; x < BOARD_SIZE; x++) {
		for (int y = 0; y < BOARD_SIZE; y++) {
			if (myBoard[depth][x][y] != EMPTY) {
				if (myBoard[depth][x][y] == mycolor) {
					value += 100000;
					if (mycolor == 1) {//黑棋集中到5,7
						value += (81 - (x - 5)*(x - 5) - (y - 7)*(y - 7)) * 25;
					}
					else {//白棋集中到6,4
						value += (81 - (x - 6)*(x - 6) - (y - 4)*(y - 4)) * 25;
					}
					for (int k = 0; k < 4; k++) {
						delta = clockDir[k];
						sunShapeDelta = sunShapeDir[k];
						//.X.
						//..X
						//X.. 形状
						int sunShape_x = x + sunShapeDelta[0];
						int sunShape_y = y + sunShapeDelta[1];
						if (isInBound(sunShape_x, sunShape_y) && myBoard[depth][sunShape_x][sunShape_y] == mycolor)
						{
							value += 300;
						}
						int near_x = x + delta[0];
						int near_y = y + delta[1];
						if (isInBound(near_x, near_y) && myBoard[depth][near_x][near_y] == EMPTY)
						{
							//判断有无可能被挑
							if (isInBound(near_x + delta[0], near_y + delta[1]) && myBoard[depth][near_x + delta[0]][near_y + delta[1]] == mycolor)
							{
								const int* delta2 = NULL;
								for (int i = 0; i < 8; i++) {
									delta2 = DIR[i];
									int Enm_Pos_x = near_x + delta2[0];
									int Enm_Pos_y = near_y + delta2[1];
									if (myBoard[depth][Enm_Pos_x][Enm_Pos_y] == other_color)
									{
										value -= 160000;
										break;
									}
								}
							}
						}
						else if (myBoard[depth][near_x][near_y] == other_color) { //判断有无可能被夹
							int sym_near_x = x - delta[0];
							int sym_near_y = y - delta[1];
							if (isInBound(sym_near_x, sym_near_y) && myBoard[depth][sym_near_x][sym_near_y] == EMPTY) {
								for (int i = 0; i < 8; i++) {
									int Enm_Pos_x = sym_near_x + delta[0];
									int Enm_Pos_y = sym_near_y + delta[1];
									if (myBoard[depth][Enm_Pos_x][Enm_Pos_y] == other_color) {
										value -= 80000;
										break;
									}
								}
							}
						}
					}
				}
				else {
					value -= 100000;
					for (int k = 0; k < 4; k++) {
						delta = clockDir[k];
						int near_x = x + delta[0];
						int near_y = y + delta[1];
						if (isInBound(near_x, near_y) && myBoard[depth][near_x][near_y] == EMPTY)
						{
							//判断有无可能被挑
							if (isInBound(near_x + delta[0], near_y + delta[1]) && myBoard[depth][near_x + delta[0]][near_y + delta[1]] == other_color)
							{
								const int* delta2 = NULL;
								for (int i = 0; i < 8; i++) {
									delta2 = DIR[i];
									int Enm_Pos_x = near_x + delta2[0];
									int Enm_Pos_y = near_y + delta2[1];
									if (myBoard[depth][Enm_Pos_x][Enm_Pos_y] == mycolor)
									{
										value += 160000;
										break;
									}
								}
							}
						}
						else if (myBoard[depth][near_x][near_y] == mycolor) { //判断有无可能被夹
							int sym_near_x = x - delta[0];
							int sym_near_y = y - delta[1];
							if (isInBound(sym_near_x, sym_near_y) && myBoard[depth][sym_near_x][sym_near_y] == EMPTY) {
								for (int i = 0; i < 8; i++) {
									int Enm_Pos_x = sym_near_x + delta[0];
									int Enm_Pos_y = sym_near_y + delta[1];
									if (myBoard[depth][Enm_Pos_x][Enm_Pos_y] == mycolor) {
										value += 80000;
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return value;
}

int lowEvaluate(int myColor) {
	int lowVal = 0;
	int other_color = 3 - myColor;
	const int* delta = NULL;
	for (int x = 0; x < BOARD_SIZE; x++) {
		for (int y = 0; y < BOARD_SIZE; y++) {
			if (myBoard[MAXDEPTH][x][y] == EMPTY) continue;
			if (myBoard[MAXDEPTH][x][y] == myColor) {
				lowVal += 100000;
				for (int k = 0; k < 4; k++) {
					delta = clockDir[k];
					int near_x = x + delta[0];
					int near_y = y + delta[1];
					if (isInBound(near_x, near_y) && myBoard[MAXDEPTH][near_x][near_y] == EMPTY)
					{
						//判断有无可能被挑
						if (isInBound(near_x + delta[0], near_y + delta[1]) && myBoard[MAXDEPTH][near_x + delta[0]][near_y + delta[1]] == MAXDEPTH)
						{
							const int* delta2 = NULL;
							for (int i = 0; i < 8; i++) {
								delta2 = DIR[i];
								int Enm_Pos_x = near_x + delta2[0];
								int Enm_Pos_y = near_y + delta2[1];
								if (myBoard[MAXDEPTH][Enm_Pos_x][Enm_Pos_y] == other_color)
								{
									lowVal -= 60000;
									break;
								}
							}
						}
					}

					else if (myBoard[MAXDEPTH][near_x][near_y] == other_color) { //判断有无可能被夹
						int sym_near_x = x - delta[0];
						int sym_near_y = y - delta[1];
						if (isInBound(sym_near_x, sym_near_y) && myBoard[MAXDEPTH][sym_near_x][sym_near_y] == EMPTY) {
							for (int i = 0; i < 8; i++) {
								int Enm_Pos_x = sym_near_x + delta[0];
								int Enm_Pos_y = sym_near_y + delta[1];
								if (myBoard[MAXDEPTH][Enm_Pos_x][Enm_Pos_y] == other_color) {
									lowVal -= 40000;
									break;
								}
							}
						}
					}
				}
			}
			else {
				lowVal -= 100000;
			}
		}
	}
	return lowVal;
}

BOOL lowCommandSort(int myColor) {
	lowTotalMoves = 0;
	int tmp = 0;
	int commandVal[128] = { -INFINITY_VALUE };
	struct Command swapTmpCommand = { 0,0,0 };
	chessboard_offset(MAXDEPTH);
	lowTotalMoves = generateLegalMoves(MAXDEPTH, myColor);
	for (int i = 0; i < lowTotalMoves; i++) {
		makeNextMove(myColor, i, MAXDEPTH);
		commandVal[i] = lowEvaluate(myColor);
		chessboard_offset(MAXDEPTH);
	}
	for (int i = 0; i < lowTotalMoves - 1; i++) {
		for (int j = 0; j < lowTotalMoves - 1; j++) {
			if (commandVal[j] < commandVal[j + 1]) {
				tmp = commandVal[j];
				commandVal[j] = commandVal[j + 1];
				commandVal[j + 1] = tmp;
				swapTmpCommand = legalMoves[MAXDEPTH][j];
				legalMoves[MAXDEPTH][j] = legalMoves[MAXDEPTH][j + 1];
				legalMoves[MAXDEPTH][j + 1] = swapTmpCommand;
			}
		}
	}
	return TRUE;
}

int AlphaBeta(int depth, int alpha, int beta, int mycolor) {
	if (depth == 0) return Evaluate(depth + 1, mycolor);
	const int* delta = NULL;
	int totalMoves = 0;
	chessboard_offset(depth); //调整棋盘
	if (depth != MAXDEPTH) {
		totalMoves = generateLegalMoves(depth, mycolor);
		if (totalMoves == 0) {
			return Evaluate(depth, mycolor);
		}
	}
	else {
		totalMoves = lowTotalMoves;
	}
	for (int i = 0; i < totalMoves; i++) {  //寻找可行着法
		int val = 0;
		struct Command possibleBestCommand = { 0,0,0 };
		tmpCommand = legalMoves[depth][i];
		if (depth == MAXDEPTH) {
			possibleBestCommand = tmpCommand;
		}
		makeNextMove(mycolor, i, depth);  //操作棋盘
		val = -AlphaBeta(depth - 1, -beta, -alpha, 3 - mycolor); //递归调用 进入下一层，将beta,alpha置为负，转换执棋方
		chessboard_offset(depth); //重新调整棋盘
		if (val >= beta) {
			return beta;
		}
		if (val > alpha) {
			alpha = val;
			if (depth == MAXDEPTH) {
				bestCommand = possibleBestCommand;
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
		bestCommand.x = 9;
		bestCommand.y = 7;
		bestCommand.option = 4;
		return TRUE;
	}
	if (step == 1) {
		bestCommand.x = 6;
		bestCommand.y = 8;
		bestCommand.option = 6;
		return TRUE;
	}
	memcpy(myBoard[MAXDEPTH + 1], board, sizeof(myBoard[MAXDEPTH + 1])); //初始化剪枝用的棋盘
	lowCommandSort(mycolor);
	AlphaBeta(MAXDEPTH, -INFINITY_VALUE, INFINITY_VALUE, mycolor);
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

// 012345678901
//0............
//1............
//2..OOO....X..
//3.........X..
//4............
//5......OOO...
//6...XXX......
//7............
//8..O.........
//9..O....XXX..
//0............
//1............
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
	struct Command command = bestCommand;
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