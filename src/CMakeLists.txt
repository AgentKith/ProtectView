qt_add_executable(unvr-carousal
    main.cpp

    app/config.h app/config.cpp
    app/storage.h app/storage.cpp
    app/fingerprint.h app/fingerprint.cpp
    app/appcontroller.h app/appcontroller.cpp
    app/cameramanager.h app/cameramanager.cpp

    unvr/types.h unvr/types.cpp
    unvr/client.h unvr/client.cpp

    video/player.h
    video/ffmpegplayer.h video/ffmpegplayer.cpp
    video/snapshotplayer.h video/snapshotplayer.cpp
    video/compositeplayer.h video/compositeplayer.cpp
    video/relayplayer.h video/relayplayer.cpp

    ui/mainwindow.h ui/mainwindow.cpp
    ui/setupwizard.h ui/setupwizard.cpp
    ui/cameratilewidget.h ui/cameratilewidget.cpp
    ui/settingsdialog.h ui/settingsdialog.cpp
    ui/pinpad.h ui/pinpad.cpp
    ui/edgemenu.h ui/edgemenu.cpp

    resources/fonts.qrc
    resources/icons.qrc
    resources/kiosk.qrc
)

target_include_directories(unvr-carousal PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(unvr-carousal PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::Multimedia Qt6::OpenGLWidgets
    OpenSSL::SSL OpenSSL::Crypto
)
