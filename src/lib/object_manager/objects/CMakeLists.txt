# Add the RenderEngine itself
add_library(ECSObject STATIC ${CMAKE_CURRENT_SOURCE_DIR}/Object.cpp)

# Set the dependencies for this library:
target_include_directories(ECSObject PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS ECSObject)

# Add the subdirectories
add_subdirectory(player)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
