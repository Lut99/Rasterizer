# Add the RenderEngine itself
add_library(MaterialSystem STATIC ${CMAKE_CURRENT_SOURCE_DIR}/MaterialSystem.cpp)

# Set the dependencies for this library:
target_include_directories(MaterialSystem PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS MaterialSystem)

# # Add the subdirectories, then add the new dependencies
add_subdirectory(textures)
add_subdirectory(variants)
add_dependencies(MaterialSystem ${EXTRA_TARGETS})

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
