
# language settings
CXXFLAGS += -std=c++11 -pedantic -fno-exceptions -fno-rtti -pedantic

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
libSources :=

gtestSources := \
	${Test}/gtest/gtest-all.cc \
	${Test}/gtest/gtest_main.cc

testSources := \
	${Test}/Either.cc

# Object file locations
gtestObjects  := ${gtestSources:%.cc=${Obj}/%.o}
libObjects    := ${libSources:%.cc=${Obj}/%.o}
testObjects   := ${testSources:%.cc=${Obj}/%.o}

CXXFLAGS += -Wall -Wextra -Weffc++
CXXFLAGS += -O3
CXXFLAGS += -I${Inc} -I${Test}

# for gtest
CXXFLAGS += -Wno-missing-field-initializers

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
	./${Out}/test-runner

.PHONY: clean
clean:
	@rm -rf ${Out}

${Out}/test-runner: ${gtestObjects} ${libObjects} ${testObjects}
	@echo "Linking $@"
	@${CXX} ${CXXFLAGS} -o $@ $^

${Obj}/%.o: %.cc
	@echo "Compiling $<"
	@${CXX} ${CXXFLAGS} -c -o $@ $<

