TARGET=i3ipc
SRCS=i3ipc.c main.c
OBJS=${SRCS:.c=.o}
CCFLAGS=-std=gnu99 -O0 -Wall -Werror -ggdb
LDFLAGS=-L./lib -li3ipc
LIBDIR=lib
LIBS=${LIBDIR}/libi3ipc.so

.PHONY: all clean
all:: ${LIBS} ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ $^

${LIBS}:
	${MAKE} -C ${LIBDIR} all
${OBJS}: %.o: %.c
	${CC} ${CCFLAGS} -o $@ -c $<

test: all
	LD_LIBRARY_PATH=./lib ./i3ipc

test-valgrind: all
	LD_LIBRARY_PATH=./${LIBDIR} valgrind ./i3ipc
clean:
	-rm -f *~ *.o ${TARGET}
	${MAKE} -C ${LIBDIR} clean
