#include <iostream>

#define MINIMUM_VAL -11111
#define MAXIMUM_VAL 11111

//commands
#define COMMAND_FIRST "GEN_ALL_POS_MOV"
#define COMMAND_SECOND "GEN_ALL_POS_MOV_CUT_IF_GAME_OVER"
#define COMMAND_THIRD "SOLVE_GAME_STATE"

//players
#define PLAYER_FIRST '1'
#define PLAYER_SEC '2'
#define EMPTY_FIELD '0'

#define LEN_OF_COMMAND 50

#define ACTIVE_WINS +10
#define ENEMY_WINS -10
#define NONE_WINS 0

#define PLAYER_FIRST_WINS_MESS "FIRST_PLAYER_WINS"
#define PLAYER_SEC_WINS_MESS "SECOND_PLAYER_WINS"
#define TIE_MESS "BOTH_PLAYERS_TIE"

using namespace std;

typedef struct board_t
{
	int n;
	int m;
	int k;
	char activePlayer;
	int getBoardSize() const
	{
		return m * n;
	}
}board_t;

int GetX(int index, int width)
{
	return index % width;
}

int GetY(int index, int width)
{
	return index / width;
}

char GetEnemySign(char sign)
{
	if (sign == PLAYER_FIRST)
		return PLAYER_SEC;
	else
		return PLAYER_FIRST;
}



bool CheckBias(char* board, const board_t& boardData, int* biasL, int* biasR, char sign)
{
	int indexOfArrayL = 0;
	int indexOfArrayR = 0;
	for (int i = 0; i < boardData.getBoardSize(); i++)
	{
		if (GetX(i, boardData.m) + boardData.k - 1 < boardData.m && GetY(i, boardData.m) + boardData.k - 1 < boardData.n)
		{
			for (int j = 0; j < boardData.k; j++)
			{
				int pos = i + boardData.m * j + j;
				if (board[pos] == sign)
				{
					biasL[indexOfArrayL]++;
					if (biasL[indexOfArrayL] == boardData.k)
						return true;
				}
				else
					biasL[indexOfArrayL] = 0;
			}
			indexOfArrayL++;
		}
		if (GetX(i, boardData.m) - boardData.k + 1 >= 0 && GetY(i, boardData.m) + boardData.k - 1 < boardData.n)
		{
			for (int j = 0; j < boardData.k; j++)
			{
				int pos = i + boardData.m * j - j;
				if (board[pos] == sign)
				{
					biasR[indexOfArrayR]++;
					if (biasR[indexOfArrayR] == boardData.k)
						return true;
				}
				else
					biasR[indexOfArrayR] = 0;
			}
			indexOfArrayR++;
		}
	}
	return false;
}

bool FreeMemory(int* biasL, int* biasR, int* vertical)
{
	delete[] biasL;
	delete[] biasR;
	delete[] vertical;
	return true;
}

bool CheckIfWinningPos(char* board, const board_t& boardData, char sign)
{
	int amOfBias = (boardData.m - (boardData.k - 1)) * (boardData.n - (boardData.k - 1));
	int horizontal = 0;
	int* biasLeft = new int[amOfBias];
	int* biasRight = new int[amOfBias];
	int* vertical = new int[boardData.m];
	for (int i = 0; i < boardData.m; i++)
		vertical[i] = 0;
	for (int i = 0; i < amOfBias; i++)
	{
		biasLeft[i] = 0;
		biasRight[i] = 0;
	}

	for (int i = 0; i < boardData.getBoardSize(); i++)
	{
		if (i % boardData.m == 0)
		{
			horizontal = 0;
		}
		if (board[i] == sign)
		{
			horizontal++;
			vertical[i % boardData.m]++;
		}
		else
		{
			horizontal = 0;
			vertical[i % boardData.m] = 0;
		}
		if (horizontal == boardData.k)
			return FreeMemory(biasLeft, biasRight, vertical);

		for (int i = 0; i < boardData.m; i++)
			if (vertical[i] == boardData.k)
				return FreeMemory(biasLeft, biasRight, vertical);

	}
	if (CheckBias(board, boardData, biasLeft, biasRight, sign))
		return FreeMemory(biasLeft, biasRight, vertical);

	return !FreeMemory(biasLeft, biasRight, vertical);
}

