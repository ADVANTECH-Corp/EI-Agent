ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -Wall -fPIC -I$(top_srcdir)/Platform -I$(top_srcdir)/Platform/Linux/ -I$(top_srcdir)/Include
lib_LTLIBRARIES = libMosquittoCarrier.la
libMosquittoCarrier_la_SOURCES = MosquittoCarrierEx.c MosquittoCarrier.c topic.c topic.h ../../Include/WiseCarrier_MQTT.h
libMosquittoCarrier_la_LDFLAGS = -L$(top_srcdir)/Platform -lWISEPlatform -lWISEUtil -lmosquitto -lpthread -ldl -release 2.0.$(PACKAGE_VERSION_PATCH).$(PACKAGE_VERSION_REVISION)
include_HEADERS = WiseCarrier_MQTT.h WiseCarrierEx_MQTT.h

