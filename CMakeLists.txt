cmake_minimum_required(VERSION 3.16)
enable_testing()

project(FileBasket VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)

set(PROJECT_SOURCES
        main.cpp
        ui/window.cpp
        ui/window.h
        ui/window.ui
)

# Get count of commits
execute_process(
    COMMAND git rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_COUNT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Updating CMake for correct count of commits
set(GIT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.git")
if(EXISTS "${GIT_DIR}/HEAD")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${GIT_DIR}/HEAD")
    if(EXISTS "${GIT_DIR}/index")
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${GIT_DIR}/index")
    endif()
endif()

# If project is built outside repository
if(NOT GIT_COMMIT_COUNT)
    set(GIT_COMMIT_COUNT "0")
endif()

add_compile_definitions(APP_BUILD_NUMBER=${GIT_COMMIT_COUNT})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FileBasket
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        model/AppModel.h model/AppModel.cpp
        model/FileListModel.h model/FileListModel.cpp
        services/FileOperationService.h services/FileOperationService.cpp
        services/PersistenceService.h services/PersistenceService.cpp

        ui/FileListView.h ui/FileListView.cpp
        controller/FileBasketController.h controller/FileBasketController.cpp
        ui/res.qrc
        ui/FileItemDelegate.h ui/FileItemDelegate.cpp
        ui/CustomTabBar.h ui/CustomTabBar.cpp
        services/UpdateCheckerService.h services/UpdateCheckerService.cpp
        services/LicenseService.h services/LicenseService.cpp
        Config.h
    )

  target_include_directories(FileBasket PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
  )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET FileBasket APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(FileBasket SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(FileBasket
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(FileBasket PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(FileBasket PRIVATE Qt6::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FileBasket)
endif()
set_target_properties(FileBasket PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FileBasket
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FileBasket)
endif()
add_subdirectory(Tests)
