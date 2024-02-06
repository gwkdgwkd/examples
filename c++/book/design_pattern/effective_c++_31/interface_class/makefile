CC			=g++
DEBUG 		=-DUSE_DEBUG
CFLAGS		=-Wall
SOURCES	 	=$(wildcard ./*.cpp)
INCLUDES  	=
LIB_NAMES 	=
LIB_PATH 	=
OBJ			=$(patsubst %.cpp, %.o, $(SOURCES))
TARGET		=person
 
$(TARGET):$(OBJ)
	$(CC) $(OBJ) $(LIB_PATH) $(LIB_NAMES) -o $(TARGET)
	
%.o: %.cpp %.h
	$(CC) $(INCLUDES) $(DEBUG) -c $(CFLAGS) $< -o $@
 
.PHONY:clean
clean:
	@echo "Remove linked and compiled files......"
	rm -rf $(OBJ) $(TARGET) 
