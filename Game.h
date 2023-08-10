#ifndef GAME_H
#define GAME_H

#include "Player.h"

class Game {
public:
	Game(const Board& b, Player* south, Player* north);
	Game(const Game& other);
	Game& operator=(const Game& rhs);
	void display() const;
	void status(bool& over, bool& hasWinner, Side& winner) const;
	bool move(Side s);
	void play();
	int beans(Side s, int hole) const;
private:
	bool isOver;
	Board m_Board;
	Player* m_SouthPlayer_P;
	Player* m_NorthPlayer_P;
	void capture(Side doingCapture, int hole); //completes a capture
};


#endif
