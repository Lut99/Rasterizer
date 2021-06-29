# Specify the libraries in this directory
add_library(VulkanRenderpass STATIC ${CMAKE_CURRENT_SOURCE_DIR}/RenderPass.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Subpass.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanRenderpass PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanRenderpass)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
