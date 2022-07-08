#include "serverobserver.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QCoreApplication>
#include <QSslSocket>
#include <QDataStream>

// server ports
#define GW_SERVER_UDP_PORT 5500
#define GW_SERVER_TCP_PORT 5939
#define GW_SERVER_LOOKUP_PORT 6060

QVariant receive(QTcpSocket *socket) {
  QVariant m_data;
  if (socket) {
    QDataStream in(socket);
    in >> m_data;
  }
  return m_data;
}
QByteArray streamOut(const QVariant &data) {
  QByteArray bytedata = QByteArray();
  QDataStream outStream(&bytedata, QIODevice::WriteOnly);
  outStream << QVariant(data);
  return bytedata;
}

void tcpSend(QVariant msg, QTcpSocket *tcpSocket) {

  if (tcpSocket) {
    QByteArray c = streamOut(msg);
   auto s = tcpSocket->write(streamOut(msg));
    if (!s) {
      qDebug() << "failed ";
      qDebug() << tcpSocket->errorString();
      return;
    }
  }
}


void ServerObserver::send(QSslSocket *socket, const QVariant &data, int delay) {
  if (!socket) {
    qDebug() << "Cannot send, Socket is null ";
    return;
  }

  qDebug() << "sent "<<data;
  tcpSend(data, socket);
}
QString readBroadcast(QUdpSocket *sock, QHostAddress *senderPtr,
                          bool readFromLocal) {
  if (!sock)
    return QString();

  QHostAddress sender;
  QString data;

  while (sock->hasPendingDatagrams()) {
    QByteArray datagram;
    datagram.resize(sock->pendingDatagramSize());
    if (sock->readDatagram(datagram.data(), datagram.size(), &sender) == -1) {
      qDebug() << sock->errorString();
      continue;
    }

    if (readFromLocal) {
      QDataStream in(&datagram, QIODevice::ReadOnly);


      QVariant msg;
      in >> msg;
      data += msg.toString();

    }


  }
  if (senderPtr)
    *senderPtr = sender;
  return data;
}



ServerObserver::ServerObserver(QObject *parent) : QObject(parent),m_tcpSocket(nullptr),m_servListener(nullptr)
{
  m_servListener = new QUdpSocket(this);
  connect(m_servListener, SIGNAL(readyRead()), this, SLOT(saveServerAddress()));
  if (!m_servListener->bind(GW_SERVER_LOOKUP_PORT,
                            QUdpSocket::ShareAddress |
                                QUdpSocket::ReuseAddressHint)) {
    qDebug() << "Failed = = = = = ";
  }
  connect(this, &ServerObserver::requestSend, this,
          [this](const QVariant &data) {
            qDebug() << "Sending "<<data;
            send(this->tcpSocket(),data);
  });
}
QByteArray wrapString(QString message) {

  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out << QVariant(message);

  return block;
}


void broadcast(QString msg, int port) {
  QUdpSocket socket;
  if (socket.writeDatagram(wrapString(msg), QHostAddress::Broadcast, port) ==
      -1) {
    qDebug() << socket.errorString();
    return;
  }
  socket.close();
}
ServerObserver::~ServerObserver() {
  if (m_tcpSocket) {
    m_tcpSocket->waitForDisconnected();
  }
}

void ServerObserver::login(int peer) {
  QVariantMap s;
  s["type"] = 1;
  s["peer"] = peer;
  qDebug() << "Send "<<s;
  tcpSend(s, m_tcpSocket);
}

void ServerObserver::broadcastForServer() {
  qDebug() << "server ";
  if (!m_server.isEmpty())
    return;
  qDebug() << "wating ";
  broadcast("waiting for server", GW_SERVER_UDP_PORT);
  QTimer::singleShot(1000, this, SLOT(broadcastForServer()));
}

void ServerObserver::saveServerAddress() {
  qDebug() << "is ready ";
  disconnect(m_servListener, SIGNAL(readyRead()), 0, 0);
  QHostAddress sender;
  readBroadcast(m_servListener, &sender, true);
  m_server = sender.toString();
  m_servListener->close();
  connectToServer();
}
void ServerObserver::connectToServer() {

  qDebug() << "Connecting to server ";
  m_tcpSocket = new QSslSocket(this);
  connect(m_tcpSocket, &QSslSocket::stateChanged, this,
          []() { qDebug() << "socket state changed "; });
  connect(m_tcpSocket, &QSslSocket::encrypted, this,
          []() { qDebug() << "socket is encrypyed "; });
  connect(m_tcpSocket, &QSslSocket::errorOccurred, this, [this]() {
    qDebug() << "An error Occured when connecting  "
             << m_tcpSocket->errorString();
  });
  connect(m_tcpSocket,
          QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors), this,
          [this](const QList<QSslError> &) { m_tcpSocket->ignoreSslErrors(); });

  m_tcpSocket->connectToHost(m_server, GW_SERVER_TCP_PORT,
                             QIODevice::ReadWrite);
  connect(m_tcpSocket, &QTcpSocket::connected, this,
          &ServerObserver::connected);
  connect(m_tcpSocket, &QTcpSocket::readyRead, this, &ServerObserver:: readServer);

  connect(m_tcpSocket, &QTcpSocket::disconnected, this, [this]() {
    m_tcpSocket->deleteLater();
    m_tcpSocket = nullptr;

    qDebug() << "socket is disconnected ";
  });
}
void ServerObserver::readServer() {

  QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
  QVariant data = receive(socket);
  auto dataAsMap = data.toMap();
  qDebug() << "Process server response" << dataAsMap;

  int type = dataAsMap.value("type").toInt();
  qDebug() << "Processing " << type;
  switch (type) {
  case 1: {
    auto response = dataAsMap.value("response");
    int peer = dataAsMap.value("peer").toInt();
    emit complete(peer, dataAsMap);
    break;
  }
  case 2: {
    int peer = dataAsMap.value("peer").toInt();
    emit complete(peer, dataAsMap);

    break;
  }
  case 3: {
    int peer = dataAsMap.value("peer").toInt();
    emit complete(peer, dataAsMap);

  }
  case 4: {
    qDebug() << "In canfdiadate ";
    int peer = dataAsMap.value("peer").toInt();
    emit complete(peer, dataAsMap);

  }
  }
}

void ServerObserver::connected() {

  emit connectedToServer();
}

QSslSocket *ServerObserver::tcpSocket() const
{
  return m_tcpSocket;
}


