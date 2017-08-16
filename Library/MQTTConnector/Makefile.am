ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Platform/Linux/ -I$(top_srcdir)/Include
lib_LTLIBRARIES = libMQTTConnector.la
libMQTTConnector_la_SOURCES = MQTTConnectorEx.c MQTTConnector.c MQTTConnector.h ../../Include/WISEConnector.h
libMQTTConnector_la_LDFLAGS = -L$(top_srcdir)/Platform -L$(top_srcdir)/Library/MosquittoCarrier -lWISEPlatform -lMosquittoCarrier -ldl -release 2.0.$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
include_HEADERS = $(top_srcdir)/Include/WISEConnector.h

