cmake_minimum_required(VERSION 3.8)

function(create_test TEST_NAME)
  add_executable("${TEST_NAME}Test" "${TEST_NAME}Test.cc")
  target_link_libraries("${TEST_NAME}Test"
    ${PROJECT_NAME}
    gtest_main)
  add_test(NAME "${TEST_NAME}Test" COMMAND "${TEST_NAME}Test")
endfunction()

create_test(MonoWorker)
create_test(PolyWorker)
create_test(Flag)