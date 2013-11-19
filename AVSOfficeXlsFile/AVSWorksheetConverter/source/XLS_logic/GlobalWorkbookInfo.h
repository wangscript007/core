#pragma once

#include <Crypt/Decryptor.h>
#include <XLS_logic/Biff_structures/BorderFillInfo.h>


namespace XLS
{;

class GlobalWorkbookInfo
{
public:
	GlobalWorkbookInfo(const WORD code_page);

	const size_t RegisterBorderId(const BorderInfo& border);
	const size_t RegisterFillId(const FillInfo& fill);
	const DWORD GenerateAXESId();

	WORD CodePage;
	CRYPT::DecryptorPtr decryptor;
	std::vector<std::wstring> sheets_names;
	std::vector<std::wstring> xti_parsed;
	std::vector<std::wstring> AddinUdfs;

	std::map<BorderInfo, size_t> border_x_ids;
	std::map<FillInfo, size_t> fill_x_ids;
private:
	DWORD last_AXES_id;
	const static DWORD initial_AXES_id = 0;
};

typedef boost::shared_ptr<GlobalWorkbookInfo> GlobalWorkbookInfoPtr;


} // namespace XLS