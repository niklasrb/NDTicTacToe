#ifndef TNEURALNETWORK_H
#define TNEURALNETWORK_H

#include <eigen3/Eigen/Dense>
//#include <eigen3/Eigen/Array>
#include <cmath>
#include <fstream>
#include <memory>
#include <chrono>


#include "TField.h"
#include "TPlayer.h"
#include "TAI.h"

template<unsigned int N, unsigned int D>
class TNeuralNetworkTrainer;

template<unsigned int N, unsigned int D>
class TNeuralNetwork : public TPlayer<N, D>
{
friend TNeuralNetworkTrainer<N, D>;
private:
	constexpr static int pow(int x, int y)
	{
		return y == 0 ? 1.0 : x * pow(x, y-1);
	}
protected:
	typedef Eigen::Matrix<double, pow(N, D), pow(N, D)> Matrix;
	typedef Eigen::Matrix<double, pow(N, D), 1> Vector;

	Matrix M1;
	Vector v1;
	Matrix M2;
	Vector v2;
	
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution;
	

	double static sigmoid(const double& x)
	{
		return 1./(1.+exp(-x));
	}
	
	Eigen::MatrixXd static sigmoid(const Eigen::MatrixXd& m)
	{
		Eigen::MatrixXd n(m);
		for(unsigned int i = 0; i< m.rows(); i++)
		{
			for(unsigned int j = 0; j < m.cols(); j++)
			{
				n(i, j) = sigmoid(m(i, j));
			}
		}
		return n;
	}
	
	Eigen::VectorXd Calculate(const Eigen::VectorXd& field) const 
	{
		Eigen::VectorXd v = sigmoid(M1*field + v1);
		v = sigmoid(M2*v + v2);
		v = v/v.lpNorm<1>();
		return v;
	}
	
	Eigen::VectorXd ConvertFieldToNeuron(const TField<N, D>& f) const
	{
		Eigen::VectorXd v(pow(N,D));
		TIndex<N, D> index;
		for(unsigned int i = 0; i < pow(N, D); i++)
		{
			TSpaceState ss = f.Space(index);
			v(i) = (ss == TSpaceState::empty ? 0 : (ss == this->sign ? 1 : -1 ));
			index++;
		}
		return v;
	}
	
	//Eigen::VectorXd ConvertDataTo
	
public:
	TNeuralNetwork() : TNeuralNetwork(Eigen::MatrixXd::Random(pow(N, D), pow(N, D)), Eigen::MatrixXd::Random(pow(N, D), 1), Eigen::MatrixXd::Random(pow(N, D), pow(N, D)), Eigen::MatrixXd::Random(pow(N, D), 1))
	{}
	
	TNeuralNetwork(const Matrix& M1, const Vector& v1, const Matrix& M2, const Vector& v2)
		: M1(M1), v1(v1), M2(M2), v2(v2)
		{
			generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
			distribution = std::uniform_real_distribution<double>(0., 1.);
		}

	virtual TIndex<N, D> Move(const TField<N, D>& f)
	{
		assert(this->sign != TSpaceState::empty);
		Eigen::VectorXd v = ConvertFieldToNeuron(f);
		v = Calculate(v);
		std::cout << "v = " << v << std::endl;
		double r = distribution(generator);
		double s = v(0);
		 TIndex<N, D> index; int i = 1;
		while(s < r && i < pow(N, D)){
			s+=v(i); i++; index++;
		}
		std::cout << r << '\t' << s << '\t' << i << std::endl;
		return index;
	}
	
	virtual void Moved(const TField<N, D>& f, TSpaceState player, const TIndex<N, D>& pos)
	{
		return;
	}
	
	void Save(std::string file) const
	{
		std::ofstream out(file, std::fstream::out);
		out.write((char*) M1.data(), pow(N, D)*pow(N, D)*sizeof(double));
		out.write((char*) v1.data(), pow(N, D)*sizeof(double));
		out.write((char*) M2.data(), pow(N, D)*pow(N, D)*sizeof(double));
		out.write((char*) v2.data(), pow(N, D)*sizeof(double));
		out.close();
	}
	
	void Load(std::string file)
	{
		std::ifstream in(file, std::fstream::in);
		in.read((char*) M1.data(), pow(N, D)*pow(N, D)*sizeof(double));
		in.read((char*) v1.data(), pow(N, D)*sizeof(double));
		in.read((char*) M2.data(), pow(N, D)*pow(N, D)*sizeof(double));
		in.read((char*) v2.data(), pow(N, D)*sizeof(double));
		in.close();
	}
	
};


template<unsigned int N, unsigned int D>
class TNeuralNetworkTrainer
{
protected:
	std::shared_ptr<TNeuralNetwork<N, D> > NeuralNetwork;
	
	std::vector<std::pair<Eigen::VectorXd, Eigen::VectorXd> > TrainingData;

public:

