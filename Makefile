config-gcc:
	cmake --preset=ninja-gcc --graphviz=docs/graph/graph.dot
.PHONY: config-gcc

fresh-gcc:
	cmake --fresh --preset=ninja-gcc --graphviz=docs/graph/graph.dot
.PHONY: fresh-gcc

build:
	cmake --build --preset=ninja-gcc
.PHONY: build

docs:
	cmake --build --preset=ninja-gcc-docs
.PHONY: docs

cmake_format:
	cmake --build --preset=ninja-gcc-cmake-format
.PHONY: cmake_format
