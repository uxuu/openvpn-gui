option(CLI_OVPN3 "Build ${PROJECT_NAME} with OpenVPN3 support" OFF)

cmake_minimum_required(VERSION 3.10)

project(openvpn-gui C CXX)
set(CMAKE_CXX_STANDARD 11)

add_executable(${PROJECT_NAME} WIN32
    access.c
    echo.c
    env_set.c
    localization.c
    main.c
    manage.c
    misc.c
    openvpn.c
    openvpn_config.c
    options.c
    proxy.c
    registry.c
    save_pass.c
    scripts.c
    service.c
    tray.c
    viewlog.c
    as.c
    pkcs11.c
    config_parser.c
    qr.c
    qrcodegen/qrcodegen.c
    res/openvpn-gui-res.rc
)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} /guard:cf /Qspectre")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /guard:cf /Qspectre")

list(APPEND CMAKE_CONFIGURATION_TYPES Asan)

set(CMAKE_C_FLAGS_ASAN
    "${CMAKE_C_FLAGS_DEBUG} /fsanitize=address" CACHE STRING
    "Flags used by the C compiler during AddressSanitizer builds." FORCE)
set(CMAKE_CXX_FLAGS_ASAN
    "${CMAKE_CXX_FLAGS_ASAN} /fsanitize=address" CACHE STRING
    "Flags used by the CXX compiler during AddressSanitizer builds." FORCE)

set(CMAKE_EXE_LINKER_FLAGS_ASAN
    "${CMAKE_EXE_LINKER_FLAGS_DEBUG}" CACHE STRING
    "Flags used by the linker during AddressSanitizer builds." FORCE)

set(CMAKE_SHARED_LINKER_FLAGS_ASAN
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}" CACHE STRING
    "Flags used by the linker during AddressSanitizer builds." FORCE)

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /guard:cf /DYNAMICBASE")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996 /wd4267 /wd4244")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996 /wd4267 /wd4244")

if(NOT (${CMAKE_C_COMPILER_ARCHITECTURE_ID} STREQUAL "ARM64"))
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /CETCOMPAT")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Wtsapi32.lib
    Netapi32.lib
    ws2_32.lib
    Winhttp.lib
    Secur32.lib
    Comctl32.lib
    Crypt32.lib
    Shlwapi.lib
    Advapi32.lib
    Shell32.lib
    Gdi32.lib
    Comdlg32.lib
    Ole32.lib
    Cryptui.lib
    Wininet.lib
    Iphlpapi.lib
    Ntdll.lib)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
set(CMAKE_RC_FLAGS "/D \"HAVE_CONFIG_H\" /D \"_MSC_VER\"")

add_custom_command(
    OUTPUT  eventmsg.h
    COMMAND mc.exe ${CMAKE_CURRENT_SOURCE_DIR}/eventmsg.mc -r ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/eventmsg.mc
    VERBATIM
    )
add_custom_target(msg_header_gen ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/eventmsg.h)
add_dependencies(${PROJECT_NAME} msg_header_gen)

function(get_version_from_configure regex_arg ret)
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/configure.ac" VERSION_STR REGEX ${regex_arg})
    string(REGEX MATCH ${regex_arg} VERSION_STR "${VERSION_STR}")
    set("${ret}" ${CMAKE_MATCH_1} PARENT_SCOPE)
endfunction()

get_version_from_configure("_GUI_VERSION_MAJOR\\], \\[([0-9]+)\\]" GUI_VERSION_MAJOR)
get_version_from_configure("_GUI_VERSION_MINOR\\], \\[([0-9]+)\\]" GUI_VERSION_MINOR)

configure_file(config-msvc.h.in config.h)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _UNICODE
    UNICODE
    WIN32_LEAN_AND_MEAN
    HAVE_CONFIG_H)

if (${CLI_OVPN3})
    find_package(json-c REQUIRED)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        json-c::json-c)

    target_compile_definitions(${PROJECT_NAME} PRIVATE
        ENABLE_OVPN3)
endif ()

if (NOT PROJECT_NAME_PLAP)
    set(PROJECT_NAME_PLAP "libopenvpn_plap")
endif()

