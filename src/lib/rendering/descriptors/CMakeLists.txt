# Specify the libraries in this directory
add_library(VulkanDescriptors STATIC ${CMAKE_CURRENT_SOURCE_DIR}/DescriptorPool.cpp ${CMAKE_CURRENT_SOURCE_DIR}/DescriptorSet.cpp ${CMAKE_CURRENT_SOURCE_DIR}/DescriptorSetLayout.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanDescriptors PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanDescriptors)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
