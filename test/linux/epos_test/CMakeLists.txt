
set(SOURCES epos_test.c)
add_executable(epos_test ${SOURCES})
target_link_libraries(epos_test soem)
install(TARGETS epos_test DESTINATION bin)

