/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 27fbe76d157af97672d8d6e085e2c68684e1024b */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_JsonPath_JsonPath_find, 0, 2, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, expression, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(JsonPath_JsonPath, find);


static const zend_function_entry class_JsonPath_JsonPath_methods[] = {
	ZEND_ME(JsonPath_JsonPath, find, arginfo_class_JsonPath_JsonPath_find, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_JsonPath_JsonPathException_methods[] = {
	ZEND_FE_END
};

static zend_class_entry *register_class_JsonPath_JsonPath(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "JsonPath", "JsonPath", class_JsonPath_JsonPath_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);

	return class_entry;
}

static zend_class_entry *register_class_JsonPath_JsonPathException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "JsonPath", "JsonPathException", class_JsonPath_JsonPathException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);

	return class_entry;
}
