#pragma once

#include "Ptg.h"

namespace XLS
{;

class CFRecord;

class PtgAttrIf : public Ptg
{
	BASE_OBJECT_DEFINE_CLASS_NAME(PtgAttrIf)
public:
	BiffStructurePtr clone();

	virtual void setXMLAttributes(MSXML2::IXMLDOMElementPtr xml_tag);
	virtual void loadFields(CFRecord& record);
	virtual void storeFields(CFRecord& record);

	virtual void assemble(AssemblerStack& ptg_stack, PtgQueue& extra_data, MSXML2::IXMLDOMElementPtr parent);

private:
	WORD offset;
};

} // namespace XLS

