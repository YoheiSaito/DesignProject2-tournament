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
/* #include <boost/algorithm/string/trim.hpp> */
namespace teyo_shogi{

// operators 
bool Board::operator==(Board & right){
	return this->hash() == right.hash();
}
bool Board::operator!=(Board & right){
	return this->hash() != right.hash();
}
Board Board::operator=(Board & right){
	this->write(right);
	return *this;
}
bool Board::operator<(Board & right){
	return this->hash() < right.hash();
}
bool Board::operator>(Board & right){
	return this->hash() > right.hash();
}


inline Move_t Board::generate_move(int turn){
	Move_t ret;
	const int enemy = (turn == BLACK)?WHITE:BLACK;
	for( int i = 0; i < BOARD_WIDTH; i++){
		for( int j = 0; j < BOARD_HEIGHT; j++){
			if( field[i][j].player == turn){
				Move_t r;
				switch(field[i][j].type){
					case HIYOKO:
						r = hiyoko_check(i, j);
						break;
					case NIWATORI:
						r = niwatori_check(i, j);
						break;
					case LION:
						r = lion_check(i, j);
						break;
					case ZOU:
						r = zou_check(i, j);
						break;
					case KIRIN:
						r = kirin_check(i, j);
						break;
					default:
						break;
				}
				if( r.size() == 0)
					continue;
				for(auto&& x : r){
					ret.push_back(x);
				}
			}
		}
	}
	for( int i = 0; i < MAX_CAPS; i++){
		// 盤面のコピー
		if(this->caps[turn][i].type == BLANK){
			break;
		}
		if(i != 0 && this->caps[turn][i].type ==
			       	this->caps[turn][i-1].type)
			continue;
		Koma release = this->caps[turn][i];

		for( int j = 0; j < BOARD_WIDTH; j++){
			for( int k = 0; k < BOARD_HEIGHT; k++){
				if( field[j][k].type == BLANK){



// Down Indent
	/* コピーして挿入 */
	Board_p nb(new Board);
	for( int l = 0; l < MAX_CAPS-1; l++){
		if( l >= i)
			nb->caps[turn][l] = this->caps[turn][l+1];
		else
			nb->caps[turn][l] = this->caps[turn][l];
	}
	for( int l = 0; l < MAX_CAPS; l++){
		nb->caps[enemy][l] = this->caps[enemy][l];
	}
	nb->caps[turn][MAX_CAPS - 1].data = 0;
	nb->write_field(this->field);
	nb->field[j][k] = release;
	ret.push_back(nb);
	/*コピーして挿入*/

// Up Indent


				}
			}
		}
	}
	return ret;
}
bool Board::check_legitimacy(Board & after, int self){
	int e = (self == BLACK)?WHITE:BLACK;
	Move_t mvs = this->generate_move(e);

	for( auto&&i : mvs){
		if(i->hash() == after.hash()){
			return true;
		}
	} 
	return false;
}
//w,hにplayerのコマの利きがあるか
bool Board::is_check(int w, int h, int player){
	if(h - 1 >= 0){
		// 直上
		Koma t = field[w][h-1];
		if(t.player == player){
			switch(t.type){
				case HIYOKO:
				//WHITEのひよこが上にいるときは利きあり
				if(t.player != WHITE)
					break;
				case NIWATORI:
				case KIRIN:
				case LION:
				return true;
				default:
					break;
			} 
		}
	}
	if(h + 1 <  BOARD_HEIGHT){
		//直下
		Koma t = field[w][h+1];
		if(t.player == player){
			switch(t.type){
				case HIYOKO:
				if(t.player != BLACK)
					break;
				case NIWATORI:
				case KIRIN:
				case LION:
				return true;
				default:
					break;
			} 
		}
	}
	if(w - 1 >= 0 ){
		//左
		Koma t = field[w-1][h];
		if(t.player == player){
			switch(t.type){
				case NIWATORI:
				case KIRIN:
				case LION:
				return true;
				default:
					break;
			} 
		}
		if(h - 1 >= 0){
			//左上
			Koma t = field[w-1][h-1];
			if(t.player == player){
				switch(t.type){
					case NIWATORI:
					//後手のひよこの利きはある
					if(t.player != WHITE)
						break;
					case ZOU:
					case LION:
					return true;
					default:
						break;
				} 
			}
		}
		if(h + 1 <  BOARD_HEIGHT){
			Koma t = field[w-1][h+1];
			if(t.player == player){
				switch(t.type){
					case NIWATORI:
					if(t.player != BLACK)
						break;
					case ZOU:
					case LION:
					return true;
					default:
						break;
				} 
			}
		}
	}

	if(w + 1 < BOARD_WIDTH){
		Koma t = field[w+1][h];
		if(t.player == player){
			switch(t.type){
				case NIWATORI:
				case KIRIN:
				case LION:
				return true;
				default:
					break;
			} 
		}
		if(h - 1 >= 0){
			Koma t = field[w+1][h-1];
			if(t.player == player){
				switch(t.type){
					case NIWATORI:
					if(t.player != WHITE)
						break;
					case LION:
					case ZOU:
					return true;
					default:
						break;
				} 
			}
		}
		if(h + 1 <  BOARD_HEIGHT){
			Koma t = field[w+1][h+1];
			if(t.player == player){
				switch(t.type){
					case NIWATORI:
					if(t.player != BLACK)
						break;
					case ZOU:
					case LION:
					return true;
					default:
						break;
				} 
			}
		}
	}
	return false;
}
Board_p Board::move_piece(std::string src, std::string dst){
	return move_piece(src[1]-'A', src[0]-'0', dst[1]-'A', dst[0]-'0');
}
Board_p Board::move_piece(int x, int y, int w, int h, bool avoid_flag){
	Board_p r(new Board);
	r->write_capture(this->caps);
	r->write_field(this->field);
	if(avoid_flag){
		int e = (field[x][y].player == BLACK)?WHITE:BLACK;
		if(this->is_check(w, h, e)){
			//利きがあって駒が敵ライオンでなければ候補に入れない
			if( this->field[w][h].type != LION)
				return nullptr;
		}
	}
	// 現在地と次の位置のコマの所属が同じなら移動できない
	// あるいは現在地に駒が無ければ移動できない
		//次の地点が空ならそのまま移動できる
	if(field[w][h].type == BLANK){
		r->field[w][h].data = this->field[x][y].data;
		r->field[x][y].type = BLANK;
		r->field[x][y].player = BLANK;
	}else if(field[w][h].player != field[x][y].player){
		int turn = field[x][y].player;
		//次の地点が現在地点のコマと別なら駒を取る
		r->caps[turn][MAX_CAPS-1].type = 
			(field[w][h].type==NIWATORI)?HIYOKO:field[w][h].type;
		r->caps[turn][MAX_CAPS-1].player = turn;
		r->field[w][h].data = field[x][y].data;
		r->field[x][y].data = BLANK;
	} else if(field[x][y].player == field[w][h].player){
		return nullptr;
	}
	
	std::sort((uint8_t*)r->caps[BLACK], 
		(uint8_t*)r->caps[BLACK] + MAX_CAPS, std::greater<uint8_t>());
	std::sort((uint8_t*)r->caps[WHITE], 
		(uint8_t*)r->caps[WHITE] + MAX_CAPS, std::greater<uint8_t>());
	return r;
}

Move_t Board::lion_check (int w, int h){
	Move_t r;
	/* int turn = field[w][h].player; */
	/* int enemy = (turn == BLACK)?WHITE:BLACK; */
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
			auto move = move_piece(w, h, w+i, h+j);
			if(move != nullptr)
				r.push_back(move);
		}
	}
	return r;
}
Move_t Board::hiyoko_check (int w, int h){
	Move_t r;
	int turn = field[w][h].player;
	if( turn == BLACK && h > 0){
		auto moved_b = move_piece(w, h, w, h - 1);
		if(moved_b != nullptr){
			if(h-1 == 0){
				moved_b->field[w][h-1].type = NIWATORI;
			}
			r.push_back(moved_b);
		}
	}
	if( turn == WHITE && h < BOARD_HEIGHT - 1){
		auto moved_b = move_piece(w, h, w, h + 1);
		if(moved_b != nullptr){
			if(h+1 == BOARD_HEIGHT - 1){
				moved_b->field[w][h+1].type = NIWATORI;
			}
			r.push_back(moved_b);
		}
	}
	return r;
}


