CC = g++
CFLAGS = -Wall -flto -Ofast -lpthread
LDFLAGS = -Ofast -lpthread

SRCDIR = src
OBJDIR = obj
BINDIR = bin

BOARD_SRCS = board.cpp
EVAL_SRCS = simple_eval.cpp
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
TRAIN_SRCS = train.cpp
SELFPLAY_SRCS = selfplay.cpp
PERFT_SRCS = perft.cpp board/board.cpp
EGTEST_SRCS = eg_test.cpp
TESTPOS_SRCS = test_position.cpp
TESTEV_SRCS = test_eval.cpp

COMMON_OBJS = $(COMMON_SRCS:.cpp=.o)
MAIN_OBJS = $(COMMON_OBJS) $(MAIN_SRCS:.cpp=.o)
CLI_OBJS = $(COMMON_OBJS) $(CLI_SRCS:.cpp=.o)
TRAIN_OBJS = $(COMMON_OBJS) $(TRAIN_SRCS:.cpp=.o)
SELFPLAY_OBJS = $(COMMON_OBJS) $(SELFPLAY_SRCS:.cpp=.o)
PERFT_OBJS = board/board.o perft.o
EGTEST_OBJS = $(COMMON_OBJS) $(EGTEST_SRCS:.cpp=.o)
TESTPOS_OBJS = $(COMMON_OBJS) $(TESTPOS_SRCS:.cpp=.o)
TESTEV_OBJS = $(COMMON_OBJS) $(TESTEV_SRCS:.cpp=.o)


all: wonky_kong cli train perft eg_test

wonky_kong: $(addprefix $(OBJDIR)/, $(MAIN_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

cli: $(addprefix $(OBJDIR)/, $(CLI_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

train: $(addprefix $(OBJDIR)/, $(TRAIN_OBJS))
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
