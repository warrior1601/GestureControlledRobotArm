@echo OFF
rem  cd C:\Users\Jedidiah\Desktop\Year3\FYP\Simulation\Code_From-_GitHub\Colored_Glove
rem "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
mkdir build
cd build

cmake source\ -DCMAKE_CXX_FLAGS="/Od"
cmake -G"Visual Studio 15 2017 Win64"..
msbuild project.sln

cd ..
