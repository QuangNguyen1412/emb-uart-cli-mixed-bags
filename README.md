# Unit Test Suite
This is a set of CLI-based projects in C/C++. I set this up so that we can easily start a project for learning purposes/quick prototyping/testings

---
## 1. Directory structure

unittest (root directory)
- first_test_suite (test suite directory)
  - test_runner.cpp
  - test_runner.h
  - etc...
- second_test_suite (test suite directory)
  - test_runner.cpp
  - test_runner.h
  - etc...
- make (directory)
  - first_test.mk
  - second_test.mk
- shared_drivers (directory): shared drivers sources
- Makefile (Master makefile)

**Notes**
- `first_test`/`second_test` should be meaningful names and ideally be the most descriptive name for the test suite
- I try to create convention for unittest: 
  - <*testname*_test_suite> as directory name
  - `test_runner.cpp` always has the `main()` of each test suite to minimize changes in test suite makefile every time we add new test scenario
  - Individual Makefile be: <*testname*_test.mk> and should be placed in make directory
---
## 2. How to use
In the same level as the root `unittest` directory:
- `make all` or just `make` - to compile all the test file
- `make first_test <DEBUG=1>` - to compile the `first-test` executable (optional with DEBUG)
- `make clean` - to clean all the test executables

---
## 3. How to contribute
Please see the root `Makefile` for references of adding new unittest

### **Adding new test**
1. Make change to the root `Makefile`
> APPS := shared_lib_test \\\
>         **new_test**

And don't forget to add new test at `clean` section in the Makefile

2. Add a `new_test_tc` directory for source files/headers
3. Add a `new_test.mk` under `make` directory
4. Copy Makefile content from the other `.mk` and correct the content to point to `new_test_tc` directory created earlier in the 2nd step
5. from `unittest` directory, run `make new_test` to compile the new test binary

### **Updating the emb_apps_drivers.a static library that includes shared_libraries and shared_drivers**
This procedure should be followed whenever you want to compile more app/yeti-hal/gz-shared_libraries for unittesting.
1. Make changes to `emb_apps_drivers.mk`
  - update `INCLUDE_PATH`: include path for `shared_libraries` and `shared_drivers`. TODO $(GZ_SHARED_LIBS_DIR)
  - update `LIB_SOURCES`: TODO $(GZ_SHARED_LIBS_DIR)/*.c
  - update `DRIVERS_SOURCES`: default to include all sources from `./shared_drivers`
2. do `make emb_apps_drivers` testing the new changes
3. do `make` testing the compatibility of new changes to existing tests
