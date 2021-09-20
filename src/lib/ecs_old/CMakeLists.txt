# Add the RenderEngine itself
add_library(EntityManager STATIC ${CMAKE_CURRENT_SOURCE_DIR}/EntityManager.cpp)

# Set the dependencies for this library:
target_include_directories(EntityManager PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS EntityManager)

# # Add the subdirectories
add_subdirectory(auxillary)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
