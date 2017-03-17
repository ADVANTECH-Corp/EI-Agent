AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Platform -I$(top_srcdir)/Platform/Linux/ -I$(top_srcdir)/Include -I$(top_srcdir)/Library/Log $(XML_CFLAGS)
lib_LTLIBRARIES = libSAHandlerLoader.la
libSAHandlerLoader_la_SOURCES = loaderlog.h moduleconfig.c moduleconfig.h SALoader.c SALoader.h
libSAHandlerLoader_la_LDFLAGS = $(XML_LIBS) -L$(top_srcdir)/Library/Log -lLog -L$(top_srcdir)/Platform -lWISEPlatform -lWISEUtil -lpthread -ldl -release 2.0.$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
include_HEADERS = $(top_srcdir)/Library/SAHandlerLoader/SALoader.h
LDADD = $(top_srcdir)/Library/SAConfig/libSAConfig.la $(INTLLIBS)
