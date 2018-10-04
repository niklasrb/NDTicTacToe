#ifndef TTTPOSITION_H
#define TTTPOSITION_H

#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

namespace TicTacToe
{

class Grid;
class Direction;

/*
 * class Position
 * Defines a position (field?) on the TicTacToe grid
 * The entries are from [1, N] in a D dimensional vector (which is externally accessed in the range [1, D] )
 * Allows easy conversion to an index for the internal mechanics of the grid class
 * One can cycle through all possible positions with the ++ operator
 * And one can move around the board with the help of directions
 * there is one invalid direction (N+1, 1,..,1) which is reached by ++ operator, and can be tested for with the bool operator
 */
class Position
{
friend class Grid;
friend class Direction;
protected:
	unsigned int N, D;
	std::vector<unsigned int> positionMarkers;
	
	bool IsValid(const std::vector<unsigned int>& markers) const;
	
public:
	Position(unsigned int N, unsigned int D, const std::vector<unsigned int>& markers);
	Position(unsigned int N, unsigned int D);
	Position(const Grid& g);
	
	bool Set(const unsigned int d, const unsigned int n);
	bool Set(const std::vector<unsigned int>& markers);
	unsigned int at(unsigned int d) const;
	
	unsigned int GetSpaceIndex() const;
	
	void Print() const;
	friend std::ostream& operator <<(std::ostream& o, const Position& pos);
	friend std::istream& operator >>(std::istream& in, Position& pos);
	Position& ReadFromCmd();
	
	friend Position& operator ++(Position& pos, int dummy);
	friend bool operator +=(Position& pos, const Direction& dir);
	
	operator bool() const;
	
	Direction IndicateCorners() const;
	
	friend bool operator ==(const Position& pos1, const Position& pos2);
	
	//friend operator <<(std::ostream& os, SpaceState ss)	
};

/*
 * class Direction
 * Allows for movement on the Grid
 * A position and a direction can be added together
 * Internally represented by a vector whose entries can be -1, 0, 1, representing direction in the different dimensions
 * One can cycle through all directions with ++ operator
 * The idea is that at a given cornerpiece there are allowed directions to move from which give a diagonal on the grid
 * 		for example on the piece (1, 2, N, 1) the allowed directions are (1, 0, -1, 1)
 * Something is a subdirection if it is in the subspace of the allowed directions
 * 		in the example above these are valid subdirections (1, 0, 0, 0), (1, 0, -1, 0), (0, 0, -1, 1) ,  while the following are NOT (1, 1, 0, 0), (-1, 0, 0, 0), (1, 0, -1, -1)
 * By cycling through all possible subdirections on can test all diagonals originating from a cornerpiece
 */
class Direction
{
friend class Position;
protected:
	unsigned int N, D;
	std::vector<int> directionMarkers;
	
public:
	Direction(unsigned int N, unsigned int D, const std::vector<int>& makers);
	Direction(unsigned int N, unsigned int D);
	Direction(const Grid& g);
	
	void Print() const;
	friend std::ostream& operator <<(std::ostream& o, const Direction& dir);
	
	bool AllowsSubdirection(const Direction& subDir) const;
	bool NonZero() const;
	
	friend Direction operator ++(Direction& dir, int dummy);
	friend bool operator +=(Position& pos, const Direction& dir);
	
	friend std::ostream& operator <<(std::ostream& os, const Direction& dir);
	
	operator bool() const;
};




}

#endif
