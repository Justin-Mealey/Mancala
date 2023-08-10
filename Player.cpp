#include "Player.h"
#include <iostream>
#include <vector>
using namespace std;
// PLAYER	PLAYER	PLAYER	PLAYER	PLAYER	PLAYER
// PLAYER	PLAYER	PLAYER	PLAYER	PLAYER	PLAYER
// PLAYER	PLAYER	PLAYER	PLAYER	PLAYER	PLAYER
// PLAYER	PLAYER	PLAYER	PLAYER	PLAYER	PLAYER
Player::Player(std::string name) : m_name(name) {}

std::string Player::name() const {
	return m_name;
}

bool Player::isInteractive() const { //most types of players aren't interactive
	return false;
}

Player::~Player() {}

// HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER
// HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER
// HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER
// HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER		HUMANPLAYER

HumanPlayer::HumanPlayer(std::string name) : Player(name) {}
bool HumanPlayer::isInteractive() const { //override Players definition because HumanPlayer is the only interactive one
	return true;
}
int HumanPlayer::chooseMove(const Board& b, Side s) const {
	if (canMakeMove(b, s)) { //if there is a possible move
		int move;
		do {
			std::cout << name() << ", enter your move: "; //asking for and storing input
			std::cin >> move;
			std::cout << std::endl; 
			if (move < 1 || move > b.holes()) //invalid input
				std::cout << "Move must be between 1 and " << b.holes() << "." << std::endl;
			else if (b.beans(s, move) < 1) //invalid input
				std::cout << "This hole is empty." << std::endl;
		} while (move < 1 || move > b.holes() || b.beans(s, move) < 1); //keep asking until input is valid
		return move; //return valid input
	}
	return -1; //if we can't make a move return -1
}

bool Player::canMakeMove(const Board& b, Side s) const {
	for (int i = 1; i <= b.holes(); i++) { //loop over all holes, if any hole has more than 0 beans, return true
		if (b.beans(s, i) > 0)
			return true;
	}
	return false;
}

// BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER
// BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER
// BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER
// BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER	BADPLAYER

BadPlayer::BadPlayer(std::string name) : Player(name) {}

//if the player were to be playing side s and had to make a move given board b, the function returns the 
//move the player would choose. If no move is possible, return -1.
int BadPlayer::chooseMove(const Board& b, Side s) const { //returns 1 if 1 is a possible move, if not then 2, if not then 3, etc.
	int i = 1;
	for (i; i <= b.holes(); i++) {
		if (b.beans(s, i) > 0) {
			return i;
		}
	}
	return -1; //still -1 of no possible moves
}

// SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER
// SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER
// SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER
// SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER		SMARTPLAYER

SmartPlayer::SmartPlayer(std::string name) : Player(name) {}

int SmartPlayer::chooseMove(const Board& b, Side s) const {
	if (canMakeMove(b, s) == false)
		return -1;
	possibleMoves* Root = new possibleMoves[b.holes()];
	int sizeOfRoot = 0;
	getRootPossibilities(s, b, Root, sizeOfRoot, b.holes()); 
	//Root array now includes all possible moves from the root and sizeOfRoot is the number of possible moves we found

	int movechosen = Root[0].hole();
	int maxrating = Root[0].rating();
	for (int i = 1; i < sizeOfRoot; i++) {
		if (Root[i].rating() > maxrating) {
			movechosen = Root[i].hole();
			maxrating = Root[i].rating();
		}
	}
	delete[] Root;
	return movechosen;
}

//EXTRA FUNCTIONS
int SmartPlayer::evaluate(Side s, const Board& b) const {
	Side mySide; //for simplicity make mySide what we are playing as and otherSide as opponent
	Side otherSide;
	if (s == NORTH) {
		mySide = NORTH;
		otherSide = SOUTH;
	}
	else {
		mySide = SOUTH;
		otherSide = NORTH;
	}
	int rating = 0;
	rating += b.beans(mySide, 0); //rating is our pot - their pot
	rating -= b.beans(otherSide, 0);
	if (canMakeMove(b, otherSide) == false) { //if opponent cant make a move (it will be opponents turn when function is called) 
		if (b.beansInPlay(mySide) + b.beans(mySide, 0) > b.beans(otherSide, 0)) //if mySide will win
			rating = 999999999;
		else if (b.beansInPlay(mySide) + b.beans(mySide, 0) < b.beans(otherSide, 0)) //if mySide loses
			rating = -999999999;
		else { //if we will tie
			rating = 0;
		}
	}
	//cout << endl << "RETURNING RATING: " << rating << endl;
	return rating;
}

void SmartPlayer::getRootPossibilities(Side s, Board b, possibleMoves* root, int& size, int maxsize) const {
	int counter = 0;
	for (int i = 1; i <= maxsize; i++) {
		if (b.beans(s, i) == 0)
			continue;
		Board temp(b);
		makeFakeMove(s, temp, i);
		int resultRating = evaluate(s, temp);//how good the board is looking after our move
		possibleMoves thismove(i, resultRating);
		//cerr << "Made a possible move of hole " << thismove.hole() << " and a rating of: " << thismove.rating() << endl;
		root[counter] = thismove;
		counter++; size++;
	}
}

void SmartPlayer::makeFakeMove(Side s, Board &temp, int hole) const{
	if (temp.beans(s, hole) == 0)
		return;
	Side endS; int endH;
	temp.sow(s, hole, endS, endH);
	if (endS == s && endH == POT) {
		int i = 1;
		for (i; i <= temp.holes() + 1; i++) {
			if (i == temp.holes() + 1) {
				i = -1;
				break;
			}
			if (temp.beans(s, i) != 0) {
				break;
			}
		}
		if (i == -1) {
			return;
		}
		makeFakeMove(s, temp, i);
	}
	else if (endS == s && temp.beans(s, endH) == 1) {
		if (temp.beans(opponent(s), endH) != 0) {
			temp.moveToPot(opponent(s), endH, s);
			temp.moveToPot(s, endH, s);
		}
	}
}
