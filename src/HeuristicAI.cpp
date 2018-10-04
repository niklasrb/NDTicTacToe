
#include "../include/HeuristicAI.h"

namespace TicTacToe
{

GameMove::GameMove(const Grid& grid, SpaceState player, const Position& pos) : player(player), move(pos)
{
	assert(grid.Space(pos) == SpaceState::empty);
	nextNode = std::make_shared<GameTreeNode >(grid, Game::NextPlayer(player));
	nextNode->grid.Space(move) = player; 
}


void GameMove::Save(std::ostream& o) const
{
	o << player << '\t' << move << std::endl;
	nextNode->Save(o);
}

GameMove GameMove::Load(std::istream& in, unsigned int N, unsigned int D)
{
	int pl; Position po(N, D);
	assert(in.good());
	in >> pl >> po;
	assert(in.good());
	auto nN = GameTreeNode::Load(in, N, D);
	return GameMove(nN, (SpaceState)pl, po); 
}

GameMove::GameMove(std::shared_ptr<GameTreeNode> nextNode, SpaceState player, const Position& pos)
	: player(player), move(pos), nextNode(nextNode) 
	{	}


GameTreeNode::GameTreeNode(const Grid& grid, SpaceState turn, double value = 0) : turn(turn), grid(grid), Value(value) {}


void GameTreeNode::Save(std::ostream& o) const
{
	o << (int)turn << '\t' << Value << '\t' << moves.size() << std::endl;
	grid.Save(o);
	for(unsigned int i = 0; i < moves.size(); i++) moves.at(i).Save(o);
}

std::shared_ptr<GameTreeNode> GameTreeNode::Load(std::istream& in, unsigned int N, unsigned int D)
{
	int t; double v; unsigned int s;
	assert(in.good());
	in >> t >> v >> s;
	assert(in.good());
	Grid g(N, D);
	g.Load(in);
	auto gtn = std::make_shared<GameTreeNode>(g, (SpaceState)t, v);
	for(unsigned int i = 0; i < s; i++) gtn->moves.push_back(GameMove::Load(in, N, D));
	return gtn;
}

void GameTreeNode::UpdateGameTree(std::shared_ptr<GameTreeNode> gt, int depth)
{
	//std::cout << "Update Game Tree " << depth << " with pointer " << gt << std::endl;
	if(depth < 1 || gt->grid.IsFull() || gt->grid.CheckForWinner() != SpaceState::empty) return;	// full depth or game over
	if(gt->moves.size() == 0)	// make more possible moves
	{
		for(Position pos(gt->grid); pos; pos++) {
			if(gt->grid.Space(pos) == SpaceState::empty) {
				gt->moves.push_back(GameMove(gt->grid, gt->turn, pos));
				//std::cout << "Added move for player " << gt->turn << " at " << index << std::endl;
			}
		}
	}
	for(auto it = gt->moves.begin(); it != gt->moves.end(); ++it)	// go down the rabbit hole
	{
		UpdateGameTree(it->nextNode, depth-1);	
	}
	//std::cout << "exit for pointer " << gt << std::endl;
	return;
}


HeuristicAI::HeuristicAI(unsigned int gameTreeDepth) : Player(SpaceState::empty, "HeuristicAI"), gameTreeDepth(gameTreeDepth)
	{	}

Position HeuristicAI::Move(const Grid& g)
{
	if(!gameTree) 
		gameTree = std::make_shared<GameTreeNode>(g, this->sign);
	if(!(gameTree->grid == g)) 
		gameTree = std::make_shared<GameTreeNode>(g, this->sign);
	GameTreeNode::UpdateGameTree(gameTree, gameTreeDepth);
	double m = MiniMax(gameTree, gameTreeDepth, this->sign);
	std::vector<unsigned int> imax; 
	for(unsigned int i = 0; i < gameTree->moves.size(); i++) {
		if(gameTree->moves.at(i).nextNode->Value == m) imax.push_back(i);;
	}
	if(imax.size() == 0) std::cout << "Error finding moves with value " << m << std::endl;
	assert(imax.size() > 0);
	std::srand(std::time(0)); 
	return gameTree->moves.at(imax.at(std::rand() % imax.size())).move;		
}


double HeuristicAI::MiniMax(std::shared_ptr<GameTreeNode> gt, int depth, SpaceState p) const
{
	//std::cout << "MiniMax at depth " << depth << " with pointer " << gt << std::endl;
	if(depth < 1 || gt->moves.size() == 0) {	// full depth or game over
		gt->grid.CheckForWinner();
		gt->Value = Evaluate(gt->grid, p);
		//std::cout << "Reached end of tree: " << std::endl; gt->field.Print();
		//std::cout << "Value for player " << this->sign << ": " << gt->Value << " with winner " << gt->field.Winner() << std::endl;
		return gt->Value;
	}
	
	bool max = gt->turn == p;
	double m = (max ? -1. : 1.)*std::numeric_limits<double>::infinity();
	for(auto it = gt->moves.begin(); it != gt->moves.end(); ++it)	// go down the rabbit hole
	{
		double val = MiniMax(it->nextNode, depth-1, p);		// minimax algorithm
		//std::cout << "Move : " << &(*it) << " with value " << val << std::endl;
		if(max && val > m) m = val;
		if(!max && val < m) m = val;
	}
	gt->Value = m;
	//std::cout << "Minimax " << depth << " with pointer " << gt << " found minimax value " << m << std::endl;
	return m;
}

void HeuristicAI::Moved(const Grid& g, const Player& player, const Position& pos) 
{
	if(gameTree) {	// if the gameTree exists
		for(auto it = gameTree->moves.begin(); it != gameTree->moves.end(); ++it)	// find move in predicted moves
		{
			if(it->move == pos && it->player == player.sign) {	// found the new move in the predicted ones
				gameTree = it->nextNode;
				if(gameTree->grid == g) return;	// this should be the case, otherwise there was an error
				else break;
			}
		}
	}
	gameTree = std::make_shared<GameTreeNode>(g, Game::NextPlayer(player.sign));		// troubleshoot
	return;
}

double HeuristicAI::Evaluate(const Grid& g, SpaceState p) const
{
	if(g.Winner() == p) 
		return std::numeric_limits<double>::max()/2.;
	SpaceState enemy = Game::NextPlayer(p);
	if(g.Winner() == enemy) 
		return -std::numeric_limits<double>::max()/2.;
	double sum = 0;
	for(Position pos(g); pos; pos++)
	{
		//std::cout << "Position: " << index << std::endl;
		sum += EvaluateLines(g, p, pos) - EvaluateLines(g, enemy, pos);
		//std::cout << "new sum: " << sum << std::endl;
	}
	return sum;
}

 double HeuristicAI::EvaluateLines(const Grid& g, SpaceState p, const Position& start) const
{		
	if(g.Space(start) != p && g.Space(start) != SpaceState::empty) 
		return 0;	// enemy player starting point
	
	Direction allowedDirections = start.IndicateCorners();
	if(!allowedDirections.NonZero()) 
		return 0;	// then we're not at a valid starting point
	Direction subDirection = Direction(g); 	// describing which indices will be changed 
	Position pos = start;	double sum = 0;
	
	for(;subDirection.NonZero(); subDirection++) {	// test all subdirections
		if(!allowedDirections.AllowsSubdirection(subDirection))
			continue;
		pos = start;
		double value = (g.Space(start) == p ? 2 : 1);
		while(pos += subDirection) {	// go along diagonal and check all spaces
			if(g.Space(pos) == p) 
				value *= 2;
			else if(g.Space(pos) != SpaceState::empty) 
				value = 0;
		}
		if(value == 1) value=0;
		sum += value;
	}
	return sum;
}
	

}
