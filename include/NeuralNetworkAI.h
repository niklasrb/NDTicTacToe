#ifndef TTTNEURALNETWORKAI_H
#define TTTNEURALNETWORKAI_H

#include <cmath>
#include <fstream>
#include <memory>
#include <chrono>

#include "../../NeuralNetwork/nnetwork.h"

#include "Grid.h"
#include "Player.h"
#include "HeuristicAI.h"

namespace TicTacToe 
{

class NeuralNetworkTrainer;

class NeuralNetworkAI : public Player
{
friend NeuralNetworkTrainer;

protected:
	unsigned int N, D;
	std::shared_ptr<NeuralNetwork::Network> network;
	std::default_random_engine generator;
	std::uniform_real_distribution<> distribution;
	

	Eigen::VectorXd ConvertFieldToNeuron(const Grid& g) const;
public:
	NeuralNetworkAI(std::string file ) ;
	NeuralNetworkAI(unsigned int N, unsigned int D) ;

	Position Move(const Grid& f);
	
	void Moved(const Grid& g, const Player& p, const Position& pos);
	
	void Save(std::string file) const;
	
	void Load(std::string file);
	
};


class NeuralNetworkTrainer
{
protected:
	std::shared_ptr<NeuralNetworkAI > TNN;
	std::shared_ptr<NeuralNetwork::Network > network;
	std::shared_ptr<NeuralNetwork::Trainer> trainer;


	NeuralNetworkTrainer(std::shared_ptr<NeuralNetworkAI> TNN);
public:

	void Training(int maxSteps) const ;


	void PrepareData(std::shared_ptr<GameTreeNode> gT);
	
	NeuralNetworkTrainer(std::shared_ptr<NeuralNetworkAI > TNN, std::shared_ptr<GameTreeNode > gT);
	
	NeuralNetworkTrainer(std::shared_ptr<NeuralNetworkAI> NN, unsigned int gameTreeDepth);
};


}

#endif
