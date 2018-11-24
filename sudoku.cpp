#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include "Grid.h"

// Load a puzzle file into a 9 by 9 matrix.

void Load(const char* fileName, std::array<char[9], 9>& cells)
{
  std::ifstream file(fileName);
  if (!file)
  {
	std::stringstream ss;
	ss << "Could not open puzzle file " << fileName;
	throw std::runtime_error(ss.str());
  }

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
	// If the line contains fewer than 9 characters, treat the remaining cells as empty.
	while (col < 9)
	  cells[row][col++] = -1;
  }
}

void Verify(const char* fileName)
{
  std::array<char[9], 9> cells;
  std::array<bool, 9> used;
  Load(fileName, cells);

  for (int row = 0; row < 9; ++row)
  {
	used.fill(false);
	for (int col = 0; col < 9; ++col)
	{
	  int n = cells[row][col];
	  if (n == -1)
	  {
		std::cout << "No value has been set for cell " << (row + 1) << ", " << (col + 1) << std::endl;
		return;
	  }

	  if (used[n])
	  {
		std::cout << "Number " << (n + 1) << " has been used multiple times in row " << (row + 1) << std::endl;
		return;
	  }
	  used[n] = true;
	}
  }

  for (int col = 0; col < 9; ++col)
  {
	used.fill(false);
	for (int row = 0; row < 9; ++row)
	{
	  int n = cells[row][col];
	  if (used[n])
	  {
		std::cout << "Number " << (n + 1) << " has been used multiple times in column " << (col + 1) << std::endl;
		return;
	  }
	  used[n] = true;
	}
  }

  for (int subgridStartRow = 0; subgridStartRow <= 6; ++subgridStartRow)
  {
	for (int subgridStartCol = 0; subgridStartCol <= 6; ++subgridStartCol)
	{
	  used.fill(false);
	  for (int row = subgridStartRow; row < subgridStartRow + 3; ++row)
	  {
		for (int col = subgridStartCol; col < subgridStartCol + 3; ++col)
		{
		  int n = cells[row][col];
		  if (used[n])
		  {
			std::cout << "Number " << (n + 1) << " has been used multiple times in subgrid "
			  << (subgridStartRow / 3 + 1) << ", " << (subgridStartCol / 3 + 1)  << std::endl;
			return;
		  }
		}
	  }
	}
  }

  std::cout << fileName << " is a valid solution." << std::endl;
}

// Find a solution for a sudoku grid. If the grid has multiple solutions, it
// outputs the first one that it finds. It returns false if the grid has no
// solution.

bool Solve(const Grid& grid)
{
  if (grid.UnknownCellCount() == 0)
  {
	// We have a solution.
	std::cout << grid;
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
  // NextPossibility returns -1 when there are no more possibilities.
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

// Read a puzzle grid from a file and try to solve it.

void Solve(const char* fileName)
{
  std::array<char[9], 9> cells;
  Load(fileName, cells);
  Grid grid(cells);
  if (!Solve(grid))
	std::cerr << "Puzzle " << fileName << " has no solution.";
}

int main(int argc, char *argv[])
{
  try
  {
	if (argc == 2)
	{
	  Solve(argv[1]);
	}
	else if (argc == 3 && strcmp(argv[1], "-v") == 0)
	{
	  Verify(argv[2]);
	}
	else
	{
	  std::cerr << "Usage: " << argv[0] << "[-v] puzzle_file" << std::endl
		<< "-v    verify that puzzle_file is a correct solution" << std::endl;
	  return 1;
	}
  }
  catch (const std::exception& e)
  {
	std::cerr << e.what() << std::endl;
	return 1;
  }
  return 0;
}
