ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Platform/Linux/ -I$(top_srcdir)/Include
lib_LTLIBRARIES = libWISECore.la
libWISECore_la_SOURCES = liteparse.c liteparse.h WISECoreEx.c WISECore.c WISECore.h ../../Include/WiseCarrier_MQTT.h
libWISECore_la_LDFLAGS = -L$(top_srcdir)/Platform -L$(top_srcdir)/Library/MQTTConnector -lWISEPlatform -lWISEUtil -lMQTTConnector -lpthread -ldl -release 2.0.$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
include_HEADERS = $(top_srcdir)/Include/WISECore.h $(top_srcdir)/Include/WISECoreEx.h
