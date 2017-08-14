ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Library3rdParty/EI-Connect/Platform -I$(top_srcdir)/Library3rdParty/EI-Connect/Platform/Linux/ -I$(top_srcdir)/Include -I$(top_srcdir)/Library/Log -I$(top_srcdir)/Library/SAConfig -I$(top_srcdir)/Library/SAClient -I$(top_srcdir)/Library3rdParty/EI-Connect/Include
bin_PROGRAMS = sampleagent
sampleagent_SOURCES = sampleagent.c
sampleagent_LDFLAGS = -L$(top_srcdir)/Library3rdParty/EI-Connect/Release -lWISEPlatform -lWISEUtil -lWISECore -lMQTTConnector -lMosquittoCarrier -lpthread -ldl -Wl,-rpath,./ -release $(PACKAGE_VERSION_MAJOR).$(PACKAGE_VERSION_MINOR).$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
LDADD = -lstdc++ $(top_srcdir)/Library/SAClient/libSAClient.la $(top_srcdir)/Library/SAConfig/libSAConfig.la $(top_srcdir)/Library/Log/libLog.la $(INTLLIBS)
