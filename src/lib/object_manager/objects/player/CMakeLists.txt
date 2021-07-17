# Add the RenderEngine itself
add_library(ECSPlayer STATIC ${CMAKE_CURRENT_SOURCE_DIR}/Player.cpp)

# Set the dependencies for this library:
target_include_directories(ECSPlayer PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS ECSPlayer)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
