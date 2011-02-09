TEMPLATE = lib
SOURCES += QVideoEncoder.cpp #\
           #VideoEncoder.cpp
HEADERS += QVideoEncoder.h \
          # VideoEncoder.h \
           ffmpeg.h

# Set list of required FFmpeg libraries
LIBS += -lavutil \
    -lavcodec \
    -lavformat \
    -lswscale
DEFINES += __STDC_CONSTANT_MACROS

DESTDIR = ../
