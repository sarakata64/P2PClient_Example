#include "client.h"
#include <QDebug>
#include <videotrack.h>
#include <webrtcengine.h>
#include <clientview.h>
#include <serverobserver.h>
#include <appmainwindow.h>
#include <QLabel>
#include <thread>
#include <webrtcvideorenderer.h>
Client::Client(bool sending)
{

}

Client::Client() {
  mVideoView = new ClientView();
  mRemoteView = new ClientView();
  count++;
  mPeer = count;
}

Client::~Client() {}


void Client::OnSignalingChange(
    PeerConnectionInterface::SignalingState new_state) {
  qDebug() << "In signal "<<"   "<<new_state;
}

void Client::OnAddStream(rtc::scoped_refptr<MediaStreamInterface> stream) {

}

void Client::OnRemoveStream(rtc::scoped_refptr<MediaStreamInterface> stream) {
  qDebug() << "In remove stream ";
}

void Client::OnDataChannel(
    rtc::scoped_refptr<DataChannelInterface> data_channel) {}

void Client::OnRenegotiationNeeded() {
  qDebug() << "In RenegotiationNeeded  \n\n\n\n";
}


void Client::OnIceConnectionChange(
    PeerConnectionInterface::IceConnectionState new_state) {

  qDebug() << "OnIceConnectionChange insd     "<<new_state;
}

void Client::OnStandardizedIceConnectionChange(
    PeerConnectionInterface::IceConnectionState new_state) {
  qDebug() << "OnStandardizedIceConnectionChange  "<<new_state;
}

void Client::OnConnectionChange(
    PeerConnectionInterface::PeerConnectionState new_state) {

  qDebug() << "OOnConnectionChange "<<static_cast<int>( new_state)<<"   "<<mPeerLoginId;
  if (new_state == PeerConnectionInterface::PeerConnectionState::kConnected) {
    qDebug() << "is Conected to peer  ";
  } else {
    qDebug() << "is resovling "<<static_cast<int>( new_state);
  }
}

void Client::OnIceGatheringChange(
    PeerConnectionInterface::IceGatheringState new_state) {
  qDebug() << "OnIceGatheringChange  "<<new_state;
}

void Client::OnIceCandidate(const IceCandidateInterface *candidate) {
  qDebug() << "OnIceCandidate  " << mPeerLoginId;
  qDebug() << __FUNCTION__ << " " << candidate->sdp_mline_index();

    qDebug() << "  ======== " << partnerId();
    std::string sdp_str;
    candidate->ToString(&sdp_str);

    /* sending sdp to remote */

    QVariantMap candidate_;
    candidate_["SdpMidName"] = QByteArray( candidate->sdp_mid().c_str());
    candidate_["SdpMlineIndexName"] = candidate->sdp_mline_index();
    candidate_["partnerId"] = partnerId();
    candidate_["type"] = 4; // Candidates
    candidate_["sdpType"] = -1;
    candidate_["sdp"] = QByteArray(sdp_str.c_str());
    qDebug() << "sending  candiatdate "<<candidate_;
    emit mServerObserver->requestSend(candidate_);


    return;

}



void Client::OnIceCandidatesRemoved(
    const std::vector<cricket::Candidate> &candidates) {
  qDebug() << "OnIceCandidatesRemoved "<<candidates.size();
}

void Client::OnIceConnectionReceivingChange(bool receiving) {
  qDebug() << "OnIceConnectionReceivingChange  "<<receiving;
}


void Client::OnAddTrack(
    rtc::scoped_refptr<RtpReceiverInterface> receiver,
    const std::vector<rtc::scoped_refptr<MediaStreamInterface>> &streams) {


}

void Client::OnTrack(rtc::scoped_refptr<RtpTransceiverInterface> transceiver) {

  qDebug() << "OnTrack";

  auto receiver = transceiver->receiver();
  mWindow->addTrack(receiver->track().release());


}

