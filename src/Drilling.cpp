// Drilling.cpp
/*
 * Copyright (c) 2009, Dan Heeks, Perttu Ahola
 * This program is released under the BSD license. See the file COPYING for
 * details.
 */

#include "stdafx.h"
#include "Drilling.h"
#include "CNCConfig.h"
#include "ProgramCanvas.h"
#include "interface/HeeksObj.h"
#include "interface/PropertyInt.h"
#include "interface/PropertyDouble.h"
#include "interface/PropertyLength.h"
#include "interface/PropertyChoice.h"
#include "tinyxml/tinyxml.h"
#include "CuttingTool.h"
#include "CorrelationTool.h"

#include <sstream>
#include <iomanip>

extern CHeeksCADInterface* heeksCAD;


void CDrillingParams::set_initial_values( const double depth )
{
	CNCConfig config;

	config.Read(_T("m_standoff"), &m_standoff, (25.4 / 4));	// Quarter of an inch
	config.Read(_T("m_dwell"), &m_dwell, 1);
	config.Read(_T("m_depth"), &m_depth, 25.4);		// One inch
	config.Read(_T("m_peck_depth"), &m_peck_depth, (25.4 / 10));	// One tenth of an inch

	if (depth > 0)
	{
		// We've found the depth we want used.  Assign it now.
		m_depth = depth;
	} // End if - then

}

void CDrillingParams::write_values_to_config()
{
	// We always want to store the parameters in mm and convert them back later on.

	CNCConfig config;

	// These values are in mm.
	config.Write(_T("m_standoff"), m_standoff);
	config.Write(_T("m_dwell"), m_dwell);
	config.Write(_T("m_depth"), m_depth);
	config.Write(_T("m_peck_depth"), m_peck_depth);
}


static void on_set_standoff(double value, HeeksObj* object){((CDrilling*)object)->m_params.m_standoff = value;}
static void on_set_dwell(double value, HeeksObj* object){((CDrilling*)object)->m_params.m_dwell = value;}
static void on_set_depth(double value, HeeksObj* object){((CDrilling*)object)->m_params.m_depth = value;}
static void on_set_peck_depth(double value, HeeksObj* object){((CDrilling*)object)->m_params.m_peck_depth = value;}


void CDrillingParams::GetProperties(CDrilling* parent, std::list<Property *> *list)
{
	list->push_back(new PropertyLength(_("standoff"), m_standoff, parent, on_set_standoff));
	list->push_back(new PropertyDouble(_("dwell"), m_dwell, parent, on_set_dwell));
	list->push_back(new PropertyLength(_("depth"), m_depth, parent, on_set_depth));
	list->push_back(new PropertyLength(_("peck_depth"), m_peck_depth, parent, on_set_peck_depth));
}

void CDrillingParams::WriteXMLAttributes(TiXmlNode *root)
{
	TiXmlElement * element;
	element = new TiXmlElement( "params" );
	root->LinkEndChild( element );  
	element->SetDoubleAttribute("standoff", m_standoff);
	element->SetDoubleAttribute("dwell", m_dwell);
	element->SetDoubleAttribute("depth", m_depth);
	element->SetDoubleAttribute("peck_depth", m_peck_depth);
}

void CDrillingParams::ReadParametersFromXMLElement(TiXmlElement* pElem)
{
	if (pElem->Attribute("standoff")) m_standoff = atof(pElem->Attribute("standoff"));
	if (pElem->Attribute("dwell")) m_dwell = atof(pElem->Attribute("dwell"));
	if (pElem->Attribute("depth")) m_depth = atof(pElem->Attribute("depth"));
	if (pElem->Attribute("peck_depth")) m_peck_depth = atof(pElem->Attribute("peck_depth"));
}

/**
	This method is called when the CAD operator presses the Python button.  This method generates
	Python source code whose job will be to generate RS-274 GCode.  It's done in two steps so that
	the Python code can be configured to generate GCode suitable for various CNC interpreters.
 */
void CDrilling::AppendTextToProgram()
{
	COp::AppendTextToProgram();

#ifdef UNICODE
	std::wostringstream ss;
#else
    std::ostringstream ss;
#endif
    ss.imbue(std::locale("C"));
	ss<<std::setprecision(10);

	std::set<Point3d> locations = FindAllLocations( m_symbols );
	for (std::set<Point3d>::const_iterator l_itLocation = locations.begin(); l_itLocation != locations.end(); l_itLocation++)
	{


		ss << "drill("
			<< "x=" << l_itLocation->x/theApp.m_program->m_units << ", "
			<< "y=" << l_itLocation->y/theApp.m_program->m_units << ", "
			<< "z=" << l_itLocation->z/theApp.m_program->m_units << ", "
			<< "depth=" << m_params.m_depth/theApp.m_program->m_units << ", "
			<< "standoff=" << m_params.m_standoff/theApp.m_program->m_units << ", "
			<< "dwell=" << m_params.m_dwell << ", "
			<< "peck_depth=" << m_params.m_peck_depth/theApp.m_program->m_units // << ", "
			<< ")\n";

	} // End for

	theApp.m_program_canvas->m_textCtrl->AppendText(ss.str().c_str());
}


