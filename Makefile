# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/user/work/monkeycpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/user/work/monkeycpp

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target test
test:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running tests..."
	/usr/bin/ctest --force-new-ctest-process $(ARGS)
.PHONY : test

# Special rule for the target test
test/fast: test
.PHONY : test/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# Special rule for the target list_install_components
list_install_components:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Available install components are: \"Unspecified\""
.PHONY : list_install_components

# Special rule for the target list_install_components
list_install_components/fast: list_install_components
.PHONY : list_install_components/fast

# Special rule for the target install
install: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Install the project..."
	/usr/bin/cmake -P cmake_install.cmake
.PHONY : install

# Special rule for the target install
install/fast: preinstall/fast
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Install the project..."
	/usr/bin/cmake -P cmake_install.cmake
.PHONY : install/fast

# Special rule for the target install/local
install/local: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing only the local directory..."
	/usr/bin/cmake -DCMAKE_INSTALL_LOCAL_ONLY=1 -P cmake_install.cmake
.PHONY : install/local

# Special rule for the target install/local
install/local/fast: preinstall/fast
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing only the local directory..."
	/usr/bin/cmake -DCMAKE_INSTALL_LOCAL_ONLY=1 -P cmake_install.cmake
.PHONY : install/local/fast

# Special rule for the target install/strip
install/strip: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing the project stripped..."
	/usr/bin/cmake -DCMAKE_INSTALL_DO_STRIP=1 -P cmake_install.cmake
.PHONY : install/strip

# Special rule for the target install/strip
install/strip/fast: preinstall/fast
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing the project stripped..."
	/usr/bin/cmake -DCMAKE_INSTALL_DO_STRIP=1 -P cmake_install.cmake
.PHONY : install/strip/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/user/work/monkeycpp/CMakeFiles /home/user/work/monkeycpp//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/user/work/monkeycpp/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named object

# Build rule for target.
object: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 object
.PHONY : object

# fast build rule for target.
object/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/object.dir/build.make CMakeFiles/object.dir/build
.PHONY : object/fast

#=============================================================================
# Target rules for targets named evaluator

# Build rule for target.
evaluator: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 evaluator
.PHONY : evaluator

# fast build rule for target.
evaluator/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/evaluator.dir/build.make CMakeFiles/evaluator.dir/build
.PHONY : evaluator/fast

#=============================================================================
# Target rules for targets named monkey_lib

# Build rule for target.
monkey_lib: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 monkey_lib
.PHONY : monkey_lib

# fast build rule for target.
monkey_lib/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/build
.PHONY : monkey_lib/fast

#=============================================================================
# Target rules for targets named monkey

# Build rule for target.
monkey: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 monkey
.PHONY : monkey

# fast build rule for target.
monkey/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey.dir/build.make CMakeFiles/monkey.dir/build
.PHONY : monkey/fast

#=============================================================================
# Target rules for targets named lexer_test

# Build rule for target.
lexer_test: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 lexer_test
.PHONY : lexer_test

# fast build rule for target.
lexer_test/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/lexer_test.dir/build.make CMakeFiles/lexer_test.dir/build
.PHONY : lexer_test/fast

#=============================================================================
# Target rules for targets named parser_test

# Build rule for target.
parser_test: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 parser_test
.PHONY : parser_test

# fast build rule for target.
parser_test/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/parser_test.dir/build.make CMakeFiles/parser_test.dir/build
.PHONY : parser_test/fast

#=============================================================================
# Target rules for targets named evaluator_test

# Build rule for target.
evaluator_test: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 evaluator_test
.PHONY : evaluator_test

# fast build rule for target.
evaluator_test/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/evaluator_test.dir/build.make CMakeFiles/evaluator_test.dir/build
.PHONY : evaluator_test/fast

#=============================================================================
# Target rules for targets named gtest

# Build rule for target.
gtest: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 gtest
.PHONY : gtest

# fast build rule for target.
gtest/fast:
	$(MAKE) $(MAKESILENT) -f _deps/googletest-build/googletest/CMakeFiles/gtest.dir/build.make _deps/googletest-build/googletest/CMakeFiles/gtest.dir/build
.PHONY : gtest/fast

#=============================================================================
# Target rules for targets named gtest_main

# Build rule for target.
gtest_main: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 gtest_main
.PHONY : gtest_main

# fast build rule for target.
gtest_main/fast:
	$(MAKE) $(MAKESILENT) -f _deps/googletest-build/googletest/CMakeFiles/gtest_main.dir/build.make _deps/googletest-build/googletest/CMakeFiles/gtest_main.dir/build
.PHONY : gtest_main/fast

ast/ast.o: ast/ast.cpp.o
.PHONY : ast/ast.o

# target to build an object file
ast/ast.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/ast/ast.cpp.o
.PHONY : ast/ast.cpp.o

