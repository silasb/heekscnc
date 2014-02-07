// Surface.h
/*
 * Copyright (c) 2013, Dan Heeks
 * This program is released under the BSD license. See the file COPYING for
 * details.
 */

#pragma once

#include "interface/IdNamedObj.h"
#include "HeeksCNCTypes.h"

class CSurface: public IdNamedObj {
public:
	std::list<int> m_solids;
	double m_tolerance;
	double m_min_z;
	double m_material_allowance;
	bool m_same_for_each_pattern_position;
	static int number_for_stl_file;

	//	Constructors.
	CSurface();
	CSurface(const std::list<int> &solids, double tol, double min_z, double mat_allowance):m_solids(solids), m_tolerance(tol), m_min_z(min_z), m_material_allowance(mat_allowance), m_same_for_each_pattern_position(true){}

	// HeeksObj's virtual functions
    int GetType()const{return SurfaceType;}
	const wxChar* GetTypeString(void) const{ return _T("Surface"); }
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
}; // End CSurface class definition.

