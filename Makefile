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

mongo-ubuntu:
	cmake -S mongo-cxx-driver -B mongo-cxx-driver/build --fresh -DCMAKE_INSTALL_PREFIX="external/msvc-cl/mongo-cxx-driver" -DBUILD_SHARED_LIBS=OFF -DENABLE_BSONCXX_POLY_USE_IMPLS=ON -DBUILD_VERSION="3.10.2" -DCMAKE_BUILD_TYPE=$(CONFIG) -DCMAKE_CXX_FLAGS_INIT="-fPIC"
	cmake --build mongo-cxx-driver/build -t install --config $(CONFIG) -j8

.PHONY:mongo-msvc
mongo-msvc:
	cmake -S mongo-cxx-driver -B mongo-cxx-driver/build --fresh -DCMAKE_INSTALL_PREFIX="external/msvc-cl/mongo-cxx-driver" -DBUILD_SHARED_LIBS=OFF -DENABLE_BSONCXX_POLY_USE_IMPLS=ON -DBUILD_VERSION="3.10.2" -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=$(CONFIG)
	cmake --build mongo-cxx-driver/build -t install --config $(CONFIG) -j8
