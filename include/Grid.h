#ifndef TTTGRID_H
#define TTTGRID_H



#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

#include "Position.h"

namespace TicTacToe
{

enum SpaceState
{
	empty,
	X, 
	O
};

/*
 * class Grid
 * Implements the N^D TTT grid
 * spaces are saved in a vector and best accessed by a Position object
 * Common Sense operations Clear, IsFull, Print
 * CheckforWinner finds a winner if it exists
 * 
 */
class Grid
{
protected:
	unsigned int N, D;
	std::vector<SpaceState> spaces;
	SpaceState winner;
	
	SpaceState CheckForWinner(const Position& index) const ;
	
public:
	Grid(unsigned int N, unsigned int D);
	
	SpaceState& Space(const Position& index);
	SpaceState Space(const Position& index) const;
	
	void Clear();
	bool IsFull() const;
	
	void Print() const;	
	
	SpaceState CheckForWinner();
	SpaceState Winner() const;
	
	friend bool operator ==(const Grid& f1, const Grid& f2);
	
	void Save(std::ostream& o) const;
	void Load(std::istream& in);
	
	unsigned int getN() const;
	unsigned int getD() const;
	
};


}

#endif
