# Add the library
add_library(ObjLoader STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ObjLoader.cpp)

# Set the dependencies for this library:
target_include_directories(ObjLoader PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS ObjLoader)

# Also add nested directories
add_subdirectory(tokenizer)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)