#Use machine's CPU-SPEED, else results will be wrong.
#example CYCLE_PER_US=2400L - 2.4 GHz processor

CYCLE_PER_US=2400L
ifndef CYCLE_PER_US
$(error CYCLE_PER_US not set. Set CYCLE_PER_US according to your machine CPU-SPEED)
endif

CC = gcc
GCCVERSIONGTEQ5 := $(shell expr `gcc -dumpversion | cut -f1 -d.` \>= 5)

# u-SCL is not tested with -O3 flag for gcc version above 5 yet.
ifeq "$(GCCVERSIONGTEQ5)" "1"
    OFLAG=-O0
else
    OFLAG=-O0
endif

FLAGS=-g -lpthread -Wall ${OFLAG} -DCYCLE_PER_US=${CYCLE_PER_US} -fno-omit-frame-pointer
TESTFLAG=-Ilocks/

AUXFILES=utils.o
DPDFLAG=-g ${TESTFLAG}
# AUXFLAGS=-lpthread -I./locks

CPTKTFLAGS=-DCPTKT cptltkt.o
CTKTTFLAGS=-DCTTKT ctkttkt.o

# having node passed to lock
HMCSFLAGS=-DHMCS hmcs.o -DHNODE
HTEPFLFLAGS=-DHTEPFL htlockepfl.o -DHNODE
HYMCSFLAGS=-DHYMCS hyshmcs.o -DHNODE

WAITINGPOLICY=-DWAITING_ORIGINAL

locks/topology.h: locks/topology.in
	# take from litl
	cat $< | sed -e "s/@nodes@/$$(numactl -H | head -1 | cut -f 2 -d' ')/g" > $@
	sed -i "s/@cpus@/$$(nproc)/g" $@
	sed -i "s/@cachelinesize@/128/g" $@  # 128 bytes is advised by intel documentation to avoid false-sharing with the HW prefetcher
	sed -i "s/@pagesize@/$$(getconf PAGESIZE)/g" $@
	sed -i 's#@cpufreq@#'$$(cat /proc/cpuinfo | grep MHz | head -1 | awk '{ x = $$4/1000; printf("%0.2g", x); }')'#g' $@
	chmod a+x $@


%.o:locks/%.c
	gcc $< -c ${DPDFLAG} ${WAITINGPOLICY} ${FLAGS} $(DEPTHFLAG) $(SLICETIME)

cptkt: ${AUXFILES} cptltkt.o
	gcc main.c ${CPTKTFLAGS} ${AUXFILES} -o cptkt ${FLAGS} ${TESTFLAG}

cttkt: ${AUXFILES} ctkttkt.o
	gcc main.c ${CTKTTFLAGS} ${AUXFILES} -o cttkt ${FLAGS} ${TESTFLAG}

hmcs: ${AUXFILES} hmcs.o
	gcc main.c ${HMCSFLAGS} ${AUXFILES} -o hmcs ${FLAGS} ${TESTFLAG}

htepfl: ${AUXFILES} htlockepfl.o
	gcc main.c ${HTEPFLFLAGS} ${AUXFILES} -o htepfl ${FLAGS} ${TESTFLAG}

hymcs: ${AUXFILES} hyshmcs.o
	gcc main.c ${HYMCSFLAGS} ${AUXFILES} -o hymcs ${FLAGS} ${TESTFLAG}

clean:
	rm -f hymcs hmcs htepfl cptkt cttkt *.o locks/*.o
#  locks/topology.h
