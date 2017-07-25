# ------------------------------------------------
# myAIapp Makefile
#
# Author: vaibhav.dhingani
#
# ------------------------------------------------

# project name (generate executable with this name)
TARGET   = myAIapp

CC       = g++
# compiling flags here
#CFLAGS   = -Wall -I.
CFLAGS   = -I.

LINKER   = g++ -o
# linking flags here
LFLAGS   = -Wall -lsqlite3 -I. -lm

# change these to set the proper directories where each files shoould be
SRCDIR   = ../src
OBJDIR   = ../obj
BINDIR   = ../bin
INCDIR   = ../inc

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(INCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm       = rm -f

all: remove clean $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS) -lsqlite3
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
