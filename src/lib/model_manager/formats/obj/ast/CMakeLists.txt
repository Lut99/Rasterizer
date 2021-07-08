# Add the library
add_library(ObjAST STATIC ${CMAKE_CURRENT_SOURCE_DIR}/NonTerminal.cpp)

# Set the dependencies for this library:
target_include_directories(ObjAST PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS ObjAST)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)