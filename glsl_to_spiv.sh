cd shaders/base/vertex || exit
rm base.spv
cd ..//fragment || exit
rm base.spv
cd ..//..//..//third_party_services/exes || exit
glslc.exe ..//..//shaders/base/vertex/base.vert -o ..//..//shaders/base/vertex/base.spv
glslc.exe ..//..//shaders/base/fragment/base.frag -o ..//..//shaders/base/fragment/base.spv
pause