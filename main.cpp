#include <QApplication>
#include <webrtcengine.h>
#include "appmainwindow.h"
#include <QDebug>
#include <iostream>
#include "rtc_base/win32_socket_server.h"
#include "rtc_base/win32_socket_init.h"
#include "rtc_base/physical_socket_server.h"
#include <clientview.h>
#include <serverobserver.h>
int main(int argc, char *argv[]) {
 rtc::LogMessage::ConfigureLogging("none debug tstamp thread");


// qSetMessagePattern(" %{file} %{function} %{message} ");
  QApplication a(argc, argv);
  AppMainWindow window;
  window.show();
  int ret = 0;

  webrtc_engine->localClient = new rtc::RefCountedObject<Client>();

  webrtc_engine->intializeEngine();


  ServerObserver serverObserver;
  QObject::connect(&serverObserver, &ServerObserver::connectedToServer,
                   [&serverObserver]() {
                     qDebug() << "is connected  ";
                     serverObserver.login(webrtc_engine->localClient->peer());
                     });
  QObject::connect(&window, &AppMainWindow::callPeer, []() {
    qDebug() << "calling player ";
    webrtc_engine->localClient->call(3);
  });
  QObject::connect(&window, &AppMainWindow::addTrack, [](void *data) {
    qDebug() << "add remote  ";
    webrtc_engine->localClient->renderRemote(data);
  });


  serverObserver.broadcastForServer();


  webrtc_engine->localClient->setServerObserver(&serverObserver);


  webrtc_engine->localClient->setWindow(&window);

    ret = a.exec();



  return ret;
}
