# Add the library
add_library(MaterialsSimpleTextured STATIC ${CMAKE_CURRENT_SOURCE_DIR}/SimpleTextured.cpp)

# Set the dependencies for this library:
target_include_directories(MaterialsSimpleTextured PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS MaterialsSimpleTextured)

# Also add the shaders
add_subdirectory(shaders)
add_dependencies(MaterialsSimpleTextured ${SHADERS})

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)