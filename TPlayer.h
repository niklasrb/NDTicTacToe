#ifndef TPLAYER_H
#define TPLAYER_H

#include <iostream>

#include "TField.h"


template<unsigned int N, unsigned int D>
class TObserver
{
public:
	virtual void Moved(const TField<N, D>& f, TSpaceState player, const TIndex<N, D>& pos) = 0;
};

template<unsigned int N, unsigned int D>
class TPlayer
{
public:
	TSpaceState sign;
	
	virtual TIndex<N, D> Move(const TField<N, D>& f) = 0;
	virtual void Moved(const TField<N, D>& f, TSpaceState player, const TIndex<N, D>& pos) = 0;
};



template<unsigned int N, unsigned int D>
class THumanPlayer : public TPlayer<N, D>
{
public:
	
	TIndex<N, D> Move(const TField<N, D>& f) 
	{
		assert(!f.Full());
		std::cout << "It's your turn, player " << this->sign << std::endl;
		auto index = TIndex<N, D>::ReadFromCmd();
		while(f.Space(index) != TSpaceState::empty) {
			std::cout << "This space is occupied" << std::endl;
			index = TIndex<N, D>::ReadFromCmd();
		}
		return index;
	}
	
	void Moved(const TField<N, D>& f, TSpaceState player, const TIndex<N, D>& pos)
	{
		std::cout << "Player " << player << " moved to position " << pos << std::endl;
		f.Print();
	}
};

template<unsigned int N, unsigned int D>
class THumanObserver : public TObserver<N, D>
{
public:
	void Moved(const TField<N, D>& f, TSpaceState player, const TIndex<N, D>& pos)
	{
		std::cout << "Player " << player << " moved to position " << pos << std::endl;
		f.Print();
	}
};




#endif
