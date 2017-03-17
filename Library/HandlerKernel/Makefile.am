AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Platform -I$(top_srcdir)/Platform/Linux/ -I$(top_srcdir)/Library/MessageGenerator -I$(top_srcdir)/Library/Log -I$(top_srcdir)/Library/cJSON -I$(top_srcdir)/Include
lib_LTLIBRARIES = libHandlerKernel.la
libHandlerKernel_la_SOURCES = HandlerKernelEx.c HandlerKernel.c HandlerParser.c HandlerThreshold.c triggerqueue.c
libHandlerKernel_la_LDFLAGS = -L$(top_srcdir)/Library/MessageGenerator -lmsggen -L$(top_srcdir)/Library/Log -lLog -L$(top_srcdir)/Library/cJSON -lcJSON -L$(top_srcdir)/Platform -lWISEPlatform -lWISEUtil  -lpthread -ldl -release $(PACKAGE_VERSION_MAJOR).$(PACKAGE_VERSION_MINOR).$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
include_HEADERS = $(top_srcdir)/Library/HandlerKernel/HandlerKernel.h $(top_srcdir)/Library/HandlerKernel/HandlerKernelEx.h
LDADD = $(INTLLIBS)
