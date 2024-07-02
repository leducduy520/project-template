config-win:
	cmake --graphviz=docs/graph/graph.dot --no-warn-unused-cli --preset=windows
.PHONY: config-win

config-ubuntu:
	cmake --graphviz=docs/graph/graph.dot --no-warn-unused-cli --preset=ubuntu
.PHONY: config-ubuntu

fresh-win:
	cmake --fresh --no-warn-unused-cli --graphviz=docs/graph/graph.dot  --preset=windows
.PHONY: fresh-win

fresh-ubuntu:
	cmake --fresh --no-warn-unused-cli --graphviz=docs/graph/graph.dot --preset=ubuntu
.PHONY: fresh-ubuntu

config-msvc:
	cmake --graphviz=docs/graph/graph.dot --no-warn-unused-cli --preset=msvc
.PHONY: config-msvc

fresh-msvc:
	cmake --fresh --graphviz=docs/graph/graph.dot --no-warn-unused-cli --preset=msvc
.PHONY:fresh-msvc
