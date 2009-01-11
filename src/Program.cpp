// Program.cpp

#include "stdafx.h"
#include "Program.h"
#include "PythonStuff.h"
#include "../../tinyxml/tinyxml.h"
#include "ProgramCanvas.h"

CProgram::CProgram()
{
	m_machine.assign("siegkx1");
	m_create_display_list_next_render = false;
}

CProgram::~CProgram()
{
	DestroyGLLists();
}

const CProgram &CProgram::operator=(const CProgram &p)
{
	m_machine = p.m_machine;
	return *this;
}

void CProgram::DestroyGLLists()
{
	if (m_gl_list)
	{
		glDeleteLists(m_gl_list, 1);
		m_gl_list = 0;
	}
}

void CProgram::GetProperties(std::list<Property *> *list)
{
	CNCObj::GetProperties(list);
}

void CProgram::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
	CNCObj::GetTools(t_list, p);
}

HeeksObj *CProgram::MakeACopy(void)const
{
	return new CProgram(*this);
}

void CProgram::CopyFrom(const HeeksObj* object)
{
	operator=(*((CProgram*)object));
}

void CProgram::WriteXML(TiXmlElement *root)
{
	TiXmlElement * element;
	element = new TiXmlElement( "Program" );
	root->LinkEndChild( element );  
	element->SetAttribute("machine", m_machine.c_str());
	element->SetAttribute("program", Ttc(theApp.m_program_canvas->m_textCtrl->GetValue()));

	WriteBaseXML(element);
}

// static member function
HeeksObj* CProgram::ReadFromXMLElement(TiXmlElement* pElem)
{
	CProgram* new_object = new CProgram;

	// get the attributes
	for(TiXmlAttribute* a = pElem->FirstAttribute(); a; a = a->Next())
	{
		std::string name(a->Name());
		if(name == "machine"){new_object->m_machine.assign(a->Value());}
		else if(name == "program"){theApp.m_program_canvas->m_textCtrl->SetValue(Ctt(a->Value()));}
	}

	new_object->ReadBaseXML(pElem);
	theApp.m_program = new_object;

	return new_object;
}

