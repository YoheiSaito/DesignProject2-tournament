#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include "definition.hpp"
namespace teyo_shogi{

extern "C"{

struct Koma{
	union{
		uint8_t data;
		struct{
			uint8_t dum:4;
			uint8_t type:3;
			uint8_t player:1;
		};
	};
};

}
struct Board;

using Board_p = std::shared_ptr<Board>;
using Move_t  = std::vector<Board_p>; 

struct Board{
	Koma caps[2][MAX_CAPS];
	Koma field[BOARD_WIDTH][BOARD_HEIGHT];

	// method
	bool operator==(Board const& right);
	Board operator=(Board & right);
	static char koma_to_char     (Koma &t);
	Move_t create_move(int turn);
	bool check_legitimacy(Board const& after);
	void write(std::string & command_str);
	void write_field(uint8_t new_field[BOARD_HEIGHT][BOARD_WIDTH]);
	void write_black_capture(uint8_t new_caps[MAX_CAPS]);
	void write_white_capture(uint8_t new_caps[MAX_CAPS]);
	void write_capture(uint8_t new_caps[][MAX_CAPS]);
	void write(Board & board);
	void print();
	size_t hash();
	private:
	Move_t lion_check     (int turn, int w, int h);
	Move_t niwatori_check (int turn, int w, int h);
	Move_t hiyoko_check   (int turn, int w, int h);
	Move_t zou_check      (int turn, int w, int h);
	Move_t kirin_check    (int turn, int w, int h);
	Board_p move_piece(int turn, int i, int j, int w, int h);

}; // struct Board


}; // namespace teyo_shogi
