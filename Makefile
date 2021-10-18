SOURCES=hdspeconf.cpp SndCard.cpp SndControl.cpp \
	HDSPeCard.cpp TCO.cpp AioPro.cpp \
	NoCardsPanel.cpp TCOPanel.cpp AioProPanel.cpp \
	AioPanel.cpp
OBJECTS=${SOURCES:.cpp=.o} 
CXXFLAGS=-Wall -g -O2 -I.. `wx-config --cxxflags`
LDFLAGS=-lasound `wx-config --libs` #  -Wl,-rpath=/usr/local/wxwidgets/lib

all: hdspeconf

hdspeconf: $(OBJECTS)
	g++ -o hdspeconf ${OBJECTS} $(LDFLAGS)

depend:
	g++ $(CXXFLAGS) -MM $(SOURCES) > deps

clean:
	-rm *.o *~ deps
	touch deps

.cpp.o:
	g++ -c ${CXXFLAGS} -o $*.o $*.cpp 

include deps
