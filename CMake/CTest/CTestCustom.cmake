#
# This file is run prior to the unit tests for preparing the testing
# environment. Currently only required / necessary for running the
# tests on the Android device emulator.
#

# Restore the required settings of the CMake configuration step
INCLUDE(dcmtkCTestConfig.cmake NO_POLICY_SCOPE)

# Emulate some required CMake commands while running inside CTest
INCLUDE(${DCMTK_CMAKE_INCLUDE}CMake/CTest/dcmtkCTestMacros.cmake)

# Prepare the Android testing environment
IF(ANDROID)
    # Load required functions for controlling the Android device emulator
    INCLUDE(${DCMTK_CMAKE_INCLUDE}CMake/dcmtkUseAndroidSDK.cmake)

    # Prepare the emulator
    DCMTK_ANDROID_START_EMULATOR(DCMTK_ANDROID_EMULATOR_INSTANCE)
    DCMTK_ANDROID_WAIT_FOR_EMULATOR(DCMTK_ANDROID_EMULATOR_INSTANCE)

    # Get the emulator's name for emitting a shutdown command when done
    DCMTK_ANDROID_GET_OBJECT_PROPERTIES(DCMTK_ANDROID_EMULATOR_INSTANCE)
    DCMTK_ANDROID_STOP_EMULATOR_COMMAND(STOP_EMULATOR ${EMULATOR_NAME})
    DCMTK_ANDROID_EMULATOR_SHUTDOWN_MESSAGE(STOP_MESSAGE)
    MESSAGE_COMMAND(STOP_MESSAGE ${STOP_MESSAGE})
    # CTEST_CUSTOM_POST_TESTS executes a sequence of 'non CMake' commands separated
    # by ';' characters. Therefore the sequence a single command consists of must
    # be translated appropriately first
    STRING(REPLACE ";" " " STOP_EMULATOR "${STOP_EMULATOR}")
    STRING(REPLACE ";" " " STOP_MESSAGE "${STOP_MESSAGE}")
    SET(CLEANUP_COMMAND "${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dcmtkCTestRunAndroid.cmake")
    # Register the cleanup hooks
    SET(CTEST_CUSTOM_POST_TEST "${CLEANUP_COMMAND}" "${STOP_MESSAGE}" "${STOP_EMULATOR}")

    # Prepare the test script, most importantly set the current emulator instance handle
    CONFIGURE_FILE(
        ${DCMTK_CMAKE_INCLUDE}CMake/CTest/dcmtkCTestRunAndroid.cmake.in
        ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dcmtkCTestRunAndroid.cmake
        @ONLY
    )

    # Collect the list of required libraries
    IF(BUILD_SHARED_LIBS)
        FOREACH(LIBRARY ${DCMTK_ALL_LIBRARIES})
            LIST(APPEND DCMTK_CREATED_SHARED_LIBRARIES ${CMAKE_BINARY_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX}.${DCMTK_PACKAGE_VERSION})
        ENDFOREACH()
    ENDIF(BUILD_SHARED_LIBS)

    # Transmit the required executables and libraries to the emulator
    DCMTK_ANDROID_PUSH(DCMTK_ANDROID_EMULATOR_INSTANCE
        ${ANDROID_RUNTIME_LIBRARIES}
        ${DCMTK_LIBRARY_DEPENDENCIES}
        ${DCMTK_CREATED_SHARED_LIBRARIES}
        ${DCMTK_TEST_EXECUTABLES}
        DESTINATION ${ANDROID_TEMPORARY_FILES_LOCATION}
    )

    # Set executable permissions
    FOREACH(TEST_EXECUTABLE ${DCMTK_TEST_EXECUTABLES})
        GET_FILENAME_COMPONENT(NAME ${TEST_EXECUTABLE} NAME)
        DCMTK_ANDROID_SHELL(DCMTK_ANDROID_EMULATOR_INSTANCE
            COMMAND chmod 755 ${ANDROID_TEMPORARY_FILES_LOCATION}/${NAME}
            OUTPUT_QUIET
            ERROR_QUIET
        )
    ENDFOREACH()

    # Create necessary softlinks
    IF(BUILD_SHARED_LIBS)
        FOREACH(LIBRARY ${DCMTK_ALL_LIBRARIES})
            DCMTK_ANDROID_SHELL(DCMTK_ANDROID_EMULATOR_INSTANCE
            COMMAND ln -s ./${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX}.${DCMTK_PACKAGE_VERSION} ${ANDROID_TEMPORARY_FILES_LOCATION}/${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX}.${DCMTK_ABI_VERSION}
            OUTPUT_QUIET
            ERROR_QUIET
            )
        ENDFOREACH()
    ENDIF(BUILD_SHARED_LIBS)
ENDIF(ANDROID)
