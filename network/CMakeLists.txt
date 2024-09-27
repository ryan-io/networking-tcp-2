# CMakeList.txt : CMake project for networking-tcp, include source and define
# project specific logic here.
#
project(network)
set(Boost_DEBUG 1)  
# project commands
include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include) # use '${CMAKE_CURRENT_SOURCE_DIR}' for library headers
 
file(GLOB networkSrc "src/*.cpp")
 
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
else()  
	set(CMAKE_CXX_STANDARD 20)
endif(MSVC)
add_library(network STATIC ${networkSrc}) # do not include header files, ideally, link via STATIC, not SHARED	
set(BOOST_ROOT C:/Development/projects-libraries/boost_1_86_0)

# define boost components to includee
find_package(Boost COMPONENTS thread REQUIRED)
include_directories(PUBLIC ${Boost_INCLUDE_DIRS})  

# libraries require target_include_directors for include and src
#	this allows us to include the library header files in the project, but will not
#	expose the implementations
target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE src)

# link the boost components to the project that will use this library
target_link_libraries(
	${PROJECT_NAME}
	Boost::thread
  )