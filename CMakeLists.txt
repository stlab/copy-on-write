cmake_minimum_required(VERSION 3.20)
project(stlab-copy-on-write
    VERSION 1.0.3
    DESCRIPTION "Copy-on-write wrapper for any type"
)

# Enable testing for this project
include(CTest)

# Only download CPM if building tests or examples
if(PROJECT_IS_TOP_LEVEL)
    # Enable CPM caching to avoid re-downloading dependencies (including CPM.cmake)
    set(CPM_SOURCE_CACHE ${CMAKE_SOURCE_DIR}/.cpm-cache CACHE PATH "Directory to cache CPM packages" FORCE)

    include(cmake/CPM.cmake)
endif()

# Create the header-only library target
add_library(stlab-copy-on-write INTERFACE)
add_library(stlab::copy-on-write ALIAS stlab-copy-on-write)

# Set include directories and requirements
target_include_directories(stlab-copy-on-write INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(stlab-copy-on-write INTERFACE cxx_std_17)

# Build tests and examples
if(PROJECT_IS_TOP_LEVEL)
    # Add doctest dependency for testing
    CPMAddPackage("gh:doctest/doctest@2.4.12")

    # Create symlink to compile_commands.json for clangd
    if(CMAKE_EXPORT_COMPILE_COMMANDS)
        add_custom_target(clangd_compile_commands ALL
            COMMAND ${CMAKE_COMMAND} -E create_symlink 
                ${CMAKE_BINARY_DIR}/compile_commands.json
                ${CMAKE_SOURCE_DIR}/compile_commands.json
            COMMENT "Creating symlink to compile_commands.json for clangd"
        )
    endif()

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

    # Add example programs (also used as tests to ensure they build and run)
    add_executable(basic_usage_example example/basic_usage.cpp)
    target_link_libraries(basic_usage_example PRIVATE stlab::copy-on-write)
    add_test(NAME basic_usage_example COMMAND basic_usage_example)

    add_executable(access_methods_demo example/access_methods_demo.cpp)
    target_link_libraries(access_methods_demo PRIVATE stlab::copy-on-write)
    add_test(NAME access_methods_demo COMMAND access_methods_demo)

    # Documentation with Doxygen
    option(BUILD_DOCS "Build documentation" OFF)

    if(BUILD_DOCS)
        find_package(Doxygen REQUIRED)
        
        if(DOXYGEN_FOUND)
            # Download doxygen-awesome-css theme via CPM
            CPMAddPackage(
                URI gh:jothepro/doxygen-awesome-css@2.3.4
                DOWNLOAD_ONLY YES
            )
            set(AWESOME_CSS_DIR ${doxygen-awesome-css_SOURCE_DIR})
            
            # Configure Doxyfile
            set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
            set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
            
            # Set variables for Doxyfile template
            set(PROJECT_NAME "stlab::copy_on_write")
            set(PROJECT_BRIEF "Copy-on-write wrapper for any type")
            # PROJECT_VERSION is already defined by CMake; no need to re-assign
            set(INPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
            set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
            set(AWESOME_CSS_PATH "${AWESOME_CSS_DIR}")
            # CMAKE_SOURCE_DIR is already set by CMake; avoid redundant shadowing
            
            configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
            
            # Add custom target for documentation
            add_custom_target(docs
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM
            )
            
            # Ensure the output directory exists
            file(MAKE_DIRECTORY ${OUTPUT_DIR})
            
            message(STATUS "Documentation target 'docs' configured")
            message(STATUS "Run 'cmake --build . --target docs' to generate documentation")
        else()
            message(WARNING "Doxygen not found. Documentation will not be built.")
        endif()
    endif()
endif() 
