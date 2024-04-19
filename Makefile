# Written-By: Til Goldan (131473651)
# Written-By: Jasmine McDuffie (825467728)
# Class: CS530, Spring 2024
# Assignment: 2
# User: cssc4033
# File: Makefile

SRC = $(wildcard src/*.cc)

BUILD_PATH = build
OBJECTS = $(patsubst src/%.cc,$(BUILD_PATH)/%.o,$(SRC))

BIN_PATH = $(BUILD_PATH)/bin
BIN_NAME = asxe

INCLUDES = -I include/

CXX = g++

LFLAGS = -g -std=c++11 -Wall -Wextra -pedantic
CFLAGS = -g -c -std=c++11 -Wall -Wextra -pedantic $(INCLUDES)

CFLAGS += -DPROGRAM_NAME=\"$(BIN_NAME)\"

$(BIN_PATH)/$(BIN_NAME): $(OBJECTS)
	mkdir -p $(BIN_PATH)
	$(CXX) $(LFLAGS) -o $@ $(OBJECTS)

$(BUILD_PATH)/%.o: src/%.cc
	mkdir -p $(BUILD_PATH)
	$(CXX) $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -fd $(BIN_PATH)/$(BIN_NAME) $(BIN_PATH) $(OBJECTS) $(BUILD_PATH)

# Testing

ARGS = $(BIN_NAME)

.PHONY: test
test:
	./$(BIN_PATH)/$(BIN_NAME) tests/P2sample.sic

# Remote sync and test on edoras

REMOTE_PATH = a2

sync: clean
	rsync -avz ./ edoras-guy:$(REMOTE_PATH)

r-test:
	ssh edoras-guy "cd $(REMOTE_PATH); make test ARGS=\"$(ARGS)\""


