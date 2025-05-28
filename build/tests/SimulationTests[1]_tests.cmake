add_test([=[SimulationTest.CanCreateSimulation]=]  /home/jakubm/Documents/Cpp/ProjektPROI/209.1-robot-sprzatajacy/build/tests/SimulationTests [==[--gtest_filter=SimulationTest.CanCreateSimulation]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[SimulationTest.CanCreateSimulation]=]  PROPERTIES WORKING_DIRECTORY /home/jakubm/Documents/Cpp/ProjektPROI/209.1-robot-sprzatajacy/build/tests SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  SimulationTests_TESTS SimulationTest.CanCreateSimulation)
