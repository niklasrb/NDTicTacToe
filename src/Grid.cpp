
#include "../include/Grid.h"

namespace TicTacToe
{


Grid::Grid(unsigned int N, unsigned int D) : N(N), D(D), winner(SpaceState::empty)
{
	spaces.resize((int)pow(N, D));
	Clear();
}

SpaceState& Grid::Space(const Position& pos)
{
	return spaces.at(pos.GetSpaceIndex());
}

SpaceState Grid::Space(const Position& pos) const
{
	return spaces.at(pos.GetSpaceIndex());
}

void Grid::Clear() 
{
	for(unsigned int i = 0; i < spaces.size(); i++) 
		spaces.at(i) = SpaceState::empty;
	winner = SpaceState::empty;
}

void Grid::Print() const 
{
	Position pos(*this); unsigned int i = 0;
	while(pos)
	{
		std::cout << pos << ": " << Space(pos) << '\t';
		if((i+1) % N == 0) std::cout << std::endl;
		i++; pos++; 
	}
}

bool Grid::IsFull() const
{
	Position pos(*this);
	while(pos) {
		if(Space(pos) == SpaceState::empty)
			return false;
		pos++;
	}
	return true;
}


SpaceState Grid::Winner() const
{
	return winner;
}

bool operator ==(const Grid& f1, const Grid& f2)
{
	if(f1.N != f2.N || f1.D != f2.D)
		return false;
	
	Position pos(f1);
	while(pos) {	
		if(f1.Space(pos) != f2.Space(pos)) 
			return false;
		pos++;
	}
	return true;
}

void Grid::Save(std::ostream& o) const
{
	o << N << '\t' << D << '\t' << (int)winner << std::endl;
	Position pos(*this);
	while(pos) {
		o << (int)Space(pos) << '\t';
		pos++;
	}
	o << std::endl;
}

void Grid::Load(std::istream& in)
	{
		assert(in.good());
		int buf; 
		in >> buf;	assert(in.good() && buf > 0);
		this->N = buf;
		in >> buf;	assert(in.good() && buf > 0);
		this->D = buf;
		in >> buf;	assert(in.good());
		this->winner = (SpaceState)buf;
		spaces.resize((int)pow(N, D));
		Position pos(*this);
		while(pos) {
			in >> buf; assert(in.good());
			Space(pos) = (SpaceState)buf;
			pos++;
		}
	}

SpaceState Grid::CheckForWinner(const Position& start) const 
{
	SpaceState ss = Space(start);
	if(ss == SpaceState::empty)	// check if starting position is filled
		return ss;
		
	Direction allowedDirections = start.IndicateCorners();	// find directions that start a diagonal
	if(!allowedDirections.NonZero())
		return SpaceState::empty;
	
	Direction subDirection = Direction(*this);
	Position pos = start; bool allSpacesFilled;
	
	for(;subDirection.NonZero(); subDirection++) {	// test all subdirections
		if(!allowedDirections.AllowsSubdirection(subDirection))
			continue;
		pos = start;
		allSpacesFilled = true;
		while(pos += subDirection) {	// go along diagonal and check all spaces
			if(Space(pos) != ss) {
				allSpacesFilled = false;
				break;
			}
		}
		if(allSpacesFilled)
			return ss;
		
	}
	
	return SpaceState::empty;
}

SpaceState Grid::CheckForWinner()
{
	if(winner != SpaceState::empty)
		return winner;
	
	Position start(*this);
	SpaceState ss;
	while(start)	{	// Take every space as a possible starting point
		ss = CheckForWinner(start);
		if(ss != SpaceState::empty) {
			winner = ss;
			break;
		}
		start++;
	}
	return winner;
}

unsigned int Grid::getN() const
{
	return N;
}
unsigned int Grid::getD() const
{
	return D;
}


}
