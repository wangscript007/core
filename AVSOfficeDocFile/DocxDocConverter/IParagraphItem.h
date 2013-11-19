#pragma once

#include "Chpx.h"

namespace AVSDocFileFormat
{
	struct IParagraphItem;

	typedef boost::shared_ptr<IParagraphItem> IParagraphItemPtr;

	struct IParagraphItem: public IVirtualConstructor
	{
		virtual unsigned long GetTextSize() const = 0;
		virtual std::wstring GetAllText() const = 0;
		virtual vector<Chpx> GetRunProperties( vector<unsigned int>* runOffsets ) const = 0;
		virtual unsigned int PrlSize () const = 0;
		virtual vector<IParagraphItemPtr> GetAllRunsCopy( vector<unsigned int>* runOffsets ) const = 0;
		
		virtual ~IParagraphItem() {}
	};
}