.PHONY: all clean run

APP = canalyz

SRC = src
OBJ = obj
BIN = bin

SRC_EXT = c
OBJ_EXT = o

APP_SRC = $(shell find $(SRC)/ -name "*.$(SRC_EXT)")
APP_OBJ = $(APP_SRC:$(SRC)/%.$(SRC_EXT)=$(OBJ)/%.$(OBJ_EXT))
LIBCANALYZ = libcanalyz.a

CFLAGS = -Wall -g
CPPFLAGS = -I $(SRC)
LIBS = -L -lcanalyz

all: $(APP)

$(APP): $(OBJ)/main.o $(OBJ)/$(LIBCANALYZ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@ $(LIBS)

$(OBJ)/$(LIBCANALYZ): $(APP_OBJ)
	ar rcs $@ $^

$(OBJ)/%.$(OBJ_EXT): $(SRC)/%.$(SRC_EXT)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	$(RM) $(APP_OBJ)

run: $(APP)
	./$(APP)