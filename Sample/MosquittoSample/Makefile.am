AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Platform -I$(top_srcdir)/Platform/Linux/ -I$(top_srcdir)/Include
bin_PROGRAMS = MosquittoSample
MosquittoSample_SOURCES = MosquittoSample.c
MosquittoSample_LDFLAGS = -L$(top_srcdir)/Platform -lWISEPlatform -lWISEUtil -lmosquitto -lpthread -ldl -Wl,-rpath,./ 
#LDADD = $(INTLLIBS)
