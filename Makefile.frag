format_files=jsonpath.c \
	php_jsonpath.h \
	src/jsonpath/*.{c,h} \

findphp:
	@echo $(PHP_EXECUTABLE)

format-check:
	@clang-format --dry-run -Werror $(format_files)

format-code:
	@clang-format -i $(format_files)

lint-code:
	@cppcheck --enable=all --error-exitcode=1 --std=c89 --suppress=missingInclude $(format_files)

setup-pre-commit:
	@ln -s ../../pre-commit .git/hooks

