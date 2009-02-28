#CCFLAGS=-Wall -fPIC -g -I "./" -I/home/dan/Desktop/wxGTK-2.8.9/buildgtk/lib/wx/include/gtk2-ansi-debug-2.8 -I/usr/include/wx-2.8 -D__WXGTK__ -DHEEKSCNC -D__WXDEBUG__ -DWXUSINGDLL
CCFLAGS=-Wall -fPIC -g -I "./" -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D__WXGTK__ -DHEEKSCNC -DUNICODE -DWXUSINGDLL -DTIXML_USE_STL

#WX_LIB_LETTER=d
WX_LIB_LETTER=u

#OBJECTS=HeeksCNC.o HeeksColor.o HeeksObj.o Interface.o NCCode.o ObjList.o OutputCanvas.o Profile.o Pocket.o Program.o ProgramCanvas.o PropertyCheck.o PropertyChoice.o PropertyColor.o PropertyDouble.o PropertyInt.o PropertyList.o PropertyString.o PropertyVertex.o PythonStuff.o strconv.o LeftAndRight.o MarkedObject.o ToolImage.o tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o ZigZag.o
OBJECTS=HeeksCNC.o HeeksColor.o HeeksObj.o Interface.o NCCode.o ObjList.o OutputCanvas.o Profile.o Pocket.o Program.o ProgramCanvas.o PropertyCheck.o PropertyChoice.o PropertyColor.o PropertyDouble.o PropertyInt.o PropertyList.o PropertyString.o PropertyVertex.o PythonStuff.o strconv.o LeftAndRight.o MarkedObject.o ToolImage.o tinyxml.o tinyxmlerror.o tinyxmlparser.o ZigZag.o Adaptive.o

SHARED_LIBRARY=../libheekscnc.so.0.5.1

${SHARED_LIBRARY}: ${OBJECTS}
	gcc -shared -fPIC -Wl,-soname,libheekscnc.so.0 -o ${SHARED_LIBRARY} ${OBJECTS} -lstdc++ -lGLU -lGL -lwx_gtk2${WX_LIB_LETTER}_richtext-2.8 -lwx_gtk2${WX_LIB_LETTER}_aui-2.8 -lwx_gtk2${WX_LIB_LETTER}_xrc-2.8 -lwx_gtk2${WX_LIB_LETTER}_qa-2.8 -lwx_gtk2${WX_LIB_LETTER}_html-2.8 -lwx_gtk2${WX_LIB_LETTER}_adv-2.8 -lwx_gtk2${WX_LIB_LETTER}_core-2.8 -lwx_base${WX_LIB_LETTER}_xml-2.8 -lwx_base${WX_LIB_LETTER}_net-2.8 -lwx_base${WX_LIB_LETTER}-2.8 -lwx_gtk2${WX_LIB_LETTER}_gl-2.8

HeeksCNC.o: HeeksCNC.cpp
	gcc -c $? ${CCFLAGS} -o $@

HeeksColor.o: ../../interface/HeeksColor.cpp
	gcc -c $? ${CCFLAGS} -o $@

HeeksObj.o: ../../interface/HeeksObj.cpp
	gcc -c $? ${CCFLAGS} -o $@

Interface.o: Interface.cpp
	gcc -c $? ${CCFLAGS} -o $@

LeftAndRight.o: ../../interface/LeftAndRight.cpp
	gcc -c $? ${CCFLAGS} -o $@

MarkedObject.o: ../../interface/MarkedObject.cpp
	gcc -c $? ${CCFLAGS} -o $@

NCCode.o: NCCode.cpp
	gcc -c $? ${CCFLAGS} -o $@

ObjList.o: ../../interface/ObjList.cpp
	gcc -c $? ${CCFLAGS} -o $@

OutputCanvas.o: OutputCanvas.cpp
	gcc -c $? ${CCFLAGS} -o $@

Profile.o: Profile.cpp
	gcc -c $? ${CCFLAGS} -o $@

Pocket.o: Pocket.cpp
	gcc -c $? ${CCFLAGS} -o $@

Program.o: Program.cpp
	gcc -c $? ${CCFLAGS} -o $@

ProgramCanvas.o: ProgramCanvas.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyCheck.o: ../../interface/PropertyCheck.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyChoice.o: ../../interface/PropertyChoice.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyColor.o: ../../interface/PropertyColor.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyDouble.o: ../../interface/PropertyDouble.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyInt.o: ../../interface/PropertyInt.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyList.o: ../../interface/PropertyList.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyString.o: ../../interface/PropertyString.cpp
	gcc -c $? ${CCFLAGS} -o $@

PropertyVertex.o: ../../interface/PropertyVertex.cpp
	gcc -c $? ${CCFLAGS} -o $@

PythonStuff.o: PythonStuff.cpp
	gcc -c $? ${CCFLAGS} -o $@

strconv.o: ../../interface/strconv.cpp
	gcc -c $? ${CCFLAGS} -o $@

ToolImage.o: ../../interface/ToolImage.cpp
	gcc -c $? ${CCFLAGS} -o $@

tinyxml.o: ../../tinyxml/tinyxml.cpp
	gcc -c $? ${CCFLAGS} -o $@

#tinystr.o: ../../tinyxml/tinystr.cpp
#	gcc -c $? ${CCFLAGS} -o $@

tinyxmlerror.o: ../../tinyxml/tinyxmlerror.cpp
	gcc -c $? ${CCFLAGS} -o $@

tinyxmlparser.o: ../../tinyxml/tinyxmlparser.cpp
	gcc -c $? ${CCFLAGS} -o $@

ZigZag.o: ZigZag.cpp
	gcc -c $? ${CCFLAGS} -o $@

Adaptive.o: Adaptive.cpp
	gcc -c $? ${CCFLAGS} -o $@

clean:
	-rm ${SHARED_LIBRARY} ${OBJECTS}


