CONFIG ?= RelWithDebInfo
PRESET ?= msvc
WARNING ?= ON

ifeq ($(origin FRESH), undefined)
    FRESH_VAR = 
else
	FRESH_VAR = --fresh
endif

ifeq ($(origin VERBOSE), undefined)
    VERBOSE_VAR = 
else
	VERBOSE_VAR = -DCMAKE_VERBOSE_MAKEFILE=ON
endif

ifeq ($(origin TIDY), undefined)
    TIDY_VAR = 
else
	TIDY_VAR = -DENABLE_CLANG_TIDY=ON
endif

configure:
	cmake $(FRESH_VAR) --graphviz=docs/graph/graph.dot --no-warn-unused-cli \
	--preset=$(PRESET) -DCMAKE_BUILD_TYPE=$(CONFIG) $(VERBOSE_VAR) \
	$(TIDY_VAR) -DENABLE_WARNINGS=$(WARNING)

git-update:
	git fetch
	git pull
