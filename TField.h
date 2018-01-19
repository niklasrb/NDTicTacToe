#ifndef TFIELD_H
#define TFIELD_H

#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

template<unsigned int N, unsigned int D>
class TField;
template<unsigned int N, unsigned int D>
class TDirection;

template<unsigned int N, unsigned int D>
class TIndex
{
	friend class TField<N, D>;
public:
	std::vector<unsigned int> index;
	
	unsigned int GetIndex() const
	{
		int i = 0;
		for(unsigned int d = 1; d <= D; d++)
		{
			i += (index[d-1] - 1)*pow(N, d-1);
		}
		return i;
	}
	
public:
	TIndex(const std::vector<unsigned int>& index) : index(index)
	{
		assert(D == index.size());
		for(unsigned int i = 0; i < D; i++) assert(1 <= index.at(i) && index.at(i) <= N);
	}
	
	TIndex()
	{
		index.resize(D);
		for(unsigned int i = 0; i < D; i++) index.at(i) = 1;
	}
	
	void Print() const
	{
		std::cout << this;
	}
	
	bool Set(const unsigned int d, const unsigned int n)
	{
		if(d == 0 || d > D) return false;
		if(n == 0 || n > N) return false;
		index.at(d-1) = n;
		return true;
	}
	bool Set(const std::vector<unsigned int>& index)
	{
		if(index.size() != D) return false;
		for(int i = 0; i < D; i++) if(index.at(i) < 1 || index.at(i) > N) return false;
		this->index = index;
		return true;
	}
	
	unsigned int at(unsigned int d) const
	{
		assert(d > 0 && d <= D);
		return index.at(d-1);
	}
	
	friend std::ostream& operator <<(std::ostream& o, const TIndex& index)
	{
		o << "("; 
		for(unsigned int i = 0; i < D; i++) o << index.index.at(i) << (i < D-1 ? ", " : ")");
		return o;
	}
	
	friend std::istream& operator >>(std::istream& in, TIndex& index)
	{
		assert(in.good());
		char cbuf; unsigned int ibuf; 
		in >> cbuf;
		assert(cbuf == '(');
		for(unsigned int i = 0; i < D; i++)  {
			assert(in.good());
			in >> ibuf; assert(1 <= ibuf && ibuf <= N);
			index.index.at(i) = ibuf;
			in >> cbuf;
			if(i < D-1) assert(cbuf == ',');
		}
		return in;
	}
	
	TIndex& operator +=(const TDirection<N, D>& dir)
	{
		for(unsigned int i = 0; i < D; i++) {
			index.at(i) += dir.dir.at(i);
			assert(index.at(i) >= 1 && index.at(i) <= N);
		}
		return *this;
	}
	
	friend TIndex operator ++(TIndex& index, int dummy)
	{
		index.index.at(D-1) += 1;
		for(unsigned int d = D-1; d > 0; d--) {
			if(index.index.at(d) > N)
			{
				index.index.at(d) = 1;
				index.index.at(d-1) += 1;
			}
		}
		if(index.index.at(0) > N) index.index.at(0) = 1;
		return index;
	}
	
	TDirection<N, D> IndicateCorners() const
	{
		std::vector<int> cp; cp.resize(D);
		for(unsigned int i = 0; i < D; i++) {
			if(index.at(i) == 1) cp.at(i) = 1;
			else if(index.at(i) == N) cp.at(i) = -1;
			else cp.at(i) = 0;
		}
		return TDirection<N, D>(cp);
	}
	
	static TIndex<N, D> ReadFromCmd()
	{
		std::vector<unsigned int> index; index.resize(D);
		std::cout << "Please enter index, seperating the numbers with spaces: ";
		for(unsigned int i = 0; i < D; i++) {
			if(std::cin.good()) std::cin >> index.at(i);
			while(index.at(i) < 1 || index.at(i) > N) {
				std::cout << "Please reenter dimension " << i+1 << ": "; std::cin >> index.at(i);
			}
		}
		return TIndex<N, D>(index);
	}
	
	friend bool operator ==(const TIndex& ind1, const TIndex& ind2)
	{
		for(unsigned int i = 0; i < D; i++) 
			if(ind1.index.at(i) != ind2.index.at(i)) return false;
		return true;
	}
};

template<unsigned int N, unsigned int D>
class TDirection
{
friend class TIndex<N, D>;
protected:
	std::vector<int> dir;
	
public:
	TDirection(const std::vector<int>& dir) : dir(dir)
	{
		assert(D == dir.size());
		for(unsigned int i = 0; i < D; i++) assert( dir.at(i) <= 1 &&  dir.at(i) >= -1);
	}
	
	TDirection()
	{
		dir.resize(D);
		for(unsigned int i = 0; i < D; i++) dir.at(i) = -1;
	}
	
	void Print() const
	{
		std::cout << this;
	}
	
	friend std::ostream& operator <<(std::ostream& o, const TDirection& dir)
	{
		o << "("; 
		for(unsigned int i = 0; i < D; i++) o << dir.dir.at(i) << (i < D-1 ? ", " : ")");
		return o;
	}
	
