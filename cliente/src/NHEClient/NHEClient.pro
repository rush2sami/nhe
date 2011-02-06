TEMPLATE = app
TARGET = NHEClient
QT += core \
    gui \
    sql \
    network \
    opengl
HEADERS += aboutnhe/nheabout.h \
    project/nheviewproject.h \
    nheclient.h
SOURCES += aboutnhe/nheabout.cpp \
    project/nheviewproject.cpp \
    nheclient.cpp \
    main.cpp
FORMS += aboutnhe/nheabout.ui \
    project/nheviewproject.ui \
    nheclient.ui
RESOURCES += 
