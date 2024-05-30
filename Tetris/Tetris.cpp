#include <iostream>
#include <Windows.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <random>

#define WIDTH 12	// 테트리스 보드 가로
#define HEIGHT 21	// 테트리스 보드 세로
#define LEFT 75		// →
#define RIGHT 77	// ←
#define UP 72		// ↑
#define DOWN 80		// ↓
#define SPACEBAR 32	// SPACEBAR
#define BlockWidth 4 // Blcok 가로
#define BlockHeight 4// Blcok 세로

// Double buffering 
static int g_nScreenIndex;
static HANDLE g_hScreen[2];
int consoleSize;
int renderBoard[HEIGHT][WIDTH];

void ScreenInit()
{
	CONSOLE_CURSOR_INFO cci;
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo{ 0, };

	// 화면 버퍼 2개를 만든다.
	g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	// 콘솔 사이즈 측정 = 사이즈가 안맞아서 버퍼 출력이 제대로 안됐음
	consoleSize = (consoleInfo.srWindow.Right - consoleInfo.srWindow.Left)
		* (consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top);

	// 커서를 숨긴다.
	cci.dwSize = 1;
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(g_hScreen[0], &cci);
	SetConsoleCursorInfo(g_hScreen[1], &cci);
}
void ScreenFlip()
{
	SetConsoleActiveScreenBuffer(g_hScreen[g_nScreenIndex]);
	g_nScreenIndex = !g_nScreenIndex;
}
void ScreenClear()
{
	COORD Coor = { 0, 0 };
	DWORD dw=0;
	// consolsize 가 제대로 입력이 안돼서 버퍼가 제대로 출력이 안됐음.
	FillConsoleOutputCharacter(g_hScreen[g_nScreenIndex], ' ', consoleSize, Coor, &dw);
	SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], Coor);

}
// 화면 버퍼 닫기
void ScreenRelease()
{
	if(g_hScreen[0]!=nullptr)
		CloseHandle(g_hScreen[0]);
	if (g_hScreen[1] != nullptr)
		CloseHandle(g_hScreen[1]);
}


// Move Cursor
void moveCursor(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], pos);
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
const char BlockType[][4] = {
	"  ", // 0
	"■",  // 1
	"■",  // 2
	"▣",  // 3
};

const int Blocks[7][BlockWidth * BlockHeight] = {
	// I
	{ 0,0,0,0,
	  1,1,1,1,
	  0,0,0,0,
	  0,0,0,0,
	},
	// J
	{ 0,0,1,0,
	  0,0,1,0,
	  0,1,1,0,
	  0,0,0,0,
	},
	// L
	{ 0,1,0,0,
	  0,1,0,0,
	  0,1,1,0,
	  0,0,0,0,
	},
	// O
	{ 0,0,0,0,
	  0,1,1,0,
	  0,1,1,0,
	  0,0,0,0,
	},
	// S
	{ 0,0,0,0,
	  0,1,1,0,
	  1,1,0,0,
	  0,0,0,0,
	},
	// Z
	{ 0,0,0,0,
	  1,1,0,0,
	  0,1,1,0,
	  0,0,0,0,
	},
	// T
	{ 0,0,0,0,
	  0,1,0,0,
	  1,1,1,0,
	  0,0,0,0,
	}
};

// void drawBoard();

