CC = clang
CFLAGS = -Wall -Wno-unused-result -g -Og
SRC_DIR = src
BIN_DIR = bin
SRCS = $(SRC_DIR)/compilador.c $(SRC_DIR)/lexico.c $(SRC_DIR)/sintatico.c
OBJS = $(SRCS:.c=.o)
TARGET = $(BIN_DIR)/compilador

.PHONY: all clean

all: $(BIN_DIR) $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(BIN_DIR) 