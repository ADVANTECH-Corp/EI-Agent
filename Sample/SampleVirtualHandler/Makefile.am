AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Old_Platforms/Linux/ -I$(top_srcdir)/Include 
lib_LTLIBRARIES = SampleVirtualHandler.la
SampleVirtualHandler_la_SOURCES = SampleVirtualHandler.cpp SampleVirtualHandler.h
SampleVirtualHandler_la_LDFLAGS = -ldl -release $(PACKAGE_VERSION_MAJOR).$(PACKAGE_VERSION_MINOR).$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION) -module
LDADD = $(INTLLIBS)