add_library(${PROJECT_NAME_PLAP} SHARED
    localization.c
    manage.c
    misc.c
    openvpn.c
    openvpn_config.c
    options.c
    proxy.c
    pkcs11.c
    registry.c
    config_parser.c
    service.c
    qr.c
    qrcodegen/qrcodegen.c
    plap/ui_glue.c
    plap/stub.c
    plap/plap_common.c
    plap/plap_provider.c
    plap/plap_connection.c
    plap/plap_dll.c
    plap/openvpn_plap.def
    plap/openvpn-plap-res.rc)

target_link_libraries(${PROJECT_NAME_PLAP} PRIVATE
    ws2_32.lib
    Comctl32.lib
    Winhttp.lib
    Crypt32.lib
    Ole32.lib
    Shlwapi.lib
    Secur32.lib
    Gdi32.lib
    Cryptui.lib
    Rpcrt4.lib
    Iphlpapi.lib
    Ntdll.lib)

add_dependencies(${PROJECT_NAME_PLAP} msg_header_gen)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
target_include_directories(${PROJECT_NAME_PLAP} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})
target_compile_definitions(${PROJECT_NAME_PLAP} PRIVATE
    _UNICODE
    UNICODE
    WIN32_LEAN_AND_MEAN
    HAVE_CONFIG_H)

if (NOT TEST_PLAP_EXE)
    set(TEST_PLAP_EXE "test_plap")
endif()

add_executable(${TEST_PLAP_EXE}
    plap/test_plap.cpp
    plap/plap_common.c)

target_link_libraries(${TEST_PLAP_EXE} PRIVATE
    Rpcrt4
    Ole32
    Shlwapi
    Gdi32)

target_include_directories(${TEST_PLAP_EXE} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})
target_compile_definitions(${TEST_PLAP_EXE} PRIVATE
    _UNICODE
    UNICODE
    WIN32_LEAN_AND_MEAN
    HAVE_CONFIG_H)

target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/Zi>")
target_compile_options(${PROJECT_NAME} PRIVATE "/source-charset:utf-8")
target_link_options(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Release>:/DEBUG:FULL>"
    "$<$<CONFIG:Release>:/OPT:REF>"
    "$<$<CONFIG:Release>:/OPT:ICF>")

target_link_options(${TEST_PLAP_EXE} PRIVATE
    "/MANIFEST:EMBED"
    "/MANIFESTINPUT:${CMAKE_SOURCE_DIR}/plap/test-plap.manifest")


# Check SOUI_PATH environment variable which is required for building the project
# SOUI is the UI framework used in this project, so we need to ensure it's properly configured
file (TO_CMAKE_PATH "$ENV{SOUI_PATH}" SOUI_PATH)
if ("${SOUI_PATH}" STREQUAL "")
    message(FATAL_ERROR "FATAL: can't find env variable SOUI_PATH, clone soui source code first please.")
endif()
message(STATUS "SOUI_PATH: ${SOUI_PATH}")

# Extract SOUI library version information from soui-version.h header file
# This is needed to ensure compatibility with the expected SOUI version
function(get_version_from_soui regex_arg ret)
    file(STRINGS "${SOUI_PATH}/SOUI/include/soui-version.h" VERSION_STR REGEX ${regex_arg})
    string(REGEX MATCH ${regex_arg} VERSION_STR "${VERSION_STR}")
    set("${ret}" ${CMAKE_MATCH_1} PARENT_SCOPE)
endfunction()

get_version_from_soui("SOUI_VER1 ([0-9]+)" SOUI_MAJOR)
get_version_from_soui("SOUI_VER2 ([0-9]+)" SOUI_MINOR)
message(STATUS "SOUI_MAJOR: ${SOUI_MAJOR}")
message(STATUS "SOUI_MINOR: ${SOUI_MINOR}")

# Normalize ARM64 platform names for consistency
# Ensures that both CMAKE_GENERATOR_PLATFORM and CMAKE_VS_PLATFORM_NAME
# use the uppercase "ARM64" form instead of lowercase "arm64"
if(CMAKE_GENERATOR_PLATFORM STREQUAL "arm64")
    set(CMAKE_GENERATOR_PLATFORM "ARM64")
endif()
if(CMAKE_VS_PLATFORM_NAME STREQUAL "arm64")
    set(CMAKE_VS_PLATFORM_NAME "ARM64")
endif()

