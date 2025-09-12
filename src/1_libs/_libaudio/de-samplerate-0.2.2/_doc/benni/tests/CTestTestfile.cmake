# CMake generated Testfile for 
# Source directory: D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests
# Build directory: D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(misc_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/misc_test.exe")
set_tests_properties(misc_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;21;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(termination_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/termination_test.exe")
set_tests_properties(termination_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;25;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(callback_hang_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/callback_hang_test.exe")
set_tests_properties(callback_hang_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;29;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(downsample_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/downsample_test.exe")
set_tests_properties(downsample_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;33;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(simple_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/simple_test.exe")
set_tests_properties(simple_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;37;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(callback_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/callback_test.exe")
set_tests_properties(callback_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;41;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(reset_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/reset_test.exe")
set_tests_properties(reset_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;45;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(clone_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/clone_test.exe")
set_tests_properties(clone_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;49;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(nullptr_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/nullptr_test.exe")
set_tests_properties(nullptr_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;53;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(multi_channel_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/multi_channel_test.exe")
set_tests_properties(multi_channel_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;61;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(varispeed_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/varispeed_test.exe" "util.c" "util.h")
set_tests_properties(varispeed_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;68;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(float_short_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/float_short_test.exe")
set_tests_properties(float_short_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;72;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
add_test(snr_bw_test "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/tests/snr_bw_test.exe" "util.c" "util.h")
set_tests_properties(snr_bw_test PROPERTIES  WORKING_DIRECTORY "D:/dev/_benni_dev/src/de-samplerate-0.2.2/benni/src" _BACKTRACE_TRIPLES "D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;80;add_test;D:/dev/_benni_dev/src/de-samplerate-0.2.2/tests/CMakeLists.txt;0;")
