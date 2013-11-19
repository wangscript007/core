#pragma once
#ifndef OOX_RID_INCLUDE_H_
#define OOX_RID_INCLUDE_H_

#include <string>


namespace OOX
{
	class RId
	{
	public:
		RId();
		RId(const size_t id);
		RId(const std::string& rid);

	public:
		const RId& operator= (const size_t id);
		const RId& operator= (const std::string& rid);

	public:
		const bool operator ==(const RId& lhs) const;
		const bool operator !=(const RId& lhs) const;
		const bool operator < (const RId& lhs) const;
		const bool operator <=(const RId& lhs) const;
		const bool operator > (const RId& lhs) const;
		const bool operator >=(const RId& lhs) const;

	public:
		const RId	next() const;
		
	public:
		const std::string ToString() const;

	private:
        std::string m_id;
	};
} // namespace OOX

#endif // OOX_RID_INCLUDE_H_