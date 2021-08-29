CC ?= gcc
CCOPTIONS = -W -Wall -g
PROJECTNAME = ircBot
SRC = example.c
SRC += ./irc/callbacks.c
SRC += ./irc/irc.c
INCLUDES = .
INCLUDES += ./irc/

OBJS = $(SRC:.c=.o)

INC = $(foreach d, $(INCLUDE),-I$d)

all: clean $(PROJECTNAME)

%.o: %.c
		$(CC) -c -o $(@) $(CCOPTIONS) $(INC) $<

$(PROJECTNAME): $(OBJS)
		$(CC) -o $(@F) $(OBJS) $(CCOPTIONS) $(INC)
clean:
	rm -f $(OBJS)
