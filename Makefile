CXX ?= g++
CXXFLAGS ?= -Wall -Wextra -Wpedantic
SOURCES := main.cpp solver.cpp

.PHONY: all debug release sanitize run clean
.DELETE_ON_ERROR:

all debug: build/debug/nbody
release: build/release/nbody
sanitize: build/sanitize/nbody

build/debug/nbody: CXXFLAGS += -O0 -g
build/release/nbody: CXXFLAGS += -O3 -DNDEBUG
build/sanitize/nbody: CXXFLAGS += -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer

build/%/nbody: $(SOURCES) solver.h Makefile
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) -std=c++23 $(CXXFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

run: build/debug/nbody
	./$< $(RUN_ARGS)

clean:
	rm -rf -- build
