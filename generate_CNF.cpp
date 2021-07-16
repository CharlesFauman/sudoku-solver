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

typedef std::list< int > clause;
typedef std::list< clause > clauses_holder;



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

bool read_file(std::ifstream &input_file, int &board_size, board &starting_board){ // pass in "global" holders
	input_file >> board_size;
	
	starting_board = board(board_size, row(board_size, 0));
	for(int y=0; y<board_size; ++y) {
		for(int x=0; x<board_size; ++x) {
			input_file >> starting_board[y][x];
		}
	}
	return true;
}

void FindClauses(const int &board_size, const board &starting_board, clauses_holder &clauses){
	// first set up the node interconstraints
	for(int y=0; y<board_size; ++y) {
		for(int x=0; x<board_size; ++x) {
			// restrict the node to exaclty one value
			int current_node_start = ((y*board_size)+x)*board_size+1;
			
			// at least one value
			clause at_least_one;
			for(int i=0; i<board_size;++i) at_least_one.push_back(current_node_start+i);
			at_least_one.push_back(0);
			clauses.push_back(at_least_one);
			
			// at most one value
			for(int i=0; i<board_size; ++i) {
				for(int n=i+1; n<board_size; ++n) {
					clause not_i_or_not_n;
					not_i_or_not_n.push_back(-(current_node_start+i));
					not_i_or_not_n.push_back(-(current_node_start+n));
					not_i_or_not_n.push_back(0);
					clauses.push_back(not_i_or_not_n);
				}
			}
		}
	}
	
	
	// next, set up the node intraconstraints
	
	// each row must have each element in {1, ... , board_size}
	for(int y=0; y<board_size; ++y) {
		// make a clause for each number
		for(int n=0; n<board_size; ++n) {
			clause row_must_have_n;
			for(int x=0; x<board_size; ++x) {
				int current_node_start = ((y*board_size)+x)*board_size+1;
				row_must_have_n.push_back(current_node_start+n);
			}
			row_must_have_n.push_back(0);
			clauses.push_back(row_must_have_n);
		}
	}
	
	// each column must have each element in {1, ... , board_size}
	for(int x=0; x<board_size; ++x) {
		// make a clause for each number
		for(int n=0; n<board_size; ++n) {
			clause column_must_have_n;
			for(int y=0; y<board_size; ++y) {
				int current_node_start = ((y*board_size)+x)*board_size+1;
				column_must_have_n.push_back(current_node_start+n);
			}
			column_must_have_n.push_back(0);
			clauses.push_back(column_must_have_n);
		}
	}
	
	// each grid of sqrt(board_size) must have each element in {1, ... , board_size}
	int grid_size = (int)std::sqrt(board_size);
	for(int y=0; y<board_size; y += grid_size) {
		for(int x=0; x<board_size; x += grid_size) {
			// make a clause for each number
			for(int n=0; n<board_size; ++n) {
				clause grid_must_have_n;
				for(int y2=0; y2<grid_size; ++y2) {
					for(int x2=0; x2<grid_size; ++x2) {
						int current_node_start = (((y+y2)*board_size)+(x+x2))*board_size+1;
						grid_must_have_n.push_back(current_node_start+n);
					}
				}
				grid_must_have_n.push_back(0);
				clauses.push_back(grid_must_have_n);
			}
		}
	}
	
	// finaly, plug in the starting constraints
	for(int y=0; y<board_size; ++y) {
		for(int x=0; x<board_size; ++x) {
			if(starting_board[y][x] != 0){
				int current_node_start = ((y*board_size)+x)*board_size+1;
				int current_node_value = starting_board[y][x];
				clause node_value;
				node_value.push_back(current_node_start + current_node_value - 1);
				node_value.push_back(0);
				clauses.push_back(node_value);
			}
		}
	}
}

int main(int argc, char* argv[]){
	// declare "global" holders here
	int board_size; // width, height
	board starting_board;
	
	// prep input
	if(argc != 3){
		std::cerr << "Invalid command line prompt; correct use: " << argv[0] << " input_file output_file" << std::endl;
		return 1;
	}
	
	std::ifstream input_file(argv[1]);
	if(!input_file){
		std::cerr << "Cannot open input file " << argv[1] << std::endl;
		return 1;
	}
	
	if(!read_file(input_file, board_size, starting_board)){ // pass in "global" holders
		std::cerr << "problem encountered while reading the file" << std::endl;
		return 1;
	}
	// find clauses
	clauses_holder clauses;
	
	FindClauses(board_size, starting_board, clauses);
	int num_variables = board_size*board_size*board_size;
	int num_clauses = clauses.size();
	
	std::ofstream output_file;
	output_file.open("cnf.txt");
	
	
	output_file << "p cnf " << num_variables << " " << num_clauses << std::endl;
	
	for(clauses_holder::iterator itr = clauses.begin(); itr != clauses.end(); ++itr){
		for(clause::iterator itr2 = itr->begin(); itr2 != itr->end(); ++itr2){
			output_file << *itr2 << " ";
		}
		output_file << std::endl;
	}
	
	output_file.close();
	
	system("./minisat.exe cnf.txt solution.txt");
	system(("./output_solution.exe solution.txt " + std::string(argv[2]) + " " + std::string(argv[1])).c_str());
	
	return 0;
}

//g++ main.cpp -Wall

