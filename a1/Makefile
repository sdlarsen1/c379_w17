TEST_DIR = tests

.PHONY: tests test1 test2 test3

# This is the top-level Makefile for assignment 1.

# To compile all test programs, enter: make p=<pattern>
# To compile individual tests, enter: make test<#> p=<pattern>
# To clean all created files from /tests, enter: make clean
# Before changing testing patterns, ensure to run make clean.

tests:
	$(MAKE) -C $(TEST_DIR) p=$(p)

test1:
	$(MAKE) -C $(TEST_DIR) p=$(p) test1

test2:
	$(MAKE) -C $(TEST_DIR) p=$(p) test2

test3:
	$(MAKE) -C $(TEST_DIR) p=$(p) test3


clean:
	$(MAKE) -C $(TEST_DIR) clean
