CC := g++
LIBS := `pkg-config --libs glibmm-2.4` -lzip
CFLAGS := -c -Wall `pkg-config --cflags glibmm-2.4` -g -O2
LDFLAGS := $(LIBS)
SOURCES := $(wildcard src/*.cpp)
HEADERS := $(wildcard include/*.h)
INCLUDES := -I$(realpath include)
OBJECTS := $(SOURCES:src/%.cpp=obj/%.o)
EXECUTABLE := djava

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.SECONDEXPANSION:
$(OBJECTS) : $$(patsubst obj/%.o,src/%.cpp,$$@) $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

.PHONY : clean
clean:
	-rm -rf obj/*.o
	-rm -f $(EXECUTABLE)
