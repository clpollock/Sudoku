#pragma once

#include "Cell.h"
#include <memory>
#include <queue>

class Grid
{
public:
  using Cells = std::array<Cell[9], 9>;

  Grid(const std::array<char[9], 9>& cells);
  const Cell& GetCell(int row, int column) const
  {
	return _cells[row][column];
  }
  // SetCell returns false if setting the cell causes
  // the grid to enter an unsolvable state.
  bool SetCell(int row, int column, int value);
  int UnknownCellCount() const { return _unknownCellCount; }
private:
  bool UpdatePossibleValues(std::deque<CellValue>& newValues);
  // Each cell takes 4 bytes, so the Grid takes (81 * 4) + 4 = 328 bytes.
  Cells _cells;
  int _unknownCellCount;
};

extern std::ostream& operator<<(std::ostream&, const Grid&);
