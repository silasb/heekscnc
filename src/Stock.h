// Stock.h
/*
 * Copyright (c) 2013, Dan Heeks
 * This program is released under the BSD license. See the file COPYING for
 * details.
 */

#pragma once

#include "interface/IdNamedObj.h"
#include "HeeksCNCTypes.h"

class CStock: public IdNamedObj {
public:
	std::list<int> m_solids;

	//	Constructors.
	CStock();
	CStock(const std::list<int> &solids):m_solids(solids){}

	// HeeksObj's virtual functions
    int GetType()const{return StockType;}
	const wxChar* GetTypeString(void) const{ return _T("Stock"); }
    HeeksObj *MakeACopy(void)const;

    void WriteXML(TiXmlNode *root);
    static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);

	void GetProperties(std::list<Property *> *list);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	void CopyFrom(const HeeksObj* object);
	bool CanAddTo(HeeksObj* owner);
	const wxBitmap &GetIcon();
	void WriteDefaultValues();
	void ReadDefaultValues();
	void GetOnEdit(bool(**callback)(HeeksObj*));
}; // End CStock class definition.

