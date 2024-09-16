# CMakeList.txt : CMake project for networking-tcp, include source and define
# project specific logic here.
#
project(client)
set(BOOST_ROOT C:/Development/project-libraries/boost_1_86_0)
file(GLOB clientDepend "src/*.cpp" "include/*.h")
set(CMAKE_CXX_STANDARD 17)    
find_package(Boost REQUIRED)
include_directories(PUBLIC ${Boost_INCLUDE_DIRS})
add_executable(client ${clientDepend})
include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CMAKE_CXX_STANDARD 14)

target_link_libraries(client PRIVATE network)
