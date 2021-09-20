# Add the RenderEngine itself
add_library(EcsAuxillary STATIC ${CMAKE_CURRENT_SOURCE_DIR}/IComponentList.cpp)

# Set the dependencies for this library:
target_include_directories(EcsAuxillary PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS EcsAuxillary)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
