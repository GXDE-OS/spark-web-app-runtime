QT += core gui concurrent webenginewidgets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17 link_pkgconfig
PKGCONFIG += dtk6core dtk6gui dtk6widget

HEADERS += \
        mainwindow.h \
        globaldefine.h \
        httpd.h \
        httplib.h \
        widget.h \
        webengineview.h \
        webenginepage.h \
        application.h \
        webengineurlrequestinterceptor.h

SOURCES += \
        main.cpp \
        httpd.cpp \
        mainwindow.cpp \
        widget.cpp \
        webengineview.cpp \
        webenginepage.cpp \
        application.cpp \
        webengineurlrequestinterceptor.cpp

RESOURCES += \
        resources/resources.qrc

TRANSLATIONS += \
        ../translations/spark-webapp-runtime_zh_CN.ts
