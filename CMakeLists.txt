cmake_minimum_required(VERSION 3.27)
set(CMAKE_PREFIX_PATH "/users/mstangel/Qt/6.6.0/macos/lib/cmake/")
project(wh40kBAlistBuilder VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

function(resource VAR SOURCE_PATH DESTINATION PATTERN)
    file(GLOB_RECURSE _LIST CONFIGURE_DEPENDS ${SOURCE_PATH}/${PATTERN})
    foreach (RESOURCE ${_LIST})
        get_filename_component(_PARENT ${RESOURCE} DIRECTORY)
        if (${_PARENT} STREQUAL ${SOURCE_PATH})
            set(_DESTINATION ${DESTINATION})
        else ()
            file(RELATIVE_PATH _DESTINATION ${SOURCE_PATH} ${_PARENT})
            set(_DESTINATION ${DESTINATION}/${_DESTINATION})
        endif ()
        set_property(SOURCE ${RESOURCE} PROPERTY MACOSX_PACKAGE_LOCATION ${_DESTINATION})
    endforeach (RESOURCE)
    set(${VAR} ${_LIST} PARENT_SCOPE)
endfunction()


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport
        REQUIRED)

# make directory for data files
set(DATAFILES_DIR $ENV{HOME}/wh40kbalistbuilder)
file(MAKE_DIRECTORY ${DATAFILES_DIR})

# copy DataFiles folder. WORKS !! Copies all the files.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/DataFiles" DESTINATION "$ENV{HOME}/wh40kbalistbuilder" )

# putting data files files in bundle.
file(GLOB_RECURSE faction_data_files "${CMAKE_CURRENT_SOURCE_DIR}/DataFiles/*.json")
foreach (FILE ${faction_data_files})
    file(RELATIVE_PATH NEW_FILE "${CMAKE_CURRENT_SOURCE_DIR}/DataFiles" ${FILE})
    get_filename_component(NEW_FILE_PATH ${NEW_FILE} DIRECTORY)
    set_source_files_properties(${FILE}
            PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/DataFiles/${NEW_FILE_PATH}"
    )
endforeach()


set(MACOSX_BUNDLE_ICON_FILE MyIcon.icns)
set(myApp_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Images/MyIcon.icns)
set_source_files_properties(${myApp_ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")


add_executable(wh40kBAlistBuilder MACOSX_BUNDLE  ${myApp_ICON}
        Sources/main.cpp
        Headers/mainwindow.h
        Sources/mainwindow.cpp
        Headers/factiondata.h
        Sources/factiondata.cpp
        Headers/factionchoicewindow.h
        Sources/factionchoicewindow.cpp
        Headers/listbuilderwindow.h
        Sources/listbuilderwindow.cpp
        Headers/errorwindow.h
        Sources/errorwindow.cpp
        ${faction_data_files}
)
#SET_SOURCE_FILES_PROPERTIES(${RES_SOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION DataFiles)

include_directories(Headers)
include_directories(Sources)
#
#foreach (FILE ${DATAFILES})
#    # Get the relative path from the data-folder to the particular file.
#    file(RELATIVE_PATH NEW_FILE "${CMAKE_CURRENT_SOURCE_DIR}/DataFiles" ${FILE})
#
#    # Get the relative path to the file.
#    get_filename_component(NEW_FILE_PATH ${NEW_FILE} DIRECTORY)
#
#    # Set it's location inside the app package (under Resources).
#    set_property(SOURCE ${FILE} PROPERTY MACOSX_PACKAGE_LOCATION "MacOS/DataFiles/${NEW_FILE_PATH}")
#
#    # Optional: Add the file to the 'Resources' folder group in Xcode.
#    #           This also preserves folder structure.
#    # source_group("Resources/${NEW_FILE_PATH}" FILES "${FILE}")
#endforeach ()


target_link_libraries(wh40kBAlistBuilder
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt6::PrintSupport
)


set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.wh40kBAlistBuilder)


set_target_properties(wh40kBAlistBuilder PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
)
include(GNUInstallDirs)

install(TARGETS wh40kBAlistBuilder
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(CPACK_GENERATOR "ZIP")
set(CPACK_SOURCE_GENERATOR "ZIP")
set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_BINARY_DIR}/package)

set(CPACK_SOURCE_IGNORE_FILES .git/ .github/ .vscode/ .mypy_cache/ _CPack_Packages/
        ${CMAKE_BINARY_DIR}/ ${PROJECT_BINARY_DIR}/
)


include(CPack)

