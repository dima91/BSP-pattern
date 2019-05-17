.PHONY = clean

BIN_DIR	= bin
CC		= g++
CFLAGS	= -I include -lpthread -Wall -pedantic -O3



${BIN_DIR}/Tiskin : src/tiskin.cpp include/bsp.hpp include/concurrentQueue.hpp
	-mkdir ${BIN_DIR}
	${CC} -o $@ $< ${CFLAGS}


${BIN_DIR}/QueueTester : src/queueTester.cpp include/concurrentQueue.hpp
	-mkdir ${BIN_DIR}
	${CC} -o $@ $< ${CFLAGS}


clean :
	-rm -fr ${BIN_DIR}