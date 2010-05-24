// Tags.h
// Copyright (c) 2010, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "interface/ObjList.h"
#include "HeeksCNCTypes.h"

#pragma once

class CTags: public ObjList {
public:
	CTags() { }
	CTags & operator= ( const CTags & rhs );
	CTags( const CTags & rhs );

	// HeeksObj's virtual functions
	bool OneOfAKind(){return true;} // only one in each profile operation
	int GetType()const{return TagsType;}
	const wxChar* GetTypeString(void)const{return _("Tags");}
	HeeksObj *MakeACopy(void)const{ return new CTags(*this);}
	const wxBitmap &GetIcon();
	bool CanAddTo(HeeksObj* owner){return owner->GetType() == ProfileType;}
	bool CanAdd(HeeksObj* object);
	bool CanBeRemoved(){return false;}
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	void WriteXML(TiXmlNode *root);
	bool AutoExpand(){return true;}
	bool UsesID() { return(false); }

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
};