ast/ast.i: ast/ast.cpp.i
.PHONY : ast/ast.i

# target to preprocess a source file
ast/ast.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/ast/ast.cpp.i
.PHONY : ast/ast.cpp.i

ast/ast.s: ast/ast.cpp.s
.PHONY : ast/ast.s

# target to generate assembly for a file
ast/ast.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/ast/ast.cpp.s
.PHONY : ast/ast.cpp.s

evaluator/evaluator.o: evaluator/evaluator.cpp.o
.PHONY : evaluator/evaluator.o

# target to build an object file
evaluator/evaluator.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/evaluator.dir/build.make CMakeFiles/evaluator.dir/evaluator/evaluator.cpp.o
.PHONY : evaluator/evaluator.cpp.o

evaluator/evaluator.i: evaluator/evaluator.cpp.i
.PHONY : evaluator/evaluator.i

# target to preprocess a source file
evaluator/evaluator.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/evaluator.dir/build.make CMakeFiles/evaluator.dir/evaluator/evaluator.cpp.i
.PHONY : evaluator/evaluator.cpp.i

evaluator/evaluator.s: evaluator/evaluator.cpp.s
.PHONY : evaluator/evaluator.s

# target to generate assembly for a file
evaluator/evaluator.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/evaluator.dir/build.make CMakeFiles/evaluator.dir/evaluator/evaluator.cpp.s
.PHONY : evaluator/evaluator.cpp.s

lexer/lexer.o: lexer/lexer.cpp.o
.PHONY : lexer/lexer.o

# target to build an object file
lexer/lexer.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/lexer/lexer.cpp.o
.PHONY : lexer/lexer.cpp.o

lexer/lexer.i: lexer/lexer.cpp.i
.PHONY : lexer/lexer.i

# target to preprocess a source file
lexer/lexer.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/lexer/lexer.cpp.i
.PHONY : lexer/lexer.cpp.i

lexer/lexer.s: lexer/lexer.cpp.s
.PHONY : lexer/lexer.s

# target to generate assembly for a file
lexer/lexer.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/lexer/lexer.cpp.s
.PHONY : lexer/lexer.cpp.s

main.o: main.cpp.o
.PHONY : main.o

# target to build an object file
main.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey.dir/build.make CMakeFiles/monkey.dir/main.cpp.o
.PHONY : main.cpp.o

main.i: main.cpp.i
.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey.dir/build.make CMakeFiles/monkey.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s
.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey.dir/build.make CMakeFiles/monkey.dir/main.cpp.s
.PHONY : main.cpp.s

object/object.o: object/object.cpp.o
.PHONY : object/object.o

# target to build an object file
object/object.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/object.dir/build.make CMakeFiles/object.dir/object/object.cpp.o
.PHONY : object/object.cpp.o

object/object.i: object/object.cpp.i
.PHONY : object/object.i

# target to preprocess a source file
object/object.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/object.dir/build.make CMakeFiles/object.dir/object/object.cpp.i
.PHONY : object/object.cpp.i

object/object.s: object/object.cpp.s
.PHONY : object/object.s

# target to generate assembly for a file
object/object.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/object.dir/build.make CMakeFiles/object.dir/object/object.cpp.s
.PHONY : object/object.cpp.s

parser/parser.o: parser/parser.cpp.o
.PHONY : parser/parser.o

# target to build an object file
parser/parser.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/parser/parser.cpp.o
.PHONY : parser/parser.cpp.o

parser/parser.i: parser/parser.cpp.i
.PHONY : parser/parser.i

# target to preprocess a source file
parser/parser.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/parser/parser.cpp.i
.PHONY : parser/parser.cpp.i

parser/parser.s: parser/parser.cpp.s
.PHONY : parser/parser.s

# target to generate assembly for a file
parser/parser.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/parser/parser.cpp.s
.PHONY : parser/parser.cpp.s

repl/repl.o: repl/repl.cpp.o
.PHONY : repl/repl.o

# target to build an object file
repl/repl.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/repl/repl.cpp.o
.PHONY : repl/repl.cpp.o

repl/repl.i: repl/repl.cpp.i
.PHONY : repl/repl.i

# target to preprocess a source file
repl/repl.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/repl/repl.cpp.i
.PHONY : repl/repl.cpp.i

repl/repl.s: repl/repl.cpp.s
.PHONY : repl/repl.s

# target to generate assembly for a file
repl/repl.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/repl/repl.cpp.s
.PHONY : repl/repl.cpp.s

tests/evaluator_test.o: tests/evaluator_test.cpp.o
.PHONY : tests/evaluator_test.o

# target to build an object file
tests/evaluator_test.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/evaluator_test.dir/build.make CMakeFiles/evaluator_test.dir/tests/evaluator_test.cpp.o
.PHONY : tests/evaluator_test.cpp.o

