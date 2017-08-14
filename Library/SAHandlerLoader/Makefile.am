AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Library3rdParty/EI-Connect/Platform -I$(top_srcdir)/Library3rdParty/EI-Connect/Platform/Linux/ -I$(top_srcdir)/Include -I$(top_srcdir)/Library/Log $(XML_CFLAGS) -I$(top_srcdir)/Library3rdParty/EI-Connect/Include
lib_LTLIBRARIES = libSAHandlerLoader.la
libSAHandlerLoader_la_SOURCES = loaderlog.h moduleconfig.c moduleconfig.h SALoader.c SALoader.h
libSAHandlerLoader_la_LDFLAGS = $(XML_LIBS) -L$(top_srcdir)/Library/Log -lLog -L$(top_srcdir)/Library3rdParty/EI-Connect/Release -lWISEPlatform -lWISEUtil -lpthread -ldl -release 2.0.$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
include_HEADERS = $(top_srcdir)/Library/SAHandlerLoader/SALoader.h
LDADD = $(top_srcdir)/Library/SAConfig/libSAConfig.la $(INTLLIBS)
