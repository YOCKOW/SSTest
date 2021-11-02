# What is `SSTest`?

It's a runner for Shell Script Tests.  
This tool has been inspired by [XCTest](https://developer.apple.com/documentation/xctest).


# Requirements

* clang (only when building)
* zsh
* macOS/Linux


# How to install

```console
% git clone https://github.com/YOCKOW/SSTest.git
% cd SSTest
% make test && sudo make install
```


# Usage

```console
% SSTest /path/to/tests

% SSTRunTestSuite /path/to/tests/test-suite

% SSTRunTestSuite /path/to/tests/test-suite --filter=test-foo
```

This repository uses `SSTest` itself. Please see the scripts in [tests](./tests) directory.


# Directory/File Structure for tests

Given the following sample structure.

```
📦 MyShellScriptProject (Current Directory)
├─ 📁 src
├─ 📁 some-other-resources
└─ 📂 tests
   ├─ 📂 TestSuiteA
   │   ├─ 🖥 test-A1
   │   └─ 🖥 test-A2
   └─ 📂 TestSuiteB
        ├─ 🖥 finalize
        ├─ 🖥 init
        ├─ 🖥 set-up
        ├─ 🖥 tear-down
        ├─ 🖥 test-B1
        └─ 🖥 test-B2
```

## Case 1. Run tests only under `TestSuiteA`

Type the following command:

```console
% SSTRunTestSuite ./tests/TestSuiteA
```

Then, the files that has prefix "test" (`test-A1` and `test-A2` in this case) will be executed. 


## Case 2. Run tests only under `TestSuiteB`

`TestSuiteB` contains some files that don't have prefix "test".

- `init`: Executed only once before the first test starts.
- `finalize`: Executed only once after the last test finishes.
- `set-up`: Executed every time before each test starts.
- `tear-down`: Executed every time after each test finishes.


As a result of that, when you run `TestSuiteB` as follows,

```console
% SSTRunTestSuite ./tests/TestSuiteB
```

1. `init` is executed.
2. `set-up` is executed.
3. `test-B1` is executed.
4. `tear-down` is executed.
5. `set-up` is executed.
6. `test-B2` is executed.
7. `tear-down` is executed.
8. `finalize` is executed.


## Case 3. Run tests under both `TestSuiteA` and `TestSuiteB`

```console
% SSTest ./tests
```


# Assertion commands

Each test case (`test*` file) will be decided as failure if the exit status is not `0` or one of assertion commands (`SSTFail` or `SSTAssert*`) fails.

## `SSTFail`

`SSTFail` always fails.

```zsh
#!/bin/zsh
# Any shell can be used.

SSTFail "Failure Message" -l ${(%):-%I}
# Line number is viewed when `-l` (a.k.a `--line`) parameter is passed.
```


## `SSTAssert*Equal`

```zsh
#!/bin/zsh

SSTAssertEqual 0 0 -l ${(%):-%I} # Passes
SSTAssertEqual 0 1 -l ${(%):-%I} # Fails
SSTAssertEqual +1.23e-3 0.00123 -l ${(%):-%I} # Passes
SSTAssertEqual "string" string -l ${(%):-%I} # Passes

SSTAssertEqual "0" "00" -l ${(%):-%I} # Passes because both "0" and "00" are regarded as integers.
SSTAssertIntegerEqual "0" "00" -l ${(%):-%I} # Passes
SSTAssertStringEqual "0" "00" -l ${(%):-%I} # Fails

SSTAssertEqual 0 1 "Not Equal🥺" -l ${(%):-%I} # You can pass a description of a failure.
```

## `SSTAssert*NotEqual`

```zsh
#!/bin/zsh

SSTAssertNotEqual 0 0 -l ${(%):-%I} # Fails
SSTAssertNotEqual foo bar -l ${(%):-%I} # Passes

SSTAssertFloatNotEqual +1.23e-3 0.00123 -l ${(%):-%I} # Fails
SSTAssertFloatNotEqual 0.1 0.12 -l ${(%):-%I} # Passes

SSTAssertIntegerNotEqual 0 0 -l ${(%):-%I} # Fails
SSTAssertIntegerNotEqual 0 1 -l ${(%):-%I} # Passes

SSTAssertStringNotEqual baz baz -l ${(%):-%I} # Fails
SSTAssertStringNotEqual fizz buzz -l ${(%):-%I} # Passes
```

## `SSTAssert*GreaterThan`, `SSTAssert*LessThan`, `SSTAssert*GreaterThanOrEqual`, and `SSTAssert*LessThanOrEqual`

```zsh
#!/bin/zsh

SSTAssertGreaterThan 2 1 -l ${(%):-%I} # Passes
SSTAssertGreaterThan "ABC" "A12" -l ${(%):-%I} # Passes

SSTAssertLessThan 1 2 -l ${(%):-%I} # Passes
SSTAssertLessThan "A12" "ABC" -l ${(%):-%I} # Passes
```

There are also the following assertion commands:

- `SSTAssertFloatGreaterThan`- `SSTAssertFloatGreaterThanOrEqual`- `SSTAssertFloatLessThan`- `SSTAssertFloatLessThanOrEqual`- `SSTAssertIntegerGreaterThan`- `SSTAssertIntegerGreaterThanOrEqual`- `SSTAssertIntegerLessThan`- `SSTAssertIntegerLessThanOrEqual`- `SSTAssertStringGreaterThan`- `SSTAssertStringGreaterThanOrEqual`- `SSTAssertStringLessThan`- `SSTAssertStringLessThanOrEqual`


## `SSTAssertMatch` and `SSTAssertNotMatch`

You can use regular expressions for assertions.

```zsh
#!/bin/zsh

string="aabbbccc"
SSTAssertMatch "$string" "^a+b+c+$" -l ${(%):-%I} # Passes
SSTAssertNotMatch "$string" "a{3,}" -l ${(%):-%I} # Passes
```


# License

MIT License.  
See "LICENSE.txt" for more information.
