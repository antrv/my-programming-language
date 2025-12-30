function(get_source_files output_variable source_dir)
    file(GLOB_RECURSE _source_files CONFIGURE_DEPENDS
        "${source_dir}/*.cpp"
        "${source_dir}/*.cxx"
        "${source_dir}/*.h"
        "${source_dir}/*.hpp"
        "${source_dir}/*.inl"
        "${source_dir}/*.ixx"
    )

    source_group(TREE "${source_dir}" FILES ${_source_files})
    set(${output_variable} ${_source_files} PARENT_SCOPE)
endfunction()

function(print_cmake_variables)
    get_cmake_property(_variable_names VARIABLES)
    list (SORT _variable_names)
    foreach (_variable_name ${_variable_names})
        message(STATUS "${_variable_name}=${${_variable_name}}")
    endforeach()
endfunction()
