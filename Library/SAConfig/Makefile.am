AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Platform -I$(top_srcdir)/Platform/Linux/ -I$(top_srcdir)/Include $(XML_CFLAGS)
lib_LTLIBRARIES = libSAConfig.la
libSAConfig_la_SOURCES = configuration.c profile.c XMLBase.c
libSAConfig_la_LDFLAGS = ${XML_LIBS} -L$(top_srcdir)/Platform -lWISEPlatform -lWISEUtil $(XML_LIBS) -lpthread -ldl -release $(PACKAGE_VERSION_MAJOR).$(PACKAGE_VERSION_MINOR).$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
include_HEADERS = $(top_srcdir)/Library/SAConfig/configuration.h $(top_srcdir)/Library/SAConfig/profile.h $(top_srcdir)/Library/SAConfig/XMLBase.h
LDADD = $(INTLLIBS)
