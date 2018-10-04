#ifndef TTTFIELD_H
#define TTTFIELD_H



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
	Y
};

class Field
{
protected:
	unsigned int N, D;
	std::vector<SpaceState> spaces;
	SpaceState tss;
	
	bool Winner(SpaceState ss, const Position& index) const ;
	bool Winner(SpaceState ss) const;
	
public:
	Field(unsigned int N, unsigned int D) : N(N), D(D)
	{
		spaces.resize((int)pow(N, D));
		Empty();
	}
	
	TSpaceState& Space(const TIndex& index)
	{
		return Space(index.GetIndex());
	}
	
	TSpaceState Space(const TIndex& index) const
	{
		return Space(index.GetIndex());
	}
	
	TSpaceState& Space(const unsigned int i)
	{
		assert(i <= pow(N,D));
		return spaces.at(i);
	}
	
	TSpaceState Space(const unsigned int i) const
	{
		assert(i <= pow(N,D));
		return spaces.at(i);
	}
	
	void Empty() 
	{
		for(unsigned int i = 0; i < spaces.size(); i++) spaces.at(i) = TSpaceState::empty;
		tss = TSpaceState::empty;
	}
	
	TDirection Direction() const
	{
		return TDirection(N, D);
	}
	
	TIndex Index() const
	{
		return TIndex(N, D);
	}
	
	void Print() const 
	{
		TIndex index = Index();
		for(unsigned int i = 0; i < pow(N,D); i++)
		{
			std::cout << index << ": " << Space(index) << '\t';
			index++;
			if((i+1) % N == 0) std::cout << std::endl;
		}
	}
	
	bool Full() const
	{
		for(unsigned int i = 0; i < pow(N,D); i++) 
			if(Space(i) == TSpaceState::empty) return false;
		return true;
	}
	
	
	TSpaceState CheckForWinner()
	{
		
		if(Winner(TSpaceState::X)) tss = TSpaceState::X;
		if(Winner(TSpaceState::Y)) 
		{
			if(tss == TSpaceState::X) tss = TSpaceState::empty;
			else  tss = TSpaceState::Y;			
		}
		return tss;
	}
	
	TSpaceState Winner() const
	{
		return tss;
	}
	
	friend bool operator ==(const TField& f1, const TField& f2)
	{
		if(f1.N != f2.N || f1.D != f2.D)
			return false;
		for(unsigned int i = 0; i < pow(f1.N, f1.D); i++) {
			if(f1.Space(i) != f2.Space(i)) return false;
		}
		return true;
	}
	
	void Save(std::ostream& o) const
	{
		o << N << '\t' << D << '\t' << (int)tss << std::endl;
		for(unsigned int i = 0; i < spaces.size(); i++)
		{
			o << (int)spaces.at(i) << (i < spaces.size()-1 ? '\t' : '\n');
		}
	}
	
	static TField Load(std::istream& in)
	{
		assert(in.good());
		unsigned int N, D;
		int buf; 
		in >> N;	in >> D;	assert( N > 0 && D> 0);
		TField tf(N, D);
		in >> buf;	tf.tss = (TSpaceState)buf;
		for(unsigned int i = 0; i < pow(N, D); i++)
		{
			assert(in.good());
			in >> buf;
			tf.Space(i) = (TSpaceState)buf;
		}
		return tf;
	}
	
};


}

#endif
