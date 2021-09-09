CC = g++
CXXFLAGS = -Wall -Ofast -flto -lpthread -mbmi2 -DPRINT_SEARCH_INFO
LDFLAGS = -Ofast -lpthread -lfmt

SRCDIR = src
OBJDIR = obj

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
CS2_SRCS = cs2.cpp

BOARD_EVAL_SRCS = train/board_eval.cpp
COMPEV_SRCS = compare_eval.cpp
TESTPOS_SRCS = test_position.cpp

TEST_SRCS = test/test.cpp
EGTEST_SRCS = eg_test.cpp
PERFT_SRCS = perft.cpp board/board.cpp

COMMON_OBJS = $(COMMON_SRCS:.cpp=.o)
MAIN_OBJS = $(COMMON_OBJS) $(MAIN_SRCS:.cpp=.o)
CLI_OBJS = $(COMMON_OBJS) $(CLI_SRCS:.cpp=.o)
CS2_OBJS = $(COMMON_OBJS) $(CS2_SRCS:.cpp=.o)
BOARD_EVAL_OBJS = $(COMMON_OBJS) $(BOARD_EVAL_SRCS:.cpp=.o)
COMPEV_OBJS = $(COMMON_OBJS) $(COMPEV_SRCS:.cpp=.o)
TESTPOS_OBJS = $(COMMON_OBJS) $(TESTPOS_SRCS:.cpp=.o)
TEST_OBJS = $(COMMON_OBJS) $(TEST_SRCS:.cpp=.o)
EGTEST_OBJS = $(COMMON_OBJS) $(EGTEST_SRCS:.cpp=.o)
PERFT_OBJS = $(PERFT_SRCS:.cpp=.o)


all: wonky_kong cs2 cli train

wonky_kong: $(addprefix $(OBJDIR)/, $(MAIN_OBJS))
	$(CC) $^ $(LDFLAGS) -o $@

cli: $(addprefix $(OBJDIR)/, $(CLI_OBJS))
	$(CC) $^ $(LDFLAGS) -o $@

cs2: $(addprefix $(OBJDIR)/, $(CS2_OBJS))
	$(CC) $^ $(LDFLAGS) -o $@

train: board_eval

test: $(addprefix $(OBJDIR)/, $(TEST_OBJS))
	$(CC) $^ $(LDFLAGS) -o $@

board_eval: $(addprefix $(OBJDIR)/, $(BOARD_EVAL_OBJS))
	$(CC) $^ $(LDFLAGS) -o $@

selfplay: $(addprefix $(OBJDIR)/, $(SELFPLAY_OBJS))
	$(CC) $^ $(LDFLAGS) -o $@

perft: $(addprefix $(OBJDIR)/, $(PERFT_OBJS))
	$(CC) $^ $(LDFLAGS) -o $@

eg_test: $(addprefix $(OBJDIR)/, $(EGTEST_OBJS))
	$(CC) $^ $(LDFLAGS) -o $@

test_position: $(addprefix $(OBJDIR)/, $(TESTPOS_OBJS))
	$(CC) $^ $(LDFLAGS) -o $@

compare_eval: $(addprefix $(OBJDIR)/, $(COMPEV_OBJS))
	$(CC) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/$(notdir %.cpp)
	mkdir -p $(dir $@)
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf wonky_kong cs2 cli board_eval $(OBJDIR)/*
