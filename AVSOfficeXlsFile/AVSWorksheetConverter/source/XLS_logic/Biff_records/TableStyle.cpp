#include "stdafx.h"
#include "TableStyle.h"

namespace XLS
{;

TableStyle::TableStyle()
{
}


TableStyle::~TableStyle()
{
}


BaseObjectPtr TableStyle::clone()
{
	return BaseObjectPtr(new TableStyle(*this));
}


void TableStyle::writeFields(CFRecord& record)
{
#pragma message("####################### TableStyle record is not implemented")
	Log::error("TableStyle record is not implemented.");
	//record << some_value;
}


void TableStyle::readFields(CFRecord& record)
{
	record.skipNBytes(12);
#pragma message("############################ frtHeader skipped here")
	WORD flags;
	record >> flags;
	fIsPivot = GETBIT(flags, 1);
	fIsTable = GETBIT(flags, 2);
	WORD cchName;
	record >> ctse >> cchName;
	rgchName.setSize(cchName);
	record >> rgchName;
}

} // namespace XLS

