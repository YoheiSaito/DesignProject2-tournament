#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "ai_core.hpp"
#include <random>

using namespace teyo_shogi;
int main(int argc, char* argv[]){
	if(argc != 3){
		std::cerr << argv[0] << " usage: hoge.evl huba.evl" << std::endl;
		return 255;
	}
	try{
		Board_p base(new Board);
		int depth = 3;
		int ret = 2;
		{
		
		DobutsuAI blackAI(argv[1]), whiteAI(argv[2]);
		Board_p pre_board(new Board);
		std::string init_str = std::string(INITBOARD);
		base->write(init_str);
		while(1){
			bool draw_flg = false;
			/* base->print(); */
			if( whiteAI.is_win(base, WHITE)){
				std::cout << "WHITE WIN" <<std::endl;
				ret--;
				break;
			}
			if( blackAI.game_tree[BLACK][base->hash()].emerge>2){
				draw_flg |= true;
			}
			base = blackAI.adventure(base, BLACK, depth);
			/* base->print(); */

			if( blackAI.is_win(base, BLACK)){
				std::cout << "BLACK WIN" <<std::endl;
				ret++;
				break;
			}
			if( whiteAI.game_tree[WHITE][base->hash()].emerge>2){
				draw_flg |= true;
			}
			base = whiteAI.adventure(base, WHITE, depth);
			if(draw_flg)
				break;
		}//while
		}
		{
		DobutsuAI blackAI(argv[2]), whiteAI(argv[1]);
		Board_p pre_board(new Board);
		std::string init_str = std::string(INITBOARD);
		base->write(init_str);
		while(1){
			bool draw_flg = false;
			/* base->print(); */
			if( whiteAI.is_win(base, WHITE)){
				std::cout << "WHITE WIN" <<std::endl;
				ret--;
				break;
			}
			if( blackAI.game_tree[BLACK][base->hash()].emerge>2){
				draw_flg |= true;
			}
			base = blackAI.adventure(base, BLACK, depth);
			/* base->print(); */
			if( blackAI.is_win(base, BLACK)){
				std::cout << "BLACK WIN" <<std::endl;
				ret++;
				break;
			}
			if( whiteAI.game_tree[WHITE][base->hash()].emerge>2){
				draw_flg |= true;
			}
			base = whiteAI.adventure(base, WHITE, depth);
			if(draw_flg)
				break;
		}//while
		}
		return ret;
	} catch(std::string e){
		std::cerr << e << std::endl;
	} catch( char const* e){
		std::cerr << e << std::endl;
	}catch (std::logic_error e){
		std::cerr << e.what() << std::endl;
	}
}


