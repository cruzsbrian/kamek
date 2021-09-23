CPPFLAGS = -DPRINT_SEARCH_INFO
CXXFLAGS = -Wall -Ofast -flto -mbmi2
LDFLAGS = -Ofast -lfmt

vpath %.cpp src

OBJDIR = build

COMMON_SRCS = common.cpp cpu.cpp alphabeta.cpp endgame.cpp \
			  hashtable.cpp board.cpp pattern_eval.cpp
MAIN_SRCS = $(COMMON_SRCS) main.cpp

MAIN_OBJS = $(addprefix $(OBJDIR)/, $(MAIN_SRCS:.cpp=.o))


.PHONY: all
all: wonky_kong

wonky_kong: $(MAIN_OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm -rf wonky_kong $(OBJDIR)
