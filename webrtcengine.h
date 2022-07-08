#ifndef WEBRTCENGINE_H
#define WEBRTCENGINE_H

#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "p2p/base/port_allocator.h"
#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include <WebrtcVideoRenderer.h>
#include <videotrack.h>
class Client;

#if defined(webrtc_engine)
#undef webrtc_engine
#endif
#define webrtc_engine (static_cast<WebRtcEngine *>(WebRtcEngine::instance()))


class WebRtcEngine
{
public:
  static  WebRtcEngine *instance();

  bool intializeEngine();

  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
      peer_connection_factory;

  bool createPeerConnectionForClient(rtc::scoped_refptr<Client> client);
  rtc::scoped_refptr<Client> localClient;
rtc::scoped_refptr<Client> remoteClient;
  // threads
  std::unique_ptr<rtc::Thread> workerThread;
  std::unique_ptr<rtc::Thread> networkThread;
  std::unique_ptr<rtc::Thread> signaling_thread;

  std::unique_ptr<WebrtcVideoRenderer> mRemoteRenderer;
  std::unique_ptr<WebrtcVideoRenderer> mLocalRenderer;

  rtc::scoped_refptr<VideoTrack> local_video_device;


protected:

  WebRtcEngine();
};

#endif // WEBRTCENGINE_H
