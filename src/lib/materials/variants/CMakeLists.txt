# Dive into each folder to compile the shaders
add_subdirectory(simple)
add_subdirectory(simple_coloured)
add_subdirectory(simple_textured)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
