CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g -Wall
LDFLAGS=-g
LDLIBS=

# I know this is supposed to have no extension, but I want this to be in the gitignore :grin:
APPNAME=proj2.out

SRCS=proj2.cpp algorithms.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(APPNAME)

$(APPNAME): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(APPNAME) $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^ >> ./.depend

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend
	$(RM) $(APPNAME)

include .depend
