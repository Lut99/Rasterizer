# Specify the libraries in this directory
add_library(VulkanGpu STATIC ${CMAKE_CURRENT_SOURCE_DIR}/GPU.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Surface.cpp ${CMAKE_CURRENT_SOURCE_DIR}/QueueInfo.cpp ${CMAKE_CURRENT_SOURCE_DIR}/SwapchainInfo.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanGpu PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanGpu)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
