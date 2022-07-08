#ifndef WEBRTCVIDEOCAPTURER_H
#define WEBRTCVIDEOCAPTURER_H
#include "test/test_video_capturer.h"
#include "api/video/video_sink_interface.h"
#include "api/video/video_frame.h"
#include "modules/video_capture/video_capture.h"
#include <videoframemanager.h>

class ClientView;

class WebrtcVideoCapturer :public VideoFrameManager,
                            public rtc::VideoSinkInterface<webrtc::VideoFrame>

{
public:
  static WebrtcVideoCapturer *Create(size_t width, size_t height,
                                     size_t target_fps,
                                     size_t capture_device_index);

  ~WebrtcVideoCapturer();


  void OnFrame(const webrtc::VideoFrame &frame) override;


private:
  void destroy();
    WebrtcVideoCapturer();
    bool Init(size_t width,
              size_t height,
              size_t target_fps,
              size_t capture_device_index);
    rtc::scoped_refptr<webrtc::VideoCaptureModule> mVideoCaptureModule;
   webrtc:: VideoCaptureCapability mCapability;

};



#endif // WEBRTCVIDEOCAPTURER_H
