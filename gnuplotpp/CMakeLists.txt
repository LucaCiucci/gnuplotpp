#[[ LC_NOTICE_BEGIN
===============================================================================
|                        Copyright (C) 2021 Luca Ciucci                       |
|-----------------------------------------------------------------------------|
| Important notices:                                                          |
|  - This work is distributed under the MIT license, feel free to use this    |
|   work as you wish.                                                         |
|  - Read the license file for further info.                                  |
| Written by Luca Ciucci <luca.ciucci99@gmail.com>, 2021                      |
===============================================================================
LC_NOTICE_END ]]

################################################################
#                           INFO
################################################################
#[[

Top level CMake project, see README.md for info

]]

################################################################
#                    basic configurations
################################################################

################################################################
#                         LIBRARY
################################################################

add_library(${PROJECT_NAME} STATIC)

target_sources(
	${PROJECT_NAME}
	PRIVATE
		"./include/${PROJECT_NAME}/${PROJECT_NAME}.hpp"
		"./src/${PROJECT_NAME}.cpp"
)

target_include_directories(
	${PROJECT_NAME}
	#INTERFACE
	PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}${PROJECT_NAME}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}>
)

################################
#          INSTALL
################################

install(DIRECTORY "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/include/${PROJECT_NAME}"
	DESTINATION "include/${PROJECT_NAME}"
)

# LIB
install(TARGETS ${PROJECT_NAME}
	EXPORT gnuplotppTargets # gruppo di export "gnuplotpp"
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

#install(TARGETS ${PROJECT_NAME}

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/gnuplotppConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/lib/cmake/LC"
        #NO_SET_AND_CHECK_MACRO
        #NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/gnuplotppConfigVersion.cmake"
  VERSION ${CMAKE_PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)
install(
    FILES 
        "${CMAKE_CURRENT_BINARY_DIR}/gnuplotppConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/gnuplotppConfigVersion.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# esporta il gruppo gnuplotpp
install(EXPORT gnuplotppTargets
    DESTINATION "lib/cmake/gnuplotpp"
)