dnl $Id$
dnl config.m4 for extension jsonpath

PHP_ARG_ENABLE([jsonpath],
  [whether to enable jsonpath support],
  [AS_HELP_STRING([--enable-jsonpath],
    [Enable jsonpath support])]
  [no])

JSONPATH_SOURCES="\
    src/jsonpath/safe_string.c \
    src/jsonpath/stack.c \
    src/jsonpath/lexer.c \
    src/jsonpath/parser.c \
  ";

if test "$PHP_JSONPATH" != "no"; then
  AC_DEFINE(HAVE_JSONPATH, 1, [ Have jsonpath support ])
  PHP_NEW_EXTENSION(jsonpath, jsonpath.c $JSONPATH_SOURCES, $ext_shared)
  PHP_ADD_BUILD_DIR($ext_builddir/src/jsonpath)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
