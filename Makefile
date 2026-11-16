# Reusable GNU Make build for one C++ executable (GCC or Clang).
# Run `make help` for commands. Source paths must not contain spaces.
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:

# Override from the command line, e.g. make release CXX=clang++ TARGET=orbit.
TARGET ?= nbody
STD ?= c++23
MODE ?= debug
BUILD_ROOT := build
BUILD_DIR := $(BUILD_ROOT)/$(MODE)
PROGRAM := $(BUILD_DIR)/$(TARGET)

# Search these directories recursively. Override SOURCES for explicit control.
# Generated files, hidden directories, and common dependency trees are excluded.
SOURCE_DIRS ?= .
SOURCES ?= $(shell find $(SOURCE_DIRS) \
  -type d \( -name '.*' ! -name . -o -name build -o -name vendor \
  -o -name third_party -o -name node_modules \) -prune -o \
  -type f \( -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' \) -print)
SOURCES := $(sort $(patsubst ./%,%,$(SOURCES)))
OBJECTS := $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(SOURCES)))
DEPS := $(OBJECTS:.o=.d)

# Add include paths with CPPFLAGS=-Iinclude; libraries with LDLIBS=-lfoo.
CPPFLAGS ?=
CXXFLAGS ?= -Wall -Wextra -Wpedantic
LDFLAGS ?=
LDLIBS ?=
RUN_ARGS ?=

ifeq ($(MODE),debug)
  MODE_FLAGS := -O0 -g3
else ifeq ($(MODE),release)
  MODE_FLAGS := -O3 -DNDEBUG
else ifeq ($(MODE),sanitize)
  MODE_FLAGS := -O1 -g3 -fsanitize=address,undefined -fno-omit-frame-pointer
  MODE_LDFLAGS := -fsanitize=address,undefined
else
  $(error Unknown MODE '$(MODE)'; choose debug, release, or sanitize)
endif

COMPILE_FLAGS := -std=$(STD) $(MODE_FLAGS) $(CXXFLAGS)
CONFIG := $(BUILD_DIR)/.config

.PHONY: all debug release sanitize run clean help FORCE
all: $(PROGRAM)

debug release sanitize:
	+$(MAKE) MODE=$@ all

run: $(PROGRAM)
	./$(PROGRAM) $(RUN_ARGS)

# Track flags, compiler, and source-list changes as well as source timestamps.
# Leave the timestamp alone when configuration is unchanged.
$(CONFIG): FORCE
	@mkdir -p "$(@D)"
	@printf '%s\n' '$(CXX)' '$(CPPFLAGS)' '$(COMPILE_FLAGS)' '$(LDFLAGS) $(MODE_LDFLAGS)' '$(LDLIBS)' '$(SOURCES)' > "$@.tmp"
	@if ! cmp -s "$@.tmp" "$@"; then mv "$@.tmp" "$@"; else rm -f "$@.tmp"; fi

$(PROGRAM): $(OBJECTS) $(CONFIG)
	@mkdir -p "$(@D)"
	$(CXX) $(LDFLAGS) $(MODE_LDFLAGS) $(OBJECTS) $(LDLIBS) -o "$@"

# Keep the source extension in object names to avoid foo.cc/foo.cpp collisions.
define compile_rule
$(BUILD_DIR)/%.$(1).o: %.$(1) $(CONFIG)
	@mkdir -p "$$(@D)"
	$$(CXX) $$(CPPFLAGS) $$(COMPILE_FLAGS) -MMD -MP -MF "$$(@:.o=.d)" -MT "$$@" -c "$$<" -o "$$@"
endef
$(foreach ext,cpp cc cxx,$(eval $(call compile_rule,$(ext))))

clean:
	rm -rf -- $(BUILD_ROOT)

help:
	@printf '%s\n' \
	  'make [-jN]           Build debug executable (default)' \
	  'make release         Build optimized executable' \
	  'make sanitize        Build with AddressSanitizer and UBSan' \
	  'make run             Build and run; pass RUN_ARGS="..."' \
	  'make MODE=release run  Build and run release executable' \
	  'make clean           Remove generated build files' \
	  'Overrides: CXX, TARGET, STD, SOURCE_DIRS, SOURCES,' \
	  '           CPPFLAGS, CXXFLAGS, LDFLAGS, LDLIBS' \
	  'Output: build/<mode>/<target>; defaults: debug, nbody'

FORCE:

-include $(DEPS)
