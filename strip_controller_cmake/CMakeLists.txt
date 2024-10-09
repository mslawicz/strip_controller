# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(
	strip_controller
	VERSION 1.0
	LANGUAGES C CXX ASM
)

# Include the definition of the slc_strip_controller target,
# which contains the content of the SLC project
include(strip_controller.cmake)

add_executable(strip_controller
    # Add additional sources here
    ../src/StripController.cpp
)

target_include_directories(strip_controller PUBLIC
    # Add additional include paths here
)

target_compile_definitions(strip_controller PUBLIC
    # Add additional macros here
)

target_compile_options(strip_controller PUBLIC
    # Set additional compiler flags here
)

target_link_options(strip_controller PUBLIC
    # Set additional linker flags here
)

# Link with the content defined in the SLC project
target_link_libraries(strip_controller PRIVATE
    slc_strip_controller
)

# Include managed project content if available
include(strip_controller_project.cmake OPTIONAL RESULT_VARIABLE managed_project)
if(managed_project)
    message(STATUS "Using managed project content from ${managed_project}")
endif()

# Force the gcc linker command
set_target_properties(strip_controller PROPERTIES LINKER_LANGUAGE C)

# Create .bin, .hex and .s37 artifacts after building the project
add_custom_command(TARGET strip_controller
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O srec "$<TARGET_FILE:strip_controller>" "$<TARGET_FILE_DIR:strip_controller>/$<TARGET_FILE_BASE_NAME:strip_controller>.s37"
    COMMAND ${CMAKE_OBJCOPY} -O ihex "$<TARGET_FILE:strip_controller>" "$<TARGET_FILE_DIR:strip_controller>/$<TARGET_FILE_BASE_NAME:strip_controller>.hex"
    COMMAND ${CMAKE_OBJCOPY} -O binary "$<TARGET_FILE:strip_controller>" "$<TARGET_FILE_DIR:strip_controller>/$<TARGET_FILE_BASE_NAME:strip_controller>.bin" 
)

# Run post-build pipeline to perform additional post-processing
if(post_build_command)
add_custom_command(TARGET strip_controller
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)
endif()