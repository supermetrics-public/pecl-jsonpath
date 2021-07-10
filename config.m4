dnl $Id$
dnl config.m4 for extension jsonpath

PHP_ARG_ENABLE([jsonpath],
  [whether to enable JSONPath support],
  [AS_HELP_STRING([--enable-jsonpath],
    [Enable JSONPath support])])

PHP_ARG_ENABLE([code-coverage],
  [whether to enable code coverage (relevant for extension development only!)],
  [AS_HELP_STRING([--enable-code-coverage],
    [Enable code coverage])],
  [no],
  [no])

JSONPATH_SOURCES="\
    src/jsonpath/exceptions.c \
    src/jsonpath/lexer.c \
    src/jsonpath/parser.c \
    src/jsonpath/interpreter.c \
  ";

if test "$PHP_JSONPATH" != "no"; then
  AC_DEFINE(HAVE_JSONPATH, 1, [JSONPath support enabled])
  PHP_NEW_EXTENSION(jsonpath, jsonpath.c $JSONPATH_SOURCES, $ext_shared)
  PHP_ADD_BUILD_DIR($ext_builddir/src/jsonpath)
  PHP_ADD_MAKEFILE_FRAGMENT
fi

if test "$PHP_CODE_COVERAGE" != "no"; then
  if test "$GCC" != "yes"; then
    AC_MSG_ERROR([GCC is required for --enable-code-coverage])
  fi

  dnl Check if ccache is being used.
  case `$php_shtool path $CC` in
    *ccache*[)] gcc_ccache=yes;;
    *[)] gcc_ccache=no;;
  esac

  if test "$gcc_ccache" = "yes" && (test -z "$CCACHE_DISABLE" || test "$CCACHE_DISABLE" != "1"); then
    AC_MSG_ERROR([ccache must be disabled when --enable-code-coverage option is used. You can disable ccache by setting environment variable CCACHE_DISABLE=1.])
  fi

  AC_DEFINE(HAVE_CODE_COVERAGE, 1, [Code coverage enabled])
  PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/Makefile.gcov, $abs_srcdir)

  dnl Remove all optimization flags from CFLAGS.
  changequote({,})
  CFLAGS=`echo "$CFLAGS" | "${SED}" -e 's/-O[0-9s]*//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | "${SED}" -e 's/-O[0-9s]*//g'`
  changequote([,])

  dnl Add the special gcc flags.
  CFLAGS="$CFLAGS -O0 -fprofile-arcs -ftest-coverage"
  CXXFLAGS="$CXXFLAGS -O0 -fprofile-arcs -ftest-coverage"
fi