void Client::OnRemoveTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver) {
  qDebug() << "OnRemoveTrack";
}

void Client::OnInterestingUsage(int usage_pattern) {

  qDebug() << "OnInterestingUsage";
}

void Client::OnSuccess(SessionDescriptionInterface *desc) {
std::string out;
  desc->ToString(&out);
  qDebug() << "In success \n\n" <<mPeerLoginId<< "\n";

  peer_connection->SetLocalDescription(
      DummySetSessionDescriptionObserver::Create(), desc);

  if (desc->GetType() == webrtc::SdpType::kAnswer && mPeerLoginId == 1) {
    qDebug() << "is answer "<<partnerId() ;
    std::string sdp_str;
    desc->ToString(&sdp_str);

    /* sending sdp to remote */
    auto sdp_cp =
        webrtc::CreateSessionDescription(desc->GetType(), sdp_str, nullptr);

    std::string cp_str;
    sdp_cp->ToString(&cp_str);
    QVariantMap answer;
   answer["partnerId"] = partnerId();
   answer["type"] = 3; // answer
   answer["sdpType"] = static_cast<int>( desc->GetType());
 answer["answer"] = QByteArray(out.c_str());

    emit mServerObserver->requestSend(answer);
   return;
 }
 std::string sdp_str;
 desc->ToString(&sdp_str);

 /* sending sdp to remote */
 auto sdp_cp =
     webrtc::CreateSessionDescription(desc->GetType(), sdp_str, nullptr);

 std::string cp_str;
 sdp_cp->ToString(&cp_str);


     QVariantMap offer;
 offer["partnerId"] = partnerId();
 offer["type"] = 2; // offer
 offer["sdpType"] = static_cast<int>( desc->GetType());
 offer["offer"] = QByteArray(out.c_str());
 qDebug() << "sending ";
emit mServerObserver->requestSend(offer);

}

void Client::OnFailure(RTCError error) {
  qDebug() << "[error] err:" << error.message();
  assert(false);
}

bool Client::isSender() const { return mIsSender; }

void Client::setIsSender(bool arg_IsSender) { mIsSender = arg_IsSender; }


void Client::setPeer_connection(
    const rtc::scoped_refptr<webrtc::PeerConnectionInterface>
        &arg_Peer_connection) {
  qDebug() << "Setting peer ";
  peer_connection = arg_Peer_connection;
  addTrack();


}

void Client::setHandleOfferAndCreateAnswer(const QVariant &data) {

  qDebug() << "reading remote response " << " \n ";
  auto dataAsMap = data.toMap();

  int sdpType = dataAsMap.value("sdpType").toUInt();
  QByteArray _sdp = dataAsMap.value("offer").toByteArray();
  absl::optional<webrtc::SdpType> type_maybe =
      static_cast<webrtc::SdpType>(sdpType);

  if (!type_maybe) {
    qDebug() << "Unknown SDP type: " << sdpType;
    return;
  }

  webrtc::SdpType type = *type_maybe;


  std::string sdp(_sdp.constData());
  webrtc::SdpParseError error;
  std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
      webrtc::CreateSessionDescription(type, sdp, &error);

  if (!session_description) {
    qDebug() << "Can't parse received session description message. "
             << "SdpParseError was: " << error.description.c_str();
    return;
  }

  qDebug() << " Received session description :";
  peer_connection->SetRemoteDescription(
      DummySetSessionDescriptionObserver::Create(),
      session_description.release());

  if (type == webrtc::SdpType::kOffer) {
    qDebug() << "Creating answer ";
    peer_connection->CreateAnswer(
        this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
  }

}

void Client::setHandleAnswer(const QVariant &data) {
  qDebug() << "reading  answer response "
           << " \n " << data;

  if (peer_connection->remote_description()) {
    qDebug() << " Remote Peer connection already set "<<peer_connection->remote_description()->type().c_str();
    return;
  }
  auto dataAsMap = data.toMap();

  int sdpType = dataAsMap.value("sdpType").toUInt();
  QByteArray _sdp = dataAsMap.value("answer").toByteArray();
  qDebug() << "sdp " << _sdp;
  absl::optional<webrtc::SdpType> type_maybe =
      static_cast<webrtc::SdpType>(sdpType);

  if (!type_maybe) {
    qDebug() << "Unknown SDP type:  ---  " << sdpType;
    return;
  }



  std::string sdp(_sdp.constData());

  std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
      webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp);

  qDebug() << "csd f  " << session_description.get();
  peer_connection->SetRemoteDescription(
      DummySetSessionDescriptionObserver::Create(),
      session_description.release());
}

