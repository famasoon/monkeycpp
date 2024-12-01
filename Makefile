CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.
TARGET = monkey
SRCS = main.cpp \
       token/token.cpp \
       lexer/lexer.cpp \
       repl/repl.cpp

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

-include $(DEPS)
