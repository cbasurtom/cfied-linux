CC       = gcc
CFLAGS   = -Wall -g -std=gnu99
LD       = gcc
LDFLAGS  = -L.
TARGETS  = findit moveit timeit nmapit curlit

all:		$(TARGETS)

#-------------------------------------------------------------------------------
# Object files
#-------------------------------------------------------------------------------

list.o: list.c findit.h
	$(CC) $(CFLAGS) -c -o $@ $<

filter.o: filter.c findit.h
	$(CC) $(CFLAGS) -c -o $@ $<

findit.o: findit.c findit.h
	$(CC) $(CFLAGS) -c -o $@ $<

socket.o: socket.c socket.h
	$(CC) $(CFLAGS) -c -o $@ $<

nmapit.o: nmapit.c socket.h
	$(CC) $(CFLAGS) -c -o $@ $<

curlit.o: curlit.c socket.h
	$(CC) $(CLFAGS) -c -o $@ $< 

#-------------------------------------------------------------------------------
# Executables
#-------------------------------------------------------------------------------

findit: findit.o list.o filter.o
	$(LD) $(LDFLAGS) -o $@ $^

moveit: moveit.c
	$(CC) $(CLFAGS) -o $@ $^

timeit: timeit.c
	$(CC) $(CLFAGS) -o $@ $^

nmapit: nmapit.o socket.o
	$(LD) $(LDFLAGS) -o $@ $^

curlit: curlit.o socket.o
	$(LD) $(LDFLAGS) -o $@ $^

#-------------------------------------------------------------------------------
# Others
#-------------------------------------------------------------------------------

clean:
	@rm -f $(TARGETS) *.o
