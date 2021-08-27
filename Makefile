CC = g++
CFLAGS = -Wall -Ofast -flto -lpthread -mbmi2 -DPRINT_SEARCH_INFO
LDFLAGS = -Ofast -lpthread -lfmt

SRCDIR = src
OBJDIR = obj
BINDIR = bin

BOARD_SRCS = board.cpp
EVAL_SRCS = pattern_eval.cpp
SEARCH_SRCS = alphabeta.cpp endgame.cpp hashtable.cpp
GAME_SRCS = cpu.cpp
UTIL_SRCS = util.cpp
COMMON_SRCS = $(addprefix board/, $(BOARD_SRCS)) \
			  $(addprefix eval/, $(EVAL_SRCS)) \
			  $(addprefix search/, $(SEARCH_SRCS)) \
			  $(addprefix game/, $(GAME_SRCS)) \
			  $(UTIL_SRCS) \

MAIN_SRCS = protocol.cpp
CLI_SRCS = cli.cpp

BOARD_EVAL_SRCS = train/board_eval.cpp
SELFPLAY_SRCS = train/selfplay.cpp
COMPEV_SRCS = compare_eval.cpp
TESTPOS_SRCS = test_position.cpp

TEST_SRCS = test/test.cpp

COMMON_OBJS = $(COMMON_SRCS:.cpp=.o)
MAIN_OBJS = $(COMMON_OBJS) $(MAIN_SRCS:.cpp=.o)
CLI_OBJS = $(COMMON_OBJS) $(CLI_SRCS:.cpp=.o)
BOARD_EVAL_OBJS = $(COMMON_OBJS) $(BOARD_EVAL_SRCS:.cpp=.o)
SELFPLAY_OBJS = $(COMMON_OBJS) $(SELFPLAY_SRCS:.cpp=.o)
COMPEV_OBJS = $(COMMON_OBJS) $(COMPEV_SRCS:.cpp=.o)
TESTPOS_OBJS = $(COMMON_OBJS) $(TESTPOS_SRCS:.cpp=.o)
TEST_OBJS = $(COMMON_OBJS) $(TEST_SRCS:.cpp=.o)


all: wonky_kong cli train

wonky_kong: $(addprefix $(OBJDIR)/, $(MAIN_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

cli: $(addprefix $(OBJDIR)/, $(CLI_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

train: board_eval

test: $(addprefix $(OBJDIR)/, $(TEST_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

board_eval: $(addprefix $(OBJDIR)/, $(BOARD_EVAL_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

selfplay: $(addprefix $(OBJDIR)/, $(SELFPLAY_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

perft: $(addprefix $(OBJDIR)/, $(PERFT_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

eg_test: $(addprefix $(OBJDIR)/, $(EGTEST_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

test_position: $(addprefix $(OBJDIR)/, $(TESTPOS_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

compare_eval: $(addprefix $(OBJDIR)/, $(COMPEV_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/$(notdir %.cpp)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BINDIR)/* $(OBJDIR)/*
