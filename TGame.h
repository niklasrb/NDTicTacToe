#ifndef TGAME_H
#define TGAME_H

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#include "TField.h"
#include "TPlayer.h"


template<unsigned int N, unsigned int D>
class TGame
{
protected:
	TField<N, D> field;
	std::vector<std::shared_ptr<TPlayer<N, D> > > players;
	std::vector<std::shared_ptr<TObserver<N, D> > > observers;
	bool lock;

public:
	TGame(const std::shared_ptr<TPlayer<N, D> >& player1, const std::shared_ptr<TPlayer<N, D> >& player2, const std::vector<std::shared_ptr<TObserver<N, D> > >& observers = std::vector<std::shared_ptr<TObserver<N, D> > >())
		: observers(observers)
	{
		players.push_back(player1);
		players.push_back(player2);
		field = TField<N, D>();
		lock = false;
	}
	
	static TSpaceState NextPlayer(TSpaceState last)
	{
		return (last == TSpaceState::X ? TSpaceState::Y : TSpaceState::X);
	}
	
	void SwitchPlayers()
	{
		if(lock) return;
		std::reverse(players.begin(), players.end());
	}
	
	TSpaceState Play()
	{
		field.Empty();
		assert(players.size() == 2);
		TSpaceState	activePlayer = TSpaceState::X;
		players.at(0)->sign = TSpaceState::X; players.at(1)->sign = TSpaceState::Y;
		TIndex<N, D> index; lock = true;
		while(!field.Full())	{
			index = players.at((int)activePlayer - 1)->Move(field);
			while(field.Space(index) != TSpaceState::empty) 
				index = players.at((int)activePlayer - 1)->Move(field);
			field.Space(index) = activePlayer;
			for(unsigned int i = 0; i < players.size(); i++) players.at(i)->Moved(field, activePlayer, index);
			for(unsigned int i = 0; i < observers.size(); i++) observers.at(i)->Moved(field, activePlayer, index);
			if( field.CheckForWinner() != TSpaceState::empty) return field.Winner();
			activePlayer = NextPlayer(activePlayer);
		}
		lock = false;
		return TSpaceState::empty;
	}
};


#endif
