VERSION = $$cat(version.txt)

PRODUCT_VERSION = $$(PRODUCT_VERSION)
BUILD_NUMBER = $$(BUILD_NUMBER)

!isEmpty(PRODUCT_VERSION){
    !isEmpty(BUILD_NUMBER){
        VERSION = $${PRODUCT_VERSION}.$${BUILD_NUMBER}
    }
}

DEFINES += INTVER=$$VERSION

PUBLISHER_NAME = $$(PUBLISHER_NAME)
isEmpty(PUBLISHER_NAME){
    PUBLISHER_NAME = $$cat(copyright.txt)
}

OO_BUILD_BRANDING = $$(OO_BRANDING)
OO_DESTDIR_BUILD_OVERRIDE = $$(DESTDIR_BUILD_OVERRIDE)

win32 {
    CURRENT_YEAR = $$system(wmic PATH Win32_LocalTime GET ^Year /FORMAT:VALUE | find \"=\")
    CURRENT_YEAR = $$replace(CURRENT_YEAR, "Year=", "")
    CURRENT_YEAR = $$replace(CURRENT_YEAR, "\r", "")
    CURRENT_YEAR = $$replace(CURRENT_YEAR, "\n", "")
    CURRENT_YEAR = $$replace(CURRENT_YEAR, "\t", "")
}

!win32 {
    CURRENT_YEAR = $$system(date +%Y)
}

QMAKE_TARGET_COMPANY = $$PUBLISHER_NAME
QMAKE_TARGET_COPYRIGHT = Copyright (C) $${PUBLISHER_NAME} $${CURRENT_YEAR}. All rights reserved

# CONFIGURATION
CONFIG(debug, debug|release) {
    CONFIG += core_debug
} else {
    CONFIG += core_release
}

not_use_dynamic_libs {
shared {
    CONFIG -= shared
    CONFIG -= plugin
    CONFIG += staticlib
}
}

#PLATFORM
win32 {
    CONFIG += core_windows

    WINDOWS_VERSION_XP = $$(WINDOWS_VERSION_XP)
    !isEmpty(WINDOWS_VERSION_XP) {
        CONFIG += build_xp
        message(xp using)
    }
}

DST_ARCH=$$QMAKE_TARGET.arch
isEqual(QT_MAJOR_VERSION, 5) {
    DST_ARCH=$$QT_ARCH
    # QT_ARCH uses 'i386' instead of 'x86',
    # so map that value back to what we expect.
    equals(DST_ARCH, i386) {
        DST_ARCH=x86
    }
}

DEFINES += INTERNAL_USE_ARRAY_AS_VECTOR

ios {
    CONFIG += core_ios
    DEFINES += _IOS IOS LINUX _LINUX MAC _MAC _XCODE
}
android {
    CONFIG += core_android
    DEFINES += __ANDROID__ LINUX _LINUX
}

win32:contains(QMAKE_TARGET.arch, x86_64): {
    CONFIG += core_win_64
}
win32:!contains(QMAKE_TARGET.arch, x86_64): {
    CONFIG += core_win_32
}

linux-g++ {
    CONFIG += core_linux
linux-g++:contains(QMAKE_HOST.arch, x86_64): {
    message("linux-64")
    CONFIG += core_linux_64
}
linux-g++:!contains(QMAKE_HOST.arch, x86_64): {
    message("linux-32")
    CONFIG += core_linux_32
}
linux-g++:contains(DST_ARCH, arm): {
    message("arm")
    CONFIG += core_linux_arm
    DEFINES += LINUX_ARM
}
}

linux-g++-64 {
    message("linux-64")
    CONFIG += core_linux
    CONFIG += core_linux_64
}
linux-g++-32 {
    message("linux-32")
    CONFIG += core_linux
    CONFIG += core_linux_32
}


mac {
    !core_ios {
        CONFIG += core_mac
        CONFIG += core_mac_64
    }
}

# DEFINES
core_windows {
    DEFINES += WIN32 _WIN32
    DEFINES += NOMINMAX
}
core_win_64 {
    DEFINES += WIN64 _WIN64
}

core_linux {
    DEFINES += LINUX _LINUX
}

core_mac {
    DEFINES += LINUX _LINUX MAC _MAC
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
    QMAKE_LFLAGS += -isysroot $$QMAKE_MAC_SDK_PATH
}

# PREFIXES
core_windows {
    CONFIG -= debug_and_release debug_and_release_target
    QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
    QMAKE_CXXFLAGS -= -Zc:strictStrings
    QMAKE_CXXFLAGS += /MP

    OO_WINDOWS_IGNORE_PACKING_MISMATCH = $$(OO_WINDOWS_IGNORE_PACKING_MISMATCH)
    isEqual(OO_WINDOWS_IGNORE_PACKING_MISMATCH, 1):QMAKE_CXXFLAGS -= /Zp

    equals(TEMPLATE, app) {
        console {
            core_win_64:QMAKE_LFLAGS_CONSOLE = /SUBSYSTEM:CONSOLE,5.02
            core_win_32:QMAKE_LFLAGS_CONSOLE = /SUBSYSTEM:CONSOLE,5.01
        } else {
            core_win_64:QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.02
            core_win_32:QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
        }
    }
}

core_linux {
    equals(TEMPLATE, app) {
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/system\'"
        QMAKE_LFLAGS += -Wl,--disable-new-dtags
    }
}

core_win_32 {
    CORE_BUILDS_PLATFORM_PREFIX = win_32
}
core_win_64 {
    CORE_BUILDS_PLATFORM_PREFIX = win_64
}
core_linux_32 {
    CORE_BUILDS_PLATFORM_PREFIX = linux_32
}
core_linux_64 {
    CORE_BUILDS_PLATFORM_PREFIX = linux_64
}
core_mac_64 {
    CORE_BUILDS_PLATFORM_PREFIX = mac_64
}
core_linux_arm {
    CORE_BUILDS_PLATFORM_PREFIX = arm
}
core_ios {
    CORE_BUILDS_PLATFORM_PREFIX = ios

    CONFIG(iphonesimulator, iphoneos|iphonesimulator): {
        message("iphonesimulator")
        CORE_BUILDS_PLATFORM_PREFIX = ios_simulator
    } else {

        QMAKE_IOS_DEPLOYMENT_TARGET = 10.0
        CONFIG += core_ios_main_arch

        QMAKE_CFLAGS += -fembed-bitcode
        QMAKE_CXXFLAGS += -fembed-bitcode
        QMAKE_LFLAGS += -fembed-bitcode

        core_ios_main_arch {
            QMAKE_APPLE_DEVICE_ARCHS = arm64
            core_ios_no_simulator_arch : QMAKE_APPLE_SIMULATOR_ARCHS=

            !core_ios_no_32 {
                QMAKE_APPLE_DEVICE_ARCHS = $$QMAKE_APPLE_DEVICE_ARCHS armv7
            }
        } else {
            plugin : TARGET = $$join(TARGET, TARGET, "", "_addition")
            QMAKE_APPLE_DEVICE_ARCHS=
            QMAKE_APPLE_SIMULATOR_ARCHS=
        }

        core_ios_nomain_arch {
            QMAKE_APPLE_DEVICE_ARCHS = $$QMAKE_APPLE_DEVICE_ARCHS arm64e
            !core_ios_no_32 {
                QMAKE_APPLE_DEVICE_ARCHS = $$QMAKE_APPLE_DEVICE_ARCHS armv7s
            }
        }

        !no_verify_bitcode {
            # for application: (using xcodebuild)
            #Q_ENABLE_BITCODE.name = ENABLE_BITCODE
            #Q_ENABLE_BITCODE.value = NO
            #
            #Q_BITCODE_GENERATION_MODE.name = BITCODE_GENERATION_MODE
            #Q_BITCODE_GENERATION_MODE.value = marker
            #
            #QMAKE_MAC_XCODE_SETTINGS += Q_ENABLE_BITCODE
            #QMAKE_MAC_XCODE_SETTINGS += Q_BITCODE_GENERATION_MODE

            # for libraries
            !core_debug {
                QMAKE_LFLAGS += -Xlinker -bitcode_verify
                QMAKE_LFLAGS += -Xlinker -bitcode_hide_symbols
                #QMAKE_LFLAGS += -Xlinker -bitcode_symbol_map -Xlinker $$CORE_BUILDS_LIBRARIES_PATH
            }
        }
    }

    !core_ios_no_unistd {
        DEFINES += HAVE_UNISTD_H
    }
}

core_android {
    CORE_BUILDS_PLATFORM_PREFIX = $$join(ANDROID_TARGET_ARCH, ANDROID_TARGET_ARCH, "android_", "")
    CORE_BUILDS_PLATFORM_PREFIX = $$replace(CORE_BUILDS_PLATFORM_PREFIX, "-", "_")
    CORE_BUILDS_PLATFORM_PREFIX = $$replace(CORE_BUILDS_PLATFORM_PREFIX, "armeabi_v7", "armv7")
    CORE_BUILDS_PLATFORM_PREFIX = $$replace(CORE_BUILDS_PLATFORM_PREFIX, "armv7a", "armv7")

    !isEmpty(OO_DESTDIR_BUILD_OVERRIDE) {
        isEqual(CORE_BUILDS_PLATFORM_PREFIX, android_arm64_v8a):OO_DESTDIR_BUILD_OVERRIDE=$$OO_DESTDIR_BUILD_OVERRIDE/arm64-v8a
        isEqual(CORE_BUILDS_PLATFORM_PREFIX, android_armv7):OO_DESTDIR_BUILD_OVERRIDE=$$OO_DESTDIR_BUILD_OVERRIDE/armeabi-v7a
        isEqual(CORE_BUILDS_PLATFORM_PREFIX, android_x86):OO_DESTDIR_BUILD_OVERRIDE=$$OO_DESTDIR_BUILD_OVERRIDE/x86
        isEqual(CORE_BUILDS_PLATFORM_PREFIX, android_x86_64):OO_DESTDIR_BUILD_OVERRIDE=$$OO_DESTDIR_BUILD_OVERRIDE/x86_64
    }

    core_debug {
        QMAKE_LFLAGS += -Wl,--build-id=sha1
    }

    !core_android_no_unistd {
        DEFINES += HAVE_UNISTD_H
    }
}

core_debug {
    CORE_BUILDS_CONFIGURATION_PREFIX    = debug
}
core_release {
    CORE_BUILDS_CONFIGURATION_PREFIX    = release
}

# MESSAGE
message($$CORE_BUILDS_PLATFORM_PREFIX/$$CORE_BUILDS_CONFIGURATION_PREFIX)

# COMPILER
CONFIG += c++11

core_linux {
core_static_link_libstd {
    QMAKE_LFLAGS += -static-libstdc++ -static-libgcc
    message(core_static_link_libstd)
}
plugin {
    QMAKE_CXXFLAGS += -fvisibility=hidden
    QMAKE_CFLAGS += -fvisibility=hidden

    TARGET_EXT = .so
}
}

core_mac {
plugin {
    QMAKE_CXXFLAGS += -fvisibility=hidden
    QMAKE_CFLAGS += -fvisibility=hidden
}
}

core_windows {
plugin {
    TARGET_EXT = .dll
}
}

# BUILD_PATHS
# MUST BE DEFINED CORE_ROOT_DIR
!core_no_dst {
DESTDIR     = $$PWD_ROOT_DIR/core_build/$$CORE_BUILDS_PLATFORM_PREFIX/$$CORE_BUILDS_CONFIGURATION_PREFIX
OBJECTS_DIR = $$PWD_ROOT_DIR/core_build/$$CORE_BUILDS_PLATFORM_PREFIX/$$CORE_BUILDS_CONFIGURATION_PREFIX/obj
MOC_DIR     = $$PWD_ROOT_DIR/core_build/$$CORE_BUILDS_PLATFORM_PREFIX/$$CORE_BUILDS_CONFIGURATION_PREFIX/moc
RCC_DIR     = $$PWD_ROOT_DIR/core_build/$$CORE_BUILDS_PLATFORM_PREFIX/$$CORE_BUILDS_CONFIGURATION_PREFIX/rcc
UI_DIR      = $$PWD_ROOT_DIR/core_build/$$CORE_BUILDS_PLATFORM_PREFIX/$$CORE_BUILDS_CONFIGURATION_PREFIX/ui
build_xp {
    OBJECTS_DIR = $$OBJECTS_DIR/xp
    MOC_DIR     = $$MOC_DIR/xp
    RCC_DIR     = $$RCC_DIR/xp
    UI_DIR      = $$UI_DIR/xp
}
}

CORE_BUILDS_LIBRARIES_PATH = $$CORE_ROOT_DIR/build/lib/$$CORE_BUILDS_PLATFORM_PREFIX
CORE_BUILDS_BINARY_PATH = $$CORE_ROOT_DIR/build/bin/$$CORE_BUILDS_PLATFORM_PREFIX

!isEmpty(OO_BUILD_BRANDING) {
    CORE_BUILDS_LIBRARIES_PATH = $$CORE_ROOT_DIR/build/$$OO_BUILD_BRANDING/lib/$$CORE_BUILDS_PLATFORM_PREFIX
    CORE_BUILDS_BINARY_PATH = $$CORE_ROOT_DIR/build/$$OO_BUILD_BRANDING/bin/$$CORE_BUILDS_PLATFORM_PREFIX
}

core_debug {
    CORE_BUILDS_LIBRARIES_PATH = $$CORE_BUILDS_LIBRARIES_PATH/debug
    CORE_BUILDS_BINARY_PATH = $$CORE_BUILDS_BINARY_PATH/debug
}

!isEmpty(OO_DESTDIR_BUILD_OVERRIDE) {
    CORE_BUILDS_LIBRARIES_PATH = $$OO_DESTDIR_BUILD_OVERRIDE
    CORE_BUILDS_BINARY_PATH = $$OO_DESTDIR_BUILD_OVERRIDE
}

plugin {
    DESTDIR = $$CORE_BUILDS_LIBRARIES_PATH
}
staticlib {
    DESTDIR = $$CORE_BUILDS_LIBRARIES_PATH
}

core_x2t {
core_windows {
    QMAKE_CXXFLAGS += /bigobj
} else {
    QMAKE_CXXFLAGS += -Wall -Wno-ignored-qualifiers
}

DEFINES += CRYPTOPP_DISABLE_ASM
}

core_ios {
    bundle_dylibs {
        plugin {
            CONFIG -= plugin
            CONFIG += lib_bundle

            QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
            #QMAKE_LFLAGS += -Xlinker -rpath -Xlinker @executable_path/Frameworks
            #QMAKE_LFLAGS += -Xlinker -rpath -Xlinker @loader_path/Frameworks

            # correct version to < 256
            VERSIONS = $$split(VERSION, ".")
            VERSION_1 = $$member(VERSIONS, 0)
            VERSION_2 = $$member(VERSIONS, 1)
            VERSION_3 = $$member(VERSIONS, 2)
            VERSION_4 = $$member(VERSIONS, 3)

            greaterThan(VERSION_1, 255): VERSION_1 = 255
            greaterThan(VERSION_2, 255): VERSION_2 = 255
            greaterThan(VERSION_3, 255): VERSION_3 = 255
            greaterThan(VERSION_4, 255): VERSION_4 = 255

            VERSION_CORRECT = $$VERSION_1
            VERSION_CORRECT = $$join(VERSION_CORRECT, "", "", ".")
            VERSION_CORRECT = $$join(VERSION_CORRECT, "", "", $$VERSION_2)
            VERSION_CORRECT = $$join(VERSION_CORRECT, "", "", ".")
            VERSION_CORRECT = $$join(VERSION_CORRECT, "", "", $$VERSION_3)
            VERSION_CORRECT = $$join(VERSION_CORRECT, "", "", ".")
            VERSION_CORRECT = $$join(VERSION_CORRECT, "", "", $$VERSION_4)

            VERSION = $$VERSION_CORRECT
        }
    }
}

defineTest(ADD_DEPENDENCY) {
    libs = $$ARGS
    for(lib, libs) {
        CORE_BUILDS_LIBRARIES_PATH_DST=$$CORE_BUILDS_LIBRARIES_PATH
        build_xp {
            isEqual(lib, doctrenderer):CORE_BUILDS_LIBRARIES_PATH_DST=$$CORE_BUILDS_LIBRARIES_PATH/xp
            isEqual(lib, ascdocumentscore):CORE_BUILDS_LIBRARIES_PATH_DST=$$CORE_BUILDS_LIBRARIES_PATH/xp
            isEqual(lib, videoplayer):CORE_BUILDS_LIBRARIES_PATH_DST=$$CORE_BUILDS_LIBRARIES_PATH/xp
            isEqual(lib, ooxmlsignature):CORE_BUILDS_LIBRARIES_PATH_DST=$$CORE_BUILDS_LIBRARIES_PATH/xp
        }
        !bundle_dylibs:LIBS += -L$$CORE_BUILDS_LIBRARIES_PATH_DST -l$$lib
        bundle_dylibs:LIBS += -F$$CORE_BUILDS_LIBRARIES_PATH_DST -framework $$lib
    }
    export(LIBS)
}

ADD_INC_PATH = $$(ADDITIONAL_INCLUDE_PATH)
!isEmpty(ADD_INC_PATH):INCLUDEPATH += $$ADD_INC_PATH
