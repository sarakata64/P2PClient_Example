
#include "webrtcvideorenderer.h"

WebrtcVideoRenderer::WebrtcVideoRenderer(webrtc::VideoTrackInterface *track_to_render,
                                         ClientView *view):rendered_track_(track_to_render),mView(view)
{
  rendered_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());

}

WebrtcVideoRenderer::~WebrtcVideoRenderer() {
  qDebug() << "Destroying ";
}

void WebrtcVideoRenderer::OnFrame(const webrtc::VideoFrame &frame)
{

     if (mView) {
     mView->renderFrame(frame);
     } else {

     }
}
