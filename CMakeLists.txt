cmake_minimum_required(VERSION 3.30)

include(GNUInstallDirs)

project(nothofagus_lua_demo_sln CXX)

option(NOTHOFAGUS_INSTALL "Disabling Nothofagus' installation" OFF)
option(NOTHOFAGUS_BUILD_EXAMPLES "Disabling Nothofagus'examples" OFF)
add_subdirectory("third_party/nothofagus")
add_subdirectory("third_party/lua_sol_cmake")

file(GLOB_RECURSE HEADERS "source/*.h")

add_executable(nothofagus_lua_demo
    "source/nothofagus_lua_demo.cpp"

    # Only to show headers in VS
    ${HEADERS}
)
set_property(TARGET nothofagus_lua_demo PROPERTY CXX_STANDARD 20)
target_include_directories(nothofagus_lua_demo PRIVATE
    ${NOTHOFAGUS_INCLUDE}
    ${LUA_SOL_INCLUDE}
    "third_party/argparse/include"
)
target_link_libraries(nothofagus_lua_demo PRIVATE
    nothofagus
    lua_sol
)

install(TARGETS nothofagus_lua_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Demo_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Demo_Runtime
        NAMELINK_COMPONENT Demo_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Demo_Development
)

# Installing files that will be needed at runtime.
install(FILES
    "source/test.lua"
    "source/bellotas.lua"
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
