#pragma once
#include "Box.h"
#include "Ball.h"
#include "vector"

class Game
{
	Ball ball;
	Box paddle;

	// TODO #1 - Instead of storing 1 brick, store a vector of bricks (by value)
	std::vector<Box> _bricks;
	Box _brick;
	Box _brick2;
	Box _brick3;
	Box _brick4;
	Box _brick5;

public:
	Game();
	bool Update();
	void Render() const;
	void Reset();
	void ResetBall();
	void CheckCollision();
	Box CreateBrick(int x, int y, ConsoleColor color);
};