# Specify the libraries in this directory
add_library(VulkanShaders STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ShaderPool.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanShaders PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanShaders)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
