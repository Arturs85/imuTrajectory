



###
project(imuTrajectoryBuilder)

cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)



if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
endif()







add_executable(${PROJECT_NAME}
   main.cpp
 
    )



set (CMAKE_SHARED_LINKER_FLAGS "-lpthread -pthread")
target_link_libraries(${PROJECT_NAME}   pthread)

