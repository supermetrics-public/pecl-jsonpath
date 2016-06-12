dnl $Id$
dnl config.m4 for extension jsonpath

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(jsonpath, for jsonpath support,
dnl Make sure that the comment is aligned:
 [  --with-jsonpath             Include jsonpath support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(jsonpath, whether to enable jsonpath support,
dnl Make sure that the comment is aligned:
dnl [  --enable-jsonpath           Enable jsonpath support])

JSONPATH_SOURCES="\
    src/jsonpath/lexer.c \
    src/jsonpath/parser.c \
  ";

if test "$PHP_JSONPATH" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-jsonpath -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/jsonpath.h"  # you most likely want to change this
  dnl if test -r $PHP_JSONPATH/$SEARCH_FOR; then # path given as parameter
  dnl   JSONPATH_DIR=$PHP_JSONPATH
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for jsonpath files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       JSONPATH_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$JSONPATH_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the jsonpath distribution])
  dnl fi

  dnl # --with-jsonpath -> add include path
  dnl PHP_ADD_INCLUDE($JSONPATH_DIR/include)

  dnl # --with-jsonpath -> check for lib and symbol presence
  dnl LIBNAME=jsonpath # you may want to change this
  dnl LIBSYMBOL=jsonpath # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $JSONPATH_DIR/$PHP_LIBDIR, JSONPATH_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_JSONPATHLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong jsonpath lib version or lib not found])
  dnl ],[
  dnl   -L$JSONPATH_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(JSONPATH_SHARED_LIBADD)

  PHP_NEW_EXTENSION(jsonpath, jsonpath.c $JSONPATH_SOURCES, $ext_shared)

  PHP_ADD_BUILD_DIR($ext_builddir/src/jsonpath)

  AC_CHECK_FUNCS([utimes strndup])
  PHP_ADD_MAKEFILE_FRAGMENT
fi
