set(PROJECT_NAME skarnc)

find_package(GTest REQUIRED)
#find_package(spdlog REQUIRED)
#find_package(Boost REQUIRED COMPONENTS parser)
#find_package(Boost REQUIRED COMPONENTS asio)
#find_package(OpenSSL REQUIRED)

get_source_files(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${SOURCE_FILES})
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

#target_link_libraries(${PROJECT_NAME} PUBLIC
#    spdlog::spdlog
#    Boost::parser
#    OpenSSL::Crypto
#    OpenSSL::SSL
#)

#---------- tests ------------

set(TEST_PROJECT_NAME ${PROJECT_NAME}-tests)
get_source_files(TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test)
add_executable(${TEST_PROJECT_NAME} ${TEST_FILES})
add_dependencies(tests ${TEST_PROJECT_NAME})

target_link_libraries(${TEST_PROJECT_NAME} PRIVATE
    ${PROJECT_NAME}
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
)

gtest_discover_tests(${TEST_PROJECT_NAME} DISCOVERY_TIMEOUT 60 DISCOVERY_MODE PRE_TEST)
