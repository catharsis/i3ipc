TARGET=i3ipc
SRCS=i3ipc.c main.c
OBJS=${SRCS:.c=.o}
CCFLAGS=-std=gnu99 -O2 -Wall -Werror -ggdb

.PHONY: all clean
all:: ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ $^

${OBJS}: %.o: %.c
	${CC} ${CCFLAGS} -o $@ -c $<

clean::
	-rm -f *~ *.o ${TARGET}
