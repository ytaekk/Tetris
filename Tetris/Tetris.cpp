#include <iostream>
#include <Windows.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <conio.h>

#define WIDTH 12	// 테트리스 보드 가로
#define HEIGHT 21	// 테트리스 보드 세로
#define LEFT 75		// →
#define RIGHT 77	// ←
#define UP 72		// ↑
#define DOWN 80		// ↓
#define SPACEBAR 32	// SPACEBAR
#define BlockWidth 4 // Blcok 가로
#define BlockHeight 4// Blcok 세로

// Move Cursor
void moveCursor(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// Delete Cursor
void CursorView(bool show) {
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

// Game Board
int board[HEIGHT][WIDTH] = {
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3}, // 10
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},
	{3,0,0,0,0,0,0,0,0,0,0,3},// 20
	{3,3,3,3,3,3,3,3,3,3,3,3},
};

const int Blocks[7][BlockWidth * BlockHeight] = {
	// I
	{ 1,1,1,1,
	  0,0,0,0,
	  0,0,0,0,
	  0,0,0,0,
	},
	// J
	{ 0,1,0,0,
	  0,1,0,0,
	  1,1,0,0,
	  0,0,0,0,
	},
	// L
	{ 0,1,0,0,
	  0,1,0,0,
	  0,1,1,0,
	  0,0,0,0,
	},
	// O
	{ 0,1,1,0,
	  0,1,1,0,
	  0,0,0,0,
	  0,0,0,0,
	},
	// S
	{ 0,1,1,0,
	  1,1,0,0,
	  0,0,0,0,
	  0,0,0,0,
	},
	// Z
	{ 1,1,0,0,
	  0,1,1,0,
	  0,0,0,0,
	  0,0,0,0,
	},
	// T
	{ 0,1,0,0,
	  1,1,1,0,
	  0,0,0,0,
	  0,0,0,0,
	}
};

class BlockClass {
private:
	int _posX;
	int _posY;
	int _nRot;
	const int blockCursor = 4;
	int printBlock[BlockHeight * BlockWidth] = { 0 };
	int numBlock;
public:
	
	BlockClass()
	{
		_posX = 0;
		_posY = 0;
		_nRot = 0;
		srand(static_cast<unsigned int>(time(NULL)));
		numBlock = rand() % 7;
		
		
		// Initiate Block 
		for (int y = 0; y < BlockHeight; y++) {
			for (int x = 0; x < BlockWidth; x++) {

				printBlock[(y * BlockHeight) + x] = Blocks[numBlock][(y * BlockWidth) + x];

			}
		}

	}

	void left() {
		_posX--;
	}
	void right() {
		_posX++;
	}
	void up() {
		_posY--;
	}
	void down() {
		_posY+=2;
	}
	void rotation() {
		rotateBlock();
	}

	// Draw Block
	void drawBlock() {

		for (int y = 0; y < BlockHeight; y++) {
			for (int x = 0; x < BlockWidth; x++)
			{
				board[_posY + y][_posX + x + blockCursor] = printBlock[(y * BlockHeight) + x];
			}
		}
	}
	// Erase Shadow
	void eraseBlock() {

		for (int y = 0; y < BlockHeight; y++) {
			for (int x = 0; x < BlockWidth; x++)
			{
				board[_posY + y][_posX + x + blockCursor] = 0;
			}
		}
	}
	void rotateBlock() {

		//int tempBlock[BlockHeight * BlockWidth] = { 0 };
		int rotBlock[BlockHeight * BlockWidth] = { 0 };
		int BlockSize = sizeof(int) * BlockHeight * BlockWidth;
		
		memcpy(rotBlock, printBlock, BlockSize);

			for (int y = 0; y < BlockWidth; y++) {
				for (int x = 0; x < BlockWidth; x++)
				{
					printBlock[x * (BlockWidth)+(BlockHeight - y - 1)] = rotBlock[(y * BlockWidth) + x];
				}
			}
	}
	void moveBlock() {
		
			if (_kbhit()) {
				int inKey = _getch();
				switch (inKey) {
				case LEFT:
					eraseBlock();
					left();
					break;
				case RIGHT:
					eraseBlock();
					right();
					break;
				case UP:
					eraseBlock();
					rotation();
					break;
				case DOWN:
					eraseBlock();
					down();
					break;
				}
			}
	}
};

// Draw Game Board => Rendering
void drawBoard() {

	for (int y = 0; y < HEIGHT; y++) {
		//moveCursor(6, 3 + y);
		for (int x = 0; x < WIDTH; x++) {
			if (board[y][x] == 0) std::cout << " .";
			else if (board[y][x] == 1) std::cout << "■";
			else if (board[y][x] == 2) std::cout << "□";
			else if (board[y][x] == 3) std::cout << "▣";

		}
		std::cout << std::endl;
	}
}

void gameLoop() {
	
	BlockClass Block;

	while (1)
	{
		Block.drawBlock();
		Block.moveBlock();
		drawBoard();
		Sleep(10);
		system("cls");
	}

}

int main()
{
	CursorView(0);
	gameLoop();
	
	

	return 0;
}
