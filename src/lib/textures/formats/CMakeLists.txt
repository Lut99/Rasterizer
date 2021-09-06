# Add the subdirectories
add_subdirectory(png)
add_subdirectory(jpg)
add_subdirectory(auxillary)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
