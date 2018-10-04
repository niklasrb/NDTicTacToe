
#include "../include/Position.h"
#include "../include/Grid.h"

namespace TicTacToe
{

Position::Position(unsigned int N, unsigned int D, const std::vector<unsigned int>& markers) : N(N), D(D), positionMarkers(markers)
{
	assert(D == markers.size());
	assert(this->IsValid(markers));
}

Position::Position(unsigned int N, unsigned int D) : N(N), D(D)
{
	positionMarkers.resize(D);
	for(unsigned int i = 0; i < D; i++) 
		positionMarkers.at(i) = 1;
}

Position::Position(const Grid& g) : Position(g.getN(), g.getD())
{ }

unsigned int Position::GetSpaceIndex() const
{
	unsigned int i = 0;
	for(unsigned int d = 1; d <= D; d++)
		i += (positionMarkers.at(d-1) - 1)*pow(N, d-1);
	return i;
}

bool Position::IsValid(const std::vector<unsigned int>& markers) const
{
	for(unsigned int i = 0; i < D; i++) 
		if(markers.at(i) < 1 || markers.at(i) > N)
			return false;
	return true;
}
	
void Position::Print() const
{
	std::cout << this << std::endl;
}

bool Position::Set(const unsigned int d, const unsigned int n)
{
	if(d == 0 || d > D) return false;
	if(n == 0 || n > N) return false;
	positionMarkers.at(d-1) = n;
	return true;
}

bool Position::Set(const std::vector<unsigned int>& markers)
{
	if(!IsValid(markers))
		return false;
	this->positionMarkers = markers;
	return true;
}

unsigned int Position::at(unsigned int d) const
{
	assert(d > 0 && d <= D);
	return positionMarkers.at(d-1);
}

std::ostream& operator <<(std::ostream& o, const Position& pos)
{
	o << "("; 
	for(unsigned int i = 0; i < pos.D; i++) 
		o << pos.at(i+1) << (i < pos.D-1 ? ", " : ")");
	return o;
}

std::istream& operator >>(std::istream& in, Position& pos)
{
	assert(in.good());
	char cbuf; unsigned int ibuf; 
	in >> cbuf;
	assert(cbuf == '(');
	for(unsigned int i = 0; i < pos.D; i++)  {
		in >> ibuf; 
		assert(in.good());
		pos.Set(i+1, ibuf);
		in >> cbuf;
		if(i < pos.D-1) 
			assert(in.good() && cbuf == ',');
	}
	
	return in;
}

bool operator +=(Position& pos, const Direction& dir)
{
	assert(pos.N == dir.N && pos.D == dir.D);
	std::vector<unsigned int> markers = pos.positionMarkers;
	for(unsigned int i = 0; i < pos.D; i++) 
		markers.at(i) += dir.directionMarkers.at(i);
	if(!pos.IsValid(markers))
		return false;
	pos.positionMarkers = markers;
	return true;
}

Position& operator ++(Position& pos, int dummy)
{
	if(pos.positionMarkers.at(0) > pos.N) {
		pos.positionMarkers.at(0) = 1;
		return pos;
	}
	pos.positionMarkers.at(pos.D-1) += 1;
	for(unsigned int d = pos.D-1; d > 0; d--) {
		if(pos.positionMarkers.at(d) > pos. N) {
			pos.positionMarkers.at(d) = 1;
			pos.positionMarkers.at(d-1) += 1;
		}
	}
	return pos;
}

Direction Position::IndicateCorners() const
{
	std::vector<int> cp; cp.resize(D);
	for(unsigned int i = 0; i < D; i++) {
		if(positionMarkers.at(i) == 1) 
			cp.at(i) = 1;
		else if(positionMarkers.at(i) == N) 
			cp.at(i) = -1;
		else 
			cp.at(i) = 0;
	}
	return Direction(N, D, cp);
}

Position& Position::ReadFromCmd()
{
	unsigned int buf;
	std::cout << "Please enter index, seperating the numbers with spaces: ";
	for(unsigned int i = 0; i < this->D; i++) { 
		std::cin >> buf;
		while(!std::cin.good() || buf < 1 || buf > this->N) {
			std::cout << "Please reenter dimension " << i+1 << ": "; 
			std::cin >> buf;
		}
		positionMarkers.at(i) = buf;
	}
	return *this;
}

bool operator ==(const Position& pos1, const Position& pos2)
{
	if(pos1.D != pos2.D || pos1.N != pos2.N)
		return false;
	for(unsigned int i = 1; i <= pos1.D; i++) 
		if(pos1.at(i) != pos2.at(i)) 
			return false;
	return true;
}


Position::operator bool() const
{
	if(positionMarkers.at(0) > N) 
		return false;
	return IsValid(positionMarkers);
}

///
///	Direction
///

Direction::Direction(unsigned int N, unsigned int D, const std::vector<int>& markers) : N(N), D(D), directionMarkers(markers)
{
	assert(D == directionMarkers.size());
	for(unsigned int i = 0; i < D; i++) 
		assert( directionMarkers.at(i) <= 1 &&  directionMarkers.at(i) >= -1);
}

Direction::Direction(unsigned int N, unsigned int D) : N(N), D(D)
{
	directionMarkers.resize(D);
	for(unsigned int i = 0; i < D; i++) 
		directionMarkers.at(i) = 0;
	directionMarkers.at(D-1) = 1;
}

Direction::Direction(const Grid& g) : Direction(g.getN(), g.getD())
{	}

bool Direction::AllowsSubdirection(const Direction& subDir) const
{
	assert(this->D == subDir.D && this->N == subDir.N);
	bool check = true; 
	for(unsigned int d = 0; d < D; d++) 
	{	
		if(directionMarkers.at(d) == 0 && subDir.directionMarkers.at(d) != 0) 
			check = false;
		if(directionMarkers.at(d) < 0 && subDir.directionMarkers.at(d) > 0) 
			check = false;
		if(directionMarkers.at(d) > 0 && subDir.directionMarkers.at(d) < 0) 
			check = false;
	}
	return check;
}

bool Direction::NonZero() const
{
	bool c = false;
	for(unsigned int i = 0; i < D; i++) 
		if(directionMarkers.at(i) != 0) 
			c = true;
	return c;
}

Direction::operator bool() const
{
	return NonZero();
}

void Direction::Print() const
{
	std::cout << this << std::endl;
}

Direction operator ++(Direction& dir, int dummy)
{
	dir.directionMarkers.at(dir.D-1) += 1;
	for(unsigned int d = dir.D-1; d > 0; d--) {
		if(dir.directionMarkers.at(d) > 1) {
			dir.directionMarkers.at(d) = -1;
			dir.directionMarkers.at(d-1) += 1;
		}
	}
	if(dir.directionMarkers.at(0) > 1) 
		dir.directionMarkers.at(0) = -1;
	return dir;
}

std::ostream& operator <<(std::ostream& os, const Direction& dir)
{
	os << "("; 
	for(unsigned int i = 0; i < dir.D; i++) 
		os << dir.directionMarkers.at(i) << (i < dir.D-1 ? ", " : ")");
	return os;
}

}
