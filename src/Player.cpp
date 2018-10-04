
#include "../include/Player.h"

namespace TicTacToe
{

Player::Player() : sign(SpaceState::empty), Name("P")
{	}
Player::Player(SpaceState sign, std::string Name = "P") : sign(sign), Name(Name)
{	}

void HumanObserver::Moved(const Grid& g, const Player& player, const Position& pos)
{
	std::cout << "Player " << player.Name << " with " << player.sign << " moved to position " << pos << std::endl;
	g.Print();
}


HumanPlayer::HumanPlayer() : Player(SpaceState::empty, "HumanPlayer")
{ }
HumanPlayer::HumanPlayer(std::string Name) : Player(SpaceState::empty, Name)
{ }

Position HumanPlayer::Move(const Grid& g) 
{
	assert(!g.IsFull());
	std::cout << "It's your turn, player " << this->Name << " to place a " << this->sign << std::endl;
	Position pos(g);
	while(true) {
		pos.ReadFromCmd();
		if(g.Space(pos) == SpaceState::empty)
			break;
		std::cout << "Space is already taken. ";
	}
	
	return pos;
}

void HumanPlayer::Moved(const Grid& g, const Player& player,  const Position& pos)
{
	HumanObserver::Moved(g, player, pos);
}

}
