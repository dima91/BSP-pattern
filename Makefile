.PHONY = clean

BIN_DIR	= bin
OBJ_DIR	= obj
CC		= g++
CFLAGS	= -I include -lpthread -Wall -pedantic -O3 -std=c++11
TESTS	= ${BIN_DIR}/QueueTester ${BIN_DIR}/BarrierTester ${BIN_DIR}/WorkerTester ${BIN_DIR}/Tiskin ${BIN_DIR}/LockableVectorTester ${BIN_DIR}/SuperstepTester ${BIN_DIR}/BspTester

all :
	-mkdir ${BIN_DIR}
	-mkdir ${OBJ_DIR}
	make ${TESTS}




${BIN_DIR}/Tiskin : src/tiskin.cpp include/bsp.hpp include/concurrentQueue.hpp
	${CC} -o $@ $< ${CFLAGS}


${OBJ_DIR}/barrier.o : src/barrier.cpp include/barrier.hpp
	${CC} -c -o $@ $< ${CFLAGS}


${OBJ_DIR}/workerThread.o : src/workerThread.cpp include/workerThread.hpp
	${CC} -c -o $@ $< ${CFLAGS}



###########
## TESTS ##

${BIN_DIR}/QueueTester : tests/queueTester.cpp include/concurrentQueue.hpp
	${CC} -o $@ $< ${CFLAGS}


${BIN_DIR}/BarrierTester : tests/barrierTester.cpp ${OBJ_DIR}/barrier.o
	${CC} $< -o $@ ${CFLAGS} ${OBJ_DIR}/barrier.o


${BIN_DIR}/WorkerTester : tests/workerTester.cpp ${OBJ_DIR}/workerThread.o ${OBJ_DIR}/barrier.o
	${CC} $< -o $@ ${CFLAGS} ${OBJ_DIR}/barrier.o ${OBJ_DIR}/workerThread.o


${BIN_DIR}/SuperstepTester : tests/superstepTester.cpp include/superstep.hpp ${OBJ_DIR}/workerThread.o ${OBJ_DIR}/barrier.o
	${CC} $< -o $@ ${OBJ_DIR}/workerThread.o ${CFLAGS} ${OBJ_DIR}/barrier.o


${BIN_DIR}/BspTester : tests/bspTester.cpp include/bsp.hpp include/superstep.hpp ${OBJ_DIR}/workerThread.o ${OBJ_DIR}/barrier.o
	${CC} $< -o $@ ${OBJ_DIR}/workerThread.o ${CFLAGS} ${OBJ_DIR}/barrier.o


${BIN_DIR}/LockableVectorTester : tests/lockableVectorTester.cpp include/lockableVector.hpp
	${CC} -o $@ $< ${CFLAGS}


clean :
	-rm -fr ${BIN_DIR}
	-rm -fr ${OBJ_DIR}