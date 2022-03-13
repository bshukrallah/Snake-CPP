#include <iostream>
#include <Windows.h>
#include <list>
#include <thread>
#include<conio.h>

int nScreenWidth = 120;
int nScreenHeight = 30;

struct sSnakeSegment
{
	int x;
	int y;
};

int main()
{
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) {
		screen[i] = L' ';
	}
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	while (1) 
	{
		std::list<sSnakeSegment> snake = { {60,15},{61,15},{62,15},{63,15},{64,15},{65,15},{66,15},{67,15},{68,15},{69,15} };
		int nFoodX = 30;
		int nFoodY = 15;
		int nScore = 0;
		int nSnakeDirection = 3;
		int bDead = false;
		bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false;

		while (!bDead)
		{
			//Get Input
			auto t1 = std::chrono::system_clock::now();
			while ((std::chrono::system_clock::now() - t1) < ((nSnakeDirection %2==1) ? std::chrono::milliseconds(100):std::chrono::milliseconds(140)))
			{

				bKeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
				bKeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;


				if (bKeyRight && !bKeyRightOld) {
					nSnakeDirection++;
					if (nSnakeDirection == 4) nSnakeDirection = 0;
				}

				if (bKeyLeft && !bKeyLeftOld) {
					nSnakeDirection--;
					if (nSnakeDirection == -1) nSnakeDirection = 3;
				}

				bKeyRightOld = bKeyRight;
				bKeyLeftOld = bKeyLeft;
			}

			//Game Logic
			//Update Snake Position
			switch (nSnakeDirection)
			{
			case 0://Up
				snake.push_front({ snake.front().x, snake.front().y - 1 });
				break;
			case 1: //Right
				snake.push_front({ snake.front().x + 1, snake.front().y });
				break;
			case 2: //Down
				snake.push_front({ snake.front().x, snake.front().y + 1 });
				break;
			case 3: //Left
				snake.push_front({ snake.front().x - 1, snake.front().y });
				break;
			}

			//Collision Detection
			//Collision Detect Snake v World
			
			if (snake.front().x < 0 || snake.front().x >= nScreenWidth) 
				bDead = true;
			if (snake.front().y < 3 || snake.front().y >= nScreenHeight) 
				bDead = true; //3 because of the border at top of screen

			//Food Collision

			if (snake.front().x == nFoodX && snake.front().y == nFoodY)
			{
				nScore++;
				while (screen[nFoodY * nScreenWidth + nFoodX] != L' ')
				{
					nFoodX = std::rand() % nScreenWidth;
					nFoodY = (std::rand() % (nScreenHeight - 3)) + 3;

				}
				for (int i = 0; i < 5; i++) {
					snake.push_back({ snake.back().x, snake.back().y });
				}
			}

			//Snake v Snake
			for (std::list<sSnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)
				if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
					bDead = true;


			//Remove one from end of snake
			snake.pop_back();

			//Display to Player
			//Clear Screen

			for (int i = 0; i < nScreenWidth * nScreenHeight; i++) {
				screen[i] = L' ';
			}

			//Draw Stats and Border

			for (int i = 0; i < nScreenWidth; i++)
			{
				screen[i] = L'=';
				screen[2 * nScreenWidth + i] = L'=';
			}
			wsprintf(&screen[nScreenWidth + 5], L"- S N A K E -    Score: %d", nScore);

			//Snake Body
			for (auto s : snake) {
				screen[s.y * nScreenWidth + s.x] = bDead ? L'▪' : L'●';
			}

			//Snake Head
			screen[snake.front().y * nScreenWidth + snake.front().x] = bDead ? L'○' : L'☻';

			//Food

			screen[nFoodY * nScreenWidth + nFoodX] = L'%';



			if (bDead)
				wsprintf(&screen[15 * nScreenWidth + 40], L"   PRESS 'SPACE' TO PLAY AGAIN");

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth* nScreenHeight, { 0,0 }, & dwBytesWritten);

		}

		//Wait for input after death
		while ((0X8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);

	}
	return 0;
}