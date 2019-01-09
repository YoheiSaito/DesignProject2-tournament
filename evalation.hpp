#pragma once
#include <string>
#include "Board.hpp"
namespace teyo_shogi{
struct Evalation
{
	//0, 175, 185, 115, 100, 10000
	// none, zou, Kirin, Niwatori hiyoko, Lion
	union{
		struct{
			int16_t Mochigoma_point[6];
			int16_t Koma_point_base[6][6];
		};
		struct{
			int16_t insert_point[7][6];
		};
	};
	int16_t *Koma_point[2][4][2];

	/* int16_t Mochigoma_point[] */
	/* int16_t Koma_point[2][4][2][6] */
	Evalation(std::string evfilename = "../evl/best.evl");
	int16_t evalate(Board_p p);
};
}
