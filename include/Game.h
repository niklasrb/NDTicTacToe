#ifndef TTTGAME_H
#define TTTGAME_H

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#include "Grid.h"
#include "Player.h"


namespace TicTacToe
{

class Game
{
protected:
	Grid grid;
	std::vector<std::shared_ptr<Player > > players;
	std::vector<std::shared_ptr<Observer > > observers;
	bool lock;

public:
	Game(unsigned int N, unsigned int D);
	
	static SpaceState NextPlayer(SpaceState last);
	
	void SwitchPlayers();
	
	void SetPlayers(std::shared_ptr<Player> playerX, std::shared_ptr<Player> playerO);
	void SetObservers(const std::vector<std::shared_ptr<Observer> >& observers);
	
	SpaceState StartGame(std::shared_ptr<Player> playerX, std::shared_ptr<Player> playerO, const std::vector<std::shared_ptr<Observer> >& observers) ;
	SpaceState StartGame();
};


}

#endif
