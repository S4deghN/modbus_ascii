cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_C_COMPILER "gcc")

project(modbus_ascii)

# if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY) set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
# ${CMAKE_BINARY_DIR}/bin) endif ()

add_executable(demo "src/modbus_ascii.c")

# if (CMAKE_CXX_COMPILER_ID MATCHES "Clang") # target_compile_options(demo
# PUBLIC -O2) # target_link_options(demo PUBLIC -lc++ -lc++abi -lm -lc -lgcc_s
# -lgcc) endif ()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(demo PUBLIC -O3)
endif()

# target_link_libraries(server spdlog::spdlog_header_only pthread
# ${BOOST_LIBRARIES})
