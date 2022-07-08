
#ifndef WEBRTCVIDEORENDERER_H
#define WEBRTCVIDEORENDERER_H

#include "api/video/video_sink_interface.h"
#include "api/video/video_frame.h"
#include "api/media_stream_interface.h"
#include <clientview.h>
class WebrtcVideoRenderer :public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
  WebrtcVideoRenderer(webrtc::VideoTrackInterface *track_to_render,
                      ClientView *view);
  ~WebrtcVideoRenderer();
  void OnFrame(const webrtc::VideoFrame& frame) override;

private:
  rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
  ClientView *mView;
};

#endif // WEBRTCVIDEORENDERER_H
