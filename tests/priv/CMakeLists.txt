find_package(GTest REQUIRED)
find_package(Boost REQUIRED)

set(TEST_NAME
    ${PROJECT_NAME}-fake-connector)

set(TEST_FILE
    ${CMAKE_SOURCE_DIR}/tests/priv/fake-connector.cpp)

foreach(name file IN ZIP_LISTS TEST_NAME TEST_FILE)
    add_executable(${name} ${CMAKE_SOURCE_DIR}/tests/main.cpp ${file})
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/inc/priv Boost::headers)
    target_link_libraries(${name} PRIVATE ${CMAKE_PROJECT_NAME} GTest::GTest)
    add_test(NAME ${name} COMMAND ${name})
endforeach()
