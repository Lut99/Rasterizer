# Add the subdirectories
add_subdirectory(rendering)
add_subdirectory(window)
add_subdirectory(models)
add_subdirectory(ecs)
add_subdirectory(tools)
add_subdirectory(glm)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
