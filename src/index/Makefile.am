
include $(top_srcdir)/flymake.mk

AUTOMAKE_OPTIONS	=	foreign

bin_PROGRAMS		=	readFits rtWriteFits offlineIndex  onlineIndex massdata test massdatapq

readFits_LDADD		=	-lCCfits -lfastbit
readFits_SOURCES	= 	readFits.cpp

offlineIndex_LDADD		=	-lCCfits -lfastbit
offlineIndex_SOURCES	= 	offlineIndex.cpp utils.cpp coreImpl.cpp

rtWriteFits_LDADD	=	-lCCfits -lfastbit
rtWriteFits_SOURCES	= 	coreImpl.cpp utils.cpp rtWrite.cpp

onlineIndex_LDADD	=	-lCCfits -lfastbit
onlineIndex_SOURCES	=  coreImpl.cpp utils.cpp onlineIndex.cpp

massdata_LDADD	=	-lmysqlclient
massdata_SOURCES	=utils.cpp   massdatamysql.cpp

test_SOURCES	=  utils.cpp test.cpp

massdatapq_LDADD	=-L/usr/local/pgsql/lib -lpq 
massdatapq_SOURCES	=utils.cpp   massdatapq.cpp
