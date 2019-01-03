#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "Board.hpp"
namespace teyo_shogi{
struct EvalScore{
	union{
		int16_t kp_array[35];
		/*  koma.type  ->  */ 
		/* top-center    5 */
		/* bottom-center 5 */
		/* midle-center  5 */
		/* top-side      5 */
		/* bottom-side   5 */
		/* midle-side    5 */
		/* mochiKoma     5 */

		struct{
			int16_t koma_point_side[3][5];
			int16_t koma_point_center[3][5];
			int16_t motigoma_point_[5];
		};
	};
	
	inline int16_t motigoma_point(int type){
		return motigoma_point_[type-1];
	}
	inline int16_t koma_point(int x, int y, int p, int type){
		static int tt[2][4]={
			{ 0,2,2,1 },
			{ 1,2,2,0 },
		};
		return (x==1) ?
			koma_point_center[tt[p][y]][type-1]:
			koma_point_side  [tt[p][y]][type-1];
	};
	
	EvalScore(std::string filename){
		std::ifstream ifs(filename);
		if(!ifs){
			throw "Eval File Open Error";
		}
		int i = 0;
		std::string buf;
		while( std::getline(ifs, buf)){
			kp_array[i] = std::stoi(buf);
			i++;
			if(i == 35)
				break;
		}
		for(int i = 0; i < 5;i++){
			std::cout << motigoma_point(i+1) <<'\t'<< std::flush;
		}
		std::cout << std::endl;
		for(int k = 0; k < 5; k++){
		for( int j = 0; j < 3; j++){
			for( int i = 0; i < 4; i++){
				std::cout << koma_point(j,i,0,k+1) <<'\t'<< std::flush;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		}
	}
	
	int16_t evalate(Board_p p, int turn){
		int16_t r = 0;
		for(int i = 0; i < BOARD_WIDTH; i++){
			for(int j = 0; j < BOARD_HEIGHT; j++){
				int kt = p->field[i][j].type;
				if(p->field[i][j].player == BLACK && kt!=BLANK)
					r += koma_point(i, j, BLACK, kt);
				else if(kt != BLANK)
					r -= koma_point(i, j, WHITE, kt);
			}
		}
		for( int i = 0; i < MAX_CAPS; i++){
			r += motigoma_point(p->caps[BLACK][i].type);
			r += motigoma_point(p->caps[WHITE][i].type);
		}
		return r;
	}
};
};
