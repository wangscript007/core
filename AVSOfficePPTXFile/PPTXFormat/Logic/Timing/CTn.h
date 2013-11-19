#pragma once
#ifndef PPTX_LOGIC_CTN_INCLUDE_H_
#define PPTX_LOGIC_CTN_INCLUDE_H_

#include "./../../WrapperWritingElement.h"
#include "CondLst.h"
#include "Cond.h"
#include "TnLst.h"
#include "Iterate.h"
#include "./../../Limit/TLRestart.h"
#include "./../../Limit/TLNodeType.h"
#include "./../../Limit/TLNodeFillType.h"
#include "./../../Limit/TLPresetClass.h"
#include "./../../Limit/TLMasterRelation.h"
#include "./../../Limit/TLSyncBehavior.h"

namespace PPTX
{
	namespace Logic
	{
		class CTn : public WrapperWritingElement
		{
		public:
			PPTX_LOGIC_BASE(CTn)

		public:
			virtual void fromXML(XmlUtils::CXmlNode& node)
			{
				node.ReadAttributeBase(L"accel", accel);
				node.ReadAttributeBase(L"afterEffect", afterEffect);
				node.ReadAttributeBase(L"autoRev", autoRev);
				node.ReadAttributeBase(L"bldLvl", bldLvl);
				node.ReadAttributeBase(L"decel", decel);
				node.ReadAttributeBase(L"display", display);
				node.ReadAttributeBase(L"dur", dur);
				node.ReadAttributeBase(L"evtFilter", evtFilter);
				node.ReadAttributeBase(L"fill", fill);
				node.ReadAttributeBase(L"grpId", grpId);
				node.ReadAttributeBase(L"id", id);
				node.ReadAttributeBase(L"masterRel", masterRel);
				node.ReadAttributeBase(L"nodePh", nodePh);
				node.ReadAttributeBase(L"nodeType", nodeType);
				node.ReadAttributeBase(L"presetClass", presetClass);
				node.ReadAttributeBase(L"presetID", presetID);
				node.ReadAttributeBase(L"presetSubtype", presetSubtype);
				node.ReadAttributeBase(L"repeatCount", repeatCount);
				node.ReadAttributeBase(L"repeatDur", repeatDur);
				node.ReadAttributeBase(L"restart", restart);
				node.ReadAttributeBase(L"spd", spd);
				node.ReadAttributeBase(L"syncBehavior", syncBehavior);
				node.ReadAttributeBase(L"tmFilter", tmFilter);

				stCondLst	= node.ReadNode(_T("p:stCondLst"));
				endCondLst	= node.ReadNode(_T("p:endCondLst"));
				endSync		= node.ReadNode(_T("p:endSync"));
				iterate		= node.ReadNode(_T("p:iterate"));
				childTnLst	= node.ReadNode(_T("p:childTnLst"));
				subTnLst	= node.ReadNode(_T("p:subTnLst"));

				Normalize();

				FillParentPointersForChilds();
			}

			virtual CString toXML() const
			{
				XmlUtils::CAttribute oAttr;

				oAttr.Write(_T("id"), id);
				oAttr.Write(_T("presetID"), presetID);
				oAttr.WriteLimitNullable(_T("presetClass"), presetClass);
				oAttr.Write(_T("presetSubtype"), presetSubtype);
				oAttr.Write(_T("dur"), dur);
				oAttr.Write(_T("repeatCount"), repeatCount);
				oAttr.Write(_T("repeatDur"), repeatDur);
				oAttr.Write(_T("spd"), spd);
				oAttr.Write(_T("accel"), accel);
				oAttr.Write(_T("decel"), decel);
				oAttr.Write(_T("autoRev"), autoRev);
				oAttr.WriteLimitNullable(_T("restart"), restart);
				oAttr.WriteLimitNullable(_T("fill"), fill);
				oAttr.WriteLimitNullable(_T("syncBehavior"), syncBehavior);
				oAttr.Write(_T("tmFilter"), tmFilter);
				oAttr.Write(_T("evtFilter"), evtFilter);
				oAttr.Write(_T("display"), display);
				oAttr.WriteLimitNullable(_T("masterRel"), masterRel);
				oAttr.Write(_T("bldLvl"), bldLvl);
				oAttr.Write(_T("grpId"), grpId);
				oAttr.Write(_T("afterEffect"), afterEffect);
				oAttr.WriteLimitNullable(_T("nodeType"), nodeType);
				oAttr.Write(_T("nodePh"), nodePh);


				XmlUtils::CNodeValue oValue;
				oValue.WriteNullable(stCondLst);
				oValue.WriteNullable(endCondLst);
				oValue.WriteNullable(endSync);
				oValue.WriteNullable(iterate);
				oValue.WriteNullable(childTnLst);
				oValue.WriteNullable(subTnLst);

				return XmlUtils::CreateNode(_T("p:cTn"), oAttr, oValue);
			}

		public:
			//Childs
			nullable<CondLst>			stCondLst;
			nullable<CondLst>			endCondLst;
			nullable<Cond>				endSync;
			nullable<Iterate>			iterate;
			nullable<TnLst>				childTnLst;
			nullable<TnLst>				subTnLst;


			//Attributes
			nullable_int							accel;
			nullable_bool							afterEffect;
			nullable_bool							autoRev;
			nullable_int							bldLvl;
			nullable_int							decel;
			nullable_bool							display;
			nullable_string							dur;	//unsigned int/"indefinite"
			nullable_string							evtFilter;
			nullable_limit<Limit::TLNodeFillType>	fill;
			nullable_int							grpId;
			nullable_int							id;
			nullable_limit<Limit::TLMasterRelation> masterRel;
			nullable_bool							nodePh;
			nullable_limit<Limit::TLNodeType>		nodeType;
			nullable_limit<Limit::TLPresetClass>	presetClass;
			nullable_int							presetID;
			nullable_int							presetSubtype;
			nullable_string							repeatCount;
			nullable_string							repeatDur;
			nullable_limit<Limit::TLRestart>		restart;
			nullable_int							spd;
			nullable_limit<Limit::TLSyncBehavior>	syncBehavior;
			nullable_string							tmFilter;
		protected:
			virtual void FillParentPointersForChilds()
			{
				if(stCondLst.IsInit())
					stCondLst->SetParentPointer(this);
				if(endCondLst.IsInit())
					endCondLst->SetParentPointer(this);
				if(endSync.IsInit())
					endSync->SetParentPointer(this);
				if(iterate.IsInit())
					iterate->SetParentPointer(this);
				if(childTnLst.IsInit())
					childTnLst->SetParentPointer(this);
				if(subTnLst.IsInit())
					subTnLst->SetParentPointer(this);
			}

			AVSINLINE void Normalize()
			{
				grpId.normalize_positive();
				id.normalize_positive();
			}
		};
	} // namespace Logic
} // namespace PPTX

#endif // PPTX_LOGIC_CTN_INCLUDE_H