add_subdirectory(kcm)

set(effect_SRCS
    blur.cpp
    blurshader.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(effect_SRCS
    blurconfig.kcfgc
)

find_package(KWaylandServer CONFIG REQUIRED)
set_package_properties(KWaylandServer PROPERTIES
    TYPE REQUIRED
    PURPOSE "For Wayland integration"
)

add_library(kwin4_effect_blur2 SHARED ${effect_SRCS})

target_link_libraries(kwin4_effect_blur2
    Qt5::Core
    Qt5::Gui
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::WindowSystem
    Plasma::KWaylandServer
    kwineffects::kwineffects
    kwineffects::kwinglutils
    epoxy::epoxy
)

install(
    TARGETS
        kwin4_effect_blur2

    DESTINATION
        ${PLUGIN_INSTALL_DIR}/kwin/effects/plugins/
)
