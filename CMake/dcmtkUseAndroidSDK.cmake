#
# Functions for detection and usage of then Android SDK
# Used when cross compiling for Android
#
# The Android device emulator (included in the SDK) can
# be "remote" controlled by the functions and macros
# implemented in this file.
# They are employing a pseudo object-oriented approach
# to handle an emulator instance, that is an emulator
# instance is described by a so called emulator instance
# object handle which has to be passed to all functions
# operating on it as their first argument.
# Android emulator object handles consist of the
# following properties, which need to be tracked to
# describe and Android emulator instance:
#  EMULATOR_STATE: the state of the emulator instance,
#    can be one of the following values:
#       * STARTING - the instance has been initialized
#                    and the emulator has been executed
#       * RUNNING  - the emulated device is online and
#                    can receive instructions
#       * STOPPED  - the emulated device has been
#                    shutdown or is currently being
#                    shutdown
#  EMULATOR_UUID: a generated ID to identify the emulator
#    instance. Meant to prevent accessing the wrong device
#    in case multiple Android devices are accessible.
#  EMULATOR_NAME: the name of the emulator instance.
#    all running emulator instances are named by the SDK
#    in a locally unique fashion. The name will only be
#    available in case the state is 'RUNNING', since
#    only the name of running devices can be retrieved
#    by UUID matching.
#    An emulator instance is accessed by the different
#    tools of the NDK (e. g. 'adb') by referring to the
#    instance with the value of EMULATOR_NAME.

INCLUDE(CMakeParseArguments)

#
# 'Unpacks' an Android emulator instance handle object to
# individual components.
# VAR - the emulator instance handle object to unpack
# EMULATOR_STATE - will contain the emulators state
# EMULATOR_UUID  - will contain the generated UUID that
#                  identifies the emulator instance
# EMULATOR_NAME  - will contain the instance name, required
#                  to access the emulator via 'adb' etc.
# All three outputs will be unset if the object in VAR
# is not a valid emulator instance handle.
# All additional arguments will be ignored.
#
MACRO(DCMTK_ANDROID_GET_OBJECT_PROPERTIES VAR)
    LIST(LENGTH ${VAR} ${VAR}_LENGTH)
    IF(${VAR}_LENGTH EQUAL 3)
        LIST(GET ${VAR} 0 EMULATOR_STATE)
        LIST(GET ${VAR} 1 EMULATOR_UUID)
        LIST(GET ${VAR} 2 EMULATOR_NAME)
    ELSE()
        UNSET(EMULATOR_STATE)
        UNSET(EMULATOR_UUID)
        UNSET(EMULATOR_NAME)
    ENDIF()
ENDMACRO(DCMTK_ANDROID_GET_OBJECT_PROPERTIES)

#
# Stores the properties of an emulator instance inside an
# emulator instance handle object.
# VAR - the name of the variable that shall contain the
#       resulting instance handle object.
# EMULATOR_STATE - the state to set
# EMULATOR_UUID  - the UUID to set
# EMULATOR_NAME  - the name to set
# All additional arguments will be ignored.
#
MACRO(DCMTK_ANDROID_SET_OBJECT_PROPERTIES VAR EMULATOR_STATE EMULATOR_UUID EMULATOR_NAME)
    SET(${VAR} "${EMULATOR_STATE}" "${EMULATOR_UUID}" "${EMULATOR_NAME}" CACHE INTERNAL "")
ENDMACRO(DCMTK_ANDROID_SET_OBJECT_PROPERTIES)

#
# Destroys an instance handle object.
# VAR - the name of the variable its emulator instance handle
#       shall be object destroyed
# All additional arguments will be ignored.
#
MACRO(DCMTK_ANDROID_DESTROY_OBJECT VAR)
    UNSET("${VAR}" CACHE)
ENDMACRO(DCMTK_ANDROID_DESTROY_OBJECT)

