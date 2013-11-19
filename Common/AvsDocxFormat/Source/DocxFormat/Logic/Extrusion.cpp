
// auto inserted precompiled begin
#include "precompiled_docxformat.h"
// auto inserted precompiled end

#include "Extrusion.h"


namespace OOX
{
	namespace Logic
	{
			
		Extrusion::Extrusion()
		{
		}


		Extrusion::~Extrusion()
		{
		}


		Extrusion::Extrusion(const XML::XNode& node)
		{
			fromXML(node);
		}


		const Extrusion& Extrusion::operator = (const XML::XNode& node)
		{
			fromXML(node);
			return *this;
		}


		void Extrusion::fromXML(const XML::XNode& node)
		{
			const XML::XElement element(node);
			On					= element.attribute("on").value();
			Ext					= element.attribute("ext").value();
			BackDepth			= element.attribute("backdepth").value();
			ViewPoint			= element.attribute("viewpoint").value();	
			ViewPointOrigin		= element.attribute("viewpointorigin").value();
			SkewAngle			= element.attribute("skewangle").value();	
			LightPosition		= element.attribute("lightposition").value();
			LightPosition2		= element.attribute("lightposition2").value();	
			Type				= element.attribute("type").value();
		}

		
		const XML::XNode Extrusion::toXML() const
		{			
			return
				XML::XElement(ns.o + "extrusion",					
					XML::XAttribute("on", On) +
					XML::XAttribute(ns.v + "ext", Ext) +
					XML::XAttribute("backdepth", BackDepth) +
					XML::XAttribute("viewpoint", ViewPoint) +
					XML::XAttribute("viewpointorigin", ViewPointOrigin) +
					XML::XAttribute("skewangle", SkewAngle) +
					XML::XAttribute("lightposition", LightPosition) +
					XML::XAttribute("lightposition2", LightPosition2) +
					XML::XAttribute("type", Type)
				);
		}

	} // namespace Logic
} // namespace OOX
