# Add the subdirectories
add_subdirectory(materials)
add_subdirectory(models)
# add_subdirectory(textures)
add_subdirectory(window)
add_subdirectory(rendering)
add_subdirectory(world)
add_subdirectory(ecs)
add_subdirectory(auxillary)
add_subdirectory(tools)
add_subdirectory(glm)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
