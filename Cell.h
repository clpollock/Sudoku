#pragma once

#include <cstdint>

// This class represents a cell in a sudoku grid. It uses a bitmap to represent
// the possible values for the cell, i.e. those that have not already been
// eliminated as impossible.

class Cell
{
public:
  // Newly constructed cells are considered to be completely unknown, with all
  // values being possible.
  Cell()
	: _possibilities(0b111111111), _possibilityCount(9),
	  _value(-1) {}
  // RemovePossibility returns false if value had already been eliminated as a possibility.
  bool RemovePossibility(char value)
  {
	uint16_t mask(1 << value);
	if ((_possibilities & mask) == 0)
	  return false;
	_possibilities ^= mask;
	if (--_possibilityCount == 1)
	  _value = FirstPossibility();
	return true;
  }
  int FirstPossibility() const
  {
	for (int i = 0; i < 9; ++i)
	{
	  if (_possibilities & (1 << i))
		return i;
	}
	return -1;
  }
  int NextPossibility(int i) const
  {
	while (++i < 9)
	{
	  if (_possibilities & (1 << i))
		return i;
	}
	return -1;
  }
  char PossibilityCount() const { return _possibilityCount; }
  int Value() const { return _value; }
  void SetValue(int value)
  {
	_possibilities = (1 << value);
	_possibilityCount = 1;
	_value = value;
  }
private:
  uint16_t _possibilities;
  char _possibilityCount;
  // We may as well cache the value for known cells. The class would be padded
  // to 4 bytes anyway, so this does not use any extra memory.
  char _value;
};

struct CellValue
{
  CellValue(char row, char column, char value)
	: _row(row), _column(column), _value(value) {}
  char _row;
  char _column;
  char _value;
};
