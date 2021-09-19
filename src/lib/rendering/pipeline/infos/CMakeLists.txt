# Specify the libraries in this directory
add_library(VulkanPipelineInfos STATIC ${CMAKE_CURRENT_SOURCE_DIR}/PipelineInfo.cpp
                                       ${CMAKE_CURRENT_SOURCE_DIR}/ShaderStageInfo.cpp
                                       ${CMAKE_CURRENT_SOURCE_DIR}/VertexInputStateInfo.cpp
                                       ${CMAKE_CURRENT_SOURCE_DIR}/ColorLogicInfo.cpp
                                       ${CMAKE_CURRENT_SOURCE_DIR}/PipelineLayoutInfo.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanPipelineInfos PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanPipelineInfos)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