Move_t Board::kirin_check ( int w, int h){
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
			auto move = move_piece(w, h, w+i, h+j);
			if(move != nullptr){
				r.push_back(move);
			}
		}
	}
	return r;
}
Move_t Board::zou_check (int w, int h){
	Move_t r;
	for( int i = -1; i <= 1; i++){
		if( w + i < 0 || w + i >= BOARD_WIDTH){
			continue;
		}
		for( int j = -1; j <= 1; j++){
			if( h + j < 0 || h + j >= BOARD_HEIGHT){
				continue;
			}
			if( i*j == 0){
				continue;
			}
			auto move = move_piece(w, h, w+i, h+j);
			if(move != nullptr){
				r.push_back(move);
			}
		}
	}
	return r;
}

Move_t Board::niwatori_check (int w, int h){
	Move_t r;
	int turn = field[w][h].player;
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
			auto move = move_piece(w, h, w+i, h+j);
			if(move != nullptr)
				r.push_back(move);
		}
	}
	return r;
}

void Board::write(std::string & command_str, bool sorting){
	std::memset(caps[BLACK], BLANK, MAX_CAPS);
	std::memset(caps[WHITE], BLANK, MAX_CAPS);
	std::memset(&field[0][0], 0, BOARD_WIDTH*BOARD_HEIGHT);
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
				k.player = info[i+3] - '1';
				break;
			case 'h':
				k.type = NIWATORI;
				k.player = info[i+3] - '1';
				break;
			case 'l':
				k.type = LION;
				k.player = info[i+3] - '1';
				break;
			case 'g':
				k.type = KIRIN;
				k.player = info[i+3] - '1';
				break;
			case 'e':
				k.type = ZOU;
				k.player = info[i+3] - '1';
				break;
			default:
				k.type = BLANK;
				k.player = BLANK;
				break;
		}
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
	if(sorting){
		std::sort((uint8_t*)caps[BLACK], (uint8_t*)caps[BLACK] +
				MAX_CAPS, std::greater<uint8_t>());
		std::sort((uint8_t*)caps[WHITE], (uint8_t*)caps[WHITE] +
				MAX_CAPS, std::greater<uint8_t>());
	}
}

