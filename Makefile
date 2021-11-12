CC=clang
CFLAGS=-Wall -O3
PREFIX=/usr/local

path_to_this_file := $(abspath $(lastword $(MAKEFILE_LIST)))
repository_dir := $(shell dirname "$(path_to_this_file)")

build:
	$(CC) $(CFLAGS) "$(repository_dir)/src/SSTTime.c" -o "$(repository_dir)/bin/SSTTime"

clean:
	rm -v "$(repository_dir)/bin/SSTTime"

view-version:
	@env PATH="$(repository_dir)/bin:$$PATH" SSTest --version

run-example: build
	env PATH="$(repository_dir)/bin:$$PATH" SSTRunTestSuite "$(repository_dir)/tests/Example"

run-all-tests: build
	env PATH="$(repository_dir)/bin:$$PATH" SSTest "$(repository_dir)/tests"

run-all-tests-verbosely: build
	env PATH="$(repository_dir)/bin:$$PATH" SSTest "$(repository_dir)/tests" -v

run-failing-tests: build
	env PATH="$(repository_dir)/bin:$$PATH" SSTest "$(repository_dir)/tests" --filter "/never-succeeds"

test-v: build
	env PATH="$(repository_dir)/bin:$$PATH" SSTest "$(repository_dir)/tests" --skip "Example/never-succeeds" -v

test: build
	env PATH="$(repository_dir)/bin:$$PATH" SSTest "$(repository_dir)/tests" --skip "Example/never-succeeds"

install: build
	@cp -vaf "$(repository_dir)/bin"/* "$(PREFIX)/bin" 
