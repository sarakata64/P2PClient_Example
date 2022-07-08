
QT += gui core opengl network
LIBS += -lopengl32 -lglu32
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# MANDATORY
DEFINES += _UNICODE _ENABLE_EXTENDED_ALIGNED_STORAGE WIN64 QT_DLL QT_WIDGETS_LIB USE_AURA=1 NO_TCMALLOC FULL_SAFE_BROWSING SAFE_BROWSING_CSD SAFE_BROWSING_DB_LOCAL CHROMIUM_BUILD _HAS_EXCEPTIONS=0 __STD_C _CRT_RAND_S _CRT_SECURE_NO_DEPRECATE _SCL_SECURE_NO_DEPRECATE _ATL_NO_OPENGL _WINDOWS CERT_CHAIN_PARA_HAS_EXTRA_FIELDS PSAPI_VERSION=2 _SECURE_ATL _USING_V110_SDK71_ WINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP WIN32_LEAN_AND_MEAN NOMINMAX NTDDI_VERSION=NTDDI_WIN10_RS2 _WIN32_WINNT=0x0A00 WINVER=0x0A00 DYNAMIC_ANNOTATIONS_ENABLED=1 WTF_USE_DYNAMIC_ANNOTATIONS=1  ABSL_ALLOCATOR_NOTHROW=1 ASIO_STANDALONE _WEBSOCKETPP_CPP11_RANDOM_DEVICE_ _WEBSOCKETPP_CPP11_INTERNAL_
DEFINES += WEBRTC_WIN WIN32


SOURCES += \
    appmainwindow.cpp \
    client.cpp \
    clientview.cpp \
    gl_video_shader.cpp \
    i420_texture_cache.cpp \
    main.cpp \
    serverobserver.cpp \
    videoframemanager.cpp \
    videotrack.cpp \
    webrtcengine.cpp \
    webrtcvideocapturer.cpp \
    webrtcvideorenderer.cpp

HEADERS += \
    appmainwindow.h \
    client.h \
    clientview.h \
    gl_video_shader.h \
    i420_texture_cache.h \
    serverobserver.h \
    videoframemanager.h \
    videotrack.h \
    webrtcengine.h \
    webrtcvideocapturer.h \
    webrtcvideorenderer.h

#Webrtc
    Path = C:/Users/assou/Desktop/Web_RTC_11_06_2022/janus-client-main
    # C:/Users/Sheep/Desktop/Inspirations/today12/janus-client-main/janus-client-main/3rd

    INCLUDEPATH +=$$Path/webrtc/include
    INCLUDEPATH += $$Path/webrtc/include/third_party/abseil-cpp

    LIBS += $$Path/webrtc/lib/windows_debug_x64/webrtc.lib -lWS2_32 -lsecur32 -lwinmm -ldmoguids -lwmcodecdspuuid -lmsdmo -lStrmiids -ladvapi32

    QMAKE_CXXFLAGS_DEBUG += /MTd

FORMS += \
    mainwindow.ui

DISTFILES += \
    shaders/I420FragmentShaderSource.frag \
    shaders/nv12FragmentShaderSource.frag \
    shaders/vertex.vert

RESOURCES += \
    shaders.qrc