void Board::write_field(Koma new_field[BOARD_WIDTH][BOARD_HEIGHT]){
	for(int i = 0; i < BOARD_HEIGHT; i++){
		for( int j = 0; j < BOARD_WIDTH; j++){
			this->field[j][i]= new_field[j][i];
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
void Board::write_capture(Koma new_caps[][MAX_CAPS]){
	for (int i = 0; i < MAX_CAPS; i ++){
		this->caps[BLACK][i] = new_caps[BLACK][i];
		this->caps[WHITE][i] = new_caps[WHITE][i];
	}

}
void Board::write(Board & board){
	for (int i = 0; i < MAX_CAPS; i ++){
		this->caps[BLACK][i].data = board.caps[BLACK][i].data;
		this->caps[WHITE][i].data = board.caps[WHITE][i].data;
	}
	for(int i = 0; i < BOARD_HEIGHT; i++){
		for( int j = 0; j < BOARD_WIDTH; j++){
			this->field[j][i].data = board.field[j][i].data;
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
			std::cout << " | " << std::endl;
	}
	std::cout << "BLACK MOCHIGOMA: " << std::flush;
	for (int i = 0; i < MAX_CAPS; i ++){
		std::cout << " " << koma_to_char(this->caps[BLACK][i]) 
			<< " " << std::flush;
	}
	std::cout <<  std::endl;
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

bool Board::to_command(Command& cmd, int turn, Board_p after){
	bool pay = false;
	std::vector<int> ws;
	std::vector<int> hs;
	for( int i = MAX_CAPS-1; i >= 0 ; i--){
		if(this->caps[turn][i].data!= after->caps[turn][i].data){
			if(after->caps[turn][i].type == 0)
				pay = true;
		}
	} 
	for(int i = 0; i < BOARD_WIDTH; i++){
		for( int j = 0; j < BOARD_HEIGHT; j++){
			if(after->field[i][j].data != this->field[i][j].data){
				ws.push_back(i);
				hs.push_back(j);
			}
		}
	}

	if( pay ){
		Koma moved_koma;
		uint8_t i = 0;
		Board row_board;
		std::string t = cmd.board();
		row_board.write( t, false);

		for(i = 0; i < MAX_CAPS; i++){
			if(this->caps[turn][i].data!=after->caps[turn][i].data){
				moved_koma.data = this->caps[turn][i].data;
				break;
			}
		} 
		if(ws.size() != 1){
			return false;
		}

		row_board.print();
		for(i = 0; i < MAX_CAPS; i++){
			/* 指定した駒と生ボード値の駒の種類が一致したらDn */
			if(row_board.caps[turn][i].type == moved_koma.type)
				break;
		}
		std::cout <<(int) i <<std::endl;
		std::string src = {(char)('D'+ turn)};
		src.push_back((char)( i+'1'));
		std::string dst = {(char)(ws[0] +'A')};
		dst.push_back((char)(hs[0] +'1'));
		return cmd.move(src, dst);
	}else{
		if(ws.size() != 2){
			return false;
		}
		uint8_t srx, sry;
		uint8_t dsx, dsy;
		if( this->field[ws[0]][hs[0]].player == turn&& 
				this->field[ws[0]][hs[0]].type != BLANK){
			srx = ws[0];
			sry = hs[0];
			dsx = ws[1];
			dsy = hs[1];
		}else{
			srx = ws[1];
			sry = hs[1];
			dsx = ws[0];
			dsy = hs[0];
		}
		std::string src = {(char)('A'+ srx)};
		src.push_back((char)( sry+'1'));
		std::string dst = {(char)(dsx +'A')};
		dst.push_back((char)(dsy +'1'));
		std::cout << "mv " << src << ' '<<  dst<< std::endl;
		return cmd.move(src, dst);
	}
}

std::size_t Board::Hash::operator()(const Board& key) const {
	std::string bytes(reinterpret_cast<const char*>(&key), sizeof(Board));
	return std::hash<std::string>()(bytes);
}

}

