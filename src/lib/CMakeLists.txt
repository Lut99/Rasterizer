# Add the subdirectories

add_subdirectory(render_engine)
add_subdirectory(tools)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
