#pragma once
#include <cstdint>
#include <string>
using namespace std;

class ListTable
{
private:
	uint16_t _status;
	uint8_t _type;
	uint16_t _listIndex;
	uint32_t _firstBlockOfData;
	uint32_t _size;

public:
	// Constructor
	ListTable()
	{
		_status = 0;
		_type = 0;
		_listIndex = 0;
		_firstBlockOfData = 0;
		_size = 0;
	}

	// Getter - Setter
	uint16_t status() { return _status; }
	void setStatus(uint16_t value) { _status = value; }

	uint8_t type() { return _type; }

	uint16_t listIndex() { return _listIndex; }

	uint32_t firstBlockOfData() { return _firstBlockOfData; }

	uint32_t size() { return _size; }
	void setSize(uint32_t value) { _size = value; }
	

	void setUseMode(uint8_t type, uint16_t listIndex, uint32_t firstBlockOfData, uint32_t size)
	{
		_status = 1;
		_type = type;
		_listIndex = listIndex;
		_firstBlockOfData = firstBlockOfData;
		_size = size;
	}

	void reset()
	{
		_status = 0;
		_type = 0;
		_listIndex = 0;
		_firstBlockOfData = 0;
		_size = 0;
	}
};