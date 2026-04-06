#include "stdafx.h"
#include "Game.h"

Game::Game()
{
	Reset();
}

void Game::Reset()
{
	Console::SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	Console::CursorVisible(false);
	paddle.width = 12;
	paddle.height = 2;
	paddle.x_position = 32;
	paddle.y_position = 30;

	ball.visage = 'O';
	ball.color = ConsoleColor::Cyan;
	ResetBall();

	// TODO #2 - Add this brick and 4 more bricks to the vector
	_brick = CreateBrick(0, 5, ConsoleColor::DarkCyan);
	_brick2 = CreateBrick(15, 5, ConsoleColor::DarkCyan);
	_brick3 = CreateBrick(30, 5, ConsoleColor::DarkCyan);
	_brick4 = CreateBrick(45, 5, ConsoleColor::DarkCyan);
	_brick5 = CreateBrick(60, 5, ConsoleColor::DarkCyan);

	_bricks.clear();
	_bricks.push_back(_brick);
	_bricks.push_back(_brick2);
	_bricks.push_back(_brick3);
	_bricks.push_back(_brick4);
	_bricks.push_back(_brick5);
}

void Game::ResetBall()
{
	ball.x_position = paddle.x_position + paddle.width / 2;
	ball.y_position = paddle.y_position - 1;
	ball.x_velocity = rand() % 2 ? 1 : -1;
	ball.y_velocity = -1;
	ball.moving = false;
}

bool Game::Update()
{
	if (GetAsyncKeyState(VK_ESCAPE) & 0x1)
		return false;

	if (GetAsyncKeyState(VK_RIGHT) && paddle.x_position < WINDOW_WIDTH - paddle.width)
		paddle.x_position += 2;

	if (GetAsyncKeyState(VK_LEFT) && paddle.x_position > 0)
		paddle.x_position -= 2;

	if (GetAsyncKeyState(VK_SPACE) & 0x1)
		ball.moving = !ball.moving;

	if (GetAsyncKeyState('R') & 0x1)
		Reset();

	ball.Update();
	CheckCollision();
	return true;
}

//  All rendering should be done between the locks in this function
void Game::Render() const
{
	Console::Lock(true);
	Console::Clear();
	
	paddle.Draw();
	ball.Draw();

	// TODO #3 - Update render to render all bricks
	for (Box brick : _bricks)
	{
		brick.Draw();
	}
	Console::Lock(false);

	if (_bricks.size() == 0)
	{
		Console::SetCursorPosition(30, 15);
		std::cout << "You Won!! Press 'R' to reset!";
	}

	if (ball.y_position > paddle.y_position)
	{
		Console::SetCursorPosition(30, 15);
		std::cout << "Press 'R' To Reset";
	}
}

void Game::CheckCollision()
{
	// TODO #4 - Update collision to check all bricks
	for (int brick = 0; brick < _bricks.size(); brick++)
	{
		if (_bricks[brick].Contains(ball.x_position + ball.x_velocity, ball.y_position + ball.y_velocity))
		{
			_bricks[brick].color = ConsoleColor(_bricks[brick].color - 1);
			ball.y_velocity *= -1;

			// TODO #5 - If the ball hits the same brick 3 times (color == black), remove it from the vector
			if (_bricks[brick].color == ConsoleColor::Black)
			{
				_bricks.erase(_bricks.begin() + brick);
			}

			if (_bricks.size() == 0)
			{
				ball.y_velocity = 0;
				ball.x_velocity = 0;
			}
		}
	}

	// TODO #6 - If no bricks remain, pause ball and display victory text with R to reset
	
	if (paddle.Contains(ball.x_position + ball.x_velocity, ball.y_velocity + ball.y_position))
	{
		ball.y_velocity *= -1;
	}

	// TODO #7 - If ball touches bottom of window, pause ball and display defeat text with R to reset
	if (ball.y_position > paddle.y_position)
	{
		ball.y_velocity = 0;
		ball.x_velocity = 0;
	}
}

Box Game::CreateBrick(int x, int y, ConsoleColor color)
{
	Box brick;
	brick.width = 10;
	brick.height = 2;
	brick.x_position = x;
	brick.y_position = y;
	brick.doubleThick = true;
	brick.color = color;
	return brick;
}