/**
	This routine generates a list of coordinates around the circumference of a circle.  It's just used
	to generate data suitable for OpenGL calls to paint a circle.  This graphics is transient but will
	help represent what the GCode will be doing when it's generated.
 */
std::list< CDrilling::Point3d > CDrilling::PointsAround( const CDrilling::Point3d & origin, const double radius, const unsigned int numPoints ) const
{
	std::list<CDrilling::Point3d> results;

	double alpha = 3.1415926 * 2 / numPoints;

	unsigned int i = 0;
	while( i++ < numPoints )
	{
		double theta = alpha * i;
		CDrilling::Point3d pointOnCircle( cos( theta ) * radius, sin( theta ) * radius, 0 );

		pointOnCircle.x += origin.x;
		pointOnCircle.y += origin.y;
		pointOnCircle.z += origin.z;

		results.push_back(pointOnCircle);
	} // End while

	return(results);

} // End PointsAround() routine


/**
	Generate a list of vertices that represent the hole that will be drilled.  Let it be a circle at the top, a
	spiral down the length and a countersunk base.

	This method is only called by the glCommands() method.  This means that the graphics is transient.

	TODO: Handle drilling in any rotational angle. At the moment it only handles drilling 'down' along the 'z' axis
 */

std::list< CDrilling::Point3d > CDrilling::DrillBitVertices( const CDrilling::Point3d & origin, const double radius, const double length ) const
{
	std::list<CDrilling::Point3d> top, spiral, bottom, countersink, result;
    
	double flutePitch = 5.0;	// 5mm of depth per spiral of the drill bit's flute.
	double countersinkDepth = -1 * radius * tan(31.0); // this is the depth of the countersink cone at the end of the drill bit. (for a typical 118 degree bevel)
	unsigned int numPoints = 20;	// number of points in one circle (360 degrees) i.e. how smooth do we want the graphics
	const double pi = 3.1415926;
	double alpha = 2 * pi / numPoints;

	// Get a circle at the top of the dill bit's path
	top = PointsAround( origin, radius, numPoints );
	top.push_back( *(top.begin()) );	// Close the circle

	double depthPerItteration;
	countersinkDepth = -1 * radius * tan(31.0);	// For a typical (118 degree bevel on the drill bit tip)

	unsigned int l_iNumItterations = numPoints * (length / flutePitch);
	depthPerItteration = (length - countersinkDepth) / l_iNumItterations;

	// Now generate the spirals.
	
	unsigned int i = 0;
	while( i++ < l_iNumItterations )
	{
		double theta = alpha * i;
		CDrilling::Point3d pointOnCircle( cos( theta ) * radius, sin( theta ) * radius, 0 );

		pointOnCircle.x += origin.x;
		pointOnCircle.y += origin.y;
		pointOnCircle.z += origin.z;
		
		// And spiral down as we go.
		pointOnCircle.z -= (depthPerItteration * i);

		spiral.push_back(pointOnCircle);
	} // End while

	// And now the countersink at the bottom of the drill bit.
	i = 0;
	while( i++ < numPoints )
	{
		double theta = alpha * i;
		CDrilling::Point3d topEdge( cos( theta ) * radius, sin( theta ) * radius, 0 );

		// This is at the top edge of the countersink
		topEdge.x += origin.x;
		topEdge.y += origin.y;
		topEdge.z = origin.z - (length - countersinkDepth);
		spiral.push_back(topEdge);

		// And now at the very point of the countersink
		CDrilling::Point3d veryTip( origin );
		veryTip.x = origin.x;
		veryTip.y = origin.y;
		veryTip.z = (origin.z - length);

		spiral.push_back(veryTip);
		spiral.push_back(topEdge);
	} // End while

	std::copy( top.begin(), top.end(), std::inserter( result, result.begin() ) );
	std::copy( spiral.begin(), spiral.end(), std::inserter( result, result.end() ) );
	std::copy( countersink.begin(), countersink.end(), std::inserter( result, result.end() ) );

	return(result);

} // End DrillBitVertices() routine


