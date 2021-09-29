# Add the library
add_library(MaterialsSimple STATIC ${CMAKE_CURRENT_SOURCE_DIR}/Simple.cpp)

# Set the dependencies for this library:
target_include_directories(MaterialsSimple PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS MaterialsSimple)

# Also add the shaders
add_subdirectory(shaders)
add_dependencies(MaterialsSimple ${SHADERS})

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)