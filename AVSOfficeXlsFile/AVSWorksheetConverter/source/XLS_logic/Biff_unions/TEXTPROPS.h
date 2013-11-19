#pragma once

#include <XLS_logic/CompositeObject.h>

namespace XLS
{;


// Logical representation of TEXTPROPS union of records 
class TEXTPROPS: public CompositeObject
{
	BASE_OBJECT_DEFINE_CLASS_NAME(TEXTPROPS)
public:
	TEXTPROPS();
	~TEXTPROPS();

	BaseObjectPtr clone();

	virtual const bool loadContent(BinProcessor& proc);

};

} // namespace XLS