	static double sign(const double& d)
	{
		if(d == 0) return 0.;
		else if(d < 0) return -1.;
		else return 1.;
	}

	void PrepareData(std::shared_ptr<TGameTreeNode<N, D> >& gT, std::shared_ptr<TNeuralNetwork<N, D> > NN)
	{
		if(gT->turn == NN->sign && gT->moves.size() > 0)
		{
			auto input = NN->ConvertFieldToNeuron(gT->field);
			Eigen::VectorXd output; output.resize(pow(N, D));
			for(unsigned int i = 0; i < pow(N, D); i++) output[i] = 0;
			for(unsigned int i = 0; i < gT->moves.size(); i++)
			{
				output[gT->moves.at(i).pos.GetIndex()] = NN->sigmoid(gT->moves.at(i).nextNode->Value);
			}
			if(output.lpNorm<1>() != 0) output = output/output.lpNorm<1>();	// because act does the same
			TrainingData.push_back(std::make_pair(input, output));
		}
		for(unsigned int i = 0; i < gT->moves.size(); i++)	PrepareData(gT->moves.at(i).nextNode, NN);
	}

	double Evaluate(std::shared_ptr<TNeuralNetwork<N, D> >& NN)
	{
		double s = 0;
		for(unsigned int i = 0; i < TrainingData.size(); i++)
		{
			auto res = NN->Calculate(TrainingData.at(i).first);
			s += pow((res - TrainingData.at(i).second).norm(), 2)/2.;
		}
		return s;
	}
	
/*	void Step(, double delta =1, double alpha = 0.1)
	{
		TNeuralNetwork<N, D> gradient;
		double s0 = Evaluate(NN), s = 0;
		for(unsigned int i = 0; i < pow(N, D); i++)
		{
			for(unsigned int j = 0; j < pow(N, D); j++)
			{
				NN->M1(i, j) += delta;
				s = Evaluate(NN);
				gradient.M1(i, j) = (s - s0)/delta;
				NN->M1(i, j) -= delta;
			}
		}
		for(unsigned int i = 0; i < pow(N, D); i++)
		{
			NN->v1(i) += delta;
			s = Evaluate(NN);
			gradient.v1(i) = (s - s0)/delta;
			NN->v1(i) -= delta;
		}
		for(unsigned int i = 0; i < pow(N, D); i++)
		{
			for(unsigned int j = 0; j < pow(N, D); j++)
			{
				NN->M2(i, j) += delta;
				s = Evaluate(NN);
				gradient.M2(i, j) = (s - s0)/delta;
				NN->M2(i, j) -= delta;
			}
		}
		for(unsigned int i = 0; i < pow(N, D); i++)
		{
			NN->v2(i) += delta;
			s = Evaluate(NN);
			gradient.v2(i) = (s - s0)/delta;
			NN->v2(i) -= delta;
		}
		std::cout << "M1: " << gradient.M1 << std::endl << "v1: " << gradient.v1 << std::endl << "M2: " << gradient.M2 << std::endl << "v2: " << gradient.v2 << std::endl;
		NN->M1 -= alpha*gradient.M1;
		NN->v1 -= alpha*gradient.v1;
		NN->M2 -= alpha*gradient.M2;
		NN->v2 -= alpha*gradient.v2;
	}*/
	
