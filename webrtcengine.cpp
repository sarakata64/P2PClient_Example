#include "webrtcengine.h"
#include <QSharedPointer>
#include <QDebug>
#include <client.h>

WebRtcEngine::WebRtcEngine()
{

}
WebRtcEngine *WebRtcEngine::instance() {
  static QSharedPointer<WebRtcEngine> engine;
  if (!engine.data())
    engine = QSharedPointer<WebRtcEngine>(new WebRtcEngine);

  return engine.data();
}

bool WebRtcEngine::intializeEngine() {

    rtc::scoped_refptr<AudioMixer> audio_mixer;
    rtc::scoped_refptr<AudioProcessing> audio_processing;
  if (!signaling_thread.get()) {
    signaling_thread = rtc::Thread::CreateWithSocketServer();
    signaling_thread->SetName("pc_signal_thread", nullptr);
    signaling_thread->Start();
  }

  if (!workerThread) {
    workerThread = rtc::Thread::Create();
    workerThread->SetName("pc_worker_thread", nullptr);
    workerThread->Start();
  }
  if (networkThread) {
    networkThread = rtc::Thread::CreateWithSocketServer();
    networkThread->SetName("pc_network_thread", nullptr);
    networkThread->Start();
  }
  peer_connection_factory = webrtc::CreatePeerConnectionFactory(
      networkThread.get() /* network_thread */,
      workerThread.get() /* worker_thread */,
      signaling_thread.get() /* signaling_thread */, nullptr /* default_adm */,
      webrtc::CreateBuiltinAudioEncoderFactory(),
      webrtc::CreateBuiltinAudioDecoderFactory(),
      webrtc::CreateBuiltinVideoEncoderFactory(),
      webrtc::CreateBuiltinVideoDecoderFactory(),
      audio_mixer ,
      audio_processing );

  if (!peer_connection_factory) {
    qWarning() << "Failed to create a peer connection instance";
    return false;
  }
  qDebug() << "is created  ";
  return true;
}

bool WebRtcEngine::createPeerConnectionForClient(rtc::scoped_refptr<Client> client) {

  if (!peer_connection_factory) {
    qWarning() << "Peer connection factory is null ";
    return false;
  }

  webrtc::PeerConnectionInterface::RTCConfiguration config;
  config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;

  webrtc::PeerConnectionInterface::IceServer server;
  server.uri = "stun:stun.l.google.com:19302";
  // config.servers.push_back(server);

  webrtc::PeerConnectionDependencies pc_dependencies(client);

  auto error_or_peer_connection =
      peer_connection_factory->CreatePeerConnectionOrError(
          config, std::move(pc_dependencies));


  if (error_or_peer_connection.ok()) {
    qDebug() << "Setting peer connection for client " << client;
    client->setPeer_connection( std::move(error_or_peer_connection.value()));
  }
  return peer_connection != nullptr;

}
