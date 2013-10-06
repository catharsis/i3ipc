TARGET=libi3ipc.so
SRCS=libi3ipc.c
OBJS=${SRCS:.c=.o}
CCFLAGS=-std=gnu99 -O0 -Wall -Werror -ggdb -fPIC

.PHONY: all clean
all:: ${TARGET}

${TARGET}: ${OBJS}
	${CC} -shared -o $@ $^
${OBJS}: %.o: %.c
	${CC} ${CCFLAGS} -o $@ -c $<

clean::
	-rm -f *~ *.o ${TARGET}
