cmake_minimum_required(VERSION 3.10.2)
project(performance_benchmark)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(pybind11)

include_directories(/usr/local/include)

add_executable(lua_benchmark src/main_lua.cpp)
add_executable(py_benchmark src/main_py.cpp)
add_executable(cpp_benchmark src/main_cpp.cpp)

target_link_libraries(lua_benchmark PRIVATE lua dl)

target_link_libraries(py_benchmark PRIVATE pybind11::embed)