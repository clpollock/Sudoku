#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include "Grid.h"

std::unique_ptr<Grid> Load(const char* fileName)
{
  std::ifstream file(fileName);
  if (!file)
  {
	std::stringstream ss;
	ss << "Could not open puzzle file " << fileName;
	throw std::runtime_error(ss.str());
  }

  std::array<char[9], 9> cells;
  for (int row = 0; row < 9; ++row)
  {
	std::string line;
	if (!std::getline(file, line))
	{
	  std::stringstream ss;
	  ss << "Puzzle file " << fileName << " is incomplete.";
	  throw std::runtime_error(ss.str());
	}
	int lineLength = std::min<int>(line.length(), 9);
	int col = 0;
	while (col < lineLength)
	{
	  char c = line[col];
	  // The puzzle should only contain spaces and digits.
	  if (c == ' ')
	  {
		// Use -1 to represent empty cells.
		cells[row][col] = -1;
	  }
	  else if (c >= '1' && c <= '9')
	  {
		// Store 0 to 8 rather than 1 to 9, because this works better with 0-based indexing.
		cells[row][col] = c - '1';
	  }
	  else
	  {
		std::stringstream ss;
		ss << "Row " << (row + 1) << ", column " << (col + 1) << " of the puzzle is invalid.";
		throw std::runtime_error(ss.str());
	  }
	  ++col;
	}
	// If the line is contains fewer than 9 characters, treat the remaining cells as empty.
	while (col < 9)
	  cells[row][col++] = -1;
  }

  return std::make_unique<Grid>(cells);
}

// Find a solution for a sudoku grid. If the grid has multiple solutions, it
// outputs the first one that it finds. It returns false if the grid has no
// solution.

bool Solve(const Grid& grid)
{
  if (grid.UnknownCellCount() == 0)
  {
	// We have a solution.
	std::cout << "Solution:" << std::endl << grid;
	return true;
  }

  // Find the cell with the fewest possible values given the values that have
  // already been assigned to other cells in the same row, column, and subgrid.
  int fewestPossibilities = 9;
  int guessRow = 0;
  int guessCol = 0;
  for (int row = 0; row < 9; ++row)
  {
	for (int column = 0; column < 9; ++column)
	{
	  const Cell& cell = grid.GetCell(row, column);
	  if (cell.PossibilityCount() > 1 && cell.PossibilityCount() < fewestPossibilities)
	  {
		fewestPossibilities = cell.PossibilityCount();
		guessRow = row;
		guessCol = column;
		// By definition we can't have an unknown cell with fewer than 2 possible values.
		if (fewestPossibilities == 2)
		  break;
	  }
	}
  }
  
  // Iterate over each possible value for the chosen cell.
  const Cell& guessCell = grid.GetCell(guessRow, guessCol);
  for (int n = guessCell.FirstPossibility(); n != -1; n = guessCell.NextPossibility(n))
  {
	// Create a copy of the grid. The Grid occupies only 328 bytes so this is
	// not very expensive.
	Grid child(grid);
	// If SetCell returns false it means that setting the cell to the specified
	// value caused the grid to reach an invalid state, so it's impossible to
	// solve the puzzle with this guess.
	if (child.SetCell(guessRow, guessCol, n))
	{
	  // Call Solve() recursively on the new grid. This does a depth-first search
	  // through the space of possible cell assignments. Since there are only 81
	  // cells, and each level of recursion reduces the number of unknown cells
	  // by at least 1 (and often much more), the recursion will never be very
	  // deep.
	  if (Solve(child))
		return true;
	}
  }

  // If none of the guesses results in a solution, it means that the grid was
  // unsolvable. This will happen if Solve() is called recursively with an
  // incorrect guess.
  return false;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
	std::cerr << "Usage: " << argv[0] << " <puzzle_file>" << std::endl;
	return 1;
  }
  
  try
  {
	// Read the puzzle grid from the file and try to solve it.
	std::unique_ptr grid = Load(argv[1]);
	if (!Solve(*grid))
	  throw std::runtime_error("The puzzle has no solution.");
  }
  catch (const std::exception& e)
  {
	std::cerr << e.what() << std::endl;
  }
}
