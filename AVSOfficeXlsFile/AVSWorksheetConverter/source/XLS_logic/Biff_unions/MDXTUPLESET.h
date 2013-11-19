#pragma once

#include <XLS_logic/CompositeObject.h>

namespace XLS
{;


// Logical representation of MDXTUPLESET union of records 
class MDXTUPLESET: public CompositeObject
{
	BASE_OBJECT_DEFINE_CLASS_NAME(MDXTUPLESET)
public:
	MDXTUPLESET();
	~MDXTUPLESET();

	BaseObjectPtr clone();

	virtual const bool loadContent(BinProcessor& proc);

};

} // namespace XLS

