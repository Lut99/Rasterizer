# Add the RenderEngine itself
add_library(ModelSystem STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ModelSystem.cpp)

# Set the dependencies for this library:
target_include_directories(ModelSystem PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS ModelSystem)

# Add the subdirectories
add_subdirectory(formats)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
