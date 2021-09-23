# Add the library
add_library(ObjLoader STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ObjLoader.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader.cpp)

# Set the dependencies for this library:
target_include_directories(ObjLoader PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS ObjLoader)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)