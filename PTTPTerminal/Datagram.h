#pragma once

// PTTP: Power to the Protocolariat 
// Definition of a PTTP packet
class Datagram {
	static constexpr int DATA_SIZE = 512;
	static constexpr int CRC_SIZE = 4;
public:
	Datagram(char* data);
	//Datagram(const Datagram& dgram);
	//virtual ~Datagram();

	char* computeCRC(const char* data);

private:
	const char mSyn = 0x16;
	const char mStx = 0x02;
	char* mData;
	char* mCrc;
};

