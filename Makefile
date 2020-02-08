OBJS = *.cpp
OBJS += Common/*.cpp
OBJS += Engine/Gfx/*.cpp
OBJS += Engine/Gfx/DataModel/*.cpp

# 3rd party
#OBJS += Libs/mar_tp1/*.cpp

CC = g++

COMPILER_FLAGS = -pthread -Wall -std=c++11 -g

LINKER_FLAGS = -lpthread -lglut -lGL -lGLU -lboost_regex

OBJ_NAME = robo

INC = -I. -ILibs

all : $(OBJS)
	$(CC) $(INC) $(COMPILER_FLAGS) $(OBJS) $(LINKER_FLAGS) -o $(OBJ_NAME)

clean :
	rm $(OBJ_NAME)
