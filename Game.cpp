#include "Game.h"
#include <iostream>
using namespace std;

Game::Game(const Board& b, Player* south, Player* north) : m_Board(b), m_SouthPlayer_P(south),
m_NorthPlayer_P(north), isOver(false) {}
//BOOKKEEPING FUNCTIONS
Game::Game(const Game& other) : m_Board(other.m_Board){
	isOver = other.isOver;
	m_NorthPlayer_P = other.m_NorthPlayer_P;
	m_SouthPlayer_P = other.m_SouthPlayer_P;
}
Game& Game::operator=(const Game& rhs) {
	isOver = rhs.isOver;
	m_Board = rhs.m_Board;
	m_NorthPlayer_P = rhs.m_NorthPlayer_P;
	m_SouthPlayer_P = rhs.m_SouthPlayer_P;
	return *this;
}

//Display the game's board in a manner of your choosing, provided you show the names of the 
//players and a reasonable representation of the state of the board.
void Game::display() const {
	cout << "North Player: " << m_NorthPlayer_P->name() << endl;
	cout << "   "; //leave space so the holes are indented (pots won't be to make the display cleaner)
	for (int i = 1; i <= m_Board.holes(); i++) {
		cout << m_Board.beans(NORTH, i) << "   ";
	}
	cout << endl;
	cout << m_Board.beans(NORTH, POT) << " ";
	for (int i = 1; i <= m_Board.holes(); i++) {
		cout << "    "; //add spaces in between the pots depending on how many spaces we need (more holes means more spaces)
	}
	cout << m_Board.beans(SOUTH, POT) << endl;
	cout << "   ";
	for (int i = 1; i <= m_Board.holes(); i++) {
		cout << m_Board.beans(SOUTH, i) << "   ";
	}
	cout << endl << "South Player: " << m_SouthPlayer_P->name() << endl << endl;
	return;
}

//If the game is over(i.e., the move member function has been called and returned false), set over to true; 
//otherwise, set over to false and do not change anything else.
//If the game is over, set hasWinner to true if the game has a winner, or false if it resulted in a tie.
//If hasWinner is set to false, leave winner unchanged; otherwise, set it to the winning side.
void Game::status(bool& over, bool& hasWinner, Side& winner) const {
	if (isOver == false) {//member variable isOver gets set to true by Game::move if the player whose turn it is can't move
		over = false;
		return;
	}
	over = true;
	int northPot = m_Board.beans(NORTH, POT);
	int southPot = m_Board.beans(SOUTH, POT);
	if (northPot > southPot) {
		hasWinner = true;
		winner = NORTH;
	}
	else if (northPot < southPot) {
		hasWinner = true;
		winner = SOUTH;
	}
	else {
		hasWinner = false;
	}
	return;
}

//Attempt to make a complete move for the player playing side s. "Complete" means that the player sows 
// the seeds from a hole and takes any additional turns required or completes a capture.
// Whenever the player gets an additional turn, you should display the board so someone 
// looking at the screen can follow what's happening. If the move can be completed, return true; 
// if not, because the move is not yet completed but side s has no holes with beans to pick up and sow, 
// sweep any beans in s's opponent's holes into that opponent's pot and return false.
bool Game::move(Side s) {
	if (s == NORTH) {
		int moveChosen = m_NorthPlayer_P->chooseMove(m_Board, NORTH);
		if (moveChosen == -1) {
			isOver = true;
			for (int i = 1; i <= m_Board.holes(); i++) { //move rest of beans where they need to go
				m_Board.moveToPot(SOUTH, i, SOUTH);
			}
			return false;
		}
		//If we get here, there is a move to make.
		Side endS;
		int endH;
		m_Board.sow(NORTH, moveChosen, endS, endH);
		if (endS == NORTH && endH == POT) { //if player should be given another turn 
			display();
			cout << m_NorthPlayer_P->name() << " gets another turn!" << endl << endl;
			return move(s); //recursive call with the same player (they move again)
		}
		else if (endS == NORTH && m_Board.beans(NORTH, endH) == 1) { //if capture should happen
			if (m_Board.beans(SOUTH, endH) != 0) { //capture doesnt happen if nothing to capture
				capture(NORTH, endH);
			}
		}
		return true;
	}
	else if (s == SOUTH) { //almost identical code as above but for the other side
		int moveChosen = m_SouthPlayer_P->chooseMove(m_Board, SOUTH);
		if (moveChosen == -1) {
			isOver = true;
			for (int i = 1; i <= m_Board.holes(); i++) {
				m_Board.moveToPot(NORTH, i, NORTH);
			}
			return false;
		}
		Side endS;
		int endH;
		m_Board.sow(SOUTH, moveChosen, endS, endH);
		if (endS == SOUTH && endH == POT) {
			display();
			cout << m_SouthPlayer_P->name() << " gets another turn!" << endl << endl;
			return move(s);
		}
		else if (endS == SOUTH && m_Board.beans(SOUTH, endH) == 1) {
			if (m_Board.beans(NORTH, endH) != 0) {
				capture(SOUTH, endH);
			}
		}
		return true;
	}
	return true;
}

