cmake_minimum_required(VERSION 3.5)

project(SalesReportManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS
  Widgets
  Core
  Network
  PrintSupport
  REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
  Widgets
  Core
  Network
  PrintSupport
  REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        appsettings.cpp
        balancesheet.cpp
        decompresser.cpp
        exchangerates.cpp
        financialreport.cpp
          dailyreport.cpp
          appcontroller.cpp
          currency.cpp
        preferencesdialog.cpp
        productsbycountry.cpp
        property.cpp
        purchase.cpp
        purchases.cpp
          saleitem.cpp
          reports/subsidiaryledgerrep.cpp
          reports/bycountryrep.cpp
          reports/balancesheetrep.cpp
        subsidiaryledgerreport.cpp
        appsettings.h
        balancesheet.h
        dailyreport.h
        appcontroller.h
        currency.h
        decompresser.h
        exchangerates.h
        financialreport.h
        preferencesdialog.h
        productsbycountry.h
        property.h
        purchase.h
        purchases.h
        saleitem.h
        reports/subsidiaryledgerrep.h
        reports/bycountryrep.h
        reports/balancesheetrep.h
        subsidiaryledgerreport.h
        preferencesdialog.ui

)

add_executable(
  SalesReportManager
  ${PROJECT_SOURCES}
  resources.qrc
)

add_subdirectory(OpenXLSX)
#find_package(ZLIB REQUIRED)
#if (ZLIB_FOUND)
#    include_directories(${ZLIB_INCLUDE_DIRS})
#endif()

#set(ZLIB_TEST_SOURCES zpipe.c)
find_package(ZLIB)
#find_library(ZLIB, static)
#SET(ZLIB_USE_STATIC_LIBS "ON")
add_library(zlib)
#target_include_directories (zlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib
##LIBS += -L/usr/local/opt/zlib/lib -lz
#cmake (-DZLIB_LIBRARY=/lib/path -DZLIB_INCLUDE_DIR=/include/path src_dir)


target_link_libraries(SalesReportManager PRIVATE
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::PrintSupport
  OpenXLSX::OpenXLSX
  ZLIB::ZLIB)

set_target_properties(SalesReportManager PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SalesReportManager)
endif()
