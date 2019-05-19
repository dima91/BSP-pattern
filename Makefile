.PHONY = clean

BIN_DIR	= bin
OBJ_DIR	= obj
CC		= g++
CFLAGS	= -I include -lpthread -Wall -pedantic -O3



${BIN_DIR}/Tiskin : src/tiskin.cpp include/bsp.hpp include/concurrentQueue.hpp
	${CC} -o $@ $< ${CFLAGS}


${OBJ_DIR}/barrier.o : src/barrier.cpp include/barrier.hpp
	${CC} -c -o $@ $< ${CFLAGS}



###########
## TESTS ##

${BIN_DIR}/QueueTester : tests/queueTester.cpp include/concurrentQueue.hpp
	${CC} -o $@ $< ${CFLAGS}


${BIN_DIR}/BarrierTester : tests/barrierTester.cpp ${OBJ_DIR}/barrier.o
	${CC} $< ${CFLAGS} -o $@ ${OBJ_DIR}/barrier.o


${BIN_DIR}/WorkerTester : tests/workerTester.cpp include/workerThread.hpp include/barrier.hpp
	${CC} -o $@ $< ${CFLAGS} ${OBJ_DIR}/barrier.o


clean :
	-rm -fr ${BIN_DIR}