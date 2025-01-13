# Compiler and flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# Cleaning commands

DELFILES = rm -rf

# Directories
SRCDIR = src
INCDIR = inc
BINDIR = bin
OBJDIR = obj

# Target executable
TARGET = webserv

# Source and object files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

# Include directories
INCLUDES = -I$(INCDIR)

# Default rule
all: $(BINDIR)/$(TARGET)

# Linking
$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# Compilation
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Create directories if they don't exist
$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean up
clean:
	${DELFILES} $(OBJDIR)

fclean:
	${DELFILES} $(OBJDIR) $(BINDIR)/$(TARGET)

re:	fclean
	$(MAKE) all

# Phony targets
.PHONY: all clean re fclean