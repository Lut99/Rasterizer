# Add the RenderEngine itself
add_library(MaterialsAuxillary STATIC ${CMAKE_CURRENT_SOURCE_DIR}/IComponentList.cpp)

# Set the dependencies for this library:
target_include_directories(MaterialsAuxillary PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS MaterialsAuxillary)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
