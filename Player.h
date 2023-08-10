#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "Board.h"


class Player {
public:
	Player(std::string name);
	std::string name() const;
	virtual bool isInteractive() const;
	virtual int chooseMove(const Board& b, Side s) const = 0;
	virtual ~Player();
private:
	std::string m_name;
protected: //derived classes get access, but nothing else.
	bool canMakeMove(const Board& b, Side s) const; //allows derived classes to see if they can make a move
};

class HumanPlayer : public Player {
public:
	HumanPlayer(std::string name);
	virtual bool isInteractive() const;
	virtual int chooseMove(const Board& b, Side s) const;
};

class BadPlayer : public Player {
public:
	BadPlayer(std::string name);
	virtual int chooseMove(const Board& b, Side s) const;
};

class SmartPlayer : public Player {
public:
	SmartPlayer(std::string name);
	virtual int chooseMove(const Board& b, Side s) const;
private:
	class possibleMoves {
	public:
		possibleMoves() { m_hole = 16; m_rating = 16; }
		possibleMoves(int h, int r) { m_hole = h; m_rating = r; }
		int hole() { return m_hole; }
		int rating() { return m_rating; }
	private:
		int m_hole;
		int m_rating;
	};
	//void chooseMove(Side player, Board b, int &bestHole, int &rating, int depth) const; //overloaded chooseMove doing recursive calls
	int evaluate(Side s, const Board& b) const; //evaluate function that determines how good a board position is for a player
	void getRootPossibilities(Side s, Board b, possibleMoves* root, int &size, int maxsize) const;
	void makeFakeMove(Side s, Board &temp, int hole)const;
};


#endif