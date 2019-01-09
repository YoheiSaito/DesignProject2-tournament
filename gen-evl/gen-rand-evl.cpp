#include <iostream>
#include <fstream>
#include <random>
int main(int argc, char* argv[]){
	if( argc == 1){
	std::cout << "usage " << argv[0] <<" : output filename"<< std::endl;
	return -1;
	}
	std::ofstream f(argv[1]);
	std::random_device rnd;
	for( int i = 0; i < 7; i++){
		f << i << '\t';
		for( int i = 0; i < 5; i++){
			int p = (int)(uint8_t)(rnd()&0xff);
			f << p << '\t';
		}
		f << '\n';
	}
	f.close();
}

