# Add the library
add_library(MaterialsSimpleColoured STATIC ${CMAKE_CURRENT_SOURCE_DIR}/SimpleColoured.cpp)

# Set the dependencies for this library:
target_include_directories(MaterialsSimpleColoured PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS MaterialsSimpleColoured)

# Also add the shaders
add_subdirectory(shaders)
add_dependencies(MaterialsSimpleColoured ${SHADERS})

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)