//Play the game.Display the progress of the game in a manner of your choosing, provided that someone looking at the screen 
//can follow what's happening. If neither player is interactive, then to keep the display from quickly scrolling through 
//the whole game, it would be reasonable periodically to prompt the viewer to press ENTER to continue and not proceed until 
//ENTER is pressed. (The ignore function for input streams is useful here.) Announce the winner at the end of the game. 
//You can apportion to your liking the responsibility for displaying the board between this function and the move function.
void Game::play() {
	cout << "Both players sow counterclockwise, good luck!" << endl << endl;
	display();
	while (isOver == false) {
		if (m_SouthPlayer_P->isInteractive() == false && m_NorthPlayer_P->isInteractive() == false) {
			cout << m_SouthPlayer_P->name() << "'s turn:" << endl;
			cout << "Press ENTER to continue."; //we need a user to be able to see the game, instead of the two
			cin.ignore(10000, '\n');            //AI players playing/finishing the game very, very quickly
			cout << endl;
		}
		move(SOUTH);
		display();
		if (isOver == true)
			break;
		if (m_SouthPlayer_P->isInteractive() == false && m_NorthPlayer_P->isInteractive() == false) {
			cout << m_NorthPlayer_P->name() << "'s turn:" << endl;
			cout << "Press ENTER to continue.";
			cin.ignore(10000, '\n');
			cout << endl;
		}
		move(NORTH);
		display();
		/*if (m_SouthPlayer_P->isInteractive() == false && m_NorthPlayer_P->isInteractive() == false) {
			cout << "Press ENTER to continue.";
			cin.ignore(10000, '\n');
			cout << endl;
		}*/
	}
	bool hasW;
	Side W;
	bool O;
	status(O, hasW, W);
	if (hasW == false)
		cout << "Game ended in a tie.";
	else if (W == NORTH)
		cout << m_NorthPlayer_P->name() << " has won!";
	else
		cout << m_SouthPlayer_P->name() << " has won!";
	return;
}

//Return the number of beans in the indicated hole or pot of the game's board, or -1 if the hole number is invalid. 
//This function exists so that we and you can more easily test your program.
int Game::beans(Side s, int hole) const {
	return m_Board.beans(s, hole);
}

//ADDITIONAL FUNCTIONS

//Completes a capture, where side doingCapture takes the beans from other sides corresponding hole
//and adds them to their own pot.
void Game::capture(Side doingCapture, int hole) {
	if (doingCapture == NORTH) {
		m_Board.moveToPot(SOUTH, hole, NORTH); //move the beans in (SOUTH, hole) to NORTH pot
		m_Board.moveToPot(NORTH, hole, NORTH); //move the 1 new bean in (NORTH, hole) to NORTH pot
	}
	else if (doingCapture == SOUTH) {
		m_Board.moveToPot(NORTH, hole, SOUTH);
		m_Board.moveToPot(SOUTH, hole, SOUTH);
	}
}