#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([soeyes], [0.1], [])
AC_CONFIG_SRCDIR([src/consist/main.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
have_pthreads=no
AC_SEARCH_LIBS([pthread_create],[pthread],[have_pthreads=yes])

# Checks for header fiples.

if test "x${have_pthreads}" = xyes; then
                AC_CHECK_HEADERS([pthread.h],[],[have_pthreads=no])
fi

if test "x${have_pthreads}" = xno; then
                AC_MSG_WARN([
                -------------------------------------------------------------
                Unable to find pthreads on this system.
                Building a single-threaded version
                -------------------------------------------------------------])
fi


# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])
# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# 定义PTHREAD的LDFLAG，以在Makefile.am中加入到链接器参数中去。
PTHREAD_LIBS=-lpthread
AC_SUBST(PTHREAD_LIBS)

CFITSIO_LIBS=-lcfitsio
AC_SUBST(CFITSIO_LIBS)

CCFITS_LIBS=-lCCfits
AC_SUBST(CCFITS_LIBS)

AC_CONFIG_FILES([Makefile
				 src/Makefile
				 src/consist/Makefile
                 src/index/Makefile
                 src/lfufs/Makefile])
AC_OUTPUT
