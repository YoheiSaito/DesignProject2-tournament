#include "util.hpp"
#define MYHW_RMAX 0xffffffff

#include <iostream>
double teyo_shogi::util::get_random(){
	std::random_device rnd;
	double r = (double)((uint32_t)rnd())/(uint32_t)(MYHW_RMAX);
	std::cout << r << std::endl;
	return r;
}

teyo_shogi::Board_p teyo_shogi::util::generate_random_board(){
	Board_p b(new Board);
	int x, y;

	std::memset(b->caps[BLACK], BLANK, MAX_CAPS);
	std::memset(b->caps[WHITE], BLANK, MAX_CAPS);
	std::memset(&b->field[0][0], 0, BOARD_WIDTH*BOARD_HEIGHT);
	do{
		x = (int)(get_random()*BOARD_WIDTH);
		y = (int)(get_random()*(BOARD_HEIGHT-1))+1;
	}while( x==1 && y==1);
	b->field[x][y].type = LION;
	b->field[x][y].player = BLACK;
	
	do{
		b->print();
		x = (int)(get_random()*BOARD_WIDTH);
		y = (int)(get_random()*(BOARD_HEIGHT-1));
	}while(b->field[x][y].type != BLANK || b->is_check(x,y, BLACK));

	b->field[x][y].type = LION;
	b->field[x][y].player = WHITE;


	int bmochi = 0;
	int wmochi = 0;
	for( int j = 0; j < 2; j++){
	for(int i = 1; i <= 3; i++){
		x = (int)(get_random()*BOARD_WIDTH);
		y = (int)(get_random()*(BOARD_HEIGHT+2));
		if(y == 4){
			if( i != 3)
				b->caps[BLACK][bmochi].type = i;
			else
				b->caps[BLACK][bmochi].type = i + 1;
			b->caps[BLACK][bmochi].player = BLACK;
			bmochi++;
		}else if(y == 5){
			if( i != 3)
				b->caps[WHITE][wmochi].type = i;
			else
				b->caps[WHITE][wmochi].type = i + 1;
			
			b->caps[WHITE][wmochi].player = WHITE;
			wmochi++;
		}else {
			while(b->field[x][y].type != BLANK){
				x = (int)(get_random()*BOARD_WIDTH);
				y = (int)(get_random()*BOARD_HEIGHT);
			}
			int t = (int)(get_random()*2);
			int hiyorni = (get_random()<0.1)?0:1;
			b->field[x][y].player = t;
			if( i != 3)
				b->field[x][y].type = i;
			else
				b->field[x][y].type = i + hiyorni;
		}
	}
	}
	return b;
}
