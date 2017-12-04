#pragma once

// PTTP: Power to the Protocolariat 
// Definition of a PTTP packet
class DataFrame {
	
public:
	static constexpr int DATA_SIZE = 512;
	static constexpr int CRC_SIZE = 4;
	static constexpr int FRAME_SIZE = DATA_SIZE + CRC_SIZE + 2;

	DataFrame(char* data);
	DataFrame(const DataFrame& dframe);
	virtual ~DataFrame();

	char* computeCRC(const char* data);

private:
	const char mSyn = 0x16;
	const char mStx = 0x02;
	char mData[DATA_SIZE];
	char mCrc[CRC_SIZE];
};

