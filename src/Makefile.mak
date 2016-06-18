CPP    = g++
WINDRES= windres
RM     = rm -f
OBJS   = ibpp/array.o \
         ibpp/blob.o \
         ibpp/database.o \
         ibpp/date.o \
         ibpp/dbkey.o \
         ibpp/events.o \
         ibpp/exception.o \
         ibpp/row.o \
         ibpp/service.o \
         ibpp/statement.o \
         ibpp/time.o \
         ibpp/transaction.o \
         ibpp/user.o \
         ibpp/_dpb.o \
         ibpp/_ibpp.o \
         ibpp/_ibs.o \
         ibpp/_rb.o \
         ibpp/_spb.o \
         ibpp/_tpb.o \
         main.o \
         AppResource.res

LIBS   =
CFLAGS = -DIBPP_WINDOWS -D_WIN32_WINNT=0x0502 -Iibpp -fno-diagnostics-show-option

.PHONY: ../bin/fdb2sql.exe clean

all: ../bin/fdb2sql.exe

clean:
	$(RM) $(OBJS) ../bin/fdb2sql.exe

../bin/fdb2sql.exe: $(OBJS)
	$(CPP) -Wall -s -O2 -o $@ $(OBJS) $(LIBS)

ibpp/array.o: ibpp/array.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/blob.o: ibpp/blob.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/database.o: ibpp/database.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/date.o: ibpp/date.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/dbkey.o: ibpp/dbkey.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/events.o: ibpp/events.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/exception.o: ibpp/exception.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/row.o: ibpp/row.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/service.o: ibpp/service.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/statement.o: ibpp/statement.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/time.o: ibpp/time.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/transaction.o: ibpp/transaction.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/user.o: ibpp/user.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/_dpb.o: ibpp/_dpb.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/_ibpp.o: ibpp/_ibpp.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/_ibs.o: ibpp/_ibs.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/_rb.o: ibpp/_rb.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/_spb.o: ibpp/_spb.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

ibpp/_tpb.o: ibpp/_tpb.cpp ibpp/_ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

main.o: main.cpp ibpp/ibpp.h
	$(CPP) -Wall -s -O2 -c $< -o $@ $(CFLAGS)

AppResource.res: AppResource.rc
	$(WINDRES) -i AppResource.rc -J rc -o AppResource.res -O coff

