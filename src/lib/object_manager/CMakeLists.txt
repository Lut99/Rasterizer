# Add the RenderEngine itself
add_library(ObjectManager STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ObjectManager.cpp)

# Set the dependencies for this library:
target_include_directories(ObjectManager PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS ObjectManager)

# Add the subdirectories
add_subdirectory(objects)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