void Client::handleCandidate(const QVariant &data) {
  qDebug() << "handle candidate " <<mPeerLoginId;
  ;

  auto dataAsMap = data.toMap();

  QByteArray sdp = dataAsMap.value("sdp").toByteArray();
  QByteArray sdp_mid = dataAsMap.value("SdpMidName").toByteArray();
  int sdp_mlineindex = dataAsMap.value("SdpMlineIndexName").toInt();


  webrtc::SdpParseError error;
  std::unique_ptr<webrtc::IceCandidateInterface> candidate(
      webrtc::CreateIceCandidate(std::string(sdp_mid.constData()),
                                 sdp_mlineindex, std::string(sdp.constData()),
                                 &error));
  if (!candidate.get()) {
    RTC_LOG(WARNING) << "Can't parse received candidate message. "
                     << "SdpParseError was: " << error.description;
    return;
  }
  if (!peer_connection->AddIceCandidate(candidate.get())) {
    RTC_LOG(WARNING) << "Failed to apply the received candidate";
    return;
  } else {

    qDebug() << "is added ";
  }
}


int Client::count = 0;
void Client::addTrack() {
  qDebug() << " senders " << peer_connection->GetSenders();
  if (!peer_connection->GetSenders().empty()) {
    return; // Already added tracks.
  }


  webrtc_engine->local_video_device  = VideoTrack::create();

  if (webrtc_engine->local_video_device) {
    qDebug() << "Video is created ";


    rtc::scoped_refptr<VideoTrackInterface> video_track(
        webrtc_engine->peer_connection_factory->CreateVideoTrack(
            "video_label", webrtc_engine->local_video_device.get()));

    qWarning() << "start rendering video  "<<video_track->id().c_str();
    auto result = peer_connection->AddTrack(video_track, {"stream_id"});

    qDebug() << " result " << result.ok() << "  " << result.error().message();
    if (!result.ok()) {
      qDebug() << "Failed to add video track to PeerConnection: "
                        << result.error().message();
    } else {
      qDebug() << "Adding video view ";
    webrtc_engine->  mLocalRenderer.reset(new WebrtcVideoRenderer(video_track,mVideoView));


    }

  } else {
    qDebug() << "Failed to create video ";
 }

}

void Client::createPeerOffer() {
  qDebug() << " " << mPeerLoginId;
  if (peer_connection && mPeerLoginId==0) {
    qDebug() << "Creating offer " ;
    PeerConnectionInterface::RTCOfferAnswerOptions options(0,1,1,1,1);

    peer_connection->CreateOffer(
        this, options);

  }
}

