#include "stdafx.h"

#include "Object.h"
#include "Stream.h"
#include "JArithmeticDecoder.h"

//-------------------------------------------------------------------------------------------------------------------------------
// JArithmeticDecoderStates
//-------------------------------------------------------------------------------------------------------------------------------

JArithmeticDecoderStats::JArithmeticDecoderStats(int nContextSize) 
{
	m_nContextSize = nContextSize;
	m_pContextTable = (unsigned char *)MemUtilsMallocArray( m_nContextSize, sizeof(unsigned char) );
	Reset();
}

JArithmeticDecoderStats::~JArithmeticDecoderStats() 
{
	MemUtilsFree( m_pContextTable );
}

JArithmeticDecoderStats *JArithmeticDecoderStats::Copy() 
{
	JArithmeticDecoderStats *pStats = new JArithmeticDecoderStats( m_nContextSize );
	memcpy( pStats->m_pContextTable, m_pContextTable, m_nContextSize );
	return pStats;
}

void JArithmeticDecoderStats::Reset() 
{
	memset( m_pContextTable, 0, m_nContextSize );
}

void JArithmeticDecoderStats::CopyFrom(JArithmeticDecoderStats *pStats) 
{
	memcpy( m_pContextTable, pStats->m_pContextTable, m_nContextSize );
}

void JArithmeticDecoderStats::SetEntry(unsigned int unCx, int nIndex, int nMPS) 
{
	m_pContextTable[unCx] = (nIndex << 1) + nMPS;
}

//-------------------------------------------------------------------------------------------------------------------------------
// JArithmeticDecoder
//-------------------------------------------------------------------------------------------------------------------------------

unsigned int JArithmeticDecoder::arrunQeTable[47] = 
{
	0x56010000, 0x34010000, 0x18010000, 0x0AC10000,
	0x05210000, 0x02210000, 0x56010000, 0x54010000,
	0x48010000, 0x38010000, 0x30010000, 0x24010000,
	0x1C010000, 0x16010000, 0x56010000, 0x54010000,
	0x51010000, 0x48010000, 0x38010000, 0x34010000,
	0x30010000, 0x28010000, 0x24010000, 0x22010000,
	0x1C010000, 0x18010000, 0x16010000, 0x14010000,
	0x12010000, 0x11010000, 0x0AC10000, 0x09C10000,
	0x08A10000, 0x05210000, 0x04410000, 0x02A10000,
	0x02210000, 0x01410000, 0x01110000, 0x00850000,
	0x00490000, 0x00250000, 0x00150000, 0x00090000,
	0x00050000, 0x00010000, 0x56010000
};

int JArithmeticDecoder::arrnNMPSTable[47] = 
{
 	 1,  2,  3,  4,  5, 38,  7,  8,  9, 10, 11, 12, 13, 29, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
	33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 45, 46
};

int JArithmeticDecoder::arrnNLPSTable[47] = 
{
	 1,  6,  9, 12, 29, 33,  6, 14, 14, 14, 17, 18, 20, 21, 14, 14,
	15, 16, 17, 18, 19, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
	30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 46
};

