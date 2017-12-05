#include "stdafx.h"
#include "DataFrame.h"
#include "CRC.h"
#include <cstdint>
#include <iterator>

//By Angus
DataFrame::DataFrame(char* data, bool isFile)
	: mData(data)
	, mCrc(DataFrame::computeCRC(data))
{
}

//By Angus
DataFrame::DataFrame(const DataFrame& dgram)
	: mData(dgram.getData())
	, mCrc(dgram.getCrc())
{
}

//By Angus
DataFrame::~DataFrame()
{
	delete mData;
}

//By Angus
std::uint32_t DataFrame::computeCRC(const char* data)
{
	return CRC::Calculate(mData, sizeof(mData), CRC::CRC_32());
}

//By Angus
char* DataFrame::serialize()
{
	char* payload = mData;
	unsigned count = 0;
	//increment it one past the character
	payload++;
	count++;
	while (payload != (payload + DataFrame::DATA_SIZE - 1))
	{
		*payload = '\0';
		payload++;
	}

	return 0;
}

//By Angus
char* DataFrame::getData() const
{
	return mData;
}

//By Angus
std::uint32_t DataFrame::getCrc() const
{
	return mCrc;
}