TEST_DIR = tests

.PHONY: test1 test2 test3

# This is the top-level Makefile for assignment 1.

# To compile all test programs, enter: make p=<pattern>
# To compile individual tests, enter: make test<#> p=<pattern>
# To clean all created files from /tests, enter: make clean

all:
	$(MAKE) -C $(TEST_DIR)

test1:
	$(MAKE) -C $(TEST_DIR) p=$(p) test1

test2:
	$(MAKE) -C $(TEST_DIR) p=$(p) test2

test3:
	$(MAKE) -C $(TEST_DIR) p=$(p) test2


clean:
	$(MAKE) -C $(TEST_DIR) clean
