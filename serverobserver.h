#ifndef SERVEROBSERVER_H
#define SERVEROBSERVER_H

#include <QObject>

class QUdpSocket;
class QSslSocket;
class QTcpSocket;
class ServerObserver : public QObject
{
  Q_OBJECT
public:
  explicit ServerObserver(QObject *parent = nullptr);

public:
  ~ServerObserver();
  void login(int peer);
void  send(QSslSocket *socket, const QVariant &data, int delay = 1);

  QSslSocket *tcpSocket() const;

public slots:
  void broadcastForServer();

signals:
  void connectedToServer();
  void complete(int peer, const QVariant &result);
  void requestSend(const QVariant &data);

protected:
  void connectToServer();

protected slots:
  void saveServerAddress();
  void readServer();
  void connected();

private:


  QUdpSocket *m_servListener; // for discovering the server
  QSslSocket *m_tcpSocket;
  QString m_server;
};

#endif // SERVEROBSERVER_H
