cmake_minimum_required (VERSION 3.10)
project(fea_slot_map VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(ExternalProject)
include(GoogleTest)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

# Require up-to-date compiler.
set(CMAKE_CXX_STANDARD 17)

# Output binary to predictable location (fixes cyclic dependency issues).
set(BINARY_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)


file(GLOB_RECURSE HEADER_FILES "${PROJECT_SOURCE_DIR}/include/fea_slot_map/*.hpp")
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${HEADER_FILES}>
)
target_include_directories(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.clang-format ${CMAKE_CURRENT_BINARY_DIR}/.clang-format COPYONLY)


##
# Install Package Configuration
##
set(FEA_SLOT_MAP_INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/fea_slot_map")
install(FILES ${HEADER_FILES} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/fea_slot_map")

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/fea_slot_mapConfigVersion.cmake"
	VERSION ${${PROJECT_NAME}_VERSION}
	COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
	cmake/fea_slot_mapConfig.cmake.in
	${FEA_SLOT_MAP_INSTALL_CONFIGDIR}/fea_slot_mapConfig.cmake
	INSTALL_DESTINATION ${FEA_SLOT_MAP_INSTALL_CONFIGDIR}
	PATH_VARS CMAKE_INSTALL_INCLUDEDIR
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(
	FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${FEA_SLOT_MAP_INSTALL_CONFIGDIR}/fea_slot_mapConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/fea_slot_mapConfigVersion.cmake"
	DESTINATION ${FEA_SLOT_MAP_INSTALL_CONFIGDIR}
)

install(TARGETS ${PROJECT_NAME} EXPORT fea_slot_mapTargets)

export(EXPORT fea_slot_mapTargets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/fea_slot_mapTargets.cmake"
)

install(
	EXPORT fea_slot_mapTargets FILE fea_slot_mapTargets.cmake
	DESTINATION ${FEA_SLOT_MAP_INSTALL_CONFIGDIR}
)

export(PACKAGE ${PROJECT_NAME})


##
# Tests
##
option(BUILD_TESTING "Build and run tests." On)
if (${BUILD_TESTING})
	enable_testing()
	
	if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
		include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
	else()
		include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	endif()
	conan_basic_setup(TARGETS)
	
	set(TEST_NAME ${PROJECT_NAME}_tests)
	file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp" "tests/*.c" "tests/*.hpp" "tests/*.h" "tests/*.tpp")
	add_executable(${TEST_NAME} ${TEST_SOURCES})
	target_link_libraries(${TEST_NAME} PRIVATE ${PROJECT_NAME} CONAN_PKG::gtest)

	gtest_discover_tests(${TEST_NAME})
	add_dependencies(${TEST_NAME} ${PROJECT_NAME})

	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TEST_NAME})

	##
	# External dependencies.
	##
	set(LIB_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
	target_include_directories(${TEST_NAME} PRIVATE ${LIB_INCLUDE_DIR})

	ExternalProject_Add(bench_util_ext
		GIT_REPOSITORY https://github.com/p-groarke/bench_util.git
		CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
			-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
			-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}
			-DBUILD_EXAMPLES=Off
		UPDATE_COMMAND ""
	)

	add_dependencies(${TEST_NAME} bench_util_ext)

	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
		# using Intel C++
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		target_compile_options(${TEST_NAME} PRIVATE /W4 /WX /Zc:__cplusplus /FAs /permissive-)
		add_definitions(-DNOMINMAX)
	endif()
endif()


##
# Compiler options
##
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	target_compile_options(${PROJECT_NAME} INTERFACE -Wall -Wextra -Wpedantic -Werror)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(${PROJECT_NAME} INTERFACE -Wall -Wextra -Wpedantic -Werror)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	# using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(${PROJECT_NAME} INTERFACE /W4 /WX /Zc:__cplusplus /FAs /permissive-)
	add_definitions(-DNOMINMAX)
endif()
