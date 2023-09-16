CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lsqlite3
SRC = main.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = myprogram

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)
