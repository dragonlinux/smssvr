#
#Project:		smssvr
#Version:		1.0
#Author:		serite.yan
#LastModifyDate:	04/29/2014

##################################################################

TOPDIR	=/root/smssvr
APRLIB	=$(TOPDIR)
INCLDIR =$(TOPDIR)/include
INCLAR	=$(TOPDIR)
EXEDIR	=$(TOPDIR)/bin
OBJDIR	=$(TOPDIR)/obj
BINDIR	=$(EXEDIR)

LIBDIR	=$(TOPDIR)/lib
ORALIB	=$(ORACLE_HOME)/lib/
ACELIB	=/root/ACE_wrappers/lib/

ORAFLAG=-lclntsh -ldl -lpthread -lnsl -lirc -lm
ACEFLAG=-lACE
MYSQLFLAG=-lmysqlclient

INCLUDE=-I$(ORACLE_HOME)/precomp/public \
	-I$(ORACLE_HOME)/rdbms/public/ \
	-I$(ORACLE_HOME)/rdbms/demo/ \
	-I$(ORACLE_HOME)/client/demo/ \
	-I$(ORACLE_HOME)/network/public \
	-I$(ORACLE_HOME)/plsql/public \
	-I/root/ACE_wrappers/ \
	-I$(INCLDIR)/

LIB	=-L$(LIBDIR) \
	-L$(ORALIB) \
	-L$(ACELIB)/

LINKFLAG =$(ORAFLAG) \
	$(ACEFLAG)

################################################################
CC	=g++
DEFS	= -D_DEBUG_
CFLAGS	=-g $(DEFS) $(INCLUDE) $(LIB) $(LINKFLAG)

${OBJ}:${SRC}
	${CC}${CFLAGS}-c${SRC}c
################################################################
CC=g++

MAINOBJ=smspda.o

SERVFLAGS= -O2 -DLINUX $(WARNINGS)

OBJSERVER= $(MAINOBJ) \
	GprsSms.o \
	ScanDbTask.o \
	DataAnalyzeTask.o \
	SmsPDU.o \
	iec_sys.o \
	iec_103.o

all:	t
#	mv smssvri ./bin
#	mv *.o ./obj

cl:
	ps aux|grep smssvr.exe|awk '{print $2}'|xargs kill -9

trans:
	scp /root/smssvr/bin/smssvr.exe root@192.168.11.65:/root/smssvr  


t: $(OBJSERVER)
	$(CC)  $(SERVFLAGS) -o smssvr $(OBJSERVER)  $(CFLAGS) ./lib/libgyapp.a


clean:
	rm -f *.a *.o *.bak *.log
	mv *.*~ ./bak 
	clear

smspda.o:	smspda.cpp
		$(CC) -c -o $@ $? $(LIB)  $(CFLAGS)


GprsSms.o:	GprsSms.cpp
		$(CC) -c -o $@ $? $(LIB)  $(CFLAGS)

ScanDbTask.o:	ScanDbTask.cpp
		$(CC) -c -o $@ $? $(LIB)  $(CFLAGS)

DataAnalyzeTask.o:	DataAnalyzeTask.cpp
		$(CC) -c -o $@ $? $(LIB)  $(CFLAGS)

SmsPDU.o:	SmsPDU.cpp
		$(CC) -c -o $@ $? $(LIB)  $(CFLAGS)

iec_103.o:	iec_103.cpp
		$(CC) -c -o $@ $? $(LIB)  $(CFLAGS)

iec_sys.o:	iec_sys.cpp
		$(CC) -c -o $@ $? $(LIB)  $(CFLAGS)


###########for test every task main ###########
test_log.o:	test_log.cpp
		$(CC) -c -o $@ $? $(LIB)  $(CFLAGS)
test_db.o:	test_db.cpp
		$(CC) -c -o $@ $? $(LIB)  $(CFLAGS)
