#include "Board.hpp"
#include <iostream>
int main(void){
	teyo_shogi::Board test;
	uint8_t initial_field[][BOARD_WIDTH] = {
		{KIRIN+WHITE,   LION+WHITE,   ZOU+WHITE},
		{BLANK+BLANK, HIYOKO+WHITE, BLANK+BLANK},
		{BLANK+BLANK, HIYOKO+BLACK, BLANK+BLANK},
		{  ZOU+BLACK,   LION+BLACK, KIRIN+BLACK},
	};
	uint8_t initial_caps[] = {
		0,0,0,0,0,0
	};
	test.write_field(initial_field);
	test.write_white_capture(initial_caps);
	test.write_black_capture(initial_caps);
	auto b = test.create_move(BLACK);
	/* test.print(); */
	std::cout << "black move"<<std::endl;
	/* for( auto x:b){ */
	/* 	std::cout<<std::endl; */
	/* 	x.print(); */
	/* 	std::cout<<std::endl; */
	/* } */
	/* auto w = test.create_move(WHITE); */
	/* std::cout << "white move"<<std::endl; */
	/* for( auto x:w){ */
	/* 	std::cout<<std::endl; */
	/* 	x.print(); */
	/* 	std::cout<<std::endl; */
	/* } */

}
