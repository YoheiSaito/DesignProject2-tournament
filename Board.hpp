#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_map>
#include "komadef.hpp"
#include "Command.hpp"
namespace teyo_shogi{

extern "C"{

struct Koma{
	union{
		uint8_t data;
		struct{
			uint8_t type:4;
			uint8_t player:4;
		};
	};
};

}
struct Board;

typedef enum {
	ALL,
	CHECK,
	AVOID,

} Genmove_t;

using Board_p = std::shared_ptr<Board>;
using Move_t  = std::vector<Board_p>; 
using Move_p  = std::shared_ptr<Move_t>; 

struct Board{
	Koma caps[2][MAX_CAPS];
	Koma field[BOARD_WIDTH][BOARD_HEIGHT];
	struct Hash{
		typedef std::size_t result_type;
		std::size_t operator()(const Board& key) const;
	};
	// method
	bool operator==(Board & right);
	bool operator<(Board & right);
	bool operator>(Board & right);
	bool operator!=(Board & right);
	Board operator=(Board & right);
	static inline char koma_to_char (Koma &t);
	Move_t generate_move(int turn);
	Move_t generate_check(int turn);
	Move_t generate_avoid_die(int turn);
	Board_p is_checkmate(int player);
	/* Move_t generate_safemove(int player){ */
	bool check_legitimacy(Board & after, int self);
	//w,hにplayerのコマの利きがあるか
	bool is_check(int w, int h, int player);
	void write(std::string & command_str, bool sorting = true);
	void write_field(Koma new_field[BOARD_WIDTH][BOARD_HEIGHT]);
	void write_black_capture(uint8_t new_caps[MAX_CAPS]);
	void write_white_capture(uint8_t new_caps[MAX_CAPS]);
	void write_capture(Koma new_caps[][MAX_CAPS]);
	void write(Board & board);
	void print();
	inline size_t hash(){
		std::string bytes(
			reinterpret_cast<const char*>(this), sizeof(Board)
		);
		return std::hash<std::string>()(bytes);
	}
	Board_p move_piece    (std::string src, std::string dst);
	bool to_command(Command& cmd, int turn, Board_p after);
	private:
	inline Move_t lion_check     (int w, int h);
	inline Move_t niwatori_check (int w, int h);
	inline Move_t hiyoko_check   (int w, int h);
	inline Move_t zou_check      (int w, int h);
	inline Move_t kirin_check    (int w, int h);
	Board_p move_piece (int i, int j, int w, int h, 
			bool avoid_ckeck = false);
}; // struct Board 


}; // namespace teyo_shogi
