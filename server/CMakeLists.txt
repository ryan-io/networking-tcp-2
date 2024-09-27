# CMakeList.txt : CMake project for networking-tcp, include source and define
# project specific logic here.
#
project(server)
#each project will also require boost if a library has a dependency on it
set(BOOST_ROOT C:/Development/projects-libraries/boost_1_86_0)
file(GLOB serverDepend "src/*.cpp" "include/*.h")

find_package(Boost COMPONENTS thread REQUIRED)
 
include_directories(PUBLIC ${Boost_INCLUDE_DIRS})   
# Add source to this project's executable.
add_executable(server ${serverDepend})
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include) # for 'server' include files
target_link_libraries(server PRIVATE network)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
else()
set(CMAKE_CXX_STANDARD 20)
endif(MSVC)