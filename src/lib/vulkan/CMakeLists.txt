# Add the subdirectories
add_subdirectory(engine)
add_subdirectory(synchronization)
add_subdirectory(renderpass)
add_subdirectory(pipeline)
add_subdirectory(commandbuffers)
add_subdirectory(descriptors)
add_subdirectory(memory)
add_subdirectory(swapchain)
add_subdirectory(gpu)
add_subdirectory(instance)
add_subdirectory(auxillary)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