	bool Allows(const TDirection& subDir)
	{
		bool check = true; 
		for(unsigned int d = 0; d < D; d++) 
		{	
			if(dir.at(d) == 0 && subDir.dir.at(d) != 0) check = false;
			if(dir.at(d) < 0 && subDir.dir.at(d) > 0) check = false;
			if(dir.at(d) > 0 && subDir.dir.at(d) < 0) check = false;
		}
		return check;
	}
	
	bool NonZero()
	{
		bool c = false;
		for(unsigned int i = 0; i < D; i++) if(dir.at(i) != 0) c = true;
		return c;
	}
	
	friend TDirection operator ++(TDirection& dir, int dummy)
	{
		dir.dir.at(D-1) += 1;
		for(unsigned int d = D-1; d > 0; d--) {
			if(dir.dir.at(d) > 1)
			{
				dir.dir.at(d) = -1;
				dir.dir.at(d-1) += 1;
			}
		}
		if(dir.dir.at(0) > 1) dir.dir.at(0) = -1;
		return dir;
	}
};


enum TSpaceState
{
	empty,
	X, 
	Y
};

template<unsigned int N, unsigned int D>
class TField
{
protected:
	std::vector<TSpaceState> spaces;
	TSpaceState tss;
	
	bool Winner(TSpaceState ss, const TIndex<N, D>& index) const 
	{
		if(Space(index) != ss) return false;	// no need to look for winners
		//std::cout << "Position: " << index << std::endl;
		TDirection<N, D> allowedDirections = index.IndicateCorners();
		
		if(!allowedDirections.NonZero()) return false;	// then we're not at a valid starting point
		//std::cout << std::endl << "with allowed directions " << allowedDirections << std::endl;
		TDirection<N, D> direction; 	// describing which indices will be changed 
		TIndex<N, D> pos = index;	
		for(unsigned int i = 0; i < pow(3, D); i++)	// go through all possible directions
		{
			//std::cout << i << ": test dir: " << direction << std::endl;
			if(allowedDirections.Allows(direction) && direction.NonZero())
			{
				//std::cout << "valid dir: " << direction << std::endl;
				pos = index;
				bool check = true;
				for(unsigned int j = 1; j < N; j++)
				{
					pos += direction;
					//std::cout << "test pos: " << pos << std::endl;
					if(Space(pos) != ss) { check = false; break; }
				}
				if(check) return true;
			}
			direction++;
		}
		return false;
	}
	
	bool Winner(TSpaceState ss) const
	{
		TIndex<N, D> index;
		
		for(unsigned int i = 0; i < pow(N, D); i++)		// Take every space as a possible starting point
		{
			if(Space(index) == ss)	// now check for straight lines, increasing and decreasing
			{
				if(Winner(ss, index)) return true;
			}
			index++;
		}
		return false;
	}
	
public:
	TField()
	{
		spaces.resize((int)pow(N, D));
		Empty();
	}
	
	TSpaceState& Space(const TIndex<N, D>& index)
	{
		return Space(index.GetIndex());
	}
	
	TSpaceState Space(const TIndex<N, D>& index) const
	{
		return Space(index.GetIndex());
	}
	
	TSpaceState& Space(const unsigned int i)
	{
		assert(i <= pow(N,D));
		return spaces.at(i);
	}
	
	TSpaceState Space(const unsigned int i) const
	{
		assert(i <= pow(N,D));
		return spaces.at(i);
	}
	
	void Empty() 
	{
		for(unsigned int i = 0; i < spaces.size(); i++) spaces.at(i) = TSpaceState::empty;
		tss = TSpaceState::empty;
	}
	
	void Print() const 
	{
		TIndex<N, D> index; 
		for(unsigned int i = 0; i < pow(N,D); i++)
		{
			std::cout << index << ": " << Space(index) << '\t';
			index++;
			if((i+1) % N == 0) std::cout << std::endl;
		}
	}
	
	bool Full() const
	{
		for(unsigned int i = 0; i < pow(N,D); i++) 
			if(Space(i) == TSpaceState::empty) return false;
		return true;
	}
	
	
	TSpaceState CheckForWinner()
	{
		
		if(Winner(TSpaceState::X)) tss = TSpaceState::X;
		if(Winner(TSpaceState::Y)) 
		{
			if(tss == TSpaceState::X) tss = TSpaceState::empty;
			else  tss = TSpaceState::Y;			
		}
		return tss;
	}
	
	TSpaceState Winner() const
	{
		return tss;
	}
	
	friend bool operator ==(const TField& f1, const TField& f2)
	{
		for(unsigned int i = 0; i < pow(N, D); i++) {
			if(f1.Space(i) != f2.Space(i)) return false;
		}
		return true;
	}
	
	void Save(std::ostream& o) const
	{
		o << N << '\t' << D << '\t' << (int)tss << std::endl;
		for(unsigned int i = 0; i < spaces.size(); i++)
		{
			o << (int)spaces.at(i) << (i < spaces.size()-1 ? '\t' : '\n');
		}
	}
	
	static TField Load(std::istream& in)
	{
		assert(in.good());
		TField tf;
		int buf; in >> buf; assert(buf == N);
		in >> buf; assert(buf == D);
		in >> buf;	tf.tss = (TSpaceState)buf;
		for(unsigned int i = 0; i < pow(N, D); i++)
		{
			assert(in.good());
			in >> buf;
			tf.Space(i) = (TSpaceState)buf;
		}
		return tf;
	}
	
};


#endif
