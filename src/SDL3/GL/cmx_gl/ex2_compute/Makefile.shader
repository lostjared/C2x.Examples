all:
	glslc vertex.vert -o vert.spv
	glslc fragment.frag -o frag.spv
	glslc compute.comp -o compute.spv
	
