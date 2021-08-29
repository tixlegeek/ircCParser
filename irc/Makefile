CC ?= gcc
CCOPTIONS = -W -Wall
SRC = irc.c callbacks.c
INCLUDES = .
OBJS = $(SRC:.c=.o)

all: clean irc

%.o: %.c
	$(CC) -c -o $(@F) $(CCOPTIONS) -I $(INCLUDES) $<

irc: $(OBJS)

clean:
	rm -f $(OBJS)
