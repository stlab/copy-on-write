cmake_minimum_required(VERSION 4.0)
project(stlab-copy-on-write)

# Enable testing for this project
include(CTest)
enable_testing()

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Add doctest dependency for testing
CPMAddPackage(
    NAME doctest
    GITHUB_REPOSITORY doctest/doctest
    GIT_TAG v2.4.12
    OPTIONS "DOCTEST_WITH_TESTS OFF"
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create symlink to compile_commands.json for clangd
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(clangd_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink 
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
        COMMENT "Creating symlink to compile_commands.json for clangd"
    )
endif()

# Create the header-only library target
add_library(stlab-copy-on-write INTERFACE)
add_library(stlab::copy-on-write ALIAS stlab-copy-on-write)

# Set include directories
target_include_directories(stlab-copy-on-write
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Function to add a test executable with all necessary setup
function(add_test_executable NAME)
    add_executable(${NAME} test/${NAME}.cpp)
    target_link_libraries(${NAME} PRIVATE stlab::copy-on-write doctest::doctest)
    
    # Add the main test that runs all test cases
    add_test(NAME ${NAME} COMMAND ${NAME})
    
    # Set test properties to help with IDE integration
    set_tests_properties(${NAME} PROPERTIES
        LABELS "doctest"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction()

# Add test executables
add_test_executable(copy_on_write_tests) 
