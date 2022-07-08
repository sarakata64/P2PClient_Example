/************************************************************************
*                                                                       *
*  Copyright (c) 2022 by LuMing Software Chenhua.                     *
*  Email: joejob@hotmail.com                                            *
*  All Rights Reserved.                                                 *
*                                                                       *
*************************************************************************/
#include "appmainwindow.h"
#include "ui_mainwindow.h"
#include <clientview.h>
#include <webrtcengine.h>

#include <QDebug>


AppMainWindow::AppMainWindow(QWidget *parent) :
                                          QMainWindow(parent),
                                          ui(new Ui::MainWindow),mLocalClientView(new ClientView())
{
  ui->setupUi(this);
  connect(this, &AppMainWindow::addRemoteView, this,
          [this](ClientView *v) { showRemoteView(v); });

}

AppMainWindow::~AppMainWindow()
{
  delete ui;
}

void AppMainWindow::setupLocalView() {
   if (webrtc_engine->createPeerConnectionForClient(localClient)) {
    qDebug() << "os added ";
    centralWidget()->layout()->addWidget(localClient->videoView());
  } else {

    qDebug() << "is fal8ied ";
  }
}

void AppMainWindow::addView(ClientView *view) {
  ui->local_view->layout()->addWidget(view);
}

void AppMainWindow::showRemoteView(ClientView *aview) {
  centralWidget()->layout()->addWidget(aview);
 }

void AppMainWindow::setPeerId(QString id) { ui->peerLabel->setText(id);
}

void AppMainWindow::on_callButton_clicked() {
  qDebug() << "is calling ";
  callPeer();
}


void AppMainWindow::on_endCallButton_clicked() { endCall(); }




