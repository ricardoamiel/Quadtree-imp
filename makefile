# -> definicion del compilador
CC=g++
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system # SFML libraries

# -> opciones de compilacion
CFLAGS=-I include
EXECUTABLE=main
TEST_EXECUTABLE=test
TEST_EXECUTABLE_2=test2

# -> estructura de folder y archivos
SRC_DIR=src
TEST_DIR=tests
INCLUDE_DIR=include

# -> interfaz main
main: main.cpp
	$(CC) -I include main.cpp -o main $(SFML_FLAGS)

# -> compilar y realizar pruebas
test: $(TEST_EXECUTABLE)
$(TEST_EXECUTABLE): test.cpp Point.h Rectangle.h Circle.h Quadtree.h
	$(CC) $(CFLAGS) test.cpp -o $(TEST_EXECUTABLE) $(SFML_FLAGS)

# -> compilar y realizar pruebas
test2: $(TEST_EXECUTABLE_2)
$(TEST_EXECUTABLE_2): test2.cpp Point.h Rectangle.h Circle.h Quadtree.h
	$(CC) $(CFLAGS) test2.cpp -o $(TEST_EXECUTABLE_2) $(SFML_FLAGS)

# -> limpiar archivos compilados
clean:
	rm -f $(EXECUTABLE) $(TEST_EXECUTABLE) $(SRC_DIR)/*.o $(TEST_DIR)/*.o

# -> eliminar lo que no se necesite
.PHONY: all clean test