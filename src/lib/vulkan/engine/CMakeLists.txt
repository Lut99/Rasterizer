# Specify the libraries in this directory
add_library(VulkanEngine STATIC ${CMAKE_CURRENT_SOURCE_DIR}/RenderEngine.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanEngine PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanEngine)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
