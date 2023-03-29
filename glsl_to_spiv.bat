cd shaders/base/vertex
del base.spv
cd ..//fragment
del base.spv
cd ..//..//..//third_party_services/exes
glslc.exe ..//..//shaders/base/vertex/base.vert -o ..//..//shaders/base/vertex/base.spv
glslc.exe ..//..//shaders/base/fragment/base.frag -o ..//..//shaders/base/fragment/base.spv