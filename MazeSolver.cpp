/*
Author: Marco Valero
Course: CSCI-235
Instructor: Tiziana Ligorio
Assignment: Project #4
This program solves a maze from the input files by backtracking.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <stack>
#include <cstdlib>
#include <cctype>
#include "MazeSolver.h"

MazeSolver::MazeSolver(std::string input_file) {
  std::ifstream file;
  file.open(input_file);
  if (file.fail()) {
    std::cerr << "Cannot read from file. Exit\n";
    exit(1);
  }
  int temp_rows = 0, temp_cols = 0;
  file >> temp_rows >> temp_cols;
  initializeMaze(temp_rows, temp_cols);
  if (mazeIsReady()) {
    fillMaze(file);
    initializeSolution();
    Position pos; // push starting pos ([0][0]) into stack
    pos.row = 0;
    pos.column = 0;
    backtrack_stack_.push(pos);
  }
  file.close();
}

MazeSolver::~MazeSolver() {
  for (int i = 0; i < maze_rows_; i++) {
    delete[] maze_[i];
  }
  delete[] maze_;

  for (int i = 0; i < maze_rows_; i++) {
    delete[] solution_[i];
  }
  delete[] solution_;

}

bool MazeSolver::mazeIsReady() {
  if (maze_rows_ > 0 && maze_columns_ > 0) {
    maze_ready = true;
    return true;
	}
	maze_ready = false;
  return false;
}

void MazeSolver::initializeMaze(int rows, int columns) {
  /* Note: A dynamic array is an array data structure that can be
    resized and which allows elements to be added or removed.
  */
  maze_rows_ = rows; //set rows and columns that were read from files to the maze rows and columns
  maze_columns_ = columns;
  maze_ = new char *[maze_rows_]; // dynamic array (size rows) of pointers to chars
  for (int i = 0; i < maze_rows_; i++) {
    maze_[i] = new char[maze_columns_]; // each i-th pointer is now pointing to dynamic array (size cols) of actual char values
  }
}

void MazeSolver::fillMaze(std::ifstream& input_stream) {
  std::string file_chars;
  getline(input_stream, file_chars);
  int space = 1; // skip the first space
  for (int i = 0; i < maze_rows_; i++) {   // for each row
    for (int j = 0; j < maze_columns_; j++) { // for each column
      maze_[i][j] = file_chars[space]; // space is the index of the string
      space = space + 2; // move by two in string because it contains space
    }
  }
}

bool MazeSolver::isExtensible(Position current_position, direction dir) {
  if (dir == SOUTH && (current_position.row+1 < maze_rows_)) { // making sure im within the matrix
    if (maze_[current_position.row+1][current_position.column] == '_'
     || maze_[current_position.row+1][current_position.column] == '$') {
       return true;
    }
  }
  if (dir == EAST && (current_position.column+1 < maze_columns_)) { // making sure im within the matrix
    if (maze_[current_position.row][current_position.column+1] == '_'
     || maze_[current_position.row][current_position.column+1] == '$') {
      return true;
    }
  }
  return false;
}

bool MazeSolver::extendPath(Position current_position) {
  bool extensible = false;
  if (isExtensible(current_position, SOUTH)) {
    backtrack_stack_.push(getNewPosition(current_position, SOUTH));
    extensible = true;
  }
  if (isExtensible(current_position, EAST)) {
    backtrack_stack_.push(getNewPosition(current_position, EAST));
    extensible = true;
  }
  return extensible;
}

Position MazeSolver::getNewPosition(Position old_position, direction dir) {
  Position new_pos;
  if (dir == SOUTH && (old_position.row+1 < maze_rows_)) { // within matrix
    new_pos.row = old_position.row + 1; // move south
    new_pos.column = old_position.column;
  }
  if (dir == EAST && (old_position.column+1 < maze_columns_)) { // within matrix
    new_pos.row = old_position.row;
    new_pos.column = old_position.column + 1; // move east
  }
  return new_pos;
}

bool MazeSolver::solveMaze() {
	Position current_pos;
	current_pos.row = backtrack_stack_.top().row;
	current_pos.column = backtrack_stack_.top().column;
	while (!backtrack_stack_.empty()) {
		if (maze_[current_pos.row][current_pos.column] == '$') {
      			std::cout << "Found the exit!!!\n";
			return true;
		}
		else if (extendPath(current_pos)) {
			solution_[current_pos.row][current_pos.column] = '>';
			current_pos = backtrack_stack_.top(); // the top should be east if east was extensible
		}
		else if (!extendPath(current_pos)) { // if it wasnt extinsible at all
			maze_[current_pos.row][current_pos.column] = 'X';
			solution_[current_pos.row][current_pos.column] = '@';
			backtrack_stack_.pop(); // backtracking
			if (!backtrack_stack_.empty()) {
				current_pos = backtrack_stack_.top(); // Moved forward by setting the current position to the one at the top of the stack.
			}
			else if (backtrack_stack_.empty()) { // if we can't move forward at all
				std::cout << "This maze has no solution.\n";
				return false;
			}
		}
	}
	return true;
}

void MazeSolver::printSolution() {
  std::cout << "The solution to this maze is:\n";
  for (int i = 0; i < maze_rows_; i++) {
    for (int j = 0; j < maze_columns_; j++) {
      std::cout << solution_[i][j] << " ";
    }
    std::cout << std::endl; // at the end of column, endl to form matrix
  }
}

void MazeSolver::copyMazetoSolution() {
  // initializing solution_ with a copy of maze_
  for (int i = 0; i < maze_rows_; i++) {
    for (int j = 0; j < maze_columns_; j++) {
      solution_[i][j] = maze_[i][j];
    }
  }
}

void MazeSolver::initializeSolution() {
  // 2D Array of solution
  solution_ = new char *[maze_rows_]; // dynamic array (size rows) of pointers to chars
  for (int i = 0; i < maze_rows_; i++) {
    solution_[i] = new char[maze_columns_]; // each i-th pointer is now pointing to dynamic array (size cols) of actual char values
  }
  copyMazetoSolution(); // 2D array for solution and copies maze to solution
}
