option(ENABLE_COVERAGE "Enable gcov code coverage" OFF)

if(ENABLE_COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

add_library(test_utils STATIC
    ../src/app/config.h ../src/app/config.cpp
    ../src/app/storage.h ../src/app/storage.cpp
    ../src/app/fingerprint.h ../src/app/fingerprint.cpp
    ../src/app/appcontroller.h ../src/app/appcontroller.cpp
    ../src/app/cameramanager.h ../src/app/cameramanager.cpp
    ../src/app/kiosk.h ../src/app/kiosk.cpp

    ../src/unvr/types.h ../src/unvr/types.cpp
    ../src/unvr/client.h ../src/unvr/client.cpp

    ../src/video/player.h
    ../src/video/ffmpegplayer.h ../src/video/ffmpegplayer.cpp
    ../src/video/snapshotplayer.h ../src/video/snapshotplayer.cpp
    ../src/video/compositeplayer.h ../src/video/compositeplayer.cpp
    ../src/video/relayplayer.h ../src/video/relayplayer.cpp

    ../src/ui/mainwindow.h ../src/ui/mainwindow.cpp
    ../src/ui/setupwizard.h ../src/ui/setupwizard.cpp
    ../src/ui/cameratilewidget.h ../src/ui/cameratilewidget.cpp
    ../src/ui/settingsdialog.h ../src/ui/settingsdialog.cpp
    ../src/ui/pinpad.h ../src/ui/pinpad.cpp
    ../src/ui/pinverifydialog.h ../src/ui/pinverifydialog.cpp
    ../src/ui/edgemenu.h ../src/ui/edgemenu.cpp

    ../src/resources/fonts.qrc
    ../src/resources/icons.qrc
    ../src/resources/kiosk.qrc
)

target_include_directories(test_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_link_libraries(test_utils PUBLIC
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Multimedia Qt6::OpenGLWidgets
    OpenSSL::SSL OpenSSL::Crypto
    X11::X11
)

qt_add_executable(test_config test_config.cpp)
target_link_libraries(test_config PRIVATE test_utils Qt6::Test)
add_test(NAME test_config COMMAND test_config)

qt_add_executable(test_storage test_storage.cpp)
target_link_libraries(test_storage PRIVATE test_utils Qt6::Test)
add_test(NAME test_storage COMMAND test_storage)

qt_add_executable(test_fingerprint test_fingerprint.cpp)
target_link_libraries(test_fingerprint PRIVATE test_utils Qt6::Test)
add_test(NAME test_fingerprint COMMAND test_fingerprint)

qt_add_executable(test_wizard test_wizard.cpp)
target_link_libraries(test_wizard PRIVATE test_utils Qt6::Test)
add_test(NAME test_wizard COMMAND test_wizard)

qt_add_executable(test_appcontroller test_appcontroller.cpp)
target_link_libraries(test_appcontroller PRIVATE test_utils Qt6::Test)
add_test(NAME test_appcontroller COMMAND test_appcontroller)

qt_add_executable(test_client test_client.cpp)
target_link_libraries(test_client PRIVATE test_utils Qt6::Test)
add_test(NAME test_client COMMAND test_client)

qt_add_executable(test_ffmpegplayer test_ffmpegplayer.cpp)
target_link_libraries(test_ffmpegplayer PRIVATE test_utils Qt6::Test)
add_test(NAME test_ffmpegplayer COMMAND test_ffmpegplayer)

qt_add_executable(test_snapshotplayer test_snapshotplayer.cpp)
target_link_libraries(test_snapshotplayer PRIVATE test_utils Qt6::Test)
add_test(NAME test_snapshotplayer COMMAND test_snapshotplayer)

qt_add_executable(test_mainwindow test_mainwindow.cpp)
target_link_libraries(test_mainwindow PRIVATE test_utils Qt6::Test)
add_test(NAME test_mainwindow COMMAND test_mainwindow)

qt_add_executable(test_cameratile test_cameratile.cpp)
target_link_libraries(test_cameratile PRIVATE test_utils Qt6::Test)
add_test(NAME test_cameratile COMMAND test_cameratile)

qt_add_executable(test_settings test_settings.cpp)
target_link_libraries(test_settings PRIVATE test_utils Qt6::Test)
add_test(NAME test_settings COMMAND test_settings)

qt_add_executable(test_pinpad test_pinpad.cpp)
target_link_libraries(test_pinpad PRIVATE test_utils Qt6::Test)
add_test(NAME test_pinpad COMMAND test_pinpad)

qt_add_executable(test_edgemenu test_edgemenu.cpp)
target_link_libraries(test_edgemenu PRIVATE test_utils Qt6::Test)
add_test(NAME test_edgemenu COMMAND test_edgemenu)

qt_add_executable(test_cameramanager test_cameramanager.cpp)
target_link_libraries(test_cameramanager PRIVATE test_utils Qt6::Test)
add_test(NAME test_cameramanager COMMAND test_cameramanager)

qt_add_executable(test_startup test_startup.cpp)
target_link_libraries(test_startup PRIVATE test_utils Qt6::Test)
add_test(NAME test_startup COMMAND test_startup)
