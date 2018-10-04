#ifndef TTTHEURISTICAI_H
#define TTTHEURISTICAI_H
#include <vector>
#include <cmath>
#include <memory>
#include <limits>
#include <cstdlib>
#include <ctime>

#include "Grid.h"
#include "Player.h"
#include "Game.h"

namespace TicTacToe
{

class GameTreeNode;

class GameMove
{
friend class GameTreeNode;
public:
	SpaceState player;
	Position move;
	std::shared_ptr<GameTreeNode > nextNode;
	
	GameMove(const Grid& grid, SpaceState player, const Position& pos);
	
	void Save(std::ostream& o) const;

	
protected:
	static GameMove Load(std::istream& in, unsigned int N, unsigned int D);
	GameMove(std::shared_ptr<GameTreeNode> nextNode, SpaceState player, const Position& pos);
	
};


class GameTreeNode
{
friend class Move;
public:
	SpaceState turn;
	Grid grid;
	double Value;
	std::vector<GameMove > moves;
	
	GameTreeNode(const Grid& grid, SpaceState turn, double value );

	void Save(std::ostream& o) const;
	
public:
	static std::shared_ptr<GameTreeNode> Load(std::istream& in, unsigned int N, unsigned int D);
	static void UpdateGameTree(std::shared_ptr<GameTreeNode>, int depth);
	
};



class HeuristicAI : public Player
{
protected:
	std::shared_ptr<GameTreeNode> gameTree;
	unsigned int gameTreeDepth;
	
public:

	HeuristicAI(unsigned int gameTreeDepth);
	
	Position Move(const Grid& g);
	
	double MiniMax(std::shared_ptr<GameTreeNode> gt, int depth, SpaceState p) const;
	
	void Moved(const Grid& g, const Player& p, const Position& pos) ;
	
	double Evaluate(const Grid& g, SpaceState p) const;
	double EvaluateLines(const Grid& g, SpaceState p, const Position& index) const;
};


}

#endif
