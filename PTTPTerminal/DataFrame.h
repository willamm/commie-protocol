#pragma once

#include <cstdint>

// PTTP: Power to the Protocolariat 
// Definition of a PTTP packet
class DataFrame {
	
public:
	static constexpr int DATA_SIZE = 512;
	static constexpr int CRC_SIZE = 4;
	static constexpr int FRAME_SIZE = DATA_SIZE + CRC_SIZE + 2;

	DataFrame(char* data, bool isFile);
	DataFrame(const DataFrame& dframe);
	virtual ~DataFrame();

	std::uint32_t computeCRC(const char* data);
	char* serialize();
	char* getData() const;
	std::uint32_t getCrc() const;

private:
	const char mSyn = 0x16;
	const char mStx = 0x02;
	char* mData;
	std::uint32_t mCrc;
};

