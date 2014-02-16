// SketchOp.h
/*
 * Copyright (c) 2014, Dan Heeks
 * This program is released under the BSD license. See the file COPYING for
 * details.
 */

// base class for machining operations which can have multiple depths

#ifndef SKETCH_OP_HEADER
#define SKETCH_OP_HEADER

#include "DepthOp.h"
#include <list>

class CSketchOp : public CDepthOp
{
public:
	int m_sketch;

	CSketchOp(int sketch, const int tool_number = -1, const int operation_type = UnknownType )
		: m_sketch(sketch), CDepthOp(tool_number, operation_type){}

	CSketchOp & operator= ( const CSketchOp & rhs );
	CSketchOp( const CSketchOp & rhs );

	// HeeksObj's virtual functions
	void GetProperties(std::list<Property *> *list);
	void WriteBaseXML(TiXmlElement *element);
	void ReadBaseXML(TiXmlElement* element);
	void ReloadPointers();

	// COp's virtual functions
	Python AppendTextToProgram();
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);

	bool operator== ( const CSketchOp & rhs ) const;
	bool operator!= ( const CSketchOp & rhs ) const { return(! (*this == rhs)); }
	bool IsDifferent(HeeksObj *other) { return(*this != (*((CSketchOp *) other))); }
};

#endif
