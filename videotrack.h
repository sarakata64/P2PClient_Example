#ifndef VIDEOTRACK_H
#define VIDEOTRACK_H
#include "pc/video_track_source.h"
#include <api/video/video_frame.h>

#include <webrtcvideocapturer.h>
class ClientView;
class VideoTrack :public webrtc::VideoTrackSource
{
public:
  ~VideoTrack();
  VideoTrack(std::unique_ptr<WebrtcVideoCapturer> capturer);
 static rtc::scoped_refptr<VideoTrack> create();

protected:
  virtual rtc::VideoSourceInterface<webrtc::VideoFrame> *source() override ;
private:
 std::unique_ptr<WebrtcVideoCapturer> mVideosource;


};

#endif // VIDEOTRACK_H
