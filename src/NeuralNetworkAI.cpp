
#include "../include/NeuralNetworkAI.h"

namespace TicTacToe
{
	
Eigen::VectorXd NeuralNetworkAI::ConvertFieldToNeuron(const Grid& g) const
{
	Eigen::VectorXd v((int)pow(g.getN(), g.getD()));
	int i = 0; SpaceState ss;
	for(Position pos(g); pos; pos++, i++){
		ss = g.Space(pos);
		if(ss == SpaceState::empty)
			v(i) = 0;
		else if(ss == this->sign)
			v(i) = 1;
		else
			v(i) = -1;		
	}
	return v;
}

NeuralNetworkAI::NeuralNetworkAI(std::string file) : network(std::make_shared<NeuralNetwork::Network>()), generator(std::default_random_engine()), distribution(std::uniform_real_distribution<>(0., 1.0))
{
	assert(!file.empty());
	Load(file);
}

NeuralNetworkAI::NeuralNetworkAI(unsigned int N, unsigned int D) : network(std::make_shared<NeuralNetwork::Network>()), generator(std::default_random_engine()), distribution(std::uniform_real_distribution<>(0., 1.0))
{	
	NeuralNetwork::Layer L1(pow(N, D), pow(N, D), std::make_shared<NeuralNetwork::RELU>());
	network->Append(L1);
	network->Append(pow(N, D), std::make_shared<NeuralNetwork::Logistic>());
	//network->Append(pow(N, D), std::make_shared<NeuralNetwork::Logistic>());
	network->GetLayer(0).InitializeRandomly(); network->GetLayer(1).InitializeRandomly(); //network->GetLayer(2).InitializeRandomly();

}

Position NeuralNetworkAI::Move(const Grid& g)
{
	assert(this->sign != SpaceState::empty);
	Eigen::VectorXd v = network->Propagate(ConvertFieldToNeuron(g));	// get recommended moves
	Position pos(g); int round = 1;
	do {
		pos = Position(g);	// pick one at random
		double r = distribution(generator);
		double s = v(0);
		 int i = 1;
		while(s < r && pos){
			s+=v(i); i++; pos++;
		}
		if(round++ > 100) {	// seems like most of the recommendations are already taken, so take the first empty space
			pos = Position(g);
			while(g.Space(pos) != SpaceState::empty)
				pos++;
			break;
		}
	} while(g.Space(pos) != SpaceState::empty);
	
	//std::cout << r << '\t' << s << '\t' << i << std::endl;
	return pos;
}

void NeuralNetworkAI::Moved(const Grid& g, const Player& p,  const Position& pos)
{
	return;
}
void NeuralNetworkAI::Save(std::string file) const
{
	std::fstream f(file, std::fstream::out);
	f << N << '\t' << D << std::endl;
	network->Save(f);
}

void NeuralNetworkAI::Load(std::string file)
{
	std::fstream f(file, std::fstream::in);
	f >> N; assert(f.good());
	f >> D; assert(f.good());
	network->Load(f);
}



NeuralNetworkTrainer::NeuralNetworkTrainer(std::shared_ptr<NeuralNetworkAI> nnAI) : TNN(nnAI), network(nnAI->network), trainer(std::make_shared<NeuralNetwork::Trainer>(nnAI->network, std::make_shared<NeuralNetwork::EuclideanDistance> ()))
	{	}
	

void NeuralNetworkTrainer::Training(int maxSteps) const
{
	trainer->Options.batchSize = 20;
	
	trainer->Options.maxBatches = maxSteps;
	
	trainer->Options.trainingRate = 0.03;
	trainer->Options.trainingRateMin = 0.001;
	trainer->Options.adjustTrainingRate = true;
	
	trainer->Options.useInertia = true;
	trainer->Options.inertiaCoefficient = 0.2;
	trainer->Options.regulationType = NeuralNetwork::Trainer::RegulatoryOptions::L2;
	trainer->Options.regulationCoefficient = 0.03;
	
	trainer->Training();
}

void NeuralNetworkTrainer::PrepareData(std::shared_ptr<GameTreeNode> gT)
{
	unsigned int N = gT->grid.getN(), D = gT->grid.getD();
	if(gT->turn == TNN->sign && gT->moves.size() > 0)
	{
		auto sigmoid = std::make_shared<NeuralNetwork::Logistic>();
		auto input = TNN->ConvertFieldToNeuron(gT->grid);
		Eigen::VectorXd output; output.resize(pow(N, D));
		for(unsigned int i = 0; i < pow(N, D); i++) 
			output[i] = 0;
		for(unsigned int i = 0; i < gT->moves.size(); i++)	{
			output[gT->moves.at(i).move.GetSpaceIndex()] = (*sigmoid)(gT->moves.at(i).nextNode->Value);
		}
		if(output.lpNorm<1>() != 0) output = output/output.lpNorm<1>();	// because act does the same
		trainer->trainSet.push_back(std::make_pair(input, output));
	}
	for(unsigned int i = 0; i < gT->moves.size(); i++)	PrepareData(gT->moves.at(i).nextNode);
}

NeuralNetworkTrainer::NeuralNetworkTrainer(std::shared_ptr<NeuralNetworkAI > nnAI, std::shared_ptr<GameTreeNode > gT) : NeuralNetworkTrainer(nnAI)
{
	PrepareData(gT);
}

NeuralNetworkTrainer::NeuralNetworkTrainer(std::shared_ptr<NeuralNetworkAI> nnAI, unsigned int gameTreeDepth) : NeuralNetworkTrainer(nnAI)
	{
		auto gameTree = std::make_shared<GameTreeNode >(Grid(nnAI->N, nnAI->D), SpaceState::X, 0);
		GameTreeNode::UpdateGameTree(gameTree, gameTreeDepth);
		HeuristicAI hAI(gameTreeDepth);
		hAI.MiniMax(gameTree, gameTreeDepth, nnAI->sign);
		PrepareData(gameTree);
	}

}
