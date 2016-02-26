CFLAGS = -Wall -g -std=c99
LDFLAGS = -lreadline

OBJS = main.o

all: shell

shell: $(OBJS)
	$(CC) $(CFLAGS) -o shell $(OBJS) $(LDFLAGS)
clean: 
	rm -rf $(OBJS) shell
