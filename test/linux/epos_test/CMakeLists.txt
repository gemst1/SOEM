add_library(SERVO servo_def.c)
add_library(ECAT_DC ecat_dc.c)

include_directories(test/linux/epos_test)

set(SOURCES epos_test.c)
add_executable(epos_test ${SOURCES})
target_link_libraries(epos_test soem)
install(TARGETS epos_test DESTINATION bin)

#set(SOURCES ecat_dc.c)
#add_executable(ecat_dc ${SOURCES})
#target_link_libraries(ecat_dc soem)
#install(TARGETS ecat_dc DESTINATION bin)
#
#set(SOURCES servo_def.c)
#add_executable(servo_def ${SOURCES})
#target_link_libraries(servo_def soem)
#install(TARGETS servo_def DESTINATION bin)

set(SOURCES epos_dc_test.c)
add_executable(epos_dc_test ${SOURCES})
target_link_libraries(epos_dc_test soem SERVO ECAT_DC m)
install(TARGETS epos_dc_test DESTINATION bin)

set(SOURCES example.c)
add_executable(example ${SOURCES})
target_link_libraries(example soem)
install(TARGETS example DESTINATION bin)

set(SOURCES cantest.c)
add_executable(cantest ${SOURCES})
target_link_libraries(cantest soem)
install(TARGETS cantest DESTINATION bin)

