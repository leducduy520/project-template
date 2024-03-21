config-win:
	cmake --preset=windows --graphviz=docs/graph/graph.dot
.PHONY: config-win

config-ubuntu:
	cmake --preset=ubuntu --graphviz=docs/graph/graph.dot
.PHONY: config-ubuntu

fresh-win:
	cmake --fresh --preset=windows --graphviz=docs/graph/graph.dot
.PHONY: fresh-win

fresh-ubuntu:
	cmake --fresh --preset=ubuntu --graphviz=docs/graph/graph.dot
.PHONY: fresh-ubuntu
