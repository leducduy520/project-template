config-win:
	cmake --graphviz=docs/graph/graph.dot --no-warn-unused-cli --preset=windows
.PHONY: config-win

config-ubuntu:
	cmake --graphviz=docs/graph/graph.dot --no-warn-unused-cli --preset=ubuntu
.PHONY: config-ubuntu

fresh-win:
	cmake --fresh --preset=windows --graphviz=docs/graph/graph.dot
.PHONY: fresh-win

fresh-ubuntu:
	cmake --fresh --preset=ubuntu --graphviz=docs/graph/graph.dot
.PHONY: fresh-ubuntu
