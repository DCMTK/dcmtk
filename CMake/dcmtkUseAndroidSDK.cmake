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

include(CMakeParseArguments)

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
macro(DCMTK_ANDROID_GET_OBJECT_PROPERTIES VAR)
    list(LENGTH ${VAR} ${VAR}_LENGTH)
    if(${VAR}_LENGTH EQUAL 3)
        list(GET ${VAR} 0 EMULATOR_STATE)
        list(GET ${VAR} 1 EMULATOR_UUID)
        list(GET ${VAR} 2 EMULATOR_NAME)
    else()
        unset(EMULATOR_STATE)
        unset(EMULATOR_UUID)
        unset(EMULATOR_NAME)
    endif()
endmacro()

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
macro(DCMTK_ANDROID_SET_OBJECT_PROPERTIES VAR EMULATOR_STATE EMULATOR_UUID EMULATOR_NAME)
    set(${VAR} "${EMULATOR_STATE}" "${EMULATOR_UUID}" "${EMULATOR_NAME}" CACHE INTERNAL "")
endmacro()

#
# Destroys an instance handle object.
# VAR - the name of the variable its emulator instance handle
#       shall be object destroyed
# All additional arguments will be ignored.
#
macro(DCMTK_ANDROID_DESTROY_OBJECT VAR)
    unset("${VAR}" CACHE)
endmacro()