void Client::handleServerResponse(int peer, const QVariant &result) {
  qDebug() << " server response  " << peer << "  " << mPeer;

  auto resultAsMap = result.toMap();
  auto type = resultAsMap.value("type").toInt();
  qDebug() << "type e  " << type;
  switch (type) {
  case 1: {
    if (peer != mPeer) {
      return;
    }
    // login
    auto response = resultAsMap.value("response").toInt();
    qDebug() << " reponse " << response;
    if (response == 0) {
      // success
      qDebug() << "Succesfully logged in";
      mPeerLoginId = resultAsMap.value("loginId").toInt();
      // initialise client


      qDebug() << " login id " << mPeerLoginId;
      webrtc_engine->createPeerConnectionForClient(this);
      videoView()->peerLabel()->setText(QString::number(mPeerLoginId));
      mWindow->setPeerId(QString("Player %1").arg(mPeerLoginId));
    mWindow->addView(videoView());

    } else {
    }
    break;
  }
  case 2:{
    qDebug() << "handle offer --- ";
    auto targetId = resultAsMap.value("partnerId").toInt();
    qDebug() << " target ids " << targetId<<"   "<<mPeerLoginId;
    if (targetId ==   mPeerLoginId) {

      qDebug() << "handling ";

      setHandleOfferAndCreateAnswer(resultAsMap);


    }
    break;
  }
  case 3: {
    qDebug() << "handle answer --- ";
    auto targetId = resultAsMap.value("partnerId").toInt();
    qDebug() << "target ids " << targetId << "   "
             << mPeerLoginId;
    if (targetId == mPeerLoginId) {

      qDebug() << "handling  ";

      setHandleAnswer(resultAsMap);
    }
    break;
  }
  case 4: {
    qDebug() << "handle candiadtae --- ";
    auto targetId = resultAsMap.value("partnerId").toInt();
    qDebug() << "ids  " << targetId << "   " << mPeerLoginId;
    if (targetId == mPeerLoginId) {

      qDebug() << "handling ";

      handleCandidate(resultAsMap);


    }
    break;
  }
  }
}

AppMainWindow *Client::window() const
{
  return mWindow;
}

void Client::setWindow(AppMainWindow *arg_Window)
{
  mWindow = arg_Window;

}

void Client::renderRemote(void *data) {
  qDebug() << "fsdf  " << data;
  auto *track = reinterpret_cast<webrtc::MediaStreamTrackInterface *>(data);
  if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind) {
    auto *video_track = static_cast<webrtc::VideoTrackInterface *>(track);
    // main_wnd_->StartRemoteRenderer(video_track);
    qDebug() << " video details  " << video_track->GetSource() << "   "
             << video_track->id().c_str() << "  " << video_track->kind().c_str()
             << "   " << video_track->state() << "  "
             << static_cast<int>(video_track->content_hint());
    webrtc_engine->mRemoteRenderer.reset(
        new WebrtcVideoRenderer(video_track, mRemoteView));
    mWindow->addRemoteView(mRemoteView);
  }
  track->Release();


}

int Client::peer() const
{
  return mPeer;
}

void Client::setPeer(int arg_Peer)
{
  mPeer = arg_Peer;
}

int Client::getMid() const
{
  return mid;
}

void Client::setMid(int arg_Mid)
{
  mid = arg_Mid;
}

int Client::partnerId() const
{
  return mPartnerId;
}

ServerObserver *Client::serverObserver() const
{
  return mServerObserver;
}

void Client::setServerObserver(ServerObserver *arg_ServerObserver)
{
  mServerObserver = arg_ServerObserver;
  QObject::connect(mServerObserver, &ServerObserver::complete,
                   [this](int peer, const QVariant &result) {
                     this->handleServerResponse(peer, result);
                   });
  ;
}

ClientView *Client::videoView() const
{
  return mVideoView;
}


void Client::call(int clientId) {
  qWarning() << "Calling player";
  // mServerObserver->login(peer());
  mPartnerId = 1;
  createPeerOffer();

}

void Client::OnIceCandidateError(const std::string &host_candidate,
                                 const std::string &url, int error_code,
                                 const std::string &error_text) {

  qDebug() << "Ice Error  " << host_candidate.c_str() << "   " << url.c_str()
           << "   " << error_code << "   " << error_text.c_str();
}






void Client::DummySetSessionDescriptionObserver::OnSuccess() { qDebug() << "is success in creating ur ";
}

void Client::DummySetSessionDescriptionObserver::OnFailure(RTCError error) {
  qDebug() << " dummy error  " << error.message();
}
