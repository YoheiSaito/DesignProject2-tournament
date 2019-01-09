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

void elite(char ine[], char out[]);
void crossover(char ine1[], char ine2[], char out[]);
int confront(char argv1[], char argv2[]);

int main(int argc, char* argv[]){
	using namespace std;
	static const int sqN = 32;
	static const int N = sqN * sqN;
	if( argc < 3){
		cerr << "usage: " << argv[0] <<" dirctory generation .hoge" 
			<< endl;
		return -1;
	}
	
	vector<pair<int, string>> entry(N);
	int g = stoi(argv[2]);
	for( int i = 0; i < N; i++){
		entry[i].first = 0;
		entry[i].second = argv[1] + to_string(g)
			+ "-" + to_string(i+1) + argv[3];
	}
	for( int i = 0; i < N; i++){
#pragma omp parallel for 
		for( int j = i+1; j < N; j++){
			int p = confront(
				const_cast<char*>(entry[i].second.c_str()),
				const_cast<char*>(entry[j].second.c_str())
			);
			entry[i].first += p;
			entry[j].first -= p;
			cout << "vs\t" << i<<"-"<<j << "\t"<<p  << endl;
		}
	}
	std::sort( entry.begin(), entry.end());
#pragma omp parallel for
	for( int i = 0; i < sqN; i++){
		for( int j = 0; j < sqN; j++){
			int id = sqN*i+j + 1;
			string opfname = argv[1] + to_string(g+1)
				+ "-" + to_string(id) + argv[3];
			if( i == j ){
				elite(
				const_cast<char*>(entry[N-i-1].second.c_str()),
				const_cast<char*>(opfname.c_str())
				);
			}else{
				crossover(
				const_cast<char*>(entry[N-i-1].second.c_str()),
				const_cast<char*>(entry[N-j-1].second.c_str()),
				const_cast<char*>(opfname.c_str())
				);
			}
			std::cout << opfname << std::endl;
			id++;
		}
	}
	std::cout << "ranking " << std::endl;
	for( auto&& i :entry){
		std::cout << i.first << i.second << std::endl;
	}
}

void elite(char ine[], char out[]){
	std::ifstream e(ine);
	std::string str((std::istreambuf_iterator<char>(e)),
			std::istreambuf_iterator<char>());
	std::ofstream f(out);
	f << str;
	f.close();
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



int confront(char argv1[], char argv2[]){
	try{
		Board_p base(new Board);
		const int depth = 2;
		int ret = 0;
		{

			DobutsuAI blackAI(argv1), whiteAI(argv2);
			Board_p pre_board(new Board);
			std::string init_str = std::string(INITBOARD);
			base->write(init_str);
			while(1){
				bool draw_flg = false;
				if( whiteAI.is_win(base, WHITE)){
					ret-=2;
					break;
				}
				if( blackAI.game_tree[BLACK][base->hash()].emerge>3){
					draw_flg |= true;
				}
				base = blackAI.adventure(base, BLACK, depth);

				if( blackAI.is_win(base, BLACK)){
					ret+=2;
					break;
				}
				if( whiteAI.game_tree[WHITE][base->hash()].emerge>3){
					draw_flg |= true;
				}
				base = whiteAI.adventure(base, WHITE, depth);
				if(draw_flg){
					ret += 1;
					break;
				}
			}//while
		}
		{
			DobutsuAI blackAI(argv2), whiteAI(argv1);
			Board_p pre_board(new Board);
			std::string init_str = std::string(INITBOARD);
			base->write(init_str);
			while(1){
				bool draw_flg = false;
				if( whiteAI.is_win(base, WHITE)){
					ret+=2;
					break;
				}
				if( blackAI.game_tree[BLACK][base->hash()].emerge>2){
					draw_flg |= true;
				}
				base = blackAI.adventure(base, BLACK, depth);
				if( blackAI.is_win(base, BLACK)){
					ret-=2;
					break;
				}
				if( whiteAI.game_tree[WHITE][base->hash()].emerge>2){
					draw_flg |= true;
				}
				base = whiteAI.adventure(base, WHITE, depth);
				if(draw_flg){
					ret -= 1;
					break;
				}
			}//while
		}
		return ret;
	} catch(std::string e){
		std::cerr << e << std::endl;
		exit(-1);
	} catch( char const* e){
		std::cerr << e << std::endl;
		exit(-1);
	}catch (std::logic_error e){
		std::cerr << e.what() << std::endl;
		exit(-1);
	}
	return 0;
}