tests/evaluator_test.i: tests/evaluator_test.cpp.i
.PHONY : tests/evaluator_test.i

# target to preprocess a source file
tests/evaluator_test.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/evaluator_test.dir/build.make CMakeFiles/evaluator_test.dir/tests/evaluator_test.cpp.i
.PHONY : tests/evaluator_test.cpp.i

tests/evaluator_test.s: tests/evaluator_test.cpp.s
.PHONY : tests/evaluator_test.s

# target to generate assembly for a file
tests/evaluator_test.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/evaluator_test.dir/build.make CMakeFiles/evaluator_test.dir/tests/evaluator_test.cpp.s
.PHONY : tests/evaluator_test.cpp.s

tests/lexer_test.o: tests/lexer_test.cpp.o
.PHONY : tests/lexer_test.o

# target to build an object file
tests/lexer_test.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/lexer_test.dir/build.make CMakeFiles/lexer_test.dir/tests/lexer_test.cpp.o
.PHONY : tests/lexer_test.cpp.o

tests/lexer_test.i: tests/lexer_test.cpp.i
.PHONY : tests/lexer_test.i

# target to preprocess a source file
tests/lexer_test.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/lexer_test.dir/build.make CMakeFiles/lexer_test.dir/tests/lexer_test.cpp.i
.PHONY : tests/lexer_test.cpp.i

tests/lexer_test.s: tests/lexer_test.cpp.s
.PHONY : tests/lexer_test.s

# target to generate assembly for a file
tests/lexer_test.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/lexer_test.dir/build.make CMakeFiles/lexer_test.dir/tests/lexer_test.cpp.s
.PHONY : tests/lexer_test.cpp.s

tests/parser_test.o: tests/parser_test.cpp.o
.PHONY : tests/parser_test.o

# target to build an object file
tests/parser_test.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/parser_test.dir/build.make CMakeFiles/parser_test.dir/tests/parser_test.cpp.o
.PHONY : tests/parser_test.cpp.o

tests/parser_test.i: tests/parser_test.cpp.i
.PHONY : tests/parser_test.i

# target to preprocess a source file
tests/parser_test.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/parser_test.dir/build.make CMakeFiles/parser_test.dir/tests/parser_test.cpp.i
.PHONY : tests/parser_test.cpp.i

tests/parser_test.s: tests/parser_test.cpp.s
.PHONY : tests/parser_test.s

# target to generate assembly for a file
tests/parser_test.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/parser_test.dir/build.make CMakeFiles/parser_test.dir/tests/parser_test.cpp.s
.PHONY : tests/parser_test.cpp.s

token/token.o: token/token.cpp.o
.PHONY : token/token.o

# target to build an object file
token/token.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/token/token.cpp.o
.PHONY : token/token.cpp.o

token/token.i: token/token.cpp.i
.PHONY : token/token.i

# target to preprocess a source file
token/token.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/token/token.cpp.i
.PHONY : token/token.cpp.i

token/token.s: token/token.cpp.s
.PHONY : token/token.s

# target to generate assembly for a file
token/token.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/monkey_lib.dir/build.make CMakeFiles/monkey_lib.dir/token/token.cpp.s
.PHONY : token/token.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... install"
	@echo "... install/local"
	@echo "... install/strip"
	@echo "... list_install_components"
	@echo "... rebuild_cache"
	@echo "... test"
	@echo "... evaluator"
	@echo "... evaluator_test"
	@echo "... gtest"
	@echo "... gtest_main"
	@echo "... lexer_test"
	@echo "... monkey"
	@echo "... monkey_lib"
	@echo "... object"
	@echo "... parser_test"
	@echo "... ast/ast.o"
	@echo "... ast/ast.i"
	@echo "... ast/ast.s"
	@echo "... evaluator/evaluator.o"
	@echo "... evaluator/evaluator.i"
	@echo "... evaluator/evaluator.s"
	@echo "... lexer/lexer.o"
	@echo "... lexer/lexer.i"
	@echo "... lexer/lexer.s"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
	@echo "... object/object.o"
	@echo "... object/object.i"
	@echo "... object/object.s"
	@echo "... parser/parser.o"
	@echo "... parser/parser.i"
	@echo "... parser/parser.s"
	@echo "... repl/repl.o"
	@echo "... repl/repl.i"
	@echo "... repl/repl.s"
	@echo "... tests/evaluator_test.o"
	@echo "... tests/evaluator_test.i"
	@echo "... tests/evaluator_test.s"
	@echo "... tests/lexer_test.o"
	@echo "... tests/lexer_test.i"
	@echo "... tests/lexer_test.s"
	@echo "... tests/parser_test.o"
	@echo "... tests/parser_test.i"
	@echo "... tests/parser_test.s"
	@echo "... token/token.o"
	@echo "... token/token.i"
	@echo "... token/token.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