#
# Tries to detect any required shared objects within the NDK
# VAR - the name of the variable that will receive a list
#       of required shared object files (if any)
# Will set VAR to an empty list if no shared objects are
# required (e. g. when they are linked statically).
# All additional arguments will be ignored.
#
FUNCTION(DCMTK_ANDROID_FIND_RUNTIME_LIBRARIES VAR)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
    FOREACH(DIR ${ANDROID_STL_INCLUDE_DIRS})
        IF(CMAKE_VERSION VERSION_LESS 2.8.11)
            GET_FILENAME_COMPONENT(DIR "${DIR}" PATH)
        ELSE()
            GET_FILENAME_COMPONENT(DIR "${DIR}" DIRECTORY)
        ENDIF()
        LIST(APPEND ANDROID_STL_LIBRARY_DIRS "${DIR}")
    ENDFOREACH()
    FIND_LIBRARY(ANDROID_STL_SHARED_OBJECT ${ANDROID_STL} PATHS ${ANDROID_STL_LIBRARY_DIRS} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    IF(ANDROID_STL_SHARED_OBJECT)
        SET("${VAR}" ${ANDROID_STL_SHARED_OBJECT} PARENT_SCOPE)
    ENDIF()
ENDFUNCTION(DCMTK_ANDROID_FIND_RUNTIME_LIBRARIES)

#
# Tries to detect the required executables and scripts automatically,
# which should be contained in the Android SDK. Emits an error that
# will halt the configuration process if that is not possible.
# Will also detect all available AVDs after tool detection was
# successful.
# All arguments will be ignored.
#
FUNCTION(DCMTK_SETUP_ANDROID_EMULATOR)
    IF(NOT ANDROID_TEMPORARY_FILES_LOCATION)
        SET(ANDROID_TEMPORARY_FILES_LOCATION "/cache" CACHE STRING "The path on the Android device that should be used for temporary files")
    ENDIF()
    IF(NOT ANDROID_SDK_ROOT)
        IF(CMAKE_HOST_SYSTEM MATCHES "Windows.*")
            FILE(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" ANDROID_SDK_SEARCH_PATHS)
            SET(ANDROID_SDK_ROOT "${ANDROID_SDK_SEARCH_PATHS}/android-sdk" CACHE PATH "Location of the Android SDK")
        ELSE()
            SET(ANDROID_SDK_ROOT "/opt/android-sdk" CACHE PATH "Location of the Android SDK")
        ENDIF()
    ENDIF()
    FIND_PROGRAM(ANDROID_EMULATOR_PROGRAM emulator PATHS ${ANDROID_SDK_ROOT} PATH_SUFFIXES tools NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    IF(CMAKE_HOST_SYSTEM MATCHES "Windows.*")
        FIND_PROGRAM(ANDROID_ANDROID_PROGRAM android.bat PATHS ${ANDROID_SDK_ROOT} PATH_SUFFIXES tools NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    ELSE()
        FIND_PROGRAM(ANDROID_ANDROID_PROGRAM android PATHS ${ANDROID_SDK_ROOT} PATH_SUFFIXES tools NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    ENDIF()
    FIND_PROGRAM(ANDROID_ADB_PROGRAM adb PATHS ${ANDROID_SDK_ROOT} PATH_SUFFIXES platform-tools NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    IF(NOT ANDROID_EMULATOR_PROGRAM OR NOT ANDROID_ANDROID_PROGRAM OR NOT ANDROID_ADB_PROGRAM)
        MESSAGE(FATAL_ERROR
            "Failed to detect the Android SDK, please set ANDROID_SDK_ROOT to the location of your Android SDK"
            "or set the missing tools manually!"
        )
    ELSE()
        EXECUTE_PROCESS(COMMAND "${ANDROID_ANDROID_PROGRAM}" list avd RESULT_VARIABLE RESULT OUTPUT_VARIABLE OUTPUT ERROR_QUIET)
        STRING(REGEX MATCHALL "Name:[ \t]*[^\r\n]*" ANDROID_AVAILABLE_AVDS ${OUTPUT})
        STRING(REGEX REPLACE "Name:[ \t]*([^\r\n;]*)" "\\1" ANDROID_AVAILABLE_AVDS "${ANDROID_AVAILABLE_AVDS}")
        SET(ANDROID_EMULATOR_AVD "${ANDROID_EMULATOR_AVD}" CACHE STRING "Android emulator Android Virtual Device (AVD) configuration" FORCE)
        SET_PROPERTY(CACHE ANDROID_EMULATOR_AVD PROPERTY STRINGS ${ANDROID_AVAILABLE_AVDS})
    ENDIF()
ENDFUNCTION(DCMTK_SETUP_ANDROID_EMULATOR)

#
# Enumerates all currently available Android devices
# (started emulators and connected devices) and separates
# them into two different lists.
# ONLINE  - the name of the variable that will be set to a
#           list of all currently online devices' names
#           (devices that can receive instructions)
# OFFLINE - the name of the variable that will be set to a
#           list of all currently offline device's names
#           (devices that are connected but cannot receive
#           instructions)
# Will ignore all additional arguments.
#
FUNCTION(DCMTK_ANDROID_LIST_EMULATORS ONLINE OFFLINE)
    DCMTK_SETUP_ANDROID_EMULATOR()
    IF(ANDROID_ADB_PROGRAM)
        EXECUTE_PROCESS(
            COMMAND "${ANDROID_ADB_PROGRAM}" devices
            RESULT_VARIABLE RESULT
            OUTPUT_VARIABLE DEVICES_RAW
            ERROR_QUIET
        )
        STRING(REPLACE "\n" ";" DEVICES "${DEVICES_RAW}")
        FOREACH(DEVICE ${DEVICES})
            STRING(REGEX REPLACE "(.+)\t(.+)" "\\1;\\2" DS "${DEVICE}")
            LIST(LENGTH DS DSL)
            IF(DSL EQUAL 2)
                LIST(GET DS 0 EMULATOR_NAME)
                LIST(GET DS 1 EMULATOR_STATE)
                IF(EMULATOR_STATE MATCHES "^device$")
                    LIST(APPEND "${ONLINE}" ${EMULATOR_NAME})
                ELSE()
                    LIST(APPEND "${OFFLINE}" ${EMULATOR_NAME})
                ENDIF()
            ENDIF()
        ENDFOREACH()
    ENDIF()
    SET("${ONLINE}" ${${ONLINE}} PARENT_SCOPE)
    SET("${OFFLINE}" ${${OFFLINE}} PARENT_SCOPE)
ENDFUNCTION(DCMTK_ANDROID_LIST_EMULATORS)

#
# Generate a random ID that is hopefully unique
# enough to be used as the instance UUID.
# VAR - the name of the variable that will receive
#       the generated UUID as a string value
# Will ignore all additional arguments.
#
IF(CMAKE_VERSION VERSION_LESS 2.8.11)
MACRO(DCMTK_ANDROID_EMULATOR_GENERATE_UUID VAR)
    STRING(RANDOM LENGTH 20 ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" ${VAR})
ENDMACRO(DCMTK_ANDROID_EMULATOR_GENERATE_UUID)
ELSE(CMAKE_VERSION VERSION_LESS 2.8.11)
FUNCTION(DCMTK_ANDROID_EMULATOR_GENERATE_UUID VAR)
    STRING(RANDOM LENGTH 20 RAND)
    STRING(TIMESTAMP TM)
    SET(${VAR} "${TM}${RAND}")
    STRING(MD5 ${VAR} ${${VAR}})
    SET(${VAR} ${${VAR}} PARENT_SCOPE)
ENDFUNCTION(DCMTK_ANDROID_EMULATOR_GENERATE_UUID)
ENDIF(CMAKE_VERSION VERSION_LESS 2.8.11)

#
# Tries to query the UUID property of an accessible Android device.
# EMULATOR_NAME - the device name, as returned by DCMTK_ANDROID_LIST_EMULATORS
# VAR           - the name of the variable that will be set to the device UUID
# Will unset the variable referred to by VAR if no device with the given name
# is accessible or the device is offline.
# Will ignore all additional arguments.
#
FUNCTION(DCMTK_ANDROID_GET_EMULATOR_UUID EMULATOR_NAME VAR)
    EXECUTE_PROCESS(
        COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" shell getprop "ro.emu.uuid"
        RESULT_VARIABLE RESULT
        OUTPUT_VARIABLE OUTPUT
        ERROR_QUIET
    )
    DCMTK_UNSET_PARENT_SCOPE(${VAR})
    IF(NOT RESULT)
        STRING(STRIP "${OUTPUT}" UUID)
        IF(UUID)
            SET("${VAR}" ${UUID} PARENT_SCOPE)
        ENDIF()
    ENDIF()
ENDFUNCTION(DCMTK_ANDROID_GET_EMULATOR_UUID)

#
# Retrieves the name of the emulator instance referred to by
# the given UUID. Will wait until the device becomes online
# to query the name.
# VAR           - the name of the variable that will be set
#                 to the device name
# EMULATOR_UUID - the emulator UUID of the device to inquired
# Will wait until all available devices are online or the right
# one has been found.
# Will ignore all additional arguments.
#
FUNCTION(DCMTK_ANDROID_GET_EMULATOR_NAME VAR EMULATOR_UUID)
    DCMTK_ANDROID_LIST_EMULATORS(ONLINE_EMULATORS OFFLINE_EMULATORS)
    FOREACH(EMULATOR ${ONLINE_EMULATORS})
        DCMTK_ANDROID_GET_EMULATOR_UUID("${EMULATOR}" UUID)
        IF(EMULATOR_UUID STREQUAL UUID)
            SET("${VAR}" "${EMULATOR}" PARENT_SCOPE)
            RETURN()
        ENDIF()
    ENDFOREACH()
    WHILE(OFFLINE_EMULATORS)
        LIST(GET OFFLINE_EMULATORS 0 EMULATOR)
        LIST(REMOVE_AT OFFLINE_EMULATORS 0)
        EXECUTE_PROCESS(
            COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR}" wait-for-device
            TIMEOUT 1
            RESULT_VARIABLE RESULT
            OUTPUT_QUIET
            ERROR_QUIET
        )
        IF(NOT RESULT)
            DCMTK_ANDROID_GET_EMULATOR_UUID("${EMULATOR}" UUID)
            IF(UUID)
                IF(EMULATOR_UUID STREQUAL UUID)
                    SET("${VAR}" "${EMULATOR}" PARENT_SCOPE)
                    RETURN()
                ENDIF()
            ELSE()
                LIST(APPEND OFFLINE_EMULATORS "${EMULATOR}")
            ENDIF()
        ELSE()
            LIST(APPEND OFFLINE_EMULATORS "${EMULATOR}")
        ENDIF()
    ENDWHILE()
ENDFUNCTION()

#
# Sets up and starts a new emulator instance or reuses an
# existing one.
# VAR - the emulator instance handle object to reuse or
#       initialize
# Will test if VAR refers to a valid emulator instance
# handle and, if so, ensure the device is started.
# Will initialize a new emulator instance and start it,
# if VAR does not refer to a valid emulator instance.
# Will ignore all additional arguments.
#
FUNCTION(DCMTK_ANDROID_START_EMULATOR VAR)
    DCMTK_SETUP_ANDROID_EMULATOR()
    IF(NOT ANDROID_EMULATOR_AVD)
        MESSAGE(FATAL_ERROR "Please select which Android emulator Android Virtual Device (AVD) configuration to use!")
    ELSE()
        DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
        IF(NOT EMULATOR_STATE)
            DCMTK_ANDROID_EMULATOR_GENERATE_UUID(EMULATOR_UUID)
        ELSEIF(EMULATOR_STATE STREQUAL "RUNNING")
            DCMTK_ANDROID_GET_EMULATOR_UUID("${EMULATOR_NAME}" UUID)
            # Do nothing if the running emulator instance is ok and can be reused.
            # Otherwise restart it.
            IF(UUID STREQUAL EMULATOR_UUID)
                MESSAGE(STATUS "Reusing already running Android device emulator...")
                RETURN()
            ENDIF()
        ELSEIF(EMULATOR_STATE STREQUAL "STARTING")
            # Is it really starting, or has somebody aborted it?
            MESSAGE(STATUS "Found previously started Android device emulator, checking if it's still present...")
            DCMTK_ANDROID_GET_EMULATOR_NAME(EMULATOR_NAME "${EMULATOR_UUID}")
            IF(EMULATOR_NAME)
              MESSAGE(STATUS "Found previously started Android device emulator, checking if it's still present... yes")
              DCMTK_ANDROID_SET_OBJECT_PROPERTIES(${VAR} RUNNING "${EMULATOR_UUID}" "${EMULATOR_NAME}")
              RETURN()
            ENDIF()
            MESSAGE(STATUS "Found previously started Android device emulator, checking if it's still present... no")
        ENDIF()
        MESSAGE(STATUS "Starting the Android device emulator...")
        IF(CMAKE_HOST_SYSTEM MATCHES "Windows.*")
            SET(COMMAND sh -c "${ANDROID_EMULATOR_PROGRAM} -avd ${ANDROID_EMULATOR_AVD} -no-boot-anim -prop ro.emu.uuid=${EMULATOR_UUID} >${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/android-emulator.log 2>&1 < /dev/null &")
        ELSE()
            SET(COMMAND sh -c "${ANDROID_EMULATOR_PROGRAM} -avd ${ANDROID_EMULATOR_AVD} -no-window -no-boot-anim -prop ro.emu.uuid=${EMULATOR_UUID} >${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/android-emulator.log 2>&1 < /dev/null &")
        ENDIF()
        EXECUTE_PROCESS(
            COMMAND ${COMMAND}
            RESULT_VARIABLE RESULT
            OUTPUT_QUIET
            ERROR_QUIET
        )
        IF(NOT RESULT)
            DCMTK_ANDROID_SET_OBJECT_PROPERTIES("${VAR}" STARTING "${EMULATOR_UUID}" "")
        ELSE()
            DCMTK_ANDROID_DESTROY_OBJECT("${VAR}")
            MESSAGE(FATAL_ERROR "Error starting Android emulator.")
        ENDIF()
    ENDIF()
ENDFUNCTION(DCMTK_ANDROID_START_EMULATOR)

#
# Restart adb/the emulated device in root mode so that we gain write access to
# the device.
# Newer versions of the SDK seem to require this for doing anything meaningful
# with it.
# EMULATOR_NAME - the name of the emulated device that shall be rooted.
# Will ignore all additional arguments.
#
FUNCTION(DCMTK_ANDROID_ADB_ROOT EMULATOR_NAME)
    EXECUTE_PROCESS(
        COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" root
        OUTPUT_QUIET
        ERROR_QUIET
    )
    # the SDK was seemingly designed by a five year old, the device will
    # become invisible while it is being rooted, therefore, wait until
    # it is ready again
    SET(STATUS 1)
    WHILE(STATUS)
        EXECUTE_PROCESS(
            COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" wait-for-device
            RESULT_VARIABLE STATUS
            OUTPUT_QUIET
            ERROR_QUIET
        )
    ENDWHILE()
ENDFUNCTION(DCMTK_ANDROID_ADB_ROOT)

#
# Waits until the given emulator instance becomes online (RUNNING).
# VAR - the emulator instance handle object referring to the emulator
#       to wait for
# Will fail with a warning message if the given variable does not
# refer to a valid emulator instance handle object.
# Will set the object's state to "RUNNING" when successful.
# Will wait indefinitely if no such device exists or the device does
# not become online.
# Will ignore all additional arguments.
#
FUNCTION(DCMTK_ANDROID_WAIT_FOR_EMULATOR VAR)
    DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
    IF(NOT EMULATOR_STATE)
        MESSAGE(AUTHOR_WARNING "Error: ${VAR} is not a valid Android emulator instance handle.")
    ELSEIF(EMULATOR_STATE STREQUAL "RUNNING")
        # Emulator is running, no need to wait!
    ELSEIF(EMULATOR_STATE STREQUAL "STOPPED")
        MESSAGE(WARNING "The Android emulator has stopped, aborting...")
    ELSE()
        MESSAGE(STATUS "Waiting until the Android device emulator is ready to receive instructions...")
        WHILE(NOT EMULATOR_NAME)
            DCMTK_ANDROID_GET_EMULATOR_NAME(EMULATOR_NAME "${EMULATOR_UUID}")
        ENDWHILE()
        DCMTK_ANDROID_ADB_ROOT("${EMULATOR_NAME}")
        DCMTK_ANDROID_SET_OBJECT_PROPERTIES("${VAR}" RUNNING "${EMULATOR_UUID}" "${EMULATOR_NAME}")
    ENDIF()
ENDFUNCTION(DCMTK_ANDROID_WAIT_FOR_EMULATOR)

#
# Prepare a command that will shutdown the given Android emulator when
# in the local shell run.
# VAR           - the name of the variable that will be set to the
#                 generated command
# EMULATOR_NAME - the name of the emulated device that shall be shut
#                 down
# Will unset the variable referred to by VAR if no such command can
# be generated for the current host platform (Windows).
# Will ignore all additional arguments.
#
MACRO(DCMTK_ANDROID_STOP_EMULATOR_COMMAND VAR EMULATOR_NAME)
    IF(CMAKE_HOST_SYSTEM MATCHES "Windows.*")
        UNSET("${VAR}")
    ELSE()
        SET("${VAR}" "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" emu kill)
    ENDIF()
ENDMACRO(DCMTK_ANDROID_STOP_EMULATOR_COMMAND)

#
# Prepare an emulator shutdown message or a warning message if automated
# shutdown is not possible.
# VAR - the name of the variable that will be set to the generated message
# Will ignore all additional arguments.
#
MACRO(DCMTK_ANDROID_EMULATOR_SHUTDOWN_MESSAGE VAR)
    IF(CMAKE_HOST_SYSTEM MATCHES "Windows.*")
        SET("${VAR}" WARNING "The Android device emulator can't be terminated automatically under Windows, please shutdown \"${EMULATOR_NAME}\" manually!")
    ELSE()
        SET("${VAR}" STATUS "Shutting down the Android device emulator...")
    ENDIF()
ENDMACRO(DCMTK_ANDROID_EMULATOR_SHUTDOWN_MESSAGE)

#
# Stops the emulator and sets its state to STOPPED when successful.
# VAR - the name of the variable referring to the emulator instance
#       handle object
# Will emit a warning message if the emulator can't be stopped and
# won't modify its state in that case.
# Will ignore all additional arguments.
#
FUNCTION(DCMTK_ANDROID_STOP_EMULATOR VAR)
    DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
    IF(NOT EMULATOR_STATE)
        MESSAGE(AUTHOR_WARNING "Error: ${VAR} is not a valid Android emulator instance handle.")
    ELSEIF(EMULATOR_STATE STREQUAL "STARTING")
        # Can't tell it to stop if it is not done starting
        DCMTK_ANDROID_WAIT_FOR_EMULATOR("${VAR}")
    ELSEIF(EMULATOR_STATE STREQUAL "STOPPED")
        # Emulator is already stopped, do nothing
        RETURN()
    ENDIF()
    DCMTK_ANDROID_EMULATOR_SHUTDOWN_MESSAGE(MESSAGE)
    MESSAGE(${MESSAGE})
    IF(NOT CMAKE_HOST_SYSTEM MATCHES "Windows.*")
        DCMTK_ANDROID_STOP_EMULATOR_COMMAND(COMMAND "${EMULATOR_NAME}")
        EXECUTE_PROCESS(
          COMMAND ${COMMAND}
          RESULT_VARIABLE RESULT
          OUTPUT_QUIET
          ERROR_QUIET
        )
        IF(NOT RESULT)
            DCMTK_ANDROID_SET_OBJECT_PROPERTIES("${VAR}" STOPPED "${EMULATOR_UUID}" "")
        ELSE()
            MESSAGE(WARNING "Unable to stop the android device emulator, please shutdown \"${EMULATOR_NAME}\" manually!")
        ENDIF()
    ENDIF()
ENDFUNCTION(DCMTK_ANDROID_STOP_EMULATOR)

#
# Uploads local files to the given Android device using 'adb push'.
# VAR - the emulator instance handle object referring to the target
#       device
# Requires the device to be running.
# Additional arguments will be parsed and interpreted similar to
# CMake's FILE(COPY ...) command.
#
FUNCTION(DCMTK_ANDROID_PUSH VAR)
    DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
    IF(NOT EMULATOR_STATE)
        MESSAGE(AUTHOR_WARNING "Error: ${VAR} is not a valid Android emulator instance handle.")
    ELSEIF(EMULATOR_STATE STREQUAL "RUNNING")
        CMAKE_PARSE_ARGUMENTS(DCMTK_ANDROID_PUSH "" "" "DESTINATION" ${ARGN})
        FOREACH(LOCAL_FILE ${DCMTK_ANDROID_PUSH_UNPARSED_ARGUMENTS})
            EXECUTE_PROCESS(
                COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" push "${LOCAL_FILE}" "${DCMTK_ANDROID_PUSH_DESTINATION}"
                RESULT_VARIABLE RESULT
                OUTPUT_QUIET
                ERROR_QUIET
            )
        ENDFOREACH()
    ELSE()
        MESSAGE(AUTHOR_WARNING "Error: the Android emulator \"${VAR}\" is not ready to receive commands")
    ENDIF()
ENDFUNCTION(DCMTK_ANDROID_PUSH)

#
# Downloads remote files from the given Android device using 'adb pull'.
# VAR - the emulator instance handle object referring to the target
#       device
# Requires the device to be running.
# Additional arguments will be parsed and interpreted similar to
# CMake's FILE(COPY ...) command.
#
FUNCTION(DCMTK_ANDROID_PULL VAR)
    DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
    IF(NOT EMULATOR_STATE)
        MESSAGE(AUTHOR_WARNING "Error: ${VAR} is not a valid Android emulator instance handle.")
    ELSEIF(EMULATOR_STATE STREQUAL "RUNNING")
        CMAKE_PARSE_ARGUMENTS(DCMTK_ANDROID_PULL "" "" "DESTINATION" ${ARGN})
        FOREACH(REMOTE_FILE ${DCMTK_ANDROID_PULL_UNPARSED_ARGUMENTS})
            EXECUTE_PROCESS(
                COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" pull "${REMOTE_FILE}" "${DCMTK_ANDROID_PULL_DESTINATION}"
                RESULT_VARIABLE RESULT
                OUTPUT_QUIET
                ERROR_QUIET
            )
        ENDFOREACH()
    ELSE()
        MESSAGE(AUTHOR_WARNING "Error: the Android emulator \"${VAR}\" is not ready to receive commands")
    ENDIF()
ENDFUNCTION(DCMTK_ANDROID_PULL)

#
# Executes a shell command on the given Android device using 'adb shell'.
# VAR - the emulator instance handle object referring to the target
#       device
# Requires the device to be running.
# Additional arguments will be parsed and interpreted similar to
# CMake's EXECUTE_PROCESS(...) command.
#
FUNCTION(DCMTK_ANDROID_SHELL VAR)
    DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
    IF(NOT EMULATOR_STATE)
        MESSAGE(AUTHOR_WARNING "Error: ${VAR} is not a valid Android emulator instance handle.")
    ELSEIF(EMULATOR_STATE STREQUAL "RUNNING")
        CMAKE_PARSE_ARGUMENTS(DCMTK_ANDROID_SHELL
            "OUTPUT_QUIET;ERROR_QUIET"
            "RESULT_VARIABLE;OUTPUT_VARIABLE;ERROR_VARIABLE;WORKING_DIRECTORY"
            "COMMAND"
            ${ARGN}
        )

        # Prepare commandline
        SET(COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" shell)
        IF(DCMTK_ANDROID_SHELL_WORKING_DIRECTORY)
            LIST(APPEND COMMAND "cd" "${DCMTK_ANDROID_SHELL_WORKING_DIRECTORY}" "&&")
        ENDIF()
        LIST(APPEND COMMAND "${DCMTK_ANDROID_SHELL_COMMAND}")

        # Inspect output variable parameters
        IF(DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE)
            SET(PUSH_OUTPUT_VAR TRUE)
            IF(DCMTK_ANDROID_SHELL_ERROR_VARIABLE)
                IF(DCMTK_ANDROID_SHELL_ERROR_VARIABLE STREQUAL DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE)
                    SET(STREAMS_MERGED TRUE)
                ELSE()
                    SET(PUSH_ERROR_VAR TRUE)
                ENDIF()
            ELSE()
                SET(DCMTK_ANDROID_SHELL_ERROR_VARIABLE DCMTK_ANDROID_SHELL_ERROR_MESSAGE_BUFFER)
            ENDIF()
        ELSE()
            SET(DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE DCMTK_ANDROID_SHELL_OUTPUT_MESSAGE_BUFFER)
            IF(DCMTK_ANDROID_SHELL_ERROR_VARIABLE)
                SET(PUSH_ERROR_VAR TRUE)
            ELSE()
                SET(STREAMS_MERGED TRUE)
            ENDIF()
        ENDIF()

        # Prefix to prevent collision of output capturing files
        IF(CMAKE_VERSION VERSION_LESS 2.8.7)
            STRING(RANDOM LENGTH 20 ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" PREFIX)
        ELSE()
            STRING(MD5 PREFIX "${DCMTK_ANDROID_SHELL_COMMAND}")
        ENDIF()

        # Prepare output redirection (buffering)
        IF(STREAMS_MERGED)
            LIST(APPEND COMMAND > "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_output" 2>&1)
        ELSE()
            LIST(APPEND COMMAND > "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_output" 2> "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_error")
        ENDIF()

        # Prepare capturing the result
        LIST(APPEND COMMAND "\;" echo -n $? > "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_result")

        # Run the command
        EXECUTE_PROCESS(
          COMMAND ${COMMAND}
          RESULT_VARIABLE ADB_RESULT
          OUTPUT_VARIABLE ADB_OUTPUT
          ERROR_VARIABLE ADB_OUTPUT
        )

        # Deal with it
        IF(ADB_RESULT)
            MESSAGE(WARNING "Error sending command to the Android emulator, adb reported the error: ${ADB_OUTPUT}")
            IF(DCMTK_ANDROID_SHELL_RESULT_VARIABLE)
                SET("${DCMTK_ANDROID_SHELL_RESULT_VARIABLE}" ${ADB_RESULT} PARENT_SCOPE)
            ENDIF()
        ELSE(ADB_RESULT)
            # Receive results
            DCMTK_ANDROID_PULL("${VAR}" "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_output" "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_error" "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_result"
                DESTINATION "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}"
            )
            # Cleanup temp files
            EXECUTE_PROCESS(
                COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" shell rm "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_output" "\;" rm "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_error" "\;" rm "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_result"
                RESULT_VARIABLE RESULT_QUIET
                OUTPUT_QUIET
                ERROR_QUIET
            )
            # Analyze results
            IF(DCMTK_ANDROID_SHELL_RESULT_VARIABLE)
                FILE(READ "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_result" "${DCMTK_ANDROID_SHELL_RESULT_VARIABLE}")
                SET("${DCMTK_ANDROID_SHELL_RESULT_VARIABLE}" ${${DCMTK_ANDROID_SHELL_RESULT_VARIABLE}} PARENT_SCOPE)
            ENDIF()
            IF(NOT DCMTK_ANDROID_SHELL_OUTPUT_QUIET OR (STREAMS_MERGED AND NOT DCMTK_ANDROID_SHELL_ERROR_QUIET))
                FILE(READ "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_output" "${DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE}")
                IF(PUSH_OUTPUT_VAR)
                    SET("${DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE}" ${${DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE}} PARENT_SCOPE)
                ELSE()
                    MESSAGE("${${DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE}}")
                ENDIF()
            ENDIF()
            IF(NOT DCMTK_ANDROID_SHELL_ERROR_QUIET AND NOT STREAMS_MERGED)
                FILE(READ "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_error" "${DCMTK_ANDROID_SHELL_ERROR_VARIABLE}")
                IF(PUSH_ERROR_VAR)
                    SET("${DCMTK_ANDROID_SHELL_ERROR_VARIABLE}" ${${DCMTK_ANDROID_SHELL_ERROR_VARIABLE}} PARENT_SCOPE)
                ELSE()
                    MESSAGE("${${DCMTK_ANDROID_SHELL_ERROR_VARIABLE}}")
                ENDIF()
            ENDIF()
            # Cleanup temp files
            FILE(REMOVE "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_result")
            FILE(REMOVE "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_output")
            FILE(REMOVE "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_error")
        ENDIF(ADB_RESULT)
    ELSE()
        MESSAGE(AUTHOR_WARNING "Error: the Android emulator \"${VAR}\" is not ready to receive commands")
    ENDIF()
ENDFUNCTION(DCMTK_ANDROID_SHELL)