	void SingleBackpropagation(std::shared_ptr<TNeuralNetwork<N, D> >& bufferNetwork, const std::pair<Eigen::VectorXd, Eigen::VectorXd>& trainingExample)
	{
		const Eigen::VectorXd& n0 = trainingExample.first;
		const Eigen::VectorXd& y = trainingExample.second;
		const Eigen::VectorXd z1 = NeuralNetwork->M1*n0 + NeuralNetwork->v1;
		const Eigen::VectorXd n1 = TNeuralNetwork<N, D>::sigmoid(z1);
		const Eigen::VectorXd z2 = NeuralNetwork->M2*n1 + NeuralNetwork->v2;
		const Eigen::VectorXd n2 = TNeuralNetwork<N, D>::sigmoid(z2); const double n2l1Norm = n2.lpNorm<1>();
		const Eigen::VectorXd n3 = n2/n2.lpNorm<1>();
		
		//std::cout << "n0 = " << n0 << std::endl << "n1 = " << n1 << std::endl;
		//std::cout << "y = " << y << std::endl << "n2 = " << n2 << std::endl << "|n2|_1 = " << n2.lpNorm<1>() << std::endl << "n3 = " << n3 << std::endl;
		
		double s = 0;
		for( int j = 0; j < (int)pow(N, D); j++)	{
			for( int l = 0; l < (int)pow(N, D); l++) {
				s = 0;
				for( int i = 0; i < (int)pow(N, D); i++) 
					s += (y[i] - n3[i])*((i == j ? 1./n2l1Norm : 0.) - sign(n2[j])*n2[i]/pow(n2l1Norm,2))*TNeuralNetwork<N, D>::sigmoid(z2[j])*(1.-TNeuralNetwork<N, D>::sigmoid(z2[j]))*n2[l];
				#pragma omp critical 
				{	bufferNetwork->M2(j, l) += s;	}
			}
			s = 0;
			for( int i = 0; i < (int)pow(N, D); i++) 
				s += (y[i] - n3[i])*((i == j ? 1./n2l1Norm : 0.) - sign(n2[j])*n2[i]/pow(n2l1Norm,2))*(TNeuralNetwork<N, D>::sigmoid(z2[j])*(1.-TNeuralNetwork<N, D>::sigmoid(z2[j])));
			#pragma omp critical 
			{	bufferNetwork->v2[j] += s; }
		
			for( int l = 0; l < (int)pow(N, D); l++) {
				s = 0;
				for( int h = 0; h < (int)pow(N, D); h++) {
					for( int i = 0; i < (int)pow(N, D); i++) {
						s += (y[i] - n3[i])*((i == h ? 1./n2l1Norm : 0.) - sign(n2[h])*n2[i]/pow(n2l1Norm,2))*(TNeuralNetwork<N, D>::sigmoid(z2[h])*(1.-TNeuralNetwork<N, D>::sigmoid(z2[h]))*NeuralNetwork->M2(h,j))
																											*(TNeuralNetwork<N, D>::sigmoid(z1[j])*(1.-TNeuralNetwork<N, D>::sigmoid(z1[j])))*n0[j];
					}
				}
				#pragma omp critical 
				{	bufferNetwork->M1(j, l) += s;	}
			}
			s = 0;
			for( int h = 0; h < (int)pow(N, D); h++) {
					for( int i = 0; i < (int)pow(N, D); i++) {
						s += (y[i] - n3[i])*((i == h ? 1./n2l1Norm : 0.) - sign(n2[h])*n2[i]/pow(n2l1Norm,2))*(TNeuralNetwork<N, D>::sigmoid(z2[h])*(1.-TNeuralNetwork<N, D>::sigmoid(z2[h]))*NeuralNetwork->M2(h,j))
																											*(TNeuralNetwork<N, D>::sigmoid(z1[j])*(1.-TNeuralNetwork<N, D>::sigmoid(z1[j])));
				}
			}
			#pragma omp critical 
			{	bufferNetwork->v1[j] += s;}
		}
		
	}
	
	void BackpropagationStep(const double trainingRate = 0.01)
	{
		auto bufferNetwork = std::make_shared<TNeuralNetwork<N, D> >(TNeuralNetwork<N, D>::Matrix::Zero(), TNeuralNetwork<N, D>::Vector::Zero(), TNeuralNetwork<N, D>::Matrix::Zero(), TNeuralNetwork<N, D>::Vector::Zero());
		
		#pragma omp parallel for
		for(int i = 0; i < TrainingData.size(); i++)
		{
			SingleBackpropagation(bufferNetwork, TrainingData.at(i));
		}
		std::cout << bufferNetwork->M1 << std::endl << bufferNetwork->v1 << std::endl << bufferNetwork->M2 << std::endl << bufferNetwork->v2 << std::endl;
		NeuralNetwork->M1 += trainingRate*bufferNetwork->M1/TrainingData.size();
		NeuralNetwork->v1 += trainingRate*bufferNetwork->v1/TrainingData.size();
		NeuralNetwork->M2 += trainingRate*bufferNetwork->M2/TrainingData.size();
		NeuralNetwork->v2 += trainingRate*bufferNetwork->v2/TrainingData.size();
	}
	
	void Backpropagation(const unsigned int maxSteps, const double minEval = 0, const double trainingRate = 0.1)
	{
		unsigned int i = 0; double e = Evaluate(NeuralNetwork);
		std::cout << "0: " << e << std::endl;
		while(i < maxSteps && e > minEval) {
			BackpropagationStep(trainingRate);
			std::cout << i++ << ": " <<  (e = Evaluate(NeuralNetwork)) << std::endl;
		}
	}
	
	TNeuralNetworkTrainer(std::shared_ptr<TNeuralNetwork<N, D> > NN, std::shared_ptr<TGameTreeNode<N, D> > gT) : NeuralNetwork(NN)
	{
		PrepareData(gT, NN);
	}
	
	TNeuralNetworkTrainer(std::shared_ptr<TNeuralNetwork<N, D> > NN, unsigned int gameTreeDepth) : NeuralNetwork(NN)
	{
		auto gameTree = std::make_shared<TGameTreeNode<N, D> >(TField<N, D>(), TSpaceState::X);
		TAI<N, D>::UpdateGameTree(gameTree, gameTreeDepth);
		TAI<N, D>::MiniMax(gameTree, gameTreeDepth, NN->sign);
		PrepareData(gameTree, NN);
	}
	
};

#endif
