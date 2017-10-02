cmake_minimum_required (VERSION 2.6)
project (agml)
include_directories ("${PROJECT_SOURCE_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}/extern")

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

IF(CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -std=c99   -pedantic-errors -Wno-long-long -mfpmath=sse -msse2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++98 -pedantic-errors -Wno-long-long -mfpmath=sse -msse2")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

###############################################################################
# Requirements

find_package(Threads REQUIRED)

###############################################################################
# Library agml_comm

file(GLOB agml_comm_sources
src/libagml_comm/server/Server.cpp
src/libagml_comm/tcp/Server.cpp
src/libagml_comm/tcp/Socket.cpp
src/libagml_comm/util/utils.cpp
src/libagml_comm/util/string.cpp
src/libagml_comm/util/file.cpp
src/libagml_comm/client/Client.cpp
src/libagml_comm/agml/node.cpp
src/libagml_comm/common/com.cpp
src/libagml_comm/common/Commands.cpp
src/libagml_comm/common/Host.cpp
src/libagml_comm/common/Message.cpp
src/libagml_comm/topology/DataHost.cpp
src/libagml_comm/topology/NodeLibrary.cpp
src/libagml_comm/topology/TopologyReader.cpp
src/libagml_comm/topology/NodeGroup.cpp
src/libagml_comm/topology/Info.cpp
src/libagml_comm/topology/Topology.cpp
src/libagml_comm/simulation/Thread.cpp
)

add_library(agml_comm SHARED ${agml_comm_sources})
target_link_libraries(agml_comm dl rt pthread)
include_directories ("${PROJECT_SOURCE_DIR}/src/libagml_comm")

###############################################################################
# Client executable

add_executable(agml_client src/agml_client/agml.cpp)
target_link_libraries(agml_client agml_comm)

###############################################################################
# Commodity toolbox library

file(GLOB agml_toolbox_sources
src/agml/com/message.cpp
src/agml/test/NodeTestMsgMatrix.cpp
src/agml/test/NodeTestMatrix.cpp
src/agml/math/Matrix.cpp
src/agml/math/math.cpp
src/agml/nodes/NodeExample.cpp
src/agml/nodes/data/NodeData.cpp
src/agml/nodes/data/NodeDataRand.cpp
src/agml/nodes/data/NodeDataFile.cpp
src/agml/nodes/basic/NodeAvg.cpp

)
add_library(agml_toolbox SHARED ${agml_toolbox_sources})
target_link_libraries(agml_toolbox agml_comm)
target_link_libraries(agml_toolbox ${PROJECT_SOURCE_DIR}/extern/libveccodec-sse2-r5static.a)
target_link_libraries(agml_toolbox ${PROJECT_SOURCE_DIR}/extern/libalgebra-sse2-r5static.a)
target_link_libraries(agml_toolbox ${PROJECT_SOURCE_DIR}/extern/libstring_tools-sse2-r5static.a)
include_directories ("${PROJECT_SOURCE_DIR}/src/agml")

###############################################################################
# Server executable with embedded clustering algorithms

file(GLOB agml_node_clustering_sources
src/agml_node_clustering/eval/NodeEvalKMeans.cpp
src/agml_node_clustering/NodeKMeans.cpp
src/agml_node_clustering/NodeEM.cpp
)
add_executable(agml_server_clustering src/agml_server/agmld.cpp ${agml_node_clustering_sources})
target_link_libraries(agml_server_clustering agml_toolbox)