int JArithmeticDecoder::arrnSwitchTable[47] = 
{
	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

JArithmeticDecoder::JArithmeticDecoder()
{
	m_pStream      = NULL;
	m_nDataSize    = 0;
	m_bLimitStream = FALSE;
}

inline unsigned int JArithmeticDecoder::ReadByte() 
{
	if ( m_bLimitStream ) 
	{
		--m_nDataSize;
		if ( m_nDataSize < 0 ) 
		{
			return 0xff;
		}
	}
	return (unsigned int)m_pStream->GetChar() & 0xff;
}

JArithmeticDecoder::~JArithmeticDecoder() 
{
	Cleanup();
}

void JArithmeticDecoder::Start() 
{
	m_unBuffer0 = ReadByte();
	m_unBuffer1 = ReadByte();

	// INITDEC
	m_unC = ( m_unBuffer0 ^ 0xff) << 16;
	ByteIn();
	m_unC <<= 7;
	m_nCT -= 7;
	m_unA = 0x80000000;
}

void JArithmeticDecoder::Restart(int nDatasize) 
{
	int nOldDataSize = m_nDataSize;
	m_nDataSize = nDatasize;
	if ( nOldDataSize == -1 ) 
	{
		m_unBuffer1 = ReadByte();
	} 
	else if ( nOldDataSize <= -2 ) 
	{
		m_unBuffer0 = ReadByte();
		m_unBuffer1 = ReadByte();
	}
}

void JArithmeticDecoder::Cleanup()
{
	if ( m_bLimitStream ) 
	{
		while ( m_nDataSize > 0 ) 
		{
			m_unBuffer0 = m_unBuffer1;
			m_unBuffer1 = ReadByte();
		}
	}
}

int JArithmeticDecoder::DecodeBit(unsigned int unContext, JArithmeticDecoderStats *pStats) 
{
	int nBit = 0;

	int nICX   = pStats->m_pContextTable[unContext] >> 1;
	int nMPSCX = pStats->m_pContextTable[unContext] & 1;
	unsigned int unQe = arrunQeTable[nICX];
	m_unA -= unQe;
	if ( m_unC < m_unA ) 
	{
		if ( m_unA & 0x80000000 ) 
		{
			nBit = nMPSCX;
		} 
		else 
		{
			// MPS_EXCHANGE
			if ( m_unA < unQe ) 
			{
				nBit = 1 - nMPSCX;
				if ( arrnSwitchTable[nICX] ) 
				{
					pStats->m_pContextTable[unContext] = (arrnNLPSTable[nICX] << 1) | (1 - nMPSCX);
				} 
				else 
				{
					pStats->m_pContextTable[unContext] = (arrnNLPSTable[nICX] << 1) | nMPSCX;
				}
			} 
			else 
			{
				nBit = nMPSCX;
				pStats->m_pContextTable[unContext] = (arrnNMPSTable[nICX] << 1) | nMPSCX;
			}
			// RENORMD
			do 
			{
				if ( m_nCT == 0 ) 
				{
					ByteIn();
				}
				m_unA <<= 1;
				m_unC <<= 1;
				--m_nCT;
			} while ( !( m_unA & 0x80000000 ) );
		}
	} 
	else 
	{
		m_unC -= m_unA;
		// LPS_EXCHANGE
		if ( m_unA < unQe ) 
		{
			nBit = nMPSCX;
			pStats->m_pContextTable[unContext] = (arrnNMPSTable[nICX] << 1) | nMPSCX;
		} 
		else 
		{
			nBit = 1 - nMPSCX;
			if ( arrnSwitchTable[nICX] ) 
			{
				pStats->m_pContextTable[unContext] = (arrnNLPSTable[nICX] << 1) | (1 - nMPSCX);
			} 
			else 
			{
				pStats->m_pContextTable[unContext] = (arrnNLPSTable[nICX] << 1) | nMPSCX;
			}
		}
		m_unA = unQe;
		// RENORMD
		do 
		{
			if ( m_nCT == 0 ) 
			{
				ByteIn();
			}
			m_unA <<= 1;
			m_unC <<= 1;
			--m_nCT;
		} while ( !( m_unA & 0x80000000 ) );
	}
	return nBit;
}

int JArithmeticDecoder::DecodeByte(unsigned int unContext, JArithmeticDecoderStats *pStats) 
{
	int nByte = 0;
	for ( int nIndex = 0; nIndex < 8; ++nIndex ) 
	{
		nByte = ( nByte << 1 ) | DecodeBit( unContext, pStats );
	}
	return nByte;
}

BOOL JArithmeticDecoder::DecodeInt(int *pnValue, JArithmeticDecoderStats *pStats) 
{
	unsigned int unVal = 0;
	m_unPrev = 1;
	int nSign = DecodeIntBit( pStats );
	if ( DecodeIntBit(pStats) ) 
	{
		if ( DecodeIntBit(pStats) ) 
		{
			if ( DecodeIntBit(pStats) ) 
			{
				if ( DecodeIntBit(pStats) ) 
				{
					if ( DecodeIntBit(pStats) ) 
					{
						unVal = 0;
						for ( int nIndex = 0; nIndex < 32; ++nIndex ) 
						{
							unVal = (unVal << 1) | DecodeIntBit(pStats);
						}
						unVal += 4436;
					} 
					else 
					{
						unVal = 0;
						for ( int nIndex = 0; nIndex < 12; ++nIndex ) 
						{
							unVal = (unVal << 1) | DecodeIntBit(pStats);
						}
						unVal += 340;
					}
				} 
				else 
				{
					unVal = 0;
					for ( int nIndex = 0; nIndex < 8; ++nIndex ) 
					{
						unVal = (unVal << 1) | DecodeIntBit(pStats);
					}
					unVal += 84;
				}
			} 
			else 
			{
				unVal = 0;
				for ( int nIndex = 0; nIndex < 6; ++nIndex ) 
				{
					unVal = (unVal << 1) | DecodeIntBit(pStats);
				}
				unVal += 20;
			}
		} 
		else 
		{
			unVal = DecodeIntBit(pStats);
			unVal = (unVal << 1) | DecodeIntBit(pStats);
			unVal = (unVal << 1) | DecodeIntBit(pStats);
			unVal = (unVal << 1) | DecodeIntBit(pStats);
			unVal += 4;
		}
	} 
	else 
	{
		unVal = DecodeIntBit(pStats);
		unVal = (unVal << 1) | DecodeIntBit(pStats);
	}

	if ( nSign ) 
	{
		if ( unVal == 0 ) 
		{
			return FALSE;
		}
		*pnValue = -(int)unVal;
	} 
	else 
	{
		*pnValue = (int)unVal;
	}
	return TRUE;
}

int JArithmeticDecoder::DecodeIntBit(JArithmeticDecoderStats *pStats) 
{
	int nBit = DecodeBit( m_unPrev, pStats );
	if ( m_unPrev < 0x100 ) 
	{
		m_unPrev = ( m_unPrev << 1 ) | nBit;
	} 
	else 
	{
		m_unPrev = ( ( ( m_unPrev << 1 ) | nBit ) & 0x1ff ) | 0x100;
	}
	return nBit;
}

unsigned int JArithmeticDecoder::DecodeIAID(unsigned int unCodeLen, JArithmeticDecoderStats *pStats) 
{
	m_unPrev = 1;
	for ( unsigned int unIndex = 0; unIndex < unCodeLen; ++unIndex ) 
	{
		int nBit = DecodeBit(m_unPrev, pStats);
		m_unPrev = (m_unPrev << 1) | nBit;
	}
	return m_unPrev - (1 << unCodeLen);
}

void JArithmeticDecoder::ByteIn() 
{
	if ( m_unBuffer0 == 0xff ) 
	{
		if ( m_unBuffer1 > 0x8f ) 
		{
			m_nCT = 8;
		} 
		else 
		{
			m_unBuffer0 = m_unBuffer1;
			m_unBuffer1 = ReadByte();
			m_unC = m_unC + 0xfe00 - ( m_unBuffer0 << 9 );
			m_nCT = 7;
		}
	} 
	else 
	{
		m_unBuffer0 = m_unBuffer1;
		m_unBuffer1 = ReadByte();
		m_unC = m_unC + 0xff00 - ( m_unBuffer0 << 8 );
		m_nCT = 8;
	}
}
