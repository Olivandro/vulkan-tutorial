SRC_DIR := vulkan-tutorial
BIN_DIR := build

CC = gcc
CPPFLAGS = -Iinclude -MMD -MP
CFLAGS = -Wall
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi



OBJ = vkTut

all:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(SRC_DIR)/*.c -o $(BIN_DIR)/$(OBJ)

clean:
	rm -f $(BIN_DIR)/$(OBJ)