#
# Tries to detect any required shared objects within the NDK
# VAR - the name of the variable that will receive a list
#       of required shared object files (if any)
# Will set VAR to an empty list if no shared objects are
# required (e. g. when they are linked statically).
# All additional arguments will be ignored.
#
function(DCMTK_ANDROID_FIND_RUNTIME_LIBRARIES VAR)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
    foreach(DIR ${ANDROID_STL_INCLUDE_DIRS})
        get_filename_component(DIR "${DIR}" DIRECTORY)
        list(APPEND ANDROID_STL_LIBRARY_DIRS "${DIR}")
    endforeach()
    find_library(ANDROID_STL_SHARED_OBJECT ${ANDROID_STL} PATHS ${ANDROID_STL_LIBRARY_DIRS} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    if(ANDROID_STL_SHARED_OBJECT)
        set("${VAR}" ${ANDROID_STL_SHARED_OBJECT} PARENT_SCOPE)
    endif()
endfunction()

#
# Tries to detect the required executables and scripts automatically,
# which should be contained in the Android SDK. Emits an error that
# will halt the configuration process if that is not possible.
# Will also detect all available AVDs after tool detection was
# successful.
# All arguments will be ignored.
#
function(DCMTK_SETUP_ANDROID_EMULATOR)
    if(NOT ANDROID_TEMPORARY_FILES_LOCATION)
        set(ANDROID_TEMPORARY_FILES_LOCATION "/cache" CACHE STRING "The path on the Android device that should be used for temporary files")
    endif()
    if(NOT ANDROID_SDK_ROOT)
        if(CMAKE_HOST_SYSTEM MATCHES "Windows.*")
            file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" ANDROID_SDK_SEARCH_PATHS)
            set(ANDROID_SDK_ROOT "${ANDROID_SDK_SEARCH_PATHS}/android-sdk" CACHE PATH "Location of the Android SDK")
        else()
            set(ANDROID_SDK_ROOT "/opt/android-sdk" CACHE PATH "Location of the Android SDK")
        endif()
    endif()
    find_program(ANDROID_EMULATOR_PROGRAM emulator PATHS ${ANDROID_SDK_ROOT} PATH_SUFFIXES tools NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    if(CMAKE_HOST_SYSTEM MATCHES "Windows.*")
        find_program(ANDROID_ANDROID_PROGRAM android.bat PATHS ${ANDROID_SDK_ROOT} PATH_SUFFIXES tools NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    else()
        find_program(ANDROID_ANDROID_PROGRAM android PATHS ${ANDROID_SDK_ROOT} PATH_SUFFIXES tools NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    endif()
    find_program(ANDROID_ADB_PROGRAM adb PATHS ${ANDROID_SDK_ROOT} PATH_SUFFIXES platform-tools NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    if(NOT ANDROID_EMULATOR_PROGRAM OR NOT ANDROID_ANDROID_PROGRAM OR NOT ANDROID_ADB_PROGRAM)
        message(FATAL_ERROR
            "Failed to detect the Android SDK, please set ANDROID_SDK_ROOT to the location of your Android SDK"
            "or set the missing tools manually!"
        )
    else()
        execute_process(COMMAND "${ANDROID_ANDROID_PROGRAM}" list avd RESULT_VARIABLE RESULT OUTPUT_VARIABLE OUTPUT ERROR_QUIET)
        string(REGEX MATCHALL "Name:[ \t]*[^\r\n]*" ANDROID_AVAILABLE_AVDS ${OUTPUT})
        string(REGEX REPLACE "Name:[ \t]*([^\r\n;]*)" "\\1" ANDROID_AVAILABLE_AVDS "${ANDROID_AVAILABLE_AVDS}")
        set(ANDROID_EMULATOR_AVD "${ANDROID_EMULATOR_AVD}" CACHE STRING "Android emulator Android Virtual Device (AVD) configuration" FORCE)
        set_property(CACHE ANDROID_EMULATOR_AVD PROPERTY STRINGS ${ANDROID_AVAILABLE_AVDS})
    endif()
endfunction()

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
function(DCMTK_ANDROID_LIST_EMULATORS ONLINE OFFLINE)
    DCMTK_SETUP_ANDROID_EMULATOR()
    if(ANDROID_ADB_PROGRAM)
        execute_process(
            COMMAND "${ANDROID_ADB_PROGRAM}" devices
            RESULT_VARIABLE RESULT
            OUTPUT_VARIABLE DEVICES_RAW
            ERROR_QUIET
        )
        string(REPLACE "\n" ";" DEVICES "${DEVICES_RAW}")
        foreach(DEVICE ${DEVICES})
            string(REGEX REPLACE "(.+)\t(.+)" "\\1;\\2" DS "${DEVICE}")
            list(LENGTH DS DSL)
            if(DSL EQUAL 2)
                list(GET DS 0 EMULATOR_NAME)
                list(GET DS 1 EMULATOR_STATE)
                if(EMULATOR_STATE MATCHES "^device$")
                    list(APPEND "${ONLINE}" ${EMULATOR_NAME})
                else()
                    list(APPEND "${OFFLINE}" ${EMULATOR_NAME})
                endif()
            endif()
        endforeach()
    endif()
    set("${ONLINE}" ${${ONLINE}} PARENT_SCOPE)
    set("${OFFLINE}" ${${OFFLINE}} PARENT_SCOPE)
endfunction()

#
# Generate a random ID that is hopefully unique
# enough to be used as the instance UUID.
# VAR - the name of the variable that will receive
#       the generated UUID as a string value
# Will ignore all additional arguments.
#
function(DCMTK_ANDROID_EMULATOR_GENERATE_UUID VAR)
    string(RANDOM LENGTH 20 RAND)
    string(TIMESTAMP TM)
    set(${VAR} "${TM}${RAND}")
    string(MD5 ${VAR} ${${VAR}})
    set(${VAR} ${${VAR}} PARENT_SCOPE)
endfunction()

#
# Tries to query the UUID property of an accessible Android device.
# EMULATOR_NAME - the device name, as returned by DCMTK_ANDROID_LIST_EMULATORS
# VAR           - the name of the variable that will be set to the device UUID
# Will unset the variable referred to by VAR if no device with the given name
# is accessible or the device is offline.
# Will ignore all additional arguments.
#
function(DCMTK_ANDROID_GET_EMULATOR_UUID EMULATOR_NAME VAR)
    execute_process(
        COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" shell getprop "ro.emu.uuid"
        RESULT_VARIABLE RESULT
        OUTPUT_VARIABLE OUTPUT
        ERROR_QUIET
    )
    DCMTK_UNSET_PARENT_SCOPE(${VAR})
    if(NOT RESULT)
        string(STRIP "${OUTPUT}" UUID)
        if(UUID)
            set("${VAR}" ${UUID} PARENT_SCOPE)
        endif()
    endif()
endfunction()

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
function(DCMTK_ANDROID_GET_EMULATOR_NAME VAR EMULATOR_UUID)
    DCMTK_ANDROID_LIST_EMULATORS(ONLINE_EMULATORS OFFLINE_EMULATORS)
    foreach(EMULATOR ${ONLINE_EMULATORS})
        DCMTK_ANDROID_GET_EMULATOR_UUID("${EMULATOR}" UUID)
        if(EMULATOR_UUID STREQUAL UUID)
            set("${VAR}" "${EMULATOR}" PARENT_SCOPE)
            return()
        endif()
    endforeach()
    while(OFFLINE_EMULATORS)
        list(GET OFFLINE_EMULATORS 0 EMULATOR)
        list(REMOVE_AT OFFLINE_EMULATORS 0)
        execute_process(
            COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR}" wait-for-device
            TIMEOUT 1
            RESULT_VARIABLE RESULT
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(NOT RESULT)
            DCMTK_ANDROID_GET_EMULATOR_UUID("${EMULATOR}" UUID)
            if(UUID)
                if(EMULATOR_UUID STREQUAL UUID)
                    set("${VAR}" "${EMULATOR}" PARENT_SCOPE)
                    return()
                endif()
            else()
                list(APPEND OFFLINE_EMULATORS "${EMULATOR}")
            endif()
        else()
            list(APPEND OFFLINE_EMULATORS "${EMULATOR}")
        endif()
    endwhile()
endfunction()

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
function(DCMTK_ANDROID_START_EMULATOR VAR)
    DCMTK_SETUP_ANDROID_EMULATOR()
    if(NOT ANDROID_EMULATOR_AVD)
        message(FATAL_ERROR "Please select which Android emulator Android Virtual Device (AVD) configuration to use!")
    else()
        DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
        if(NOT EMULATOR_STATE)
            DCMTK_ANDROID_EMULATOR_GENERATE_UUID(EMULATOR_UUID)
        elseif(EMULATOR_STATE STREQUAL "RUNNING")
            DCMTK_ANDROID_GET_EMULATOR_UUID("${EMULATOR_NAME}" UUID)
            # Do nothing if the running emulator instance is ok and can be reused.
            # Otherwise restart it.
            if(UUID STREQUAL EMULATOR_UUID)
                message(STATUS "Reusing already running Android device emulator...")
                return()
            endif()
        elseif(EMULATOR_STATE STREQUAL "STARTING")
            # Is it really starting, or has somebody aborted it?
            message(STATUS "Found previously started Android device emulator, checking if it's still present...")
            DCMTK_ANDROID_GET_EMULATOR_NAME(EMULATOR_NAME "${EMULATOR_UUID}")
            if(EMULATOR_NAME)
              message(STATUS "Found previously started Android device emulator, checking if it's still present... yes")
              DCMTK_ANDROID_SET_OBJECT_PROPERTIES(${VAR} RUNNING "${EMULATOR_UUID}" "${EMULATOR_NAME}")
              return()
            endif()
            message(STATUS "Found previously started Android device emulator, checking if it's still present... no")
        endif()
        message(STATUS "Starting the Android device emulator...")
        if(CMAKE_HOST_SYSTEM MATCHES "Windows.*")
            set(COMMAND sh -c "${ANDROID_EMULATOR_PROGRAM} -avd ${ANDROID_EMULATOR_AVD} -no-boot-anim -prop ro.emu.uuid=${EMULATOR_UUID} >${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/android-emulator.log 2>&1 < /dev/null &")
        else()
            set(COMMAND sh -c "${ANDROID_EMULATOR_PROGRAM} -avd ${ANDROID_EMULATOR_AVD} -no-window -no-boot-anim -prop ro.emu.uuid=${EMULATOR_UUID} >${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/android-emulator.log 2>&1 < /dev/null &")
        endif()
        execute_process(
            COMMAND ${COMMAND}
            RESULT_VARIABLE RESULT
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(NOT RESULT)
            DCMTK_ANDROID_SET_OBJECT_PROPERTIES("${VAR}" STARTING "${EMULATOR_UUID}" "")
        else()
            DCMTK_ANDROID_DESTROY_OBJECT("${VAR}")
            message(FATAL_ERROR "Error starting Android emulator.")
        endif()
    endif()
endfunction()

#
# Restart adb/the emulated device in root mode so that we gain write access to
# the device.
# Newer versions of the SDK seem to require this for doing anything meaningful
# with it.
# EMULATOR_NAME - the name of the emulated device that shall be rooted.
# Will ignore all additional arguments.
#
function(DCMTK_ANDROID_ADB_ROOT EMULATOR_NAME)
    execute_process(
        COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" root
        OUTPUT_QUIET
        ERROR_QUIET
    )
    # the SDK was seemingly designed by a five year old, the device will
    # become invisible while it is being rooted, therefore, wait until
    # it is ready again
    set(STATUS 1)
    while(STATUS)
        execute_process(
            COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" wait-for-device
            RESULT_VARIABLE STATUS
            OUTPUT_QUIET
            ERROR_QUIET
        )
    endwhile()
endfunction()

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
function(DCMTK_ANDROID_WAIT_FOR_EMULATOR VAR)
    DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
    if(NOT EMULATOR_STATE)
        message(AUTHOR_WARNING "Error: ${VAR} is not a valid Android emulator instance handle.")
    elseif(EMULATOR_STATE STREQUAL "RUNNING")
        # Emulator is running, no need to wait!
    elseif(EMULATOR_STATE STREQUAL "STOPPED")
        message(WARNING "The Android emulator has stopped, aborting...")
    else()
        message(STATUS "Waiting until the Android device emulator is ready to receive instructions...")
        while(NOT EMULATOR_NAME)
            DCMTK_ANDROID_GET_EMULATOR_NAME(EMULATOR_NAME "${EMULATOR_UUID}")
        endwhile()
        DCMTK_ANDROID_ADB_ROOT("${EMULATOR_NAME}")
        DCMTK_ANDROID_SET_OBJECT_PROPERTIES("${VAR}" RUNNING "${EMULATOR_UUID}" "${EMULATOR_NAME}")
    endif()
endfunction()

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
macro(DCMTK_ANDROID_STOP_EMULATOR_COMMAND VAR EMULATOR_NAME)
    if(CMAKE_HOST_SYSTEM MATCHES "Windows.*")
        unset("${VAR}")
    else()
        set("${VAR}" "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" emu kill)
    endif()
endmacro()

#
# Prepare an emulator shutdown message or a warning message if automated
# shutdown is not possible.
# VAR - the name of the variable that will be set to the generated message
# Will ignore all additional arguments.
#
macro(DCMTK_ANDROID_EMULATOR_SHUTDOWN_MESSAGE VAR)
    if(CMAKE_HOST_SYSTEM MATCHES "Windows.*")
        set("${VAR}" WARNING "The Android device emulator can't be terminated automatically under Windows, please shutdown \"${EMULATOR_NAME}\" manually!")
    else()
        set("${VAR}" STATUS "Shutting down the Android device emulator...")
    endif()
endmacro()

#
# Stops the emulator and sets its state to STOPPED when successful.
# VAR - the name of the variable referring to the emulator instance
#       handle object
# Will emit a warning message if the emulator can't be stopped and
# won't modify its state in that case.
# Will ignore all additional arguments.
#
function(DCMTK_ANDROID_STOP_EMULATOR VAR)
    DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
    if(NOT EMULATOR_STATE)
        message(AUTHOR_WARNING "Error: ${VAR} is not a valid Android emulator instance handle.")
    elseif(EMULATOR_STATE STREQUAL "STARTING")
        # Can't tell it to stop if it is not done starting
        DCMTK_ANDROID_WAIT_FOR_EMULATOR("${VAR}")
    elseif(EMULATOR_STATE STREQUAL "STOPPED")
        # Emulator is already stopped, do nothing
        return()
    endif()
    DCMTK_ANDROID_EMULATOR_SHUTDOWN_MESSAGE(MESSAGE)
    message(${MESSAGE})
    if(NOT CMAKE_HOST_SYSTEM MATCHES "Windows.*")
        DCMTK_ANDROID_STOP_EMULATOR_COMMAND(COMMAND "${EMULATOR_NAME}")
        execute_process(
          COMMAND ${COMMAND}
          RESULT_VARIABLE RESULT
          OUTPUT_QUIET
          ERROR_QUIET
        )
        if(NOT RESULT)
            DCMTK_ANDROID_SET_OBJECT_PROPERTIES("${VAR}" STOPPED "${EMULATOR_UUID}" "")
        else()
            message(WARNING "Unable to stop the android device emulator, please shutdown \"${EMULATOR_NAME}\" manually!")
        endif()
    endif()
endfunction()

#
# Uploads local files to the given Android device using 'adb push'.
# VAR - the emulator instance handle object referring to the target
#       device
# Requires the device to be running.
# Additional arguments will be parsed and interpreted similar to
# CMake's file(COPY ...) command.
#
function(DCMTK_ANDROID_PUSH VAR)
    DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
    if(NOT EMULATOR_STATE)
        message(AUTHOR_WARNING "Error: ${VAR} is not a valid Android emulator instance handle.")
    elseif(EMULATOR_STATE STREQUAL "RUNNING")
        cmake_parse_arguments(DCMTK_ANDROID_PUSH "" "" "DESTINATION" ${ARGN})
        foreach(LOCAL_FILE ${DCMTK_ANDROID_PUSH_UNPARSED_ARGUMENTS})
            execute_process(
                COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" push "${LOCAL_FILE}" "${DCMTK_ANDROID_PUSH_DESTINATION}"
                RESULT_VARIABLE RESULT
                OUTPUT_QUIET
                ERROR_QUIET
            )
        endforeach()
    else()
        message(AUTHOR_WARNING "Error: the Android emulator \"${VAR}\" is not ready to receive commands")
    endif()
endfunction()

#
# Downloads remote files from the given Android device using 'adb pull'.
# VAR - the emulator instance handle object referring to the target
#       device
# Requires the device to be running.
# Additional arguments will be parsed and interpreted similar to
# CMake's file(COPY ...) command.
#
function(DCMTK_ANDROID_PULL VAR)
    DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
    if(NOT EMULATOR_STATE)
        message(AUTHOR_WARNING "Error: ${VAR} is not a valid Android emulator instance handle.")
    elseif(EMULATOR_STATE STREQUAL "RUNNING")
        cmake_parse_arguments(DCMTK_ANDROID_PULL "" "" "DESTINATION" ${ARGN})
        foreach(REMOTE_FILE ${DCMTK_ANDROID_PULL_UNPARSED_ARGUMENTS})
            execute_process(
                COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" pull "${REMOTE_FILE}" "${DCMTK_ANDROID_PULL_DESTINATION}"
                RESULT_VARIABLE RESULT
                OUTPUT_QUIET
                ERROR_QUIET
            )
        endforeach()
    else()
        message(AUTHOR_WARNING "Error: the Android emulator \"${VAR}\" is not ready to receive commands")
    endif()
endfunction()

#
# Executes a shell command on the given Android device using 'adb shell'.
# VAR - the emulator instance handle object referring to the target
#       device
# Requires the device to be running.
# Additional arguments will be parsed and interpreted similar to
# CMake's execute_process(...) command.
#
function(DCMTK_ANDROID_SHELL VAR)
    DCMTK_ANDROID_GET_OBJECT_PROPERTIES("${VAR}")
    if(NOT EMULATOR_STATE)
        message(AUTHOR_WARNING "Error: ${VAR} is not a valid Android emulator instance handle.")
    elseif(EMULATOR_STATE STREQUAL "RUNNING")
        cmake_parse_arguments(DCMTK_ANDROID_SHELL
            "OUTPUT_QUIET;ERROR_QUIET"
            "RESULT_VARIABLE;OUTPUT_VARIABLE;ERROR_VARIABLE;WORKING_DIRECTORY"
            "COMMAND"
            ${ARGN}
        )

        # Prepare commandline
        set(COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" shell)
        if(DCMTK_ANDROID_SHELL_WORKING_DIRECTORY)
            list(APPEND COMMAND "cd" "${DCMTK_ANDROID_SHELL_WORKING_DIRECTORY}" "&&")
        endif()
        list(APPEND COMMAND "${DCMTK_ANDROID_SHELL_COMMAND}")

        # Inspect output variable parameters
        if(DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE)
            set(PUSH_OUTPUT_VAR TRUE)
            if(DCMTK_ANDROID_SHELL_ERROR_VARIABLE)
                if(DCMTK_ANDROID_SHELL_ERROR_VARIABLE STREQUAL DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE)
                    set(STREAMS_MERGED TRUE)
                else()
                    set(PUSH_ERROR_VAR TRUE)
                endif()
            else()
                set(DCMTK_ANDROID_SHELL_ERROR_VARIABLE DCMTK_ANDROID_SHELL_ERROR_MESSAGE_BUFFER)
            endif()
        else()
            set(DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE DCMTK_ANDROID_SHELL_OUTPUT_MESSAGE_BUFFER)
            if(DCMTK_ANDROID_SHELL_ERROR_VARIABLE)
                set(PUSH_ERROR_VAR TRUE)
            else()
                set(STREAMS_MERGED TRUE)
            endif()
        endif()

        # Prefix to prevent collision of output capturing files
        string(MD5 PREFIX "${DCMTK_ANDROID_SHELL_COMMAND}")

        # Prepare output redirection (buffering)
        if(STREAMS_MERGED)
            list(APPEND COMMAND > "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_output" 2>&1)
        else()
            list(APPEND COMMAND > "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_output" 2> "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_error")
        endif()

        # Prepare capturing the result
        list(APPEND COMMAND "\;" echo -n $? > "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_result")

        # Run the command
        execute_process(
          COMMAND ${COMMAND}
          RESULT_VARIABLE ADB_RESULT
          OUTPUT_VARIABLE ADB_OUTPUT
          ERROR_VARIABLE ADB_OUTPUT
        )

        # Deal with it
        if(ADB_RESULT)
            message(WARNING "Error sending command to the Android emulator, adb reported the error: ${ADB_OUTPUT}")
            if(DCMTK_ANDROID_SHELL_RESULT_VARIABLE)
                set("${DCMTK_ANDROID_SHELL_RESULT_VARIABLE}" ${ADB_RESULT} PARENT_SCOPE)
            endif()
        else()
            # Receive results
            DCMTK_ANDROID_PULL("${VAR}" "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_output" "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_error" "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_result"
                DESTINATION "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}"
            )
            # Cleanup temp files
            execute_process(
                COMMAND "${ANDROID_ADB_PROGRAM}" -s "${EMULATOR_NAME}" shell rm "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_output" "\;" rm "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_error" "\;" rm "${ANDROID_TEMPORARY_FILES_LOCATION}/${PREFIX}_result"
                RESULT_VARIABLE RESULT_QUIET
                OUTPUT_QUIET
                ERROR_QUIET
            )
            # Analyze results
            if(DCMTK_ANDROID_SHELL_RESULT_VARIABLE)
                file(READ "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_result" "${DCMTK_ANDROID_SHELL_RESULT_VARIABLE}")
                set("${DCMTK_ANDROID_SHELL_RESULT_VARIABLE}" ${${DCMTK_ANDROID_SHELL_RESULT_VARIABLE}} PARENT_SCOPE)
            endif()
            if(NOT DCMTK_ANDROID_SHELL_OUTPUT_QUIET OR (STREAMS_MERGED AND NOT DCMTK_ANDROID_SHELL_ERROR_QUIET))
                file(READ "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_output" "${DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE}")
                if(PUSH_OUTPUT_VAR)
                    set("${DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE}" ${${DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE}} PARENT_SCOPE)
                else()
                    message("${${DCMTK_ANDROID_SHELL_OUTPUT_VARIABLE}}")
                endif()
            endif()
            if(NOT DCMTK_ANDROID_SHELL_ERROR_QUIET AND NOT STREAMS_MERGED)
                file(READ "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_error" "${DCMTK_ANDROID_SHELL_ERROR_VARIABLE}")
                if(PUSH_ERROR_VAR)
                    set("${DCMTK_ANDROID_SHELL_ERROR_VARIABLE}" ${${DCMTK_ANDROID_SHELL_ERROR_VARIABLE}} PARENT_SCOPE)
                else()
                    message("${${DCMTK_ANDROID_SHELL_ERROR_VARIABLE}}")
                endif()
            endif()
            # Cleanup temp files
            file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_result")
            file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_output")
            file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PREFIX}_error")
        endif()
    else()
        message(AUTHOR_WARNING "Error: the Android emulator \"${VAR}\" is not ready to receive commands")
    endif()
endfunction()
