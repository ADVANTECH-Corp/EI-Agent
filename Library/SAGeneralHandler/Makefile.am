AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Platform -I$(top_srcdir)/Platform/Linux/ -I$(top_srcdir)/Include -I$(top_srcdir)/Library/SAConfig -I$(top_srcdir)/Library/Log -I$(top_srcdir)/Library/FtpHelper -I$(top_srcdir)/Library/MD5 -I$(top_srcdir)/Library/cJSON -I$(top_srcdir)/Library3rdParty/curl-7.47.1/include $(XML_CFLAGS)
lib_LTLIBRARIES = libSAGeneralHandler.la
libSAGeneralHandler_la_SOURCES = general_def.h generallog.h agentupdater.c agentupdater.h ghparser.c ghparser.h SAGeneralHandler.c SAGeneralHandler.h
libSAGeneralHandler_la_LDFLAGS = -L$(top_srcdir)/Library3rdParty/curl-7.47.1/lib $(XML_LIBS) -L$(top_srcdir)/Library/Log -lLog -L$(top_srcdir)/Platform -lWISEPlatform -lWISEUtil -L$(top_srcdir)/Library/FtpHelper -lftphelper -L$(top_srcdir)/Library/MD5 -lmd5 -L$(top_srcdir)/Library/cJSON -lcJSON -L$(top_srcdir)/Library/SAConfig -lSAConfig -lpthread -ldl -lcurl -release 2.0.$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
LDADD = $(INTLLIBS)
