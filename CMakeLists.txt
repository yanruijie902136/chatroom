# ./CMakeLists.txt

cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CHATROOM_LIBRARIES_DIR
    ./libs
    )

set(CHATROOM_LIBRARIES 
    utils
    )

project(chatroom)

add_executable(client ./src/client.c)
add_executable(server ./src/server.c)

foreach(LIBRARY ${CHATROOM_LIBRARIES})
    add_subdirectory("${CHATROOM_LIBRARIES_DIR}/${LIBRARY}")
endforeach(LIBRARY)

target_link_libraries(client ${CHATROOM_LIBRARIES})
target_link_libraries(server ${CHATROOM_LIBRARIES})
