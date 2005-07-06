CC=g++
CFLAGS=-s -Wall
#CFLAGS=-s -Wstrict-prototypes -O2 -fomit-frame-pointer
WXFLAGS=-D__WXGTK__ -g `wx-config --cxxflags`
LIBS=-lwx_gtk-2.4

OBJS=duplicatex.o minerdlg.o picfidlg.o picframe.o metfile.o metfile_tag.o \
	wxthread.o md4hash.o database.o

PROG=duplicatex

all: $(PROG) $(OBJS)

duplicatex.o: duplicatex.cpp
	$(CC) $(CFLAGS) $(WXFLAGS) -c -o $@ $<
minerdlg.o: minerdlg.cpp
	$(CC) $(CFLAGS) $(WXFLAGS) -c -o $@ $<
picfidlg.o: picfidlg.cpp
	$(CC) $(CFLAGS) $(WXFLAGS) -c -o $@ $<
picframe.o: picframe.cpp
	$(CC) $(CFLAGS) $(WXFLAGS) -c -o $@ $<
metfile.o: metfile.cpp
	$(CC) $(CFLAGS) $(WXFLAGS) -c -o $@ $<
metfile_tag.o: metfile_tag.cpp
	$(CC) $(CFLAGS) $(WXFLAGS) -c -o $@ $<
wxthread.o: wxthread.cpp
	$(CC) $(CFLAGS) $(WXFLAGS) -c -o $@ $<
md4hash.o: md4hash.cpp
	$(CC) $(CFLAGS) $(WXFLAGS) -c -o $@ $<
database.o: database.cpp
	$(CC) $(CFLAGS) $(WXFLAGS) -c -o $@ $<

duplicatex: $(OBJS)
	$(CC) $(CFLAGS) $(WXFLAGS) -o $@ $(LIBS) $(OBJS)
clean:
	rm -f $(PROG) $(OBJS)
	
