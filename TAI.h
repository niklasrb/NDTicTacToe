#ifndef TAI_H
#define TAI_H
#include <vector>
#include <cmath>
#include <memory>
#include <limits>
#include <cstdlib>
#include <ctime>

#include "TField.h"
#include "TPlayer.h"
#include "TGame.h"

template<unsigned int N, unsigned int D>
class TGameTreeNode;

template<unsigned int N, unsigned int D>
class TMove
{
friend class TGameTreeNode<N, D>;
public:
	TSpaceState player;
	TIndex<N, D> pos;
	std::shared_ptr<TGameTreeNode<N, D> > nextNode;
	
	TMove(const TField<N, D>& field, const TSpaceState player, const TIndex<N, D>& pos) : player(player), pos(pos)
	{
		assert(field.Space(pos) == TSpaceState::empty);
		nextNode = std::make_shared<TGameTreeNode<N, D> >(field, TGame<N, D>::NextPlayer(player));
		nextNode->field.Space(pos) = player; 
	}
	
	void Save(std::ostream& o) const
	{
		o << player << '\t' << pos << std::endl;
		nextNode->Save(o);
	}

protected:
	static TMove Load(std::istream& in)
	{
		int pl; TIndex<N, D> po;
		assert(in.good());
		in >> pl >> po;
		assert(in.good());
		auto nN = TGameTreeNode<N, D>::Load(in);
		return TMove(nN, (TSpaceState)pl, po); 
	}
	TMove(const std::shared_ptr<TGameTreeNode<N, D> >& nextNode, const TSpaceState player, const TIndex<N, D>& pos) : player(player), pos(pos), nextNode(nextNode) {}
};

template<unsigned int N, unsigned int D>
class TGameTreeNode
{
friend class TMove<N, D>;
public:
	TSpaceState turn;
	TField<N, D> field;
	double Value;
	std::vector<TMove<N, D> > moves;
	
	TGameTreeNode(const TField<N, D>& field, TSpaceState turn, double value = 0) : turn(turn), field(field), Value(value) {}

	void Save(std::ostream& o) const
	{
		o << (int)turn << '\t' << Value << '\t' << moves.size() << std::endl;
		field.Save(o);
		for(unsigned int i = 0; i < moves.size(); i++) moves.at(i).Save(o);
	}
public:
	static std::shared_ptr<TGameTreeNode> Load(std::istream& in)
	{
		int t; double v; unsigned int s;
		assert(in.good());
		in >> t >> v >> s;
		assert(in.good());
		auto f = TField<N, D>::Load(in);
		auto gtn = std::make_shared<TGameTreeNode>(f, (TSpaceState)t, v);
		for(unsigned int i = 0; i < s; i++) gtn->moves.push_back(TMove<N, D>::Load(in));
		return gtn;
	}
};


template<unsigned int N, unsigned int D>
class TAI : public TPlayer<N, D>
{
public:
	std::shared_ptr<TGameTreeNode<N, D> > gameTree;
	unsigned int gameTreeDepth;
	
public:

	TAI(unsigned int gameTreeDepth) : gameTreeDepth(gameTreeDepth)
	{	}
	
	TIndex<N, D> Move(const TField<N, D>& f)
	{
		if(!gameTree) gameTree = std::make_shared<TGameTreeNode<N, D>>(f, this->sign);
		if(!(gameTree->field == f)) gameTree = std::make_shared<TGameTreeNode<N, D>>(f, this->sign);
		UpdateGameTree(gameTree, gameTreeDepth);
		double m = MiniMax(gameTree, gameTreeDepth, this->sign);
		std::vector<unsigned int> imax; 
		for(unsigned int i = 0; i < gameTree->moves.size(); i++) {
			if(gameTree->moves.at(i).nextNode->Value == m) imax.push_back(i);;
		}
		if(imax.size() == 0) std::cout << "Error finding moves with value " << m << std::endl;
		assert(imax.size() > 0);
		std::srand(std::time(0)); 
		return gameTree->moves.at(imax.at(std::rand() % imax.size())).pos;		
	}
	
