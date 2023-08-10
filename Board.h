#ifndef BOARD_H
#define BOARD_H

#include <string>
#include "Side.h"
#include <vector>

class Board {
public:
	Board(int nHoles, int nInitialBeansPerHole);
	Board(const Board& other);
	~Board();
	Board& operator=(const Board& rhs);
	int holes() const;
	int beans(Side s, int hole) const;
	int beansInPlay(Side s) const;
	int totalBeans() const;
	bool sow(Side s, int hole, Side& endSide, int& endHole);
	bool moveToPot(Side s, int hole, Side potOwner);
	bool setBeans(Side s, int hole, int beans);
private:
	int m_holesPerSide;
	class Hole { //class inside Board to represent a hole, a hole has a position, side, bean# and name
	public:
		Hole(int position, int ogNumOfBeans, std::string name, Side s) : m_pos(position), m_beans(ogNumOfBeans), m_name(name), m_s(s) {
			if (ogNumOfBeans < 0)
				m_beans = 0;
		} //set values for a hole
		Hole(const Hole& other) { //copy constructor for a hole
			m_s = other.m_s;
			m_pos = other.m_pos;
			m_beans = other.m_beans;
			m_name = other.m_name;
		}
		int pos() const { return m_pos; }
		int beansInHole() const { return m_beans; }
		void setBeansInHole(int numbeans) { m_beans = numbeans; }
		void addBeansToHole(int numbeans) { m_beans += numbeans; }
		void setToZero() { m_beans = 0; }
		std::string name() const { return m_name; }
		Side side() const { return m_s; }
	private:
		Side m_s;
		int m_pos;
		int m_beans;
		std::string m_name;
	};
	std::vector<Hole*> m_southHoles_VofP; 
	std::vector<Hole*> m_northHoles_VofP;
	Hole* m_southPot_P;
	Hole* m_northPot_P;
	bool isValidHole(Side s, int hole) const; //see's if Side is something crazy of hole# isn't valid
};

#endif