# Specify the libraries in this directory
add_library(VulkanSwapchain STATIC ${CMAKE_CURRENT_SOURCE_DIR}/Swapchain.cpp ${CMAKE_CURRENT_SOURCE_DIR}/SwapchainFrame.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ConceptualFrame.cpp ${CMAKE_CURRENT_SOURCE_DIR}/FrameManager.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanSwapchain PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanSwapchain)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
