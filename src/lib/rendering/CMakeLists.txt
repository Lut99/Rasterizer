# Add the RenderEngine itself
add_library(RenderSystem STATIC ${CMAKE_CURRENT_SOURCE_DIR}/RenderSystem.cpp)

# Set the dependencies for this library:
target_include_directories(RenderSystem PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS RenderSystem)

# Add the subdirectories
add_subdirectory(sampling)
add_subdirectory(views)
add_subdirectory(synchronization)
add_subdirectory(renderpass)
add_subdirectory(pipeline)
add_subdirectory(shaders)
add_subdirectory(depthtesting)
add_subdirectory(commandbuffers)
add_subdirectory(descriptors)
add_subdirectory(memory)
add_subdirectory(swapchain)
add_subdirectory(gpu)
add_subdirectory(instance)
add_subdirectory(auxillary)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
