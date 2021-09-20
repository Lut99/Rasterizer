# Add the library
add_library(MtlTokenizer STATIC ${CMAKE_CURRENT_SOURCE_DIR}/Tokenizer.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Terminal.cpp)
# No need to include the ValueTerminal, since it's a template and thus needs to be known in the .hpp

# Set the dependencies for this library:
target_include_directories(MtlTokenizer PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS MtlTokenizer)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
