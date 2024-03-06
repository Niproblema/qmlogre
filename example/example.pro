CONFIG += qt
CONFIG += warn_off
QT += qml quick
TEMPLATE = app
TARGET = qmlogre

LIBS +=

UI_DIR = ./.ui
OBJECTS_DIR = ./.obj
MOC_DIR = ./.moc
SOURCES += main.cpp \
    cameranodeobject.cpp \
    exampleapp.cpp \
    ogreitem.cpp \
    ogrenode.cpp \
    ogrecamerawrapper.cpp \
    ogreengine.cpp \
    glad.c

HEADERS += cameranodeobject.h \
    exampleapp.h \
    ogreitem.h \
    ogrenode.h \
    ogrecamerawrapper.h \
    ogreengine.h

OTHER_FILES += \
    resources/example.qml

# Adjust to your OGRE installation path either here in as environment variable
# /usr/local equates to /usr/local/include/OGRE, /usr/local/lib/OGRE installation
OGRE_HOME = /usr/local      

OGREDIR = $$OGRE_HOME
message(Using Ogre libraries in $$OGREDIR)
INCLUDEPATH += $$OGREDIR/include/OGRE

ARCH = $$ARCH
contains(ARCH, "x86") {
    DEFINES += JIBO_GL
    INCLUDEPATH += $$OGREDIR/include/OGRE/RenderSystems/GL
}
#QMAKE_LFLAGS += -F$$OGREDIR/lib/release
LIBS += -L$$OGREDIR/lib \
        -lOgreMain  \
        -lGL \
        -lGLU \
        -lX11 \
        -lXxf86vm \
        -lXrandr \
        -pthread \
        -lXi \
        -ldl

RESOURCES += resources/resources.qrc

# Copy all resources to build folder
Resources.path = $$OUT_PWD/resources
Resources.files = resources/*.zip

# Copy all config files to build folder
Config.path = $$OUT_PWD
Config.files = config/*

# make install
INSTALLS += Resources Config

DISTFILES += \
    plugins.cfg \
    resource.cfg

QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$PWD/resources $$OUT_PWD && $$QMAKE_COPY_DIR $$PWD/config/* $$OUT_PWD
