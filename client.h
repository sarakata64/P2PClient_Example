#ifndef CLIENT_H
#define CLIENT_H

#include "api/ice_transport_interface.h"
#include "api/peer_connection_interface.h"
#include "api/media_stream_interface.h"
#include "api/data_channel_interface.h"
#include "webrtcvideocapturer.h"

class ClientView;
using namespace webrtc;
class ServerObserver;
class QVariant;
class AppMainWindow;

class Client : public webrtc::PeerConnectionObserver,
    public webrtc::CreateSessionDescriptionObserver
{
public:
  Client(bool sending);
  Client();
  ~Client();
  ClientView *videoView() const;
  static int count;
  void call(int clientId);

  virtual void
  OnSignalingChange(PeerConnectionInterface::SignalingState new_state) override;

  // Triggered when media is received on a new stream from remote peer.
  virtual void
  OnAddStream(rtc::scoped_refptr<MediaStreamInterface> stream) override;

  // Triggered when a remote peer closes a stream.
  virtual void
  OnRemoveStream(rtc::scoped_refptr<MediaStreamInterface> stream) override;
  virtual void OnIceCandidateError(const std::string& host_candidate,
                                   const std::string& url,
                                   int error_code,
                                   const std::string& error_text) override;


  // Triggered when a remote peer opens a data channel.
  virtual void
  OnDataChannel(rtc::scoped_refptr<DataChannelInterface> data_channel) override;


  virtual void OnRenegotiationNeeded() override;

  virtual void OnIceConnectionChange(
      PeerConnectionInterface::IceConnectionState new_state) override;

  // Called any time the standards-compliant IceConnectionState changes.
  virtual void OnStandardizedIceConnectionChange(
      PeerConnectionInterface::IceConnectionState new_state) override;

  // Called any time the PeerConnectionState changes.
  virtual void OnConnectionChange(
      PeerConnectionInterface::PeerConnectionState new_state) override;

  // Called any time the IceGatheringState changes.
  virtual void OnIceGatheringChange(
      PeerConnectionInterface::IceGatheringState new_state) override;

  // A new ICE candidate has been gathered.
  virtual void OnIceCandidate(const IceCandidateInterface *candidate) override;



  // Ice candidates have been removed.

  virtual void OnIceCandidatesRemoved(
      const std::vector<cricket::Candidate> &candidates) override;

  // Called when the ICE connection receiving status changes.
  virtual void OnIceConnectionReceivingChange(bool receiving) override;

  // Called when the selected candidate pair for the ICE connection changes.


  virtual void
  OnAddTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver,
             const std::vector<rtc::scoped_refptr<MediaStreamInterface> >
                 &streams) override;

  // This is called when signaling indicates a transceiver will be receiving
  // media from the remote endpoint.
  virtual void
  OnTrack(rtc::scoped_refptr<RtpTransceiverInterface> transceiver) override;

  // Called when signaling indicates that media will no longer be received on a
  // track.

  virtual void
  OnRemoveTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver) override;

  // Called when an interesting usage is detected by WebRTC.

  virtual void OnInterestingUsage(int usage_pattern) override;

  virtual void OnSuccess(webrtc::SessionDescriptionInterface *desc) override;
  virtual void OnFailure(webrtc::RTCError error) override;

  bool isSender() const;
  void setIsSender(bool arg_IsSender);


  void
  setPeer_connection(const rtc::scoped_refptr<webrtc::PeerConnectionInterface>
                         &arg_Peer_connection);
  void setHandleOfferAndCreateAnswer(const QVariant &data);
void setHandleAnswer(const QVariant &data);
void handleCandidate(const QVariant &data);

  class DummySetSessionDescriptionObserver
      : public webrtc::SetSessionDescriptionObserver {
  public:
    static DummySetSessionDescriptionObserver *Create() {
      return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
    }
    virtual void OnSuccess();
    virtual void OnFailure(webrtc::RTCError error);
  };


  ServerObserver *serverObserver() const;
  void setServerObserver(ServerObserver *arg_ServerObserver);

  int partnerId() const;

  int getMid() const;
  void setMid(int arg_Mid);

  int peer() const;
  void setPeer(int arg_Peer);

  AppMainWindow *window() const;
  void setWindow(AppMainWindow *arg_Window);
  void renderRemote(void *data);
protected:
  void addTrack();
  void createPeerOffer();
  void handleServerResponse(int peer, const QVariant &result);

private:
  bool mIsSender;
  ServerObserver *mServerObserver;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection = nullptr;
  ClientView *mVideoView, *mRemoteView;

   int mPartnerId=0;
  int mid;
  int mPeer;
  int mPeerLoginId = -1;
  SessionDescriptionInterface *descript;
  AppMainWindow *mWindow;

};

#endif // CLIENT_H
