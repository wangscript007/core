#pragma once
#ifndef PPTX_LOGIC_EXCL_INCLUDE_H_
#define PPTX_LOGIC_EXCL_INCLUDE_H_

#include "./../../WrapperWritingElement.h"
#include "property.h"
#include "CTn.h"

namespace PPTX
{
	namespace Logic
	{
		class Excl : public WrapperWritingElement
		{
		public:
			Excl();
			virtual ~Excl();			
			explicit Excl(const XML::XNode& node);
			const Excl& operator =(const XML::XNode& node);

		public:
			virtual void fromXML(const XML::XNode& node);
			virtual const XML::XNode toXML() const;

		public:
			property<CTn> cTn;
		protected:
			virtual void FillParentPointersForChilds();
		};
	} // namespace Logic
} // namespace PPTX

#endif // PPTX_LOGIC_EXCL_INCLUDE_H