# using Clang
add_compile_options(-Wall -Wextra -Wpedantic)
#add_compile_options(-fcxx-exceptions -fdata-sections -ffunction-sections -fomit-frame-pointer)
add_compile_options(${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT})
add_compile_options(${DEFAULT_CXX_EXCEPTION_HANDLING})
#add_compile_options(-march=x86-64-v3)
