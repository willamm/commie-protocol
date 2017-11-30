#pragma once

struct ControlFrame
{
	enum class ControlChar : char
	{
		EOT = 0x04,
		ENQ,
		ACK
	};
	const char SYN = 0x16;
	ControlChar ctl;
};
