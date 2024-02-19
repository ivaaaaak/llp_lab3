cmake_minimum_required(VERSION 3.16.3)
project(server C)

set(CMAKE_C_STANDARD 11)

# add flags to capture memory leaks
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -Wextra -Wpedantic -fsanitize=address -fno-omit-frame-pointer")


find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

add_executable(server 
        database/src/database.c
        database/src/file.c
        database/src/graph.c
        database/src/query.c

        database/include/database.h
        database/include/file.h
        database/include/graph.h
        database/include/query.h
        
        src/executor.c
        src/network.c
        src/main.c
        src/xml.c

        include/executor.h
        include/network.h
        include/xml.h
)

TARGET_LINK_LIBRARIES(server ${LIBXML2_LIBRARIES})
