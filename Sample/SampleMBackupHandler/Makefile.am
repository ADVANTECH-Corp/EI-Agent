AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Platform/Linux -I$(top_srcdir)/Include -I$(top_srcdir)/Library/MessageGenerator -I$(top_srcdir)/Library/HandlerKernel
lib_LTLIBRARIES = SampleMBackupHandler.la
SampleMBackupHandler_la_SOURCES = SampleMBackupHandler.cpp SampleMBackupHandler.h
SampleMBackupHandler_la_LDFLAGS = -L$(top_srcdir)/Library/HandlerKernel -lHandlerKernel -L$(top_srcdir)/Library/MessageGenerator -lmsggen -lpthread -ldl -lm -release $(PACKAGE_VERSION_MAJOR).$(PACKAGE_VERSION_MINOR).$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION) -module
