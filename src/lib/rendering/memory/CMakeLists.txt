# Specify the libraries in this directory
add_library(VulkanMemory STATIC ${CMAKE_CURRENT_SOURCE_DIR}/MemoryManager.cpp ${CMAKE_CURRENT_SOURCE_DIR}/MemoryPool.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Buffer.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Image.cpp)

# Set the dependencies for this library:
target_include_directories(VulkanMemory PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanMemory)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
