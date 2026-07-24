cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

project(inekf VERSION 1.0.0 LANGUAGES CXX)

option(EXPORT_COMPILE_COMMANDS "Export compile commands" ON)
option(USE_MUTEX "Use mutex in code" OFF)
option(INEKF_BUILD_EXAMPLES "Build example binaries" ON)
option(INEKF_BUILD_SPEED_TESTS "Build speed test binaries" ON)

include(CTest)

message(STATUS "USE_MUTEX: ${USE_MUTEX}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ${EXPORT_COMPILE_COMMANDS})

# Preserve the legacy output layout expected by existing scripts.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

find_package(Threads REQUIRED)
find_package(Eigen3 REQUIRED)

set(INEKF_NEEDS_BOOST OFF)
if(INEKF_BUILD_EXAMPLES OR INEKF_BUILD_SPEED_TESTS)
    set(INEKF_NEEDS_BOOST ON)
endif()

if(INEKF_NEEDS_BOOST)
    find_package(Boost 1.57 QUIET COMPONENTS date_time)
    if(NOT Boost_FOUND)
        message(WARNING "Boost not found. Disabling example and speed test binaries.")
        set(INEKF_BUILD_EXAMPLES OFF)
        set(INEKF_BUILD_SPEED_TESTS OFF)
    endif()
endif()

set(INEKF_PUBLIC_HEADERS
    include/InEKF.h
    include/RobotState.h
    include/LieGroup.h
    include/NoiseParams.h
)

set(INEKF_SOURCES
    src/InEKF.cpp
    src/RobotState.cpp
    src/LieGroup.cpp
    src/NoiseParams.cpp
)

set(INEKF_COMMON_COMPILE_OPTIONS
    -O3
)

set(INEKF_MUTEX_DEFINITION INEKF_USE_MUTEX=false)
if(USE_MUTEX)
    set(INEKF_MUTEX_DEFINITION INEKF_USE_MUTEX=true)
endif()

add_library(inekf SHARED ${INEKF_SOURCES})
add_library(inekf::inekf ALIAS inekf)

target_include_directories(inekf
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(inekf
    PUBLIC
        Eigen3::Eigen
    PRIVATE
        Threads::Threads
)

target_compile_options(inekf PRIVATE ${INEKF_COMMON_COMPILE_OPTIONS})
target_compile_definitions(inekf
    PUBLIC
        ${INEKF_MUTEX_DEFINITION}
    PRIVATE
        EIGEN_NO_DEBUG
)

set_target_properties(inekf PROPERTIES
    OUTPUT_NAME inekf
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${INEKF_PUBLIC_HEADERS}"
)

set(INEKF_RUNTIME_TARGETS)

if(INEKF_BUILD_EXAMPLES)
    add_executable(landmarks src/examples/landmarks.cpp)
    add_executable(kinematics src/examples/kinematics.cpp)
    list(APPEND INEKF_RUNTIME_TARGETS landmarks kinematics)

    foreach(inekf_example landmarks kinematics)
        target_link_libraries(${inekf_example} PRIVATE inekf)
        target_compile_options(${inekf_example} PRIVATE ${INEKF_COMMON_COMPILE_OPTIONS})
        target_compile_definitions(${inekf_example} PRIVATE EIGEN_NO_DEBUG)
    endforeach()

    # boost::algorithm::string is header-only and resolved via Boost include dirs.
    if(TARGET Boost::headers)
        target_link_libraries(landmarks PRIVATE Boost::headers)
        target_link_libraries(kinematics PRIVATE Boost::headers)
    elseif(TARGET Boost::boost)
        target_link_libraries(landmarks PRIVATE Boost::boost)
        target_link_libraries(kinematics PRIVATE Boost::boost)
    endif()
endif()

if(INEKF_BUILD_SPEED_TESTS)
    add_executable(propagation_speed src/tests/propagation_speed.cpp)
    add_executable(correction_speed src/tests/correction_speed.cpp)
    list(APPEND INEKF_RUNTIME_TARGETS propagation_speed correction_speed)

    foreach(inekf_test propagation_speed correction_speed)
        target_link_libraries(${inekf_test} PRIVATE inekf)
        target_compile_options(${inekf_test} PRIVATE ${INEKF_COMMON_COMPILE_OPTIONS})
        target_compile_definitions(${inekf_test} PRIVATE EIGEN_NO_DEBUG)
    endforeach()

    if(TARGET Boost::headers)
        target_link_libraries(propagation_speed PRIVATE Boost::headers)
        target_link_libraries(correction_speed PRIVATE Boost::headers)
    elseif(TARGET Boost::boost)
        target_link_libraries(propagation_speed PRIVATE Boost::boost)
        target_link_libraries(correction_speed PRIVATE Boost::boost)
    endif()

    target_link_libraries(propagation_speed PRIVATE Boost::date_time)
    target_link_libraries(correction_speed PRIVATE Boost::date_time)

    if(BUILD_TESTING)
        add_test(NAME inekf_propagation_speed COMMAND propagation_speed)
        add_test(NAME inekf_correction_speed COMMAND correction_speed)
        set_tests_properties(inekf_propagation_speed PROPERTIES TIMEOUT 120)
        set_tests_properties(inekf_correction_speed PROPERTIES TIMEOUT 120)
    endif()
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(INEKF_CMAKE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/inekf)

# Build-tree package export for local development.
export(TARGETS inekf
    FILE "${CMAKE_CURRENT_BINARY_DIR}/inekfTargets.cmake"
    NAMESPACE inekf::
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/inekfConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/inekfConfig.cmake
    INSTALL_DESTINATION ${INEKF_CMAKE_INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/inekfConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

export(PACKAGE inekf)

install(TARGETS inekf
    EXPORT inekfTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(INEKF_RUNTIME_TARGETS)
    install(TARGETS ${INEKF_RUNTIME_TARGETS}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

install(EXPORT inekfTargets
    FILE inekfTargets.cmake
    NAMESPACE inekf::
    DESTINATION ${INEKF_CMAKE_INSTALL_DIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/inekfConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/inekfConfigVersion.cmake
    DESTINATION ${INEKF_CMAKE_INSTALL_DIR}
)