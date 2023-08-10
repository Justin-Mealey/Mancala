#include "Board.h"
#include <iostream>
using namespace std;
//A Board object knows about the holes, pots, and beans.

// Construct a Board with the indicated number of holes per side (not counting the pot) and initial number of beans per hole. 
// If nHoles is not positive, act as if it were 1; if nInitialBeansPerHole is negative, act as if it were 0.
Board::Board(int nHoles, int nInitialBeansPerHole) : m_holesPerSide(nHoles) {
	if (nHoles <= 0)
		m_holesPerSide = 1;
	m_northPot_P = new Hole(0, 0, "NORTHPOT", NORTH); //name will be useful to check later
	m_southPot_P = new Hole(0, 0, "SOUTHPOT", SOUTH); //making it have position 0, 0 beans, name "SOUTHPOT" and side SOUTH
	//PUT THESE POTS AT ELEMENT ONE OF THE HOLES VECTORS
	m_northHoles_VofP.push_back(m_northPot_P);
	m_southHoles_VofP.push_back(m_southPot_P);
	for (int i = 1; i <= m_holesPerSide; i++) { //add the necessary amount of holes to our north and south vectors
		m_southHoles_VofP.push_back(new Hole(i, nInitialBeansPerHole, "NORMAL", SOUTH));
		m_northHoles_VofP.push_back(new Hole(i, nInitialBeansPerHole, "NORMAL", NORTH));
	}
}
//BOOKKEEPING FUNCTIONS
Board::Board(const Board& other) {
	m_holesPerSide = other.m_holesPerSide;
	m_southPot_P = new Hole(*other.m_southPot_P); //use hole's copy constructor to make a new hole*
	m_northPot_P = new Hole(*other.m_northPot_P); //so they don't point to the same thing
	m_northHoles_VofP.push_back(m_northPot_P);
	m_southHoles_VofP.push_back(m_southPot_P);
	for (int i = 1; i <= m_holesPerSide; i++) {
		m_southHoles_VofP.push_back(new Hole(*other.m_southHoles_VofP[i])); //similar to Board's constructor but 
		m_northHoles_VofP.push_back(new Hole(*other.m_northHoles_VofP[i])); //each hole* is constructed with the corresponding other hole
	}
}
Board::~Board() {//get rid of our dynamically allocated hole pointers
	for (int i = 0; i <= m_holesPerSide; i++) {
		delete m_southHoles_VofP[i];
		delete m_northHoles_VofP[i];
	}
}
Board& Board::operator=(const Board& rhs) {
	if (this != &rhs)
	{
		for (int i = 0; i <= m_holesPerSide; i++) { //avoid memory leak
			delete m_southHoles_VofP[i];
			delete m_northHoles_VofP[i];
		}
		m_southHoles_VofP.clear(); //clear our vectors
		m_northHoles_VofP.clear(); 
		//do process very similar to copy constructor, because now our object is essentially empty
		m_holesPerSide = rhs.m_holesPerSide;
		m_southPot_P = new Hole(*rhs.m_southPot_P);
		m_northPot_P = new Hole(*rhs.m_northPot_P);
		m_northHoles_VofP.push_back(m_northPot_P);
		m_southHoles_VofP.push_back(m_southPot_P);
		for (int i = 1; i <= m_holesPerSide; i++) {
			m_southHoles_VofP.push_back(new Hole(*rhs.m_southHoles_VofP[i]));
			m_northHoles_VofP.push_back(new Hole(*rhs.m_northHoles_VofP[i]));
		}
	}
	return *this;
}
//Return the number of holes on a side (not counting the pot).
int Board::holes() const {
	return m_holesPerSide;
}
//Return # beans in hole or pot or -1 if the hole number is invalid.
int Board::beans(Side s, int hole) const {
	if (isValidHole(s, hole)) {
		if (s == NORTH)
			return (m_northHoles_VofP[hole])->beansInHole(); //one of hole's functions that returns how many beans it has
		return (m_southHoles_VofP[hole])->beansInHole();
	}
	return -1;
}
//Return the total number of beans in all the holes on the indicated side, not counting the beans in the pot.
int Board::beansInPlay(Side s) const {
	int totalBeans = 0;
	for (int i = 1; i <= m_holesPerSide; i++) {
		totalBeans += beans(s, i);
	}
	return totalBeans;
}
//Return the total number of beans in the game, including any in the pots.
int Board::totalBeans() const {
	int totalBeans = 0;
	totalBeans += beansInPlay(NORTH);
	totalBeans += beansInPlay(SOUTH);
	totalBeans += beans(NORTH, POT);
	totalBeans += beans(SOUTH, POT);
	return totalBeans;
}
//If the hole indicated by (s,hole) is empty or invalid or a pot, this function returns false without changing anything. 
//Otherwise, it will return true after sowing the beans: the beans are removed from hole (s,hole) and sown counterclockwise, 
//including s's pot if encountered, but skipping s's opponent's pot. 
//The function sets the parameters endSide and endHole to the side and hole where the last bean was placed. 
//(This function does not make captures or multiple turns; different Kalah variants have different rules about these issues, 
//so dealing with them should not be the responsibility of the Board class.)
bool Board::sow(Side s, int hole, Side& endSide, int& endHole) {
	if (isValidHole(s, hole) == false) //check validity
		return false;
	if (hole == POT)	//check if pot
		return false;
	if (beans(s, hole) == 0)  //check if empty
		return false;
	if (s == NORTH) { 
		int stash = m_northHoles_VofP[hole]->beansInHole(); //put the beans in our "stash" or hand to sow from
		m_northHoles_VofP[hole]->setToZero();				//hole we're sowing from has 0 beans now
		Hole* at = m_northHoles_VofP[hole - 1]; 
		//at always points to where we need to put a bean, until we finish sowing and at points to the last hole we dropped a bean into
		while (stash != 0) {
			at->addBeansToHole(1);
			stash--;
			if (stash == 0)
				break; //we dont want to change at or do anything else, we are done sowing
			else if (at->name() == "NORTHPOT") { //special case, at jumps to start of south's holes
				at = m_southHoles_VofP[1];
			}
			else if (at->side() == NORTH) { //if we are still north we just move one to the left
				at = m_northHoles_VofP[at->pos() - 1];
			}
			else { //WE ARE SOUTH
				if (at->pos() == m_holesPerSide) { //special case at the end of south's hole, we skip past south's pot and jump to rightmost north hole
					at = m_northHoles_VofP[m_holesPerSide];
				}
				else { //move one to the right
					at = m_southHoles_VofP[at->pos() + 1];
				}
			}
		}
		//once done, at points to ending hole
		endSide = at->side(); //side and pos are two of hole's getter functions
		endHole = at->pos();
	}
	else { //SOUTH - different special cases for NORTH vs SOUTH
		int stash = m_southHoles_VofP[hole]->beansInHole();
		m_southHoles_VofP[hole]->setToZero();
		Hole* at;
		if (hole == m_holesPerSide)
			at = m_southHoles_VofP[POT];
		else
			at = m_southHoles_VofP[hole + 1];
		while (stash != 0) {
			at->addBeansToHole(1);
			stash--;
			if (stash == 0)
				break;
			else if (at->name() == "SOUTHPOT") { //after pot we go to end of north's holes (rightmost)
				at = m_northHoles_VofP[m_holesPerSide];
			}
			else if (at->side() == SOUTH) { 
				if (at->pos() == m_holesPerSide) { //this is the end of the vector, but the pot is at element 0
					at = m_southHoles_VofP[POT]; //instead of incrementing by one, we set at to element POT (which is 0)
				}
				else { //normal case where we just move to the right
					at = m_southHoles_VofP[at->pos() + 1];
				}
			}
			else { //WE ARE NORTH
				if (at->pos() == 1) { //we want to skip past NORTH's pot and go to beginning of south's holes
					at = m_southHoles_VofP[1];
				}
				else { //otherwise we just move to the left one
					at = m_northHoles_VofP[at->pos() - 1];
				}
			}
		}
		endSide = at->side();
		endHole = at->pos();
	}
	return true;
}
//If the indicated hole is invalid or a pot, return false without changing anything.
//Otherwise, move all the beans in hole(s, hole) into the pot belonging to potOwner and return true.
bool Board::moveToPot(Side s, int hole, Side potOwner) {
	if (isValidHole(s, hole) == false)
		return false;
	if (hole == POT)
		return false;
	int beansToMove = beans(s, hole);
	if (s == NORTH) {
		(m_northHoles_VofP[hole])->setToZero();
		if (potOwner == SOUTH) //Happens during a capture
			m_southPot_P->addBeansToHole(beansToMove); //one of hole's functions that adds the beans to the hole
		else {
			m_northPot_P->addBeansToHole(beansToMove); //End of the game when other player can't make a turn
		}
	}
	else {//Side is SOUTH
		(m_southHoles_VofP[hole])->setToZero();
		if (potOwner == NORTH)
			m_northPot_P->addBeansToHole(beansToMove);
		else {
			m_southPot_P->addBeansToHole(beansToMove);
		}
	}
	return true;
}
//If the indicated hole is invalid or beans is negative, this function returns false without changing anything.
//Otherwise, it will return true after setting the number of beans in the indicated hole or pot to the value of the third parameter. 
//(This could change what beansInPlay and totalBeans return if they are called later.) 
//This function exists solely so that we and you can more easily test your program : None of your code that implements the 
//member functions of any class is allowed to call this function directly or indirectly. (We'll show an example of its use below.)
bool Board::setBeans(Side s, int hole, int beans) {
	if (isValidHole(s, hole) == false)
		return false;
	if (beans < 0)
		return false;
	if (s == NORTH) {
		(m_northHoles_VofP[hole])->setBeansInHole(beans); //another hole function, sets the beans in the hole to the beans argument
	}
	else {
		(m_southHoles_VofP[hole])->setBeansInHole(beans);
	}
	return true;
}

//EXTRA FUNCTIONS

//Checks if requested hole is valid, returns true if it is, otherwise false
bool Board::isValidHole(Side s, int hole) const {
	if (s != NORTH && s != SOUTH)//Side should always either be NORTH or SOUTH
		return false;
	if (hole < 0 || hole > m_holesPerSide) //hole cant be negative or more than the number of holes on the side
		return false;
	return true;
}