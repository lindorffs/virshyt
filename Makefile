CC=gcc
LIBS=-lvirt -lncurses
INCLUDE_DIR=include/

SRC_DIR=src/
SRC=${SRC_DIR}main.c

OUT_DIR=build/
OUT=virshyt

main: ${SRC}
	${CC} $^ -o ${OUT_DIR}${OUT} -I${INCLUDE_DIR} ${LIBS}
	${CC} src/notifier.c -o ${OUT_DIR}notifier -I${INCLUDE_DIR}
clean: ${OUT_DIR}${OUT}
	rm $^
	rm ${OUT_DIR}notifier
