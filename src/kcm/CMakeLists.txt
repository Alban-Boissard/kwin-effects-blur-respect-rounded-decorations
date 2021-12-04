set(kcm_SRCS
    blur_config.cpp
)

kconfig_add_kcfg_files(kcm_SRCS
    ../blurconfig.kcfgc
)

qt5_wrap_ui(kcm_SRCS blur_config.ui)

qt5_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)

add_library(kwin_blur2_config MODULE ${kcm_SRCS})

target_link_libraries(kwin_blur2_config
    Qt5::Core
    Qt5::DBus
    Qt5::Gui
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::ConfigWidgets
)

kcoreaddons_desktop_to_json(kwin_blur2_config blur_config.desktop SERVICE_TYPES kcmodule.desktop)

install(
    TARGETS
        kwin_blur2_config

    DESTINATION
        ${PLUGIN_INSTALL_DIR}/kwin/effects/configs
)
