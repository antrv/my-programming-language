set(PROJECT_NAME skarn)

get_source_files(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_dependencies(programs ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE
    skarnc
)
