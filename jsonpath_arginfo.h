/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 8c7e66dd5931d6f4c0d2baee3efdcb1eb834c9f7 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_JsonPath_find, 0, 2, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, expression, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(JsonPath, find);


static const zend_function_entry class_JsonPath_methods[] = {
	ZEND_ME(JsonPath, find, arginfo_class_JsonPath_find, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
