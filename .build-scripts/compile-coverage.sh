#!/bin/bash
phpize
./configure --enable-jsonpath --enable-code-coverage
make
