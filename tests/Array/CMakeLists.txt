# Specify the libraries in this directory
add_library(ArrayTest STATIC ${CMAKE_CURRENT_SOURCE_DIR}/initialization.cpp ${CMAKE_CURRENT_SOURCE_DIR}/consistancy.cpp ${CMAKE_CURRENT_SOURCE_DIR}/resize.cpp ${CMAKE_CURRENT_SOURCE_DIR}/push_erase.cpp ${CMAKE_CURRENT_SOURCE_DIR}/constructors.cpp)

# Set the dependencies for this library:
target_include_directories(ArrayTest PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND ARRAY_TEST_LIBS ArrayTest)

# Carry the list to the parent scope
set(ARRAY_TEST_LIBS "${ARRAY_TEST_LIBS}" PARENT_SCOPE)