int CheckWins(char* board, const board_t& boardData, char playerOnTurn)
{
	char enemy = GetEnemySign(playerOnTurn);
	if (CheckIfWinningPos(board, boardData, playerOnTurn))
		return ACTIVE_WINS;
	else if (CheckIfWinningPos(board, boardData, enemy))
		return ENEMY_WINS;
	return NONE_WINS;
}

bool CheckIfMovesLeft(char* board, int size)
{
	for (int i = 0; i < size; i++)
		if (board[i] == EMPTY_FIELD)
			return true;

	return false;
}

int Minimax(char* board, const board_t& boardData, int depth, bool isMax, char playerOnTrun, int alpha, int beta)
{
	if (depth == 0)
		return 0;

	int score = CheckWins(board, boardData, playerOnTrun);
	int fncResult = 0;
	if (score == ACTIVE_WINS || score == ENEMY_WINS)
		return score;
	
	if (!CheckIfMovesLeft(board, boardData.getBoardSize()))
		return 0;

	int best = MAXIMUM_VAL;

	char enemy = GetEnemySign(playerOnTrun);

	bool continueLoop = true;

	if (isMax)
	{
		best = MINIMUM_VAL;

		for (int i = 0; i < boardData.n && continueLoop; i++)
		{
			for (int j = 0; j < boardData.m && continueLoop; j++)
			{
				if (board[i * boardData.m + j] == EMPTY_FIELD)
				{
					board[i * boardData.m + j] = playerOnTrun;
					if (best < (fncResult = Minimax(board, boardData, depth -1, !isMax, playerOnTrun, alpha, beta)))
						best = fncResult;

					if (alpha < best)
						alpha = best;

					if (beta <= alpha)
						continueLoop = false;
					board[i * boardData.m + j] = EMPTY_FIELD;
				}
			}
		}
		return best;
	}
	else
	{
		
		for (int i = 0; i < boardData.n && continueLoop; i++)
		{
			for (int j = 0; j < boardData.m && continueLoop; j++)
			{
				if (board[i * boardData.m + j] == EMPTY_FIELD)
				{
					board[i * boardData.m + j] = enemy;
					if (best > (fncResult = Minimax(board, boardData, depth -1, !isMax, playerOnTrun, alpha, beta)))
						best = fncResult;
					if (beta > fncResult)
						beta = fncResult;

					if (beta <= alpha)
						continueLoop = false;
					board[i * boardData.m + j] = EMPTY_FIELD;
				}
			}
		}
		return best;
	}
}

//void PrintBoard(char* board, board_t const boardData);

int FindBestMove(char* board, const board_t& boardData, char playerOnTurn, int numOfFreePos)
{
	int bestValue = MINIMUM_VAL;
	int index = -1;
	for (int i = 0; i < boardData.n; i++)
	{
		for (int j = 0; j < boardData.m; j++)
		{
			if (board[i * boardData.m + j] == EMPTY_FIELD)
			{
				board[i * boardData.m + j] = playerOnTurn;
				//PrintBoard(board, boardData);
				int moveVal = Minimax(board, boardData, numOfFreePos, false, playerOnTurn, MINIMUM_VAL, MAXIMUM_VAL);
				board[i * boardData.m + j] = EMPTY_FIELD;

				if (moveVal > bestValue)
				{
					index = i * boardData.m + j;
					bestValue = moveVal;
				}
			}
		}
	}
	return index;
}

//void PrintBoard(char* board, board_t const boardData)
//{
//	for (int i = 0; i < boardData.getBoardSize(); i++)
//	{
//		if (i % boardData.m == 0)
//			cout << '\n';
//
//		cout << board[i] << " ";
//	}
//}

void PrintBoard(char* board, const board_t& boardData, int index)
{
	for (int i = 0; i < boardData.getBoardSize(); i++)
	{
		if (i % boardData.m == 0)
			cout << '\n';

		if (i == index)
		{
			cout << boardData.activePlayer << " ";
		}
		else
		{
			cout << board[i] << " ";
		}
	}
}


