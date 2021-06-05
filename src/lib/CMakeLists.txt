# Add the subdirectories

add_subdirectory(vulkan)
add_subdirectory(tools)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
