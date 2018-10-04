#ifndef TTTPLAYER_H
#define TTTPLAYER_H

#include <iostream>
#include <cassert>

#include "Grid.h"

namespace TicTacToe 
{
class Player;

class Observer
{
public:
	virtual void Moved(const Grid& g, const Player& player, const Position& pos) = 0;
};

class Player : public Observer
{
public:
	SpaceState sign;
	std::string Name;
	Player();
	Player(SpaceState sign, std::string Name);
	virtual Position Move(const Grid& g) = 0;
};

class HumanObserver : public Observer
{
public:
	void Moved(const Grid& g, const Player& player, const Position& pos);
};

class HumanPlayer : public HumanObserver, public Player
{
public:
	HumanPlayer();
	HumanPlayer(std::string Name);
	Position Move(const Grid& g) ;
	void Moved(const Grid& g, const Player& player,  const Position& pos);
};





}
#endif
