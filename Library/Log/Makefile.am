AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Platform -I$(top_srcdir)/Platform/Linux/ -I$(top_srcdir)/Library3rdParty/AdvLog/Tools/AdvLog/include
lib_LTLIBRARIES = libLog.la
libLog_la_SOURCES = Log.cpp Log.h ../../Platform/Linux/util_path.c
libLog_la_LDFLAGS = -L$(top_srcdir)/Library3rdParty/AdvLog/Tools/AdvJSON/src/.libs -lAdvJSON -L$(top_srcdir)/Library3rdParty/AdvLog/Tools/AdvLog/src/.libs -lAdvLog -ldl -release $(PACKAGE_VERSION_MAJOR).$(PACKAGE_VERSION_MINOR).$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
