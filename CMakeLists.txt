cmake_minimum_required(VERSION 3.10.2)
project(performance_benchmark)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

execute_process(COMMAND julia -e "print(joinpath(Sys.BINDIR, Base.DATAROOTDIR, \"julia\"))" OUTPUT_VARIABLE JL_SHARE)
execute_process(COMMAND julia -e "print(joinpath(Sys.BINDIR, Base.INCLUDEDIR, \"julia\"))" OUTPUT_VARIABLE JL_INCLUDE)
execute_process(COMMAND julia -e "print(joinpath(Sys.BINDIR, Base.LIBDIR))" OUTPUT_VARIABLE JL_LIB)

execute_process(COMMAND ${JL_SHARE}/julia-config.jl --cflags    OUTPUT_VARIABLE CFLAGS   OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${JL_SHARE}/julia-config.jl --cflags    OUTPUT_VARIABLE CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${JL_SHARE}/julia-config.jl --ldflags   OUTPUT_VARIABLE LDFLAGS  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${JL_SHARE}/julia-config.jl --ldlibs    OUTPUT_VARIABLE LDLIBS   OUTPUT_STRIP_TRAILING_WHITESPACE)

# message("Value of Julia include dir: ${JL_INCLUDE}")
# message("Value of Julia share dir:   ${JL_SHARE}" )
# message("Value of Julia lib dir:     ${JL_LIB}" )
# 
message("Value of Julia --cflags:     ${CXXFLAGS}" )
message("Value of Julia --ldflags:      ${LDFLAGS}" )
message("Value of Julia --ldlibs:       ${LDLIBS}" )

#LUA
add_executable(lua_benchmark src/main_lua.cpp)
target_include_directories(lua_benchmark PRIVATE /usr/local/include)
target_link_libraries(lua_benchmark PRIVATE lua dl)

#CPP
add_executable(cpp_benchmark src/main_cpp.cpp)

#JULIA
add_executable(jl_benchmark src/main_jl.cpp)
target_include_directories(jl_benchmark PRIVATE ${JL_INCLUDE})
string(REPLACE "-std=gnu99" "" CXXFLAGS "${CXXFLAGS}")
separate_arguments(CXXFLAGS UNIX_COMMAND "${CXXFLAGS}")

target_compile_options(jl_benchmark PRIVATE ${CXXFLAGS})
target_link_libraries(jl_benchmark PRIVATE ${LDFLAGS} ${LDLIBS})

#PYTHON
add_subdirectory(pybind11)
add_executable(py_benchmark src/main_py.cpp)
target_link_libraries(py_benchmark PRIVATE pybind11::embed)