#pragma once

#include <XLS_logic/CompositeObject.h>

namespace XLS
{;


// Logical representation of HLINK union of records 
class HLINK: public CompositeObject
{
	BASE_OBJECT_DEFINE_CLASS_NAME(HLINK)
public:
	HLINK();
	~HLINK();

	BaseObjectPtr clone();

	virtual const bool loadContent(BinProcessor& proc);

};

} // namespace XLS

