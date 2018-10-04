
#include <thread>
#include <chrono>
#include <iostream>
#include <random>	
#include <fstream>
#include <sstream>
#include <omp.h>

#include "TicTacToe.h"


int main(int argc, char** argv)
{	
	const unsigned int N = 4, D =2;
	
	//int gameTreeDepth = 5;
	//auto gameTree = std::make_shared<TicTacToe::GameTreeNode >(TicTacToe::Grid(N, D), TicTacToe::SpaceState::X, 0);
	//TicTacToe::GameTreeNode::UpdateGameTree(gameTree, gameTreeDepth);
	//TicTacToe::HeuristicAI hAI(gameTreeDepth);
	//hAI.MiniMax(gameTree, gameTreeDepth, TicTacToe::SpaceState::X);
	std::string gtFilename = (std::string("data/gt") + std::to_string(N) + std::string("_") + std::to_string(D) + std::string(".dat"));
	std::fstream f(gtFilename, std::ios::in);
	//gameTree->Save(f); 
	auto gameTree = TicTacToe::GameTreeNode::Load(f, N, D);
	f.close();
	
	std::cout << "finished processing gameTree" << std::endl;
	
	auto nnAI = std::make_shared<TicTacToe::NeuralNetworkAI>(N, D); nnAI->sign = TicTacToe::SpaceState::X;
	std::string nnFilename = (std::string("data/nn") + std::to_string(N) + std::string("_") + std::to_string(D) + std::string(".dat"));
	nnAI->Load(nnFilename);
	//auto nnaiTrainer = std::make_shared<TicTacToe::NeuralNetworkTrainer>(nnAI, gameTree);
	//nnaiTrainer->Training(5000);
	//nnAI->Save(nnFilename);
	
	
	auto game = std::make_shared<TicTacToe::Game>(N, D);
	
	std::shared_ptr<TicTacToe::Player> p1, p2;
	p1 = nnAI; p1->Name ="nnAI 1";
	p2 = std::make_shared<TicTacToe::HumanPlayer>(); p2->Name = "Niklas";
	game->SetPlayers(p1, p2);
	std::vector<std::shared_ptr<TicTacToe::Observer> > observers; observers.push_back(std::make_shared<TicTacToe::HumanObserver>());
	game->SetObservers(observers);
	while(true) {
		std::cout << game->StartGame() << " won" << std::endl;
		game->SwitchPlayers();
		std::cin.ignore();
		std::cin.get();
	}
	return 0;
}

