CC = g++
CFLAGS = -Wall -flto -Ofast -lpthread -mbmi2 -DPRINT_SEARCH_INFO
LDFLAGS = -Ofast -lpthread

SRCDIR = src
OBJDIR = obj
BINDIR = bin

BOARD_SRCS = board.cpp
EVAL_SRCS = pattern_eval.cpp
SEARCH_SRCS = basic.cpp endgame.cpp
GAME_SRCS = game.cpp cpu.cpp
UTIL_SRCS = util.cpp
COMMON_SRCS = $(addprefix board/, $(BOARD_SRCS)) \
			  $(addprefix eval/, $(EVAL_SRCS)) \
			  $(addprefix search/, $(SEARCH_SRCS)) \
			  $(addprefix game/, $(GAME_SRCS)) \
			  $(UTIL_SRCS) \

MAIN_SRCS = protocol.cpp
CLI_SRCS = cli.cpp game/human_player.cpp

BOARD_EVAL_SRCS = train/board_eval.cpp
SELFPLAY_SRCS = train/selfplay.cpp

COMMON_OBJS = $(COMMON_SRCS:.cpp=.o)
MAIN_OBJS = $(COMMON_OBJS) $(MAIN_SRCS:.cpp=.o)
CLI_OBJS = $(COMMON_OBJS) $(CLI_SRCS:.cpp=.o)
BOARD_EVAL_OBJS = $(COMMON_OBJS) $(BOARD_EVAL_SRCS:.cpp=.o)
SELFPLAY_OBJS = $(COMMON_OBJS) $(SELFPLAY_SRCS:.cpp=.o)


all: wonky_kong cli train

wonky_kong: $(addprefix $(OBJDIR)/, $(MAIN_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

cli: $(addprefix $(OBJDIR)/, $(CLI_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

train: board_eval selfplay

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

test_eval: $(addprefix $(OBJDIR)/, $(TESTEV_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/$(notdir %.cpp)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BINDIR)/* $(OBJDIR)/*
