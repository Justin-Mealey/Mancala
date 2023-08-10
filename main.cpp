#include "Game.h"
#include "Player.h"
#include "Board.h"
#include "Side.h"
#include <iostream>
#include <cassert>
using namespace std;

int main()
{
	HumanPlayer p1("Player 1");
	HumanPlayer p2("Player 2");
	Board b(6, 4);
	Game g(b, &p1, &p2);
	g.play();
}