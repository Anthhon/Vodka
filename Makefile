#!/bin/bash

CC=gcc
FLAGS=-Wall -Werror -Wextra -pedantic
LIBS=

SRC_PATH=src
BUILD_PATH=.
EXECUTABLE=Vodka

run:
	./$(BUILD_PATH)/$(EXECUTABLE)

clean:
	@echo "[!] Removing content from `$(BUILD_PATH)`"
	rm -i $(BUILD_PATH)/$(EXECUTABLE)

build-linux:
	@echo "[+] Checking build folder existence..."
	mkdir -p $(BUILD_PATH)
	@echo "[+] Building project..."
	$(CC) $(FLAGS) -o $(BUILD_PATH)/$(EXECUTABLE) $(SRC_PATH)/*.c $(LIBS) 
	@echo "[+] Project built successfully!"

debug-linux:
	@echo "[+] Checking build folder existence..."
	mkdir -p $(BUILD_PATH)
	@echo "[+] Building project..."
	$(CC) -o $(BUILD_PATH)/$(EXECUTABLE) $(SRC_PATH)/*.c $(LIBS) -g
	@echo "[+] Project built successfully!"

install-linux:
	@echo "[+] Checking build folder existence..."
	mkdir -p $(BUILD_PATH)
	@echo "[+] Building project..."
	$(CC) $(FLAGS) -o $(BUILD_PATH)/$(EXECUTABLE) $(SRC_PATH)/*.c $(LIBS) 
	@echo "[+] Project built successfully!"
	sudo cp $(BUILD_PATH)/$(EXECUTABLE) /usr/local/bin/;
	echo "[+] $(EXECUTABLE) installed into /usr/local/bin/";
