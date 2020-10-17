TEMPLATE = app
DEPENDPATH += ./
INCLUDEPATH += ./ ../ ../imagelib ## relative path to imagelib
INCLUDEPATH += /usr/include/eigen3
RESOURCES += 

CONFIG += qt c++11 debug_and_release
QT += widgets gui

TARGET = imageviewer 
LIBS+= -L../imagelib/ -limagelib -ljpeg -ltiff

HEADERS += qtimageviewer.hpp

SOURCES += imageviewer.cpp \
	qtimageviewer.cpp 

## add call to compile image library
imagelib.target = myimagelibtarget
imagelib.commands = make -w -C ../imagelib/;
imagelib.depends =
QMAKE_EXTRA_TARGETS += imagelib
PRE_TARGETDEPS += myimagelibtarget

