cmake_minimum_required(VERSION 3.16)

project(json_builder2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(json_builder2 main.cpp
    json_builder.h json_builder.cpp json.h json.cpp)

include(GNUInstallDirs)
install(TARGETS json_builder2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