void DBRender();

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
		initBlock();
	}
	void initBlock() {
		_posX = 0;
		_posY = 0;
		_nRot = 0;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(0, 6);
		numBlock = dist(gen); // 한번만 블럭 번호 생성.

		// Initiate Block 
		for (int y = 0; y < BlockHeight; y++) {
			for (int x = 0; x < BlockWidth; x++) {

				printBlock[(y * BlockHeight) + x] = Blocks[numBlock][(y * BlockWidth) + x];

			}
		}
	}
	void left() {
		if(!isCollision(_posX - 1, _posY, 0)) {
			_posX--;
		}
	}
	void right() {
		if (!isCollision(_posX + 1, _posY, 0)) {
			_posX++;
		}
	}
	void down() {
		if (!isCollision(_posX, _posY + 1, 0)) {
			_posY++;
		}
		// Collision : Stack Block
		else {
			for (int y = 0; y < BlockHeight; y++) {
				for (int x = 0; x < BlockWidth; x++)
				{
					if (printBlock[(y * BlockHeight) + x] == 1 && board[_posY + y][_posX + x + blockCursor] == 0) {
						board[_posY + y][_posX + x + blockCursor] = 2;
					}
				}
			}
			initBlock();
		}
	}
	void rotation() {
		if (!isCollision(_posX, _posY, 1)) {
			rotateBlock();
		}
	}
	void dive() {
		while (1) {
			down();

			if (isCollision(_posX, _posY + 1, 0)) {
				break;
			}
		}
	}

	// Draw Block
	void drawBlock() {

		for (int y = 0; y < BlockHeight; y++) {
			for (int x = 0; x < BlockWidth; x++)
			{
				if(printBlock[(y * BlockHeight) + x] == 1 && board[_posY + y][_posX + x + blockCursor] == 0) {
					board[_posY + y][_posX + x + blockCursor] = 1;
				}
			}
		}
	}
	// Erase Shadow
	void eraseBlock() {

		for (int y = 0; y < BlockHeight; y++) {
			for (int x = 0; x < BlockWidth; x++)
			{
				if (printBlock[(y * BlockHeight) + x] == 1) {
					board[_posY + y][_posX + x + blockCursor] = 0;
				}
			}
		}
	}
	void rotateBlock() {

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
				
				case SPACEBAR:
					eraseBlock();
					dive();
					break;
				}
			}
			
	}
	// Collision Check
	bool isCollision(int newX, int newY, int nRot) {
		
		int tempBlock[BlockHeight * BlockWidth] = { 0 };
		int rotBlock[BlockHeight * BlockWidth] = { 0 };
		int BlockSize = sizeof(int) * BlockHeight * BlockWidth;

		// Copy printBlock to tempBlock for checking rotation.
		if (nRot) {
			memcpy(tempBlock, printBlock, BlockSize);
			
			for (int y = 0; y < BlockWidth; y++) {
				for (int x = 0; x < BlockWidth; x++)
				{
					rotBlock[x * (BlockWidth)+(BlockHeight - y - 1)] = tempBlock[(y * BlockWidth) + x];
				}
			}

		}
		//Collision Check
		for (int y = 0; y < BlockHeight; y++) {
			for (int x = 0; x < BlockWidth; x++)
			{
				if (nRot == 1 && (board[newY + y][newX + x + blockCursor] == 3 ||
					board[newY + y][newX + x + blockCursor] == 2) && rotBlock[(y * BlockHeight) + x] == 1) {
					// Collision
					return true;
				}
				else if ((board[newY + y][newX + x + blockCursor] == 3 || board[newY + y][newX + x + blockCursor] == 2)
					&& printBlock[(y * BlockHeight) + x] == 1) {
					// Collision
					return true;
				}
			}
		}
		return false;
	}
	// line Check and Delete Line.
	void lineCheck() {
		int sum = 0;
		for (int y = HEIGHT -1; y > 1; y--) {
			for (int x = 1; x < 11; x++) {
				sum += board[y][x];
			}
			if (sum == 20) {
				for (int x = 1; x < 11; x++) {
					board[y][x] = 0;
				}
				// 블럭 생성 되는 곳 제외 한칸씩 내리기
				for (; y > 4; y--) {
					for (int x = 1; x < 11; x++) {
						board[y][x] = board[y - 1][x];
					}
				}
			}
			else {
				sum = 0;
			}
		}

	}
	// Game Over Condition
	bool gameOver() {
		for (int x = 1; x < 11; x++) {
			if (board[3][x]==2) {
				return true;
			}
		}
		return false;
	}
	// Game Over  Message Rendering
	void overMessage() {
		
		char message[256] = { 0 };
		int meslen = sprintf_s(message, sizeof(message),
			"☆★☆★☆★☆★☆★☆★\n☆      Game Over     ☆\n☆★☆★☆★☆★☆★☆★");
		DWORD dw;
		COORD pos = { 0, 8 };
		SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], pos);
		WriteFile(g_hScreen[g_nScreenIndex],message,meslen, &dw, NULL);
		
		ScreenFlip();

	}

};

// Draw Game Board => Rendering
//void drawBoard() {
//
//	for (int y = 0; y < HEIGHT; y++) {
//		for (int x = 0; x < WIDTH; x++) {
//			if (board[y][x] == 0) std::cout << " .";
//			else if (board[y][x] == 1) std::cout << "■";
//			else if (board[y][x] == 2) std::cout << "■";
//			else if (board[y][x] == 3) std::cout << "▣";
//
//		}
//		std::cout << std::endl;
//	}
//}

// DB Render
void DBRender() {

	DWORD dw = 0;
	COORD pos = { 0, 0 };
	int space = 0;

	for (int y = 0; y < HEIGHT; y++) {
		space = 0;
		for (int x = 0; x < WIDTH; x++) {
			pos.X = x*2;
			pos.Y = y;

			SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], pos);
			WriteFile(g_hScreen[g_nScreenIndex], BlockType[board[y][x]],
				strlen(BlockType[board[y][x]]), &dw, NULL);
		}
	}

}
// Game over Animation
void fillBlock() {
	for (int y = 0; y <= HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			board[y][x] = 1;
		
		}
		DBRender();
		ScreenFlip();
	}
}
void gameLoop() {
	
	BlockClass Block;
	int count = 0;
	const int gravitySpeed = 10;

	while (1)
	{
		if (count % gravitySpeed == 0) {
			Block.eraseBlock();
			Block.down();
		}
		Block.moveBlock();
		Block.lineCheck();
		Block.drawBlock();


		if (Block.gameOver()) {
			fillBlock();
			Block.overMessage();
			Sleep(4000);
			break;
		}

		DBRender();
		ScreenClear();
		ScreenFlip();

		count++;
		
	}

}

int main()
{
	ScreenInit();
	CursorView(0);
	gameLoop();
	ScreenRelease();


	return 0;
}
