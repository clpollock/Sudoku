#include "Grid.h"
#include <iostream>

// The cells grid contains the numbers 0 to 8 for the known cells.
// and -1 to represent the unknown cells.

Grid::Grid(const std::array<char[9], 9>& cells)
  : _unknownCellCount(0)
{
  std::deque<CellValue> knownCells;
  
  for (int row = 0; row < 9; ++row)
  {
	for (int column = 0; column < 9; ++column)
	{
	  char n = cells[row][column];
	  if (n == -1)
	  {
		++_unknownCellCount;
	  }
	  else
	  {
		_cells[row][column].SetValue(n);
		knownCells.emplace_back(CellValue(row, column, n));
	  }
	}
  }

  // Set the possible values for the unknown cells.
  if (!UpdatePossibleValues(knownCells))
	throw std::runtime_error("The puzzle is invalid.");
}

bool Grid::SetCell(int row, int column, int value)
{
  Cell& cell = _cells[row][column];
#ifdef _DEBUG
  if (cell.Value() != -1)
	throw std::runtime_error("Cell value has already been set.");
#endif
	--_unknownCellCount;
  cell.SetValue(value);
  // Update the possible values for other cells.
  std::deque<CellValue> newValues;
  newValues.emplace_back(row, column, value);
  return UpdatePossibleValues(newValues);
}

// Once we set the value of a cell, other cells in the same row, column, and
// subgrid cannot have the same value. UpdatePossibleValues updates the possible
// values of other cells accordingly. If any of these cells are left with only a
// single possible value, it repeats the process. If any cells are left with no
// possible values, the grid has reached an invalid state, so it returns false
// to indicate this.

// The newValues deque holds the cells that have been assigned values since the
// last call to UpdatePossibleValues.

bool Grid::UpdatePossibleValues(std::deque<CellValue>& newValues)
{
  while (!newValues.empty())
  {
	const CellValue& cv = newValues.front();
	int row = cv._row;
	int column = cv._column;
	int value = cv._value;
	newValues.pop_front();

	// None of the cells in the same row can have the same value as this cell.
	for (int r = 0; r < 9; ++r)
	{
	  if (r != row)
	  {
		Cell& cell = _cells[r][column];
		// RemovePossibility returns true if this value had not already been
		// eliminated as a possibility.
		if (cell.RemovePossibility(value))
		{
		  if (cell.PossibilityCount() == 1)
		  {
			// We now know the value of this cell.
			--_unknownCellCount;
			// Assigning a value to this cell may force the value of other
			// cells, so add this cell to the queue.
			newValues.emplace_back(r, column, cell.Value());
		  }
		  else if (cell.PossibilityCount() == 0)
			return false;
		}
	  }
	}

	// None of the cells in the same column can have the same value as this cell.
	for (int c = 0; c < 9; ++c)
	{
	  if (c != column)
	  {
		Cell& cell = _cells[row][c];
		if (cell.RemovePossibility(value))
		{
		  if (cell.PossibilityCount() == 1)
		  {
			--_unknownCellCount;
			newValues.emplace_back(row, c, cell.Value());
		  }
		  else if (cell.PossibilityCount() == 0)
			return false;
		}
	  }
	}

	// None of the cells in the same subgrid can have the same value as this cell.
	int subGridStartRow = (row / 3) * 3;
	int subGridStartCol = (column / 3) * 3;
	int subGridEndRow = subGridStartRow + 3;
	int subGridEndCol = subGridStartCol + 3;
	for (int r = subGridStartRow; r < subGridEndRow; ++r)
	{
	  if (r != row)
	  {
		for (int c = subGridStartCol; c < subGridEndCol; ++c)
		{
		  if (c != column)
		  {
			Cell& cell = _cells[r][c];
			if (cell.RemovePossibility(value))
			{
			  if (cell.PossibilityCount() == 1)
			  {
				--_unknownCellCount;
				newValues.emplace_back(r, c, cell.Value());
			  }
			  else if (cell.PossibilityCount() == 0)
				return false;
			}
		  }
		}
	  }
	}
  }

  return true;
}

std::ostream& operator<<(std::ostream& os, const Grid& grid)
{
  for (int row = 0; row < 9; ++row)
  {
	for (int column = 0; column < 9; ++column)
	{
	  // Note that we store 0 to 8 rather than 1 to 9, so we have to add 1 here.
	  int value = grid.GetCell(row, column).Value();
	  if (value == -1)
		os << ' '; // Unknown cell.
	  else
		os << (value + 1); // Known cell.
	}
	std::cout << std::endl;
  }
  return os;
}