bool PrintBoardCutIfWinnigPos(char* board, const board_t& boardData, int index)
{
	bool win = false;
	char* tmpBoard = new char[boardData.getBoardSize()];
	for (int i = 0; i < boardData.getBoardSize(); i++)
	{
		if (i == index)
			tmpBoard[i] = boardData.activePlayer;
		else
			tmpBoard[i] = board[i];

	}
	if (CheckIfWinningPos(tmpBoard, boardData, boardData.activePlayer))
		win = true;

	delete[] tmpBoard;

	if (win)
		return true;

	return false;

}

void GenerateMove(char* board, const board_t& boardData)
{
	for (int i = 0; i < boardData.getBoardSize(); i++)
		if (board[i] == EMPTY_FIELD)
			PrintBoard(board, boardData, i);
}


void ServeFirstCommand(char* board, const board_t& boardData, int numOfEmptyFields)
{
	char enemySign;
	if (boardData.activePlayer == PLAYER_FIRST)
		enemySign = PLAYER_SEC;
	else
		enemySign = PLAYER_FIRST;

	if (!CheckIfWinningPos(board, boardData, enemySign))
	{
		cout << numOfEmptyFields;
		GenerateMove(board, boardData);
	}
	else
		cout << "0\n";
}

void ServeSecondCommand(char* board, const board_t& boardData, int numOfEmptyFields)
{
	char enemySign = GetEnemySign(boardData.activePlayer);
	if (!CheckIfWinningPos(board, boardData, enemySign))
	{
		int winPos = -1;
		for (int i = 0; i < boardData.getBoardSize(); i++)
		{
			if (board[i] == EMPTY_FIELD)
			{
				if (PrintBoardCutIfWinnigPos(board, boardData, i))
				{
					winPos = i;
					break;
				}
			}
		}
		if (winPos != -1)
		{
			cout << "1\n";
			PrintBoard(board, boardData, winPos);
		}
		else
		{
			cout << numOfEmptyFields;
			GenerateMove(board, boardData);
		}
	}
	else
		cout << "0\n";
}

void ServeThirdCommand(char* board, const board_t &boardData, int numOfEmptyFields)
{
	char enemySign = GetEnemySign(boardData.activePlayer);
	char playerOnTurn = boardData.activePlayer;
	bool won = false;
	if (!CheckIfWinningPos(board, boardData, enemySign))
	{
		int i = 0;
		while (i != numOfEmptyFields)
		{
			board[FindBestMove(board, boardData, playerOnTurn, numOfEmptyFields - i)] = playerOnTurn;
			i++;
			//PrintBoard(board, boardData);
			if (CheckIfWinningPos(board, boardData, PLAYER_FIRST))
			{
				cout << PLAYER_FIRST_WINS_MESS << '\n';
				won = true;
				break;
			}

			if (CheckIfWinningPos(board, boardData, PLAYER_SEC))
			{
				cout << PLAYER_SEC_WINS_MESS << '\n';
				won = true;
				break;
			}
			playerOnTurn = GetEnemySign(playerOnTurn);
		}
		if (i == numOfEmptyFields && !won)
			cout << TIE_MESS << '\n';
	}
	else
	{
		if (enemySign == PLAYER_SEC)
			cout << PLAYER_SEC_WINS_MESS << '\n';
		else
			cout << PLAYER_FIRST_WINS_MESS << '\n';
	}
}

int main()
{
	char command[LEN_OF_COMMAND];
	board_t boardData;
	while (cin >> command)
	{
		int numOfEmptyFields = 0;
		cin >> boardData.n >> boardData.m >> boardData.k >> boardData.activePlayer;
		char* board = new char[boardData.getBoardSize()];
		for (int i = 0; i < boardData.getBoardSize(); i++)
		{
			cin >> board[i];
			if (board[i] == EMPTY_FIELD)
				numOfEmptyFields++;
		}
		//PrintBoard(board, boardData);

		if (strcmp(command, COMMAND_FIRST) == 0)
		{
			ServeFirstCommand(board, boardData, numOfEmptyFields);
			cout << '\n';
		}
		else if (strcmp(command, COMMAND_SECOND) == 0)
		{
			ServeSecondCommand(board, boardData, numOfEmptyFields);
			cout << '\n';
		}
		else if (strcmp(command, COMMAND_THIRD) == 0)
		{
			ServeThirdCommand(board, boardData, numOfEmptyFields);
		}
		else
			break;
		delete[] board;
	}
	return 0;
}