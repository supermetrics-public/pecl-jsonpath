#include "exceptions.h"

#include <stdarg.h>

#include "ext/spl/spl_exceptions.h"
#include "zend_exceptions.h"

extern zend_class_entry *jsonpath_exception_ce;

void throw_jsonpath_exception(const char* msg, ...) {
  va_list args;
  char *formatted_msg = NULL;

  va_start(args, msg);
  zend_vspprintf(&formatted_msg, 0, msg, args);
  zend_throw_exception(jsonpath_exception_ce, formatted_msg, 0);
  efree(formatted_msg);
  va_end(args);
}
