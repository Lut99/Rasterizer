# Specify the libraries in this directory
add_library(VulkanDepthTesting STATIC ${CMAKE_CURRENT_SOURCE_DIR}/DepthStencil.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanDepthTesting PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanDepthTesting)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
