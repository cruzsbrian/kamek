CC = g++
CFLAGS = -Wall -flto -Ofast -lpthread -march=native
LDFLAGS = -Ofast -lpthread -march=native

SRCDIR = src
OBJDIR = obj
BINDIR = bin

BOARD_SRCS = board.cpp
EVAL_SRCS =
SEARCH_SRCS = endgame.cpp
GAME_SRCS = game.cpp
UTIL_SRCS = util.cpp
COMMON_SRCS = $(addprefix board/, $(BOARD_SRCS)) \
			  $(addprefix eval/, $(EVAL_SRCS)) \
			  $(addprefix search/, $(SEARCH_SRCS)) \
			  $(addprefix game/, $(GAME_SRCS)) \
			  $(UTIL_SRCS) \

MAIN_SRCS = protocol.cpp
CLI_SRCS = cli.cpp game/human_player.cpp
TRAIN_SRCS = train.cpp
PERFT_SRCS = perft.cpp board/board.cpp
EGTEST_SRCS = eg_test.cpp board/board.cpp search/endgame.cpp

COMMON_OBJS = $(COMMON_SRCS:.cpp=.o)
MAIN_OBJS = $(COMMON_OBJS) $(MAIN_SRCS:.cpp=.o)
CLI_OBJS = $(COMMON_OBJS) $(CLI_SRCS:.cpp=.o)
TRAIN_OBJS = $(COMMON_OBJS) $(TRAIN_SRCS:.cpp=.o)
PERFT_OBJS = board/board.o perft.o
EGTEST_OBJS = $(EGTEST_SRCS:.cpp=.o)


all: wonky_kong wonky_cli wonky_train perft eg_test

wonky_kong: $(addprefix $(OBJDIR)/, $(MAIN_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

wonky_cli: $(addprefix $(OBJDIR)/, $(CLI_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

wonky_train: $(addprefix $(OBJDIR)/, $(TRAIN_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

perft: $(addprefix $(OBJDIR)/, $(PERFT_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

eg_test: $(addprefix $(OBJDIR)/, $(EGTEST_OBJS))
	mkdir -p bin
	$(CC) $^ $(LDFLAGS) -o $(BINDIR)/$@

$(OBJDIR)/%.o: $(SRCDIR)/$(notdir %.cpp)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BINDIR)/* $(OBJDIR)/*
