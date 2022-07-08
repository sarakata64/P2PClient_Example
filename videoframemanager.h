#ifndef VIDEOFRAMEMANAGER_H
#define VIDEOFRAMEMANAGER_H
#include "pc/video_track_source.h"
#include <api/video/video_frame.h>
#include <media/base/video_adapter.h>
#include <media/base/video_broadcaster.h>
#include "modules/video_capture/video_capture.h"
class VideoFrameManager :public rtc::VideoSourceInterface<webrtc::VideoFrame>
{
public:
  VideoFrameManager();
  ~VideoFrameManager();
  void on_frame(const webrtc::VideoFrame &frame);

private:
  void AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink,
                       const rtc::VideoSinkWants &wants) override;
  void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink) override;

private:
  rtc::VideoBroadcaster broadcaster_;
  cricket::VideoAdapter video_adapter_;


};


#endif // VIDEOFRAMEMANAGER_H
