# Compiler
CC = g++ -std=c++17

# Source files
SRCS = IgnoreComments.cpp Tokenization.cpp RecursiveDescentParser.cpp symboltable.cpp main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable file
MAIN = symboltable

# Build rule for the executable
$(MAIN): $(OBJS)
	$(CC) -o $(MAIN) $(OBJS)

# Rule for building .o files from .cpp files
.cpp.o:
	$(CC) -c $< -o $@

# Clean rule
clean:
	$(RM) *.o *~ $(MAIN) 

# Dependency generation rule
depend: $(SRCS)
	makedepend $^

# DO NOT DELETE THIS LINE -- make depend needs it
