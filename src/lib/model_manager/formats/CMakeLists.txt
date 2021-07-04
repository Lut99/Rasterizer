# Add the subdirectories
add_subdirectory(obj)
add_subdirectory(auxillary)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
