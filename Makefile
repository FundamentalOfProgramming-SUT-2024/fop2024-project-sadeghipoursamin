CC = gcc
INCLUDES = -I/opt/homebrew/include/SDL2 -I/opt/homebrew/include
LIBS = -L/opt/homebrew/lib -lSDL2 -lSDL2_mixer -lncurses

SRCS = menu.c audio.c roguegame.c
OBJS = $(SRCS:.c=.o)
TARGET = game

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) -c $< $(INCLUDES)

clean:
	rm -f $(OBJS) $(TARGET)