/**
	This is the Graphics Library Commands (from the OpenGL set).  This method calls the OpenGL
	routines to paint the drill action in the graphics window.  The graphics is transient.

	Part of its job is to re-paint the elements that this CDrilling object refers to so that
	we know what CAD objects this CNC operation is referring to.
 */
void CDrilling::glCommands(bool select, bool marked, bool no_color)
{
	if(marked && !no_color)
	{
		double l_dHoleDiameter = 12.7;	// Default at half-inch (in mm)

		if (m_cutting_tool_number > 0)
		{
			HeeksObj* cuttingTool = heeksCAD->GetIDObject( CuttingToolType, m_cutting_tool_number );
			if (cuttingTool != NULL)
			{
				l_dHoleDiameter = ((CCuttingTool *) cuttingTool)->m_params.m_diameter*(theApp.m_program->m_units);
			} // End if - then
		} // End if - then

		std::set<Point3d> locations = FindAllLocations( m_symbols );

		for (Symbols_t::const_iterator l_itSymbol = m_symbols.begin(); l_itSymbol != m_symbols.end(); l_itSymbol++)
		{
			HeeksObj* object = heeksCAD->GetIDObject(l_itSymbol->first, l_itSymbol->second);

			// If we found something, ask its CAD code to draw itself highlighted.
			if(object)object->glCommands(false, true, false);
		} // End for

		for (std::set<Point3d>::const_iterator l_itLocation = locations.begin(); l_itLocation != locations.end(); l_itLocation++)
		{
			GLdouble start[3], end[3];

			start[0] = l_itLocation->x;
			start[1] = l_itLocation->y;
			start[2] = l_itLocation->z;

			end[0] = l_itLocation->x;
			end[1] = l_itLocation->y;
			end[2] = l_itLocation->z;

			end[2] -= m_params.m_depth;
				
			glBegin(GL_LINE_STRIP);
			glVertex3dv( start );
			glVertex3dv( end );
			glEnd();

			std::list< CDrilling::Point3d > pointsAroundCircle = DrillBitVertices( 	*l_itLocation, 
												l_dHoleDiameter / 2, 
												m_params.m_depth);

			glBegin(GL_LINE_STRIP);
			CDrilling::Point3d previous = *(pointsAroundCircle.begin());
			for (std::list< CDrilling::Point3d >::const_iterator l_itPoint = pointsAroundCircle.begin();
				l_itPoint != pointsAroundCircle.end();
				l_itPoint++)
			{
				
				glVertex3d( l_itPoint->x, l_itPoint->y, l_itPoint->z );
			}
			glEnd();
		} // End for
	} // End if - then
}


void CDrilling::GetProperties(std::list<Property *> *list)
{
	m_params.GetProperties(this, list);
	COp::GetProperties(list);
}

HeeksObj *CDrilling::MakeACopy(void)const
{
	return new CDrilling(*this);
}

void CDrilling::CopyFrom(const HeeksObj* object)
{
	operator=(*((CDrilling*)object));
}

bool CDrilling::CanAddTo(HeeksObj* owner)
{
	return owner->GetType() == OperationsType;
}

void CDrilling::WriteXML(TiXmlNode *root)
{
	TiXmlElement * element = new TiXmlElement( "Drilling" );
	root->LinkEndChild( element );  
	m_params.WriteXMLAttributes(element);

	TiXmlElement * symbols;
	symbols = new TiXmlElement( "symbols" );
	element->LinkEndChild( symbols );  

	for (Symbols_t::const_iterator l_itSymbol = m_symbols.begin(); l_itSymbol != m_symbols.end(); l_itSymbol++)
	{
		TiXmlElement * symbol = new TiXmlElement( "symbol" );
		symbols->LinkEndChild( symbol );  
		symbol->SetAttribute("type", l_itSymbol->first );
		symbol->SetAttribute("id", l_itSymbol->second );
	} // End for

	WriteBaseXML(element);
}

// static member function
HeeksObj* CDrilling::ReadFromXMLElement(TiXmlElement* element)
{
	CDrilling* new_object = new CDrilling;

	// read point and circle ids
	for(TiXmlElement* pElem = TiXmlHandle(element).FirstChildElement().Element(); pElem; pElem = pElem->NextSiblingElement())
	{
		std::string name(pElem->Value());
		if(name == "params"){
			new_object->m_params.ReadParametersFromXMLElement(pElem);
		}
		else if(name == "symbols"){
			for(TiXmlElement* child = TiXmlHandle(pElem).FirstChildElement().Element(); child; child = child->NextSiblingElement())
			{
				if (child->Attribute("type") && child->Attribute("id"))
				{
					new_object->AddSymbol( atoi(child->Attribute("type")), atoi(child->Attribute("id")) );
				}
			} // End for
		} // End if
	}

	new_object->ReadBaseXML(element);

	return new_object;
}