# Configure SOUI build options and features
# Disable demos games tools builds, enable Spectre mitigations, enable richedit builds
# and pass through the SOUI library linking options
option(SOUI_BUILD_DEMOS "" OFF)
option(SOUI_BUILD_GAMES "" OFF)
option(SOUI_BUILD_TOOLS "" ON)
option(SOUI_BUILD_RICHEDIT "" ON)
option(SOUI_ENABLE_SPECTRE "" ON)
option(SOUI_ENABLE_CORE_LIB "" ON)
option(SOUI_ENABLE_COM_LIB "" ON)

# Configure SOUI library linking options - allows choosing between dynamic and static linking
# By default, dynamic linking is used (DLL_CORE and DLL_SOUI_COM)
# Users can opt for static linking by setting SOUI_ENABLE_CORE_LIB and/or SOUI_ENABLE_COM_LIB to ON
set(SOUI_DEFINES DLL_CORE DLL_SOUI_COM)

if(SOUI_ENABLE_CORE_LIB)
    list(TRANSFORM SOUI_DEFINES REPLACE "DLL_CORE" "LIB_CORE")
endif()
if(SOUI_ENABLE_COM_LIB)
    list(TRANSFORM SOUI_DEFINES REPLACE "DLL_SOUI_COM" "LIB_SOUI_COM")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${SOUI_PATH}/utilities/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${SOUI_PATH}/SOUI/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${SOUI_PATH}/controls.extend)

if(SOUI_ENABLE_COM_LIB AND SOUI_ENABLE_CORE_LIB)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SOUI_PATH}/soui-sys-resource)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE ${SOUI_DEFINES})

target_link_libraries(${PROJECT_NAME} PRIVATE soui4 utilities4)

# Add SOUI library as a subdirectory to be built as part of the project
# This includes the SOUI UI framework which is essential for the GUI components
add_subdirectory(${SOUI_PATH} soui4)

# Configure SOUI framework subdirectories and targets
# This block collects all SOUI subdirectories recursively, then configures each target
# to be excluded from the main build (EXCLUDE_FROM_ALL ON) and sets their output
# directories to the current binary directory. This ensures SOUI is built as a dependency
# with its outputs placed in the correct location for the openvpn-gui project to use.
function(get_subdirectory subdir ret)
    set(subdirs ${${ret}})
    get_property(subdirs2 DIRECTORY ${subdir} PROPERTY SUBDIRECTORIES)
    list(APPEND subdirs ${subdirs2})
    foreach(subdir2 ${subdirs2})
        get_subdirectory(${subdir2} subdirs)
    endforeach()
    set(${ret} ${subdirs} PARENT_SCOPE)
endfunction()

get_subdirectory(${SOUI_PATH} SOUI_SUBDIRS)

foreach (subdir ${SOUI_SUBDIRS})
    get_property(targets DIRECTORY ${subdir} PROPERTY BUILDSYSTEM_TARGETS)
    foreach (target ${targets})
        set_target_properties(${target} PROPERTIES EXCLUDE_FROM_ALL ON)
        set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
        set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
        set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endforeach ()
endforeach ()

# Define SOUI library dependencies and add them to the project
# These are the core SOUI components required for the GUI functionality
# Including image decoders, resource providers, renderers, and utility libraries
set(SOUI_LIBS
    soui4
    utilities4
    imgdecoder-stb
    imgdecoder-gdip
    render-gdi
    render-skia
    taskloop
    translator)

if(SOUI_BUILD_RICHEDIT)
    list(APPEND SOUI_LIBS msftedit msls31)
endif()

if(SOUI_ENABLE_COM_LIB AND SOUI_ENABLE_CORE_LIB)
    list(APPEND SOUI_LIBS aupng)
else()
    add_dependencies(${PROJECT_NAME} soui-sys-resource soui-sys-resource2)
endif()

add_dependencies(${PROJECT_NAME} ${SOUI_LIBS})

if(SOUI_ENABLE_COM_LIB)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SOUI_LIBS})
endif()

add_custom_command(
    TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMAND ${SOUI_PATH}/tools/uiresbuilder.exe
    -i ${PROJECT_SOURCE_DIR}/uires/uires.idx
    -p ..\\uires
    -r soui-res.rc
    -h soui-res.h idtable || exit 0)

