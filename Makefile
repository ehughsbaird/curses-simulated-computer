## Various variables
#
#

# Warnings - enable as many as possible!
WARNINGS = \
-Wall -Wextra -Wpedantic

# External libraries, etc
LDFLAGS = -lncursesw

# Compiler to use
# Let's use the gcc by default
CC ?= gcc

# What version of C to use
# Let's just default to C11
C_VER = -std=c11

# Compiler flags (not warnings or version)
# Ones provided here generate dependency files
C_FLAGS = -MMD -MP

# Optimization level
OPT_LEVEL = -Os

# location of our source files
SRCDIR = src

# location of compiled object files
OBJDIR = obj

# The source files contained in said directory
SRC = $(wildcard $(SRCDIR)/*.c)

# The headers from said source directory
HEADERS = $(wildcard $(SRCDIR)/*.h)

# What object files we'll be making
OBJ = $(SRC:$(SRCDIR)/%.c=%.o)

# Now in the right directory!
OBJS = $(patsubst %,$(OBJDIR)/%,$(OBJ))

# What do we call the binary we output?
BINNAME = sim_computer


## Options
# All options should default to 0, turning them off.
#

# list of all command line options

# enables debug symbols, turns down optimizations
DEBUG ?= 0
# use the GCC instead of clang
CLANG ?= 0
# Disable all optimizations
NOOPT ?= 0
# Don't treat warnings as errors
NOWERROR ?= 0

# The effects of the command line options

ifneq ($(DEBUG), 0)
    C_FLAGS += -g
    OPT_LEVEL = -Og
endif

ifneq ($(NOOPT), 0)
    OPT_LEVEL = -O0
endif

ifeq ($(NOWERROR), 0)
    WARNINGS += -Werror
endif

ifneq ($(CLANG), 0)
    CC = clang
endif

# And finally, make sure the obj directory exists
$(shell mkdir -p obj)


## Targets
#
#

.PHONY: all
# Default target - build the program
all: $(OBJS)
	$(CC) -o $(BINNAME) $(C_VER) $(WARNINGS) $(C_FLAGS) $(OBJS) $(OPT_LEVEL) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< $(WARNINGS) $(C_VER) $(C_FLAGS) $(OPT_LEVEL) -o $@

.PHONY: format_check
format_check:
	clang-format --dry-run $(SRC) $(HEADERS)

.PHONY: clean
clean:
	rm $(OBJS)

.PHONY: format
format:
	clang-format -i $(SRC) $(HEADERS)


# The dependency files generated by the compiler
# These must be at the bottom
DEPS = $(OBJS:%.o=%.d)
# Recompile files based on them
-include $(DEPS)
