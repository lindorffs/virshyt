CC=gcc
LIBS=-lvirt -lpcap
INCLUDE_DIR=include/

SRC_DIR=src/
SRC=${SRC_DIR}main.c

OUT_DIR=build/
OUT=virtman

main: ${SRC}
	${CC} $^ -o ${OUT_DIR}${OUT} -I${INCLUDE_DIR} ${LIBS}
	${CC} -I${INCLUDE_DIR} ${SRC_DIR}notifier.c -o ${OUT_DIR}notifier
clean: ${OUT_DIR}${OUT}
	rm $^
	rm ${OUT_DIR}notifier
	rm ${INCLUDE_DIR}*.gch

