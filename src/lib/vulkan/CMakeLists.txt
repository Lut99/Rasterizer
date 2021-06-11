# Add the subdirectories
add_subdirectory(commandbuffers)
add_subdirectory(descriptors)
add_subdirectory(memory)
# add_subdirectory(commandbuffers)
add_subdirectory(swapchain)
add_subdirectory(gpu)
add_subdirectory(instance)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
