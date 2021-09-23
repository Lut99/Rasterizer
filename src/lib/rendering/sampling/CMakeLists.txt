# Specify the libraries in this directory
add_library(VulkanSampler STATIC ${CMAKE_CURRENT_SOURCE_DIR}/Sampler.cpp ${CMAKE_CURRENT_SOURCE_DIR}/SamplerPool.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanSampler PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanSampler)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
