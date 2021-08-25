# Specify the libraries in this directory
add_library(VulkanImageViews STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ImageView.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ImageViewPool.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanImageViews PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanImageViews)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
