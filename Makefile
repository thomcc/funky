
# language settings
CXXFLAGS += -std=c++11 -pedantic

ifeq (${shell uname}, Darwin)
	# OS X is weird
	CXX = clang++
	CXXFLAGS += -stdlib=libc++
endif

Src  := src
Inc  := include
Test := test
Out  := build

# Object directories
Obj      := ${Out}/.obj
ObjSrc   := ${Obj}/src
ObjTest  := ${Obj}/test
ObjGTest := ${ObjTest}/gtest


# Source file locations
libSources    := ${wildcard ${Src}/*.cc}
gtestSources  := ${wildcard ${Test}/gtest/*.cc}
testSources   := ${wildcard ${Test}/*.cc}

# Object file locations
gtestObjects  := ${gtestSources:%.cc=${Obj}/%.o}
libObjects    := ${libSources:%.cc=${Obj}/%.o}
testObjects   := ${testSources:%.cc=${Obj}/%.o}

CXXFLAGS += -Wall -Wextra -Weffc++ -O3 -I${Inc}

# generate and use make dependancy files
CXXFLAGS += -MMD

allObjects := ${gtestObjects} ${libObjects} ${testObjects}

-include ${allObjects:.o=.d}


.PHONY: all
all: run-tests

.PHONY: out
out:
	@mkdir -p ${ObjSrc}
	@mkdir -p ${ObjTest}
	@mkdir -p ${ObjGTest}

.PHONY: test
test: out ${Out}/test-runner

.PHONY: run-tests
run-tests: test
	@echo "Running tests"
	@./${Out}/test-runner

.PHONY: clean
clean:
	@rm -rf ${Out}

${Out}/test-runner: gtest lib tests
	@echo "Linking $@"
	@${CXX} ${CXXFLAGS} -o $@ ${allObjects}

.PHONY: gtest
gtest: CXXFLAGS += -I${Test} -Wno-missing-field-initializers
gtest: ${gtestObjects}

.PHONY: tests
tests: CXXFLAGS += -I${Test}
tests: ${testObjects}

.PHONY: lib
lib: ${libObjects}
# TODO: when we aren't header-only, compile a static library here.

${Obj}/%.o: %.cc
	@echo "Compiling $<"
	@${CXX} ${CXXFLAGS} -c -o $@ $<

