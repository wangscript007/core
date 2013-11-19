#pragma once
#ifndef OOX_LOGIC_OVAL_INCLUDE_H_
#define OOX_LOGIC_OVAL_INCLUDE_H_

#include "Wrap.h"
#include "Shadow.h"
#include "TextBox.h"
#include "LineStyle.h"
#include "FillStyle.h"
#include "ShapeStyle.h"

namespace OOX
{
	namespace Logic
	{
		class Oval : public WritingElement
		{
		public:
			Oval();
			virtual ~Oval();			
			explicit Oval(const XML::XNode& node);
			const Oval& operator =(const XML::XNode& node);

		public:
			virtual void fromXML(const XML::XNode& node);
			virtual const XML::XNode toXML() const;

		public:
			property<std::string>				Id;		
			property<ShapeStyle>				style;

			nullable_property<Wrap>				Wrap;
			nullable_property<NullElement>		anchorlock;

			nullable_property<FillStyle>		fillstyle;
			nullable_property<LineStyle>		linestyle;
			nullable_property<Shadow>			shadow;

			nullable_property<TextBox>			textbox;
		};
	} // namespace Logic
} // namespace OOX

#endif // OOX_LOGIC_OVAL_INCLUDE_H_