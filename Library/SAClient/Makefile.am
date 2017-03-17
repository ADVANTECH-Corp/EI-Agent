ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Library/Log -I$(top_srcdir)/Platform -I$(top_srcdir)/Platform/Linux/ -I$(top_srcdir)/Include -I$(top_srcdir)/Library/WISECore -I$(top_srcdir)/Library/cJSON -I$(top_srcdir)/Library/Base64 -I$(top_srcdir)/Library/DES
lib_LTLIBRARIES = libSAClient.la
libSAClient_la_SOURCES = topic.c eventqueue.c msgqueue.c smloader.c scparser.c SAClient.c
libSAClient_la_LDFLAGS = -L$(top_srcdir)/Library/Log -lLog -L$(top_srcdir)/Platform -lWISEPlatform -lWISEUtil -L$(top_srcdir)/Library/WISECore -lWISECore -L$(top_srcdir)/Library/Base64 -lBase64 -L$(top_srcdir)/Library/DES -lDES -L$(top_srcdir)/Library/cJSON -lcJSON -lpthread -ldl -release 2.0.$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
include_HEADERS = $(top_srcdir)/Library/SAClient/SAClient.h
# LDADD = -lstdc++ $(INTLLIBS)
