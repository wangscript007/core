#pragma once

#include <XLS_logic/CompositeObject.h>

namespace XLS
{;


// Logical representation of SXADDLSXFILT union of records 
class SXADDLSXFILT: public CompositeObject
{
	BASE_OBJECT_DEFINE_CLASS_NAME(SXADDLSXFILT)
public:
	SXADDLSXFILT();
	~SXADDLSXFILT();

	BaseObjectPtr clone();

	virtual const bool loadContent(BinProcessor& proc);

};

} // namespace XLS

