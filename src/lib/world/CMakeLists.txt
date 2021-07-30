# Add the RenderEngine itself
add_library(WorldSystem STATIC ${CMAKE_CURRENT_SOURCE_DIR}/WorldSystem.cpp)

# Set the dependencies for this library:
target_include_directories(WorldSystem PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS WorldSystem)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
