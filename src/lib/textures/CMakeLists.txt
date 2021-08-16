# Add the RenderEngine itself
add_library(TextureSystem STATIC ${CMAKE_CURRENT_SOURCE_DIR}/TextureSystem.cpp)

# Set the dependencies for this library:
target_include_directories(TextureSystem PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS TextureSystem)

# Add the subdirectories
add_subdirectory(formats)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
