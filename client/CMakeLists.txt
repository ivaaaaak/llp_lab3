cmake_minimum_required(VERSION 3.16.3)
project(client C)

set(CMAKE_C_STANDARD 11)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})


set(BISON_INPUT_FILE parser/bison+flex/parser.y)
set(FLEX_INPUT_FILE parser/bison+flex/lexer.l)

BISON_TARGET(MyParser ${BISON_INPUT_FILE} ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
FLEX_TARGET(MyLexer ${FLEX_INPUT_FILE} ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)

ADD_FLEX_BISON_DEPENDENCY(MyLexer MyParser)

add_executable(client ${BISON_MyParser_OUTPUTS} ${FLEX_MyLexer_OUTPUTS} ${LIBXML2_LIBRARIES}
        parser/src/query.c
        parser/include/query.h

        src/main.c
        src/xml.c
        src/network.c

        include/xml.h
        include/network.h)

TARGET_LINK_LIBRARIES(client ${LIBXML2_LIBRARIES})