	void static UpdateGameTree(const std::shared_ptr<TGameTreeNode<N, D> >& gt, int depth)
	{
		//std::cout << "Update Game Tree " << depth << " with pointer " << gt << std::endl;
		if(depth < 1 || gt->field.Full() || gt->field.CheckForWinner() != TSpaceState::empty) return;	// full depth or game over
		if(gt->moves.size() == 0)	// make more possible moves
		{
			TIndex<N, D> index; 
			for(unsigned int i = 0; i < pow(N, D); i++) {
				if(gt->field.Space(index) == TSpaceState::empty) {
					gt->moves.push_back(TMove<N, D>(gt->field, gt->turn, index));
					//std::cout << "Added move for player " << gt->turn << " at " << index << std::endl;
				}
				index++;
			}
		}
		for(auto it = gt->moves.begin(); it != gt->moves.end(); ++it)	// go down the rabbit hole
		{
			UpdateGameTree(it->nextNode, depth-1);	
		}
		//std::cout << "exit for pointer " << gt << std::endl;
		return;
	}
	
	double static MiniMax(const std::shared_ptr<TGameTreeNode<N, D> >& gt, int depth, TSpaceState p)
	{
		//std::cout << "MiniMax at depth " << depth << " with pointer " << gt << std::endl;
		if(depth < 1 || gt->moves.size() == 0) {	// full depth or game over
			gt->field.CheckForWinner();
			gt->Value = Evaluate(gt->field, p);
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
	
	
	void Moved(const TField<N, D>& f, TSpaceState player, const TIndex<N, D>& pos) 
	{
		if(gameTree) {	// if the gameTree exists
			for(auto it = gameTree->moves.begin(); it != gameTree->moves.end(); ++it)	// find move in predicted moves
			{
				if(it->pos == pos && it->player == player) {	// found the new move in the predicted ones
					gameTree = it->nextNode;
					if(gameTree->field == f) return;	// this should be the case, otherwise there was an error
					else break;
				}
			}
		}
		gameTree = std::make_shared<TGameTreeNode<N, D>>(f, TGame<N, D>::NextPlayer(player));		// troubleshoot
		return;
	}
	
	
	static double Evaluate(const TField<N, D>& f, TSpaceState p)
	{
		if(f.Winner() == p) return std::numeric_limits<double>::max();
		TSpaceState enemy = TGame<N, D>::NextPlayer(p);
		if(f.Winner() == enemy) return std::numeric_limits<double>::lowest();
		TIndex<N, D> index; double sum = 0;
		for(unsigned int i = 0; i < pow(N, D); i++)
		{
			//std::cout << "Position: " << index << std::endl;
			sum += EvaluateLines(f, p, index) - EvaluateLines(f, enemy, index);
			//std::cout << "new sum: " << sum << std::endl;
			index++;
		}
		return sum;
	}
	
	static double EvaluateLines(TField<N, D> f, TSpaceState p, const TIndex<N, D>& index) 
	{		
		if(f.Space(index) != p && f.Space(index) != TSpaceState::empty) return 0;	// enemy player starting point
		
		TDirection<N, D> allowedDirections = index.IndicateCorners();
		if(!allowedDirections.NonZero()) return false;	// then we're not at a valid starting point
		TDirection<N, D> direction; 	// describing which indices will be changed 
		TIndex<N, D> pos = index;	double sum = 0;
		for(unsigned int i = 0; i < pow(3, D); i++)	// go through all possible directions
		{
			if(allowedDirections.Allows(direction) && direction.NonZero())
			{
				pos = index;
				double value = (f.Space(index) == p ? 2 : 1);
				for(unsigned int j = 1; j < N; j++)
				{
					pos += direction;
					if(f.Space(pos) == p) value *= 2;
					else if(f.Space(pos) != TSpaceState::empty) value = 0;
				}
				if(value == 1) value=0;
				//std::cout << "direction " << direction << " gives value: " << value << std::endl;
				sum += value;
			}
			direction++;
		}
		return sum;
	}
	
};


#endif
