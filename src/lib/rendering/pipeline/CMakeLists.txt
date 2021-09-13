# Specify the libraries in this directory
add_library(VulkanPipeline STATIC ${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp ${CMAKE_CURRENT_SOURCE_DIR}/PipelineProperties.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Pipeline.cpp ${CMAKE_CURRENT_SOURCE_DIR}/PipelinePool.cpp )

# Set the dependencies for this library:
target_include_directories(VulkanPipeline PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanPipeline)

# Add the subdirectories
add_subdirectory(properties)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
