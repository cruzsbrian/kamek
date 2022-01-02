CPPFLAGS += -march=native
CXXFLAGS = -Wall -g -O3 -flto -mbmi2
LDFLAGS = -g -O3 -lfmt

vpath %.cpp src

OBJDIR = build

COMMON_SRCS = common.cpp cpu.cpp alphabeta.cpp endgame.cpp \
			  hashtable.cpp board.cpp pattern_eval.cpp
MAIN_SRCS = $(COMMON_SRCS) main.cpp
EG_TEST_SRCS = $(COMMON_SRCS) eg_test.cpp

MAIN_OBJS = $(addprefix $(OBJDIR)/, $(MAIN_SRCS:.cpp=.o))
EG_TEST_OBJS = $(addprefix $(OBJDIR)/, $(EG_TEST_SRCS:.cpp=.o))


.PHONY: all
all: wonky_kong eg_test

wonky_kong: $(MAIN_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

eg_test: $(EG_TEST_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm -rf wonky_kong $(OBJDIR)
