# pathfinder
This c++ project is path finding library for games base on recast navigation.

# [官网](https://www.supercline.com/)

## Get Code
### 1. get build project
	git clone https://github.com/SuperCLine/cmake_cline.git
### 2. get core project
	cd cmake_cline
	git clone https://github.com/SuperCLine/core.git
### 3. get log project
	git clone https://github.com/SuperCLine/log.git
### 4. get external jsoncpp
	git submodule update --init --recursive
### 5. get pathfinder project
	git clone https://github.com/SuperCLine/pathfinder.git
### 5. get external recast
	git submodule update --init --recursive
## Build on centos
	sh Build.sh

## Build on windows
	using cmake gui and visual studio 2017

# Installation requirements
### 1. CMAKE 3.15.0
	wget https://cmake.org/files/v3.15/cmake-3.15.0.tar.gz 
	tar xzvf cmake-3.15.0.tar.gz
	cd cmake-3.15.0
	./bootstrap
	gmake
	make install
	cmake --version
	hash -r
	cmake --version
### 2. upgrade gcc to 7.3 to support c++ 17
	yum -y install centos-release-scl
	yum -y install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
	scl enable devtoolset-7 bash
### 3. opengl
	yum -y install mesa*
	yum -y install freeglut*
	yum -y install *GLEW*
### 4. SDL2
	yum -y install SDL2-devel