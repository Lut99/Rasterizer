# Define the command to parse the .peg files
add_custom_target(
    obj_peg
    COMMAND leg -o"${CMAKE_CURRENT_SOURCE_DIR}/pegleg/test.c" ${CMAKE_CURRENT_SOURCE_DIR}/pegleg/test.leg
)

# Add the library
add_library(ModelsObj STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ObjLoader.cpp)
add_dependencies(ModelsObj obj_peg)

# Set the dependencies for this library:
target_include_directories(ModelsObj PUBLIC
                           "${INCLUDE_DIRS}"
                           "${CMAKE_CURRENT_SOURCE_DIR}/flex/lib")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS ModelsObj)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
