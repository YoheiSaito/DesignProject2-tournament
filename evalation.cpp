#include "evalation.hpp"

#include <fstream>
#include <list>
#include <boost/algorithm/string.hpp>

using namespace teyo_shogi;
Evalation::Evalation(std::string evfile){
	for(int i = 0; i < 7; i++){
		insert_point[i][0] = 0;
		insert_point[i][5] = 10000;
	}
	std::string   istr;
	std::ifstream fs(evfile);
	if(fs.fail()){
		throw "file open error " + evfile;
	}
	while(getline(fs, istr)){
		std::list<std::string> list_str;
		std::string delim("\t");
		boost::split(list_str, istr, boost::is_any_of(delim));
		if(list_str.size() < 5)
			continue;
		auto itr = list_str.begin();
		int i = -1;
		try{
			i = std::stoi(*itr);
		}catch (std::invalid_argument e) {
			continue;
		} catch (std::out_of_range e) {
			continue;
		}
		if( i >= 0 && i <= 6){
			for( int j = 0; j < 4; j++){
				itr++;
				insert_point[i][j+1] = std::stoi(*itr);
			}
		} 
		
	}

	Koma_point[0][0][0] = &Koma_point_base[0][0];
	Koma_point[0][0][1] = &Koma_point_base[4][0];
	Koma_point[0][1][0] = &Koma_point_base[2][0];
	Koma_point[0][1][1] = &Koma_point_base[2][0];
	Koma_point[0][2][0] = &Koma_point_base[2][0];
	Koma_point[0][2][1] = &Koma_point_base[2][0];
	Koma_point[0][3][0] = &Koma_point_base[4][0];
	Koma_point[0][3][1] = &Koma_point_base[0][0];

	Koma_point[1][0][0] = &Koma_point_base[1][0];
	Koma_point[1][0][1] = &Koma_point_base[5][0];
	Koma_point[1][1][0] = &Koma_point_base[3][0];
	Koma_point[1][1][1] = &Koma_point_base[3][0];
	Koma_point[1][2][0] = &Koma_point_base[3][0];
	Koma_point[1][2][1] = &Koma_point_base[3][0];
	Koma_point[1][3][0] = &Koma_point_base[5][0];
	Koma_point[1][3][1] = &Koma_point_base[1][0];

#include <iostream>
	for(int t = 0; t < 6; t++){
		for(int x = 0; x < BOARD_WIDTH; x++){
			for(int y = 0; y < BOARD_HEIGHT; y++){
				std::cout << Koma_point[x%2][y][0][t] 
					<< '\t'<< std::flush;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	for(int t = 0; t < 6; t++){
		for(int x = 0; x < BOARD_WIDTH; x++){
			for(int y = 0; y < BOARD_HEIGHT; y++){
				std::cout << Koma_point[x%2][y][1][t] 
					<< '\t'<< std::flush;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

}

int16_t Evalation::evalate(Board_p p){

	int16_t r = 0;
	for(int i = 0; i < BOARD_WIDTH; i++){
		for(int j = 0; j < BOARD_HEIGHT; j++){
			int kt = p->field[i][j].type;
			if( p->field[i][j].player == BLACK){
				r += Koma_point[i%2][j][BLACK][kt];
			}else{
				r -= Koma_point[i%2][j][WHITE][kt];
			}
		}
	}
	for( int i = 0; i < MAX_CAPS; i++){
		r += Mochigoma_point[p->caps[BLACK][i].type];
		r -= Mochigoma_point[p->caps[WHITE][i].type];
	}
	return r;

}

