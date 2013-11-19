#pragma once
#ifndef OOX_THEME_MAJOR_FONT_INCLUDE_H_
#define OOX_THEME_MAJOR_FONT_INCLUDE_H_

#include "./../WritingElement.h"
#include "Font.h"
#include <string>
#include <vector>


namespace OOX
{
	namespace Theme
	{
		class MajorFont : public WritingElement
		{
		public:
			MajorFont();
			virtual ~MajorFont();
			explicit MajorFont(const XML::XNode& node);
			const MajorFont& operator =(const XML::XNode& node);

		public:
			virtual void fromXML(const XML::XNode& node);
			virtual const XML::XNode toXML() const;

		public:
			nullable_property<std::string> latin;
			nullable_property<std::string> ea;
			nullable_property<std::string> cs;
			property<std::vector<Font>> fonts;
		};
	} // namespace Theme
} // namespace OOX

#endif // OOX_THEME_MAJOR_FONT_INCLUDE_H