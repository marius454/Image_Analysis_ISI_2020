HEADERS = image.hpp
SOURCES = image.cpp imagetiff.cpp imagejpeg.cpp
OBJS = $(SOURCES:.cpp=.o)

TARGET = libimagelib.so

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(HEADERS) $(OBJS)
	g++ $(OBJS) -shared -o $(TARGET)

# '$@' matches target, '%<' matches source
%.o: %.cpp image.hpp
	g++ -fPIC -O3 -c $< -o $@ -I./

clean:
	rm *.o;
	rm $(TARGET)
