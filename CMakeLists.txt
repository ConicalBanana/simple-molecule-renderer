cmake_minimum_required(VERSION 3.10)

# Set project name
project(SimpleMoleculeRenderer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Add executable
add_executable(molecules main.cpp)

# Link libraries
if(APPLE)
    # On macOS, link with frameworks
    target_link_libraries(molecules 
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
    )
    # Add framework directories for macOS
    target_include_directories(molecules PRIVATE ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
elseif(WIN32)
    # On Windows, you might need to specify the path to GLUT
    # Assuming GLUT files are in the glut/ directory
    target_include_directories(molecules PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/glut)
    target_link_libraries(molecules 
        ${OPENGL_LIBRARIES}
        ${CMAKE_CURRENT_SOURCE_DIR}/glut/glut32.lib
    )
else()
    # On Linux and other Unix-like systems
    target_link_libraries(molecules 
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
    )
    target_include_directories(molecules PRIVATE ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
endif()

# Set output name to match original Makefile
set_target_properties(molecules PROPERTIES OUTPUT_NAME "molecules.out")

# Optional: Add custom target to run the executable (similar to the original Makefile)
add_custom_target(run
    COMMAND molecules.out
    DEPENDS molecules
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running molecules application"
)
