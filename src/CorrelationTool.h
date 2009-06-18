
#ifndef CORRELATION_TOOL_CLASS_DEFINTION
#define CORRELATION_TOOL_CLASS_DEFINTION

// CorrelationTool.h
/*
 * Copyright (c) 2009, Dan Heeks, Perttu Ahola
 * This program is released under the BSD license. See the file COPYING for
 * details.
 */

#include "Program.h"
#include "Op.h"
#include "HeeksCNCTypes.h"
#include "interface/Box.h"

class CCorrelationTool;

class CCorrelationToolParams{
	
public:
	double m_min_correlation_factor;// How similar do other objects need to be before we include them in our 'selection set'?
	double m_max_scale_threshold;	// How much scaling will we allow before obtaining a correlation factor?
	int m_number_of_sample_points;	// How many points will we compare when determining correlation factor?

	void set_initial_values();
	void write_values_to_config();
	void GetProperties(CCorrelationTool* parent, std::list<Property *> *list);
	void WriteXMLAttributes(TiXmlNode* pElem);
	void ReadParametersFromXMLElement(TiXmlElement* pElem);
};



/**
	The CCorrelation class stores a reference to a single object.  When another NC operation (such as
	a DrillingCycle for example) asks us to provide a set of coordinates for 'our' locations, we run through
	all objects in the data file looking for individual objects that 'look' like this one.  The comparison
	algorithm will be;
		- obtain the bounding box for both the reference and the sample objects.
		- scale the reference object up/down (with a maximum of m_params.m_scale_threshold) until
		  both objects are of a similar scale.  If this scaling cannot make their bounding boxes similar
		  enough within the configured maximum threshold then the sample object is discarded as 'not a match'.
		- If the scaling process can produce a similar bounding box, the application draws a logical line
		  from the centroid of each object (reference and sample) at a range of angles (eg: every 'n' degrees
		  around the full 360 circle).  It then intersects this line with the object to find both how many
		  intersections there are and what distance is found from the centroid to the intersection point(s).
		  The difference in these distances for both the reference and sample objects will be combined to
		  produce a correlation factor (score).  If this factor is within the m_params.m_min_correlation_factor
		  then the sample object's centroid is added to this object's reference points.

	The whole idea of this class is not to do anything with these locations itself but, rather, to be used as
	a reference object from which another operation may be performed.

	The scenario I began with was to take a raster image of a Printed Circuit Board (PCB) layout.  I used
	Inkscape to perform an edge detection on this to produce a vector graphics file (SVG format).  I then
	read this vector graphics file into HeeksCNC to produce a series of 'sketch' objects.  I can use these
	sketches (or indeed use the GCode generator available within Inkscape) to do an 'isolation route' to produce
	the PCB itself.  What I then want to do is select one of the small circles representing a hole in the middle
	of a pad.  This circle is represented as a sketch object.  I want to find other sketch objects that are
	also small circles.  When I've found them all, I want to generate a DrillingCycle operation to drill holes
	at these locations.  If I set the maximum scale factor well enough then the set of holes selected for one
	DrillingCycle operation will allow all holes of a similar size to be selected concurrently.  I can then
	repeat the process with a Correlation object of a different scale factor option.  This way I can
	have the HeeksCNC application generate a drilling cycle for all the holes on the board in a, mostly,
	automatic process.
 */

class CCorrelationTool: public HeeksObj {
public:
	//	These are references to the CAD elements whose position indicate where the CorrelationTool Cycle begins.

	/**
                Define some data structures to hold references to CAD elements.  We store both the type and id because
                the ID values are only relevant within the context of a type.
         */
        typedef int SymbolType_t;
        typedef unsigned int SymbolId_t;
        typedef std::pair< SymbolType_t, SymbolId_t > Symbol_t;
        typedef std::list< Symbol_t > Symbols_t;


	CCorrelationToolParams m_params;
        wxString m_title;
	Symbol_t m_reference_symbol;

	Symbols_t m_similar_symbols;	// NOTE: This is a transient list ONLY.  It gets re-generated
					// when this object is 'used' for real work.  It's just to save
					// time

	/**
                There are all types of 3d point classes around but most of them seem to be in the HeeksCAD code
                rather than in cod that's accessible by the plugin.  I suspect I'm missing something on this
                but, just in case I'm not, here is a special one (just for this class)
         */
        typedef struct Point3d {
                double x;
                double y;
                double z;
                Point3d( double a, double b, double c ) : x(a), y(b), z(c) { }
                Point3d() : x(0), y(0), z(0) { }

                bool operator==( const Point3d & rhs ) const
                {
                        if (x != rhs.x) return(false);
                        if (y != rhs.y) return(false);
                        if (z != rhs.z) return(false);

                        return(true);
                } // End equivalence operator

                bool operator!=( const Point3d & rhs ) const
                {
                        return(! (*this == rhs));
                } // End not-equal operator

                bool operator<( const Point3d & rhs ) const
                {
                        if (x > rhs.x) return(false);
                        if (x < rhs.x) return(true);
                        if (y > rhs.y) return(false);
                        if (y < rhs.y) return(true);
                        if (z > rhs.z) return(false);
                        if (z < rhs.z) return(true);

                        return(false);  // They're equal
                } // End equivalence operator
        } Point3d;

	typedef double Angle_t;
	typedef struct {
		std::set<double> m_intersection_distances;
		std::set<Point3d> m_intersection_points;
	} CorrelationSample_t;

	typedef std::map< Angle_t, CorrelationSample_t >	CorrelationData_t;

	Symbols_t SimilarSymbols( const Symbol_t & reference_symbol ) const;

	//	Constructors.
        CCorrelationTool(const wxChar *title, const Symbol_t & reference_symbol ) : m_reference_symbol(reference_symbol)
	{
		m_params.set_initial_values(); 
		if (title != NULL) 
		{
			m_title = title;
		} // End if - then
		else
		{
			m_title = GetTypeString();
		} // End if - else

		printf("constructor ref type='%d',id='%d'\n", m_reference_symbol.first, m_reference_symbol.second );
	
		CorrelationData( 	reference_symbol, reference_symbol, 10, 1.5 );

		m_similar_symbols = SimilarSymbols( reference_symbol );
	} // End constructor

	 // HeeksObj's virtual functions
        int GetType()const{return CorrelationToolType;}
	const wxChar* GetTypeString(void) const{ return _T("CorrelationTool"); }
        HeeksObj *MakeACopy(void)const;

        void WriteXML(TiXmlNode *root);
        static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);

	// program whose job is to generate RS-274 GCode.
	void AppendTextToProgram();

	void glCommands(bool select, bool marked, bool no_color);
	void GetProperties(std::list<Property *> *list);
	void CopyFrom(const HeeksObj* object);
	bool CanAddTo(HeeksObj* owner);
	wxString GetIcon() { return theApp.GetResFolder() + _T("/icons/tool"); }
        const wxChar* GetShortString(void)const{return m_title.c_str();}

        bool CanEditString(void)const{return true;}
        void OnEditString(const wxChar* str);

	// Obtain a single set of correlation data for the sample_symbol.
	CorrelationData_t CorrelationData( 	const Symbol_t & sample_symbol, 
						const Symbol_t & reference_symbol,
						const int number_of_sample_points,
						const double max_scale_threshold ) const;

	std::set<Point3d> ReferencePoints( const Symbols_t & sample_symbols ) const;

	bool SimilarScale( const CBox &reference_box, const CBox &sample_box, const double max_scale_threshold ) const;
	double Score( const CorrelationData_t & sample, const CorrelationData_t & reference ) const;

	std::set<Point3d> FindAllLocations() const;

}; // End CCorrelationTool class definition.


#endif // CORRELATION_TOOL_CLASS_DEFINTION
