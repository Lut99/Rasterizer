# CMAKELIST for the RASTERIZER project
#   by Lut99

# Specify the version required
cmake_minimum_required(VERSION 3.10)

# Set the project name & version
project(Rasterizer VERSION 1.0.0)

# If we're on Windows, prefix the GLFW path
IF(WIN32)
set(GLFW3_PATH "C:/Program Files (x86)/GLFW/lib/cmake/glfw3")
list(APPEND CMAKE_PREFIX_PATH "${GLFW3_PATH}")
ENDIF()

# Get the VULKAN, GLM & CppDebugger library
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)
# find_package(CppDebugger REQUIRED)

# Specify the C++-standard to use
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the Wall Wextra flags
set(WARNING_FLAGS "-Wall -Wextra")
if(WIN32)
set(WARNING_FLAGS "")
endif()

# Add extra flags needed
set(DEBUG_FLAGS "-g -rdynamic -DNTRACE")
if(WIN32)
set(DEBUG_FLAGS "")
endif()
message("Debug flags: '${DEBUG_FLAGS}'")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${WARNING_FLAGS} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

# Define all include directories
get_target_property(GLFW_DIR glfw INTERFACE_INCLUDE_DIRECTORIES)
SET(INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/lib" "${Vulkan_INCLUDE_DIRS}" "${GLFW_DIR}")

# Load the libraries
add_subdirectory(src)



##### RASTERIZER TARGET #####
# Specify which file we want to compile
add_executable(rasterizer ${PROJECT_SOURCE_DIR}/src/Main.cpp)
# Set the output to the bin directory
set_target_properties(rasterizer
                      PROPERTIES 
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin
                      )

# Add the include directories for this target
target_include_directories(rasterizer PUBLIC "${INCLUDE_DIRS}")

# Add which libraries to link
target_link_libraries(rasterizer PUBLIC
                      ${EXTRA_LIBS}
                      ${Vulkan_LIBRARIES}
                      glfw)



##### BUILDING SHADERS #####
# Define the custom commands to compile the shaders
add_custom_target(shaders
    COMMAND glslc -fshader-stage=vertex -o ${PROJECT_SOURCE_DIR}/bin/shaders/vertex_v1.spv ${PROJECT_SOURCE_DIR}/src/shaders/vertex_v1.glsl
    COMMAND glslc -fshader-stage=vertex -o ${PROJECT_SOURCE_DIR}/bin/shaders/vertex_v2.spv ${PROJECT_SOURCE_DIR}/src/shaders/vertex_v2.glsl
    COMMAND glslc -fshader-stage=vertex -o ${PROJECT_SOURCE_DIR}/bin/shaders/vertex_v3.spv ${PROJECT_SOURCE_DIR}/src/shaders/vertex_v3.glsl
    COMMAND glslc -fshader-stage=vertex -o ${PROJECT_SOURCE_DIR}/bin/shaders/vertex_v4.spv ${PROJECT_SOURCE_DIR}/src/shaders/vertex_v4.glsl
    COMMAND glslc -fshader-stage=vertex -o ${PROJECT_SOURCE_DIR}/bin/shaders/vertex_v5.spv ${PROJECT_SOURCE_DIR}/src/shaders/vertex_v5.glsl
    COMMAND glslc -fshader-stage=frag -o ${PROJECT_SOURCE_DIR}/bin/shaders/frag_v1.spv ${PROJECT_SOURCE_DIR}/src/shaders/fragment_v1.glsl
    COMMAND glslc -fshader-stage=frag -o ${PROJECT_SOURCE_DIR}/bin/shaders/frag_v2.spv ${PROJECT_SOURCE_DIR}/src/shaders/fragment_v2.glsl
    COMMENT "Building shaders..."
)



##### EXPORT #####
if (WIN32)
add_custom_target(export
                  # Copy the rasterizer exe & libraries
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/rasterizer.exe ${PROJECT_SOURCE_DIR}/export/rasterizer/rasterizer.exe
                  # COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/assimp-vc142-mtd.dll ${PROJECT_SOURCE_DIR}/export/rasterizer/assimp-vc142-mtd.dll
                  # Copy the necessary shaders
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/shaders/materials/simple_vert.spv ${PROJECT_SOURCE_DIR}/export/rasterizer/shaders/materials/simple_vert.spv
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/shaders/materials/simple_frag.spv ${PROJECT_SOURCE_DIR}/export/rasterizer/shaders/materials/simple_frag.spv
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/shaders/materials/simple_coloured_vert.spv ${PROJECT_SOURCE_DIR}/export/rasterizer/shaders/materials/simple_coloured_vert.spv
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/shaders/materials/simple_coloured_frag.spv ${PROJECT_SOURCE_DIR}/export/rasterizer/shaders/materials/simple_coloured_frag.spv
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/shaders/materials/simple_textured_vert.spv ${PROJECT_SOURCE_DIR}/export/rasterizer/shaders/materials/simple_textured_vert.spv
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/shaders/materials/simple_textured_frag.spv ${PROJECT_SOURCE_DIR}/export/rasterizer/shaders/materials/simple_textured_frag.spv
                  # Copy the necessary models/materials/textures
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/data/models/viking_room.obj ${PROJECT_SOURCE_DIR}/export/rasterizer/data/models/viking_room.obj
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/data/textures/viking_room.png ${PROJECT_SOURCE_DIR}/export/rasterizer/data/textures/viking_room.png
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/data/materials/viking_room.mtl ${PROJECT_SOURCE_DIR}/export/rasterizer/data/materials/viking_room.mtl
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/data/models/capsule.obj ${PROJECT_SOURCE_DIR}/export/rasterizer/data/models/capsule.obj
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/data/textures/capsule.jpg ${PROJECT_SOURCE_DIR}/export/rasterizer/data/textures/capsule.jpg
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/data/materials/capsule.mtl ${PROJECT_SOURCE_DIR}/export/rasterizer/data/materials/capsule.mtl
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/data/models/watermill.obj ${PROJECT_SOURCE_DIR}/export/rasterizer/data/models/watermill.obj
                  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/bin/data/materials/watermill.mtl ${PROJECT_SOURCE_DIR}/export/rasterizer/data/materials/watermill.mtl
                  )
endif()



##### ARRAY TESTS #####
# Add the test directory
add_subdirectory(tests/Array)

# Define the executable for the tests
add_executable(test_array ${PROJECT_SOURCE_DIR}/tests/Array/test_array.cpp)
# Define the executable's include directory
target_include_directories(test_array PUBLIC "${INCLUDE_DIRS}")

# Add which libraries to link
target_link_libraries(test_array PUBLIC
                      ${ARRAY_TEST_LIBS}
                      )
