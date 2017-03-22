// output/input
#include <iostream>
#include <fstream>
#include <ostream>

// types
#include <list>
#include <vector>
#include <sstream>
#include <map>
#include <set>

//extra
#include <algorithm>
#include <cmath>

// custom includes

// typedefs
typedef std::vector< int > row;
typedef std::vector< row > board;

void const PrintBoard(std::ostream &ostr, board &input_board){
	ostr << "Board: " << std::endl;
	for(unsigned int y = 0; y < input_board.size(); ++y){
		ostr << "  ";
		for(unsigned int x = 0; x < input_board[0].size(); ++x){
			ostr << input_board[y][x];
		}
		if(y != input_board.size()-1) ostr << std::endl;
	}
}

bool read_file(std::ifstream &input_file, int &board_size, board &solution_board){ // pass in "global" holders
	std::string solved;
	input_file >> solved;
	if(solved != "SAT") return false;
	solution_board = board(board_size, row(board_size, 0));
	int current_number;
	for(int y=0; y<board_size; ++y) {
		for(int x=0; x<board_size; ++x) {
			for(int n=1; n<=9; ++n){
				input_file >> current_number;
				if(current_number>0) solution_board[y][x] = n;
			}
		}
	}
	return true;
}

int main(int argc, char* argv[]){
	// declare "global" holders here
	int board_size; // width, height
	board solution_board;
	
	//for(int i= 0; i <= argc; ++i) std::cout << argv[i] << " ";
	
	// prep input
	if(argc != 4){
		std::cerr << "Invalid command line prompt; correct use: " << argv[0] << " input_file output_file input_orig" << std::endl;
		return 1;
	}
	
	std::ifstream input_orig(argv[3]);
	input_orig >> board_size;
	
	std::ifstream input_file(argv[1]);
	if(!input_file){
		std::cerr << "Cannot open input file " << argv[1] << std::endl;
		return 1;
	}
	
	if(!read_file(input_file, board_size, solution_board)){ // pass in "global" holders
		std::cerr << "problem encountered while reading the file" << std::endl;
		return 1;
	}
	
	std::ofstream output_file;
	output_file.open(argv[2]);
	
	for(board::iterator itr = solution_board.begin(); itr != solution_board.end(); ++itr){
		for(row::iterator itr2 = itr->begin(); itr2 != itr->end(); ++itr2){
			output_file << *itr2 << " ";
		}
		output_file << std::endl;
	}
	
	output_file.close();
	return 0;
}

//g++ main.cpp -Wall

