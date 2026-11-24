format_files=jsonpath.c \
	php_jsonpath.h \
	$(wildcard src/jsonpath/*.c) \
	$(wildcard src/jsonpath/*.h) \

findphp:
	@echo $(PHP_EXECUTABLE)

format-check:
	@clang-format --dry-run -Werror $(format_files)

format-code:
	@clang-format -i $(format_files)

lint-code:
	@cppcheck --enable=all --error-exitcode=1 --std=c89 --suppress=missingInclude $(format_files)

lint-ci:
	@cppcheck --enable=warning,performance,portability --error-exitcode=1 --std=c89 \
		--suppress=unknownMacro:jsonpath_arginfo.h $(format_files)

release: php_$(PHP_PECL_EXTENSION).h
	@php ./tools/build-packagexml.php

setup-pre-commit:
	@ln -s ../../pre-commit .git/hooks

