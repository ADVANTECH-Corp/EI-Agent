AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Platform -I$(top_srcdir)/Platform/Linux/ -I$(top_srcdir)/Include -I$(top_srcdir)/Library/cJSON -I$(top_srcdir)/Library/Log
lib_LTLIBRARIES = libSAManager.la
libSAManager_la_SOURCES = ghloader.c hlloader.c SAManager.c  pktparser.c topic.c keepalive.c ghloader.h hlloader.h SAManager.h  pktparser.h topic.h keepalive.h SAManagerLog.h SADataSync.c SADataSync.h reloader.c reloader.h
libSAManager_la_LDFLAGS = -L$(top_srcdir)/Platform -lWISEPlatform -lWISEUtil -L$(top_srcdir)/Library/cJSON -L$(top_srcdir)/Library/Log -lcJSON -lLog -lpthread -ldl -release 2.0.$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
include_HEADERS = $(top_srcdir)/Library/SAManager/SAManager.h
