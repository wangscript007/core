#include "precompiled_cpcommon.h"

#include <cpdoccore/common/readdocelement.h>
#include <cpdoccore/xml/attributes.h>
#include <cpdoccore/xml/sax.h>
#include <iostream>

namespace cpdoccore { 
namespace common {

bool read_doc_element::read_sax( xml::sax * Reader )
{
    xml::attributes_wc_ptr attrb = xml::read_attributes( Reader );
    add_attributes( attrb );
    
#ifdef _DEBUG
    if (!attrb->check())
    {
        std::wcerr << " -- end (" << Reader->nodeQualifiedName() << ")\n";
    }
#endif

    if (Reader->isEmptyElement())
        return false;

    const unsigned int currentDepth = Reader->depth();
    
    xml::NodeType nodeType;

    while (true)
	{
		nodeType = Reader->next();
       
		if (nodeType == xml::typeEOF || nodeType == xml::typeNone ) break;
       
		if (Reader->depth() <= currentDepth + 1 && nodeType == xml::typeEndElement) break;
        
		switch( nodeType )
        {
			case xml::typeElement:
				{
					const std::wstring namespacePrefix = Reader->namespacePrefix();
					const std::wstring localName = Reader->nodeLocalName();
					add_child_element(Reader, namespacePrefix, localName);
				}
				break;
			case xml::typeWhitespace:
			case xml::typeText:
				{
					const std::wstring value = Reader->value();
					add_text(value);
				}
				break;
        }
                        
    }
    return true;
}

}
}