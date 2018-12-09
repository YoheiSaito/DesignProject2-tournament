#include "Board.hpp"
#include <cstring>
#include <memory>
#include <algorithm>
#include <iostream>
#include <memory>
#include <tr1/unordered_map>
#include <functional>
#include <boost/algorithm/string/classification.hpp> 
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
namespace teyo_shogi{

// operators 
bool Board::operator==(Board const& right){
	for (int i = 0; i < MAX_CAPS; i++){
		if( this->caps[BLACK][i].data != right.caps[BLACK][i].data)
			return false;
		if( this->caps[WHITE][i].data != right.caps[WHITE][i].data)
			return false;
	}
	for(int i = 0; i < BOARD_WIDTH; i++){
		for(int j = 0; j < BOARD_HEIGHT; j++){
			if( this->field[j][i].data != right.field[j][i].data)
				return false;
		}
	}
	return true;

}
Board Board::operator=(Board & right){
	this->write(right);
	return *this;
}


Move_t Board::create_move(int turn){
	Move_t moves;
	return moves;
}
bool Board::check_legitimacy(Board const& after){
	return false;
}
Board_p Board::move_piece(int turn, int i, int j, int w, int h){
	Board_p r;
	return r;
}

Move_t Board::lion_check (int turn, int w, int h){
	Move_t r;
	for( int i = -1; i <= 1; i++){
		if( w + i < 0 || w + i >= BOARD_WIDTH){
			continue;
		}
		for( int j = -1; j <= 1; j++){
			if( h + j < 0 || h + j >= BOARD_HEIGHT){
				continue;
			}
			if( i == 0 && j == 0){
				continue;
			}
			auto move = move_piece(turn, w, h, w+i, h+j);
			if(move != nullptr)
				r.push_back(move);
		}
	}
	return r;
}

Move_t Board::hiyoko_check (int turn, int w, int h){
	Move_t r;
	if( turn == BLACK && h > 0){
		auto moved_b = move_piece(turn, w, h, w, h - 1);
		if(moved_b != nullptr){
			if(h == 0){
				moved_b->field[h-1][w].type = NIWATORI;
			}
			r.push_back(moved_b);
		}
	}
	if( turn == WHITE && h < BOARD_HEIGHT - 1){
		auto moved_b = move_piece(turn, w, h, w, h + 1);
		if(moved_b != nullptr){
			if(h == BOARD_HEIGHT - 1){
				moved_b->field[h+1][w].type = NIWATORI;
		}
			r.push_back(moved_b);
		}
	}
	return r;
}


Move_t Board::kirin_check (int turn, int w, int h){
	Move_t r;
	for( int i = -1; i <= 1; i++){
		if( w + i < 0 || w + i >= BOARD_WIDTH){
			continue;
		}
		for( int j = -1; j <= 1; j++){
			if( h + j < 0 || h + j >= BOARD_HEIGHT){
				continue;
			}
			if( i == 0 && j == 0){
				continue;
			}
			if( i*j == 0){
				continue;
			}
			auto move = move_piece(turn, w, h, w+i, h+j);
			if(move != nullptr)
				r.push_back(move);
		}
	}
	return r;
}
Move_t Board::zou_check (int turn, int w, int h){
	Move_t r;
	for( int i = -1; i <= 1; i++){
		if( w + i < 0 || w + i >= BOARD_WIDTH){
			continue;
		}
		for( int j = -1; j <= 1; j++){
			if( h + j < 0 || h + j >= BOARD_HEIGHT){
				continue;
			}
			if( i == 0 && j == 0){
				continue;
			}
			if( i*j != 0){
				continue;
			}
			auto move = move_piece(turn, w, h, w+i, h+j);
			if(move != nullptr)
				r.push_back(move);
		}
	}
	return r;
}

Move_t Board::niwatori_check (int turn, int w, int h){
	Move_t r;
	for( int i = -1; i <= 1; i++){
		if( w + i < 0 || w + i >= BOARD_WIDTH){
			continue;
		}
		for( int j = -1; j <= 1; j++){
			if( h + j < 0 || h + j >= BOARD_HEIGHT){
				continue;
			}
			if( i == 0 && j == 0){
				continue;
			}
			if(turn == BLACK && j == 1 && i != 0){
				continue;
			}
			if(turn == WHITE && j == -1 && i != 0){
				continue;
			}
			auto move = move_piece(turn, w, h, w+i, h+j);
			if(move != nullptr)
				r.push_back(move);
		}
	}
	return r;
}

void Board::write(std::string & command_str){
	std::memset(caps[BLACK], BLANK, MAX_CAPS);
	std::memset(caps[WHITE], BLANK, MAX_CAPS);
	for(int i = 0; i < BOARD_WIDTH; i++){
		std::memset(field[i], 0, BOARD_HEIGHT);
	}
	std::memset(caps[WHITE], BLANK, MAX_CAPS);
	std::string info = command_str;
	int i = 0;
	int n = info.size();
	while(i < n){
		if(info[i] == ' ' || info[i] == ','){
			info.erase(info.begin() + i);
			n--;
		}else{
			i++;
		}
	}
	n = info.size();
	for(i = 0; i < n-1; i += 4){
		Koma k;
		int w, h;
		switch (info[i+2]){
			case 'c':
				k.type = HIYOKO;
				break;
			case 'h':
				k.type = NIWATORI;
				break;
			case 'l':
				k.type = LION;
				break;
			case 'g':
				k.type = KIRIN;
				break;
			case 'e':
				k.type = ZOU;
				break;
			default:
				k.type = BLANK;
		}
		k.player = info[i+3] - '1';
		w = info[i+0] - 'A';
		h = info[i+1] - '1';
		if( w == 3 ){
			this->caps[BLACK][h] = k;
		}else if( w == 4 ){
			this->caps[WHITE][h] = k;
		}else{
			this->field[w][h] = k;
		}
	}
	std::sort((char*)caps[BLACK], (char*)caps[BLACK] + MAX_CAPS);
	std::sort((char*)caps[WHITE], (char*)caps[WHITE] + MAX_CAPS);

}

void Board::write_field(uint8_t new_field[BOARD_HEIGHT][BOARD_WIDTH]){
	for(int i = 0; i < BOARD_HEIGHT; i++){
		for( int j = 0; j < BOARD_WIDTH; j++){
			this->field[i][j].data = new_field[i][j];
		}
	}
}

void Board::write_black_capture(uint8_t new_caps[MAX_CAPS]){
	for (int i = 0; i < MAX_CAPS; i ++){
		this->caps[BLACK][i].data = new_caps[i];
		this->caps[BLACK][i].player = BLACK;
	}
}
void Board::write_white_capture(uint8_t new_caps[MAX_CAPS]){
	for (int i = 0; i < MAX_CAPS; i ++){
		this->caps[WHITE][i].data = new_caps[i];
		this->caps[WHITE][i].player = WHITE;
	}
}
void Board::write_capture(uint8_t new_caps[][MAX_CAPS]){
	for (int i = 0; i < MAX_CAPS; i ++){
		this->caps[BLACK][i].data = new_caps[BLACK][i];
		this->caps[BLACK][i].player = BLACK;
		this->caps[WHITE][i].data = new_caps[WHITE][i];
		this->caps[WHITE][i].player = WHITE;
	}

}
void Board::write(Board & board){
	for (int i = 0; i < MAX_CAPS; i ++){
		this->caps[BLACK][i].data = board.caps[BLACK][i].data;
		this->caps[WHITE][i].data = board.caps[WHITE][i].data;
	}
	for(int i = 0; i < BOARD_HEIGHT; i++){
		for( int j = 0; j < BOARD_WIDTH; j++){
			this->field[i][j].data = board.field[i][j].data;
		}
	}
}


void Board::print(){

	std::cout << this->hash() << std::endl;
	std::cout << "WHITE MOCHIGOMA: " << std::flush;
		for (int i = 0; i < MAX_CAPS; i ++){
		std::cout << " " << koma_to_char(this->caps[WHITE][i]) 
			<< " " << std::flush;
	}
	std::cout << std::endl;
	for(int i = 0; i < BOARD_HEIGHT; i++){
		for( int j = 0; j < BOARD_WIDTH; j++){
			std::cout << " | "
				<< koma_to_char(this->field[j][i])
				<< std::flush;
		}
			std::cout << '|' << std::endl;
	}
	std::cout << "BLACK MOCHIGOMA: " << std::flush;
	for (int i = 0; i < MAX_CAPS; i ++){
		std::cout << " " << koma_to_char(this->caps[BLACK][i]) 
			<< " " << std::flush;
	}
	std::cout << std::endl;
}

char Board::koma_to_char (Koma& t){
	char rt = '-';
	switch(t.type){
		case LION:
			rt = 'l';
			break;
		case KIRIN:
			rt = 'k';
			break;
		case ZOU:
			rt = 'z';
			break;
		case NIWATORI:
			rt = 'n';
			break;
		case HIYOKO:
			rt = 'h';
			break;
		default:
			return rt;
	}
	if(t.player == BLACK){
		rt += 'A' - 'a';
	}
	return rt;
}

size_t Board::hash(){
	std::string board_str = "";
	for( int i = 0; i < BOARD_WIDTH; i++){
		for(int j = 0; j < BOARD_HEIGHT; j++)
		board_str.push_back(field[i][j].data);
	}
	for( int i = 0; i < MAX_CAPS; i++){
		board_str.push_back(caps[BLACK][i].data);
	}
	for( int i = 0; i < MAX_CAPS; i++){
		board_str.push_back(caps[WHITE][i].data);
	}
	return std::hash<std::string>()(board_str);
}
};
