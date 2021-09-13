# Specify the libraries in this directory
add_library(VulkanAuxillary STATIC ${CMAKE_CURRENT_SOURCE_DIR}/VulkanSwap.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Rectangle.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Vertex.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanAuxillary PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanAuxillary)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
