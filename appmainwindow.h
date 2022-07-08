#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <client.h>
namespace Ui {
class MainWindow;
}

class ClientView;

class AppMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit AppMainWindow(QWidget *parent = nullptr);
  ~AppMainWindow();

  void setupLocalView();
  void addView(ClientView *view);
  void showRemoteView(ClientView *view);
  void setPeerId(QString id);
signals:
  void callPeer();
  void endCall();
  void addTrack(void *data);
  void addRemoteView(ClientView *view);
private slots:
  void on_callButton_clicked();

  void on_endCallButton_clicked();


private:
  Ui::MainWindow *ui;
  ClientView *mLocalClientView;
  rtc::scoped_refptr<Client> localClient;
};

#endif // MAINWINDOW_H
