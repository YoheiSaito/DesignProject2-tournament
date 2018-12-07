#include "Board.hpp"
#include <cstring>
#include <memory>
#include <algorithm>
#include <iostream>
namespace teyo_shogi{

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

Move_t Board::create_move(int turn){}
bool Board::check_legitimacy(Board const& after){}
//TODO create
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

}
