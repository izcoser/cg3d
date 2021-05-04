CC = gcc
CXX = g++
CFLAGS  = -g -Wall
LINKING = -lglut -lGL -lGLU -lm
TARGET = *

all:
	$(CXX) $(CFLAGS) -o trabalhocg $(TARGET).cpp $(LINKING)

clean:
	$(RM) trabalhocg
