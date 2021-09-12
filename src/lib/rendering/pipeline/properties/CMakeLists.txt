# Specify the libraries in this directory
add_library(VulkanPipelineProperties STATIC
            ${CMAKE_CURRENT_SOURCE_DIR}/ShaderStage.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/VertexBinding.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/VertexAttribute.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/VertexInputState.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/InputAssemblyState.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/DepthTesting.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ViewportTransformation.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Rasterization.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Multisampling.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ColorBlending.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ColorLogic.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/PipelineLayout.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanPipelineProperties PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanPipelineProperties)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
