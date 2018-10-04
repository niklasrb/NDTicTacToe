
#include "../include/Game.h"

namespace TicTacToe
{

Game::Game(unsigned int N, unsigned int D) : grid(N, D), lock(false)
{ }

SpaceState Game::NextPlayer(SpaceState last)
{
	return (last == SpaceState::X ? SpaceState::O : SpaceState::X);
}

void Game::SwitchPlayers()
{
	if(lock) return;
	std::reverse(players.begin(), players.end());
}

void Game::SetPlayers(std::shared_ptr<Player> playerX, std::shared_ptr<Player> playerO)
{
	players.clear();
	players.push_back(playerX);
	players.push_back(playerO);
}

void Game::SetObservers(const std::vector<std::shared_ptr<Observer> >& observers)
{
	this->observers.clear();
	this->observers = observers;
}

SpaceState Game::StartGame(std::shared_ptr<Player> playerX, std::shared_ptr<Player> playerO, const std::vector<std::shared_ptr<Observer> >& observers = std::vector<std::shared_ptr<Observer> >()) 
{
	SetPlayers(playerX, playerO);
	if(observers.size() > 0)
		SetObservers(observers);
	return StartGame();
}

SpaceState Game::StartGame()
{
	grid.Clear();
	assert(players.size() == 2);
	SpaceState activeSign = SpaceState::X;
	std::shared_ptr<Player> activePlayer;
	players.at(0)->sign = SpaceState::X; players.at(1)->sign = SpaceState::O;
	Position desiredMove(grid); lock = true;
	while(!grid.IsFull())	{
		activePlayer = players.at((int)activeSign - 1);
		desiredMove = activePlayer->Move(grid);
		while(grid.Space(desiredMove) != SpaceState::empty) 
			desiredMove = activePlayer->Move(grid);
		grid.Space(desiredMove) = activeSign;
		for(unsigned int i = 0; i < players.size(); i++) 
			players.at(i)->Moved(grid, *activePlayer, desiredMove);
		for(unsigned int i = 0; i < observers.size(); i++) 
			observers.at(i)->Moved(grid, *activePlayer, desiredMove);
		if( grid.CheckForWinner() != SpaceState::empty) 
			break;
		activeSign= NextPlayer(activeSign);
	}
	lock = false;
	return grid.Winner();
}


}

