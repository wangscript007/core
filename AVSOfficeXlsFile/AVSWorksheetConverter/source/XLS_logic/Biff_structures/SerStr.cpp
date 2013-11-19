#include "stdafx.h"
#include "SerStr.h"
#include <XLS_bin/CFRecord.h>

namespace XLS
{;


SerStr::SerStr()
{
}


SerStr::SerStr(const std::wstring& word)
:	string_(_bstr_t(word.c_str()))
{
}


BiffStructurePtr SerStr::clone()
{
	return BiffStructurePtr(new SerStr(*this));
}


void SerStr::store(CFRecord& record)
{
	record.storeAnyData(fixed_type);
	record << string_;
}


void SerStr::load(CFRecord& record)
{
	record >> string_;
}


const std::wstring SerStr::toString() const
{
	return L"\"" + boost::algorithm::replace_all_copy(std::wstring(string_), L"\"", L"\"\"") + L"\"";
}


} // namespace XLS
