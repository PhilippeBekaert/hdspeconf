SOURCES=hdspeconf.cpp SndCard.cpp SndControl.cpp \
	HDSPeCard.cpp TCO.cpp Aio.cpp AioPro.cpp RayDAT.cpp \
	NoCardsPanel.cpp TCOPanel.cpp AioPanel.cpp AioProPanel.cpp \
	RayDATPanel.cpp
OBJECTS=${SOURCES:.cpp=.o} 
CXXFLAGS=-Wall -g -O2 -I.. `wx-config --cxxflags`
LDFLAGS=-lasound `wx-config --libs`

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
