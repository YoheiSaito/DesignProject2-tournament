#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <random>
#include <string>
#include <map>
#include <algorithm>
#include <omp.h>

#include "ai_core.hpp"
#define MYHW_RMAX 0xffffffff
#define BLX_ALPHA 0.4
using namespace teyo_shogi;

void crossover(char ine1[], char ine2[], char out[]);

int main(int argc, char* argv[]){
	using namespace std;
	if( argc < 3){
		cerr << "usage: " << argv[0] <<" hoge.evl fuge.evl out.evl" 
			<< endl;
		return -1;
	}
	
	crossover(argv[1], argv[2], argv[3]);
	return 0;
}

void crossover(char ine1[], char ine2[], char out[]){
	std::random_device rnd;
	std::ofstream f(out);
	Evalation in1(ine1);
	Evalation in2(ine2);


	for(int i = 0; i < 7; i++){
		f << i << '\t';
		for( int t = 1; t < 5; t++){
			{
				//TODO:BLX-alpha
				double rd;
				rd = (double)((uint32_t)rnd())/(uint32_t)(MYHW_RMAX);
				int tmp1 = in1.insert_point[i][t];
				int tmp2 = in2.insert_point[i][t];
				int min = (tmp1 < tmp2)? tmp1:tmp2;
				int max = (tmp1 > tmp2)? tmp1:tmp2;
				double minP = min - BLX_ALPHA*(max-min);
				double maxP = max + BLX_ALPHA*(max-min);
				minP = (minP < 0) ? 0: minP;
				f << (int)(minP + rd*(maxP - minP)) <<'\t';
			}
		}
		{
			double rd;
			rd = (double)((uint32_t)rnd())/(uint32_t)(MYHW_RMAX);
			uint8_t tmp1 = in1.insert_point[i][5];
			uint8_t tmp2 = in2.insert_point[i][5];
			uint8_t min = (tmp1 < tmp2)? tmp1:tmp2;
			uint8_t max = (tmp1 > tmp2)? tmp1:tmp2;
			double minP = min - BLX_ALPHA*(max-min);
			double maxP = max + BLX_ALPHA*(max-min);
			minP = (minP < 0) ? 0: minP;
			f << (int)(uint8_t)(minP + rd* (maxP - minP));


		}
		f << '\n';
	}
	f.close();
}