/**
 * 	This method looks through the symbols in the list.  If they're PointType objects
 * 	then the object's location is added to the result set.  If it's a circle object
 * 	that doesn't intersect any other element (selected) then add its centre to
 * 	the result set.  Finally, find the intersections of all of these elements and
 * 	add the intersection points to the result set.  We use std::set<Point3d> so that
 * 	we end up with no duplicate points.
 */
std::set<CDrilling::Point3d> CDrilling::FindAllLocations( const CDrilling::Symbols_t & symbols )
{
	std::set<CDrilling::Point3d> locations;

	// We want to avoid calling the (expensive) intersection code too often.  If we've
	// already intersected objects a and b then we shouldn't worry about intersecting 'b' with 'a'
	// the next time through the loop.
	// std::set< std::pair< Symbol_t, Symbol_t > > alreadyChecked;

	// Look to find all intersections between all selected objects.  At all these locations, create
        // a drilling cycle.

        for (CDrilling::Symbols_t::const_iterator lhs = symbols.begin(); lhs != symbols.end(); lhs++)
        {
		bool l_bIntersectionsFound = false;	// If it's a circle and it doesn't
							// intersect anything else, we want to know
							// about it.
		
		if (lhs->first == PointType)
		{
			HeeksObj *lhsPtr = heeksCAD->GetIDObject( lhs->first, lhs->second );
			if (lhsPtr != NULL)
			{
				double pos[3];
				lhsPtr->GetStartPoint(pos);
				locations.insert( CDrilling::Point3d( pos[0], pos[1], pos[2] ) );
			} // End if - then

			continue;	// No need to intersect a point with anything.
		} // End if - then		

		if (lhs->first == CorrelationToolType)
		{
			HeeksObj *lhsPtr = heeksCAD->GetIDObject( lhs->first, lhs->second );
			if (lhsPtr)
			{
				std::set<CCorrelationTool::Point3d> ref = ((CCorrelationTool *)lhsPtr)->FindAllLocations();
				for (std::set<CCorrelationTool::Point3d>::const_iterator l_itPoint = ref.begin(); l_itPoint != ref.end(); l_itPoint++)
				{
					locations.insert( CDrilling::Point3d( l_itPoint->x, l_itPoint->y, l_itPoint->z ) );
				} // End for
			} // End if - then

			continue;
		} // End if - then

                for (CDrilling::Symbols_t::const_iterator rhs = symbols.begin(); rhs != symbols.end(); rhs++)
                {
                        if (lhs == rhs) continue;
			if (lhs->first == PointType) continue;	// No need to intersect a point type.

                        std::list<double> results;
                        HeeksObj *lhsPtr = heeksCAD->GetIDObject( lhs->first, lhs->second );
                        HeeksObj *rhsPtr = heeksCAD->GetIDObject( rhs->first, rhs->second );

                        if ((lhsPtr != NULL) && (rhsPtr != NULL) && (lhsPtr->Intersects( rhsPtr, &results )))
                        {
				l_bIntersectionsFound = true;
                                while (((results.size() % 3) == 0) && (results.size() > 0))
                                {
                                        CDrilling::Point3d intersection;

                                        intersection.x = *(results.begin());
                                        results.erase(results.begin());

                                        intersection.y = *(results.begin());
                                        results.erase(results.begin());

                                        intersection.z = *(results.begin());
                                        results.erase(results.begin());

                                        locations.insert(intersection);
                                } // End while
                        } // End if - then
                } // End for

		if (! l_bIntersectionsFound)
		{
			// This element didn't intersect anything else.  If it's a circle
			// then add its centre point to the result set.

			if (lhs->first == CircleType)
			{
                        	HeeksObj *lhsPtr = heeksCAD->GetIDObject( lhs->first, lhs->second );
				double pos[3];
				if ((lhsPtr != NULL) && (heeksCAD->GetArcCentre( lhsPtr, pos )))
				{
					locations.insert( CDrilling::Point3d( pos[0], pos[1], pos[2] ) );
				} // End if - then
			} // End if - then
		} // End if - then
        } // End for

	return(locations);
} // End FindAllLocations() method

std::set<CDrilling::Point3d> CDrilling::FindAllLocations() const
{
	return( FindAllLocations( m_symbols ) );
} // End FindAllLocations() method

