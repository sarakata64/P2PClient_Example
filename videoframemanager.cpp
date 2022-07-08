#include "videoframemanager.h"
#include <QDebug>
VideoFrameManager::VideoFrameManager() { qDebug() << "Creating video manabher ";
}
VideoFrameManager::~VideoFrameManager()
{

}

void VideoFrameManager::on_frame(const webrtc::VideoFrame& frame) {
// broadcast frames directly
  broadcaster_.OnFrame(frame);

}

void VideoFrameManager::AddOrUpdateSink(
    rtc::VideoSinkInterface<webrtc::VideoFrame> *sink,
    const rtc::VideoSinkWants &wants) {
  qDebug() << "Adding sink "<<sink<<"   "<<wants.max_pixel_count<<"  "<<wants.black_frames<<"  "<<wants.resolutions.size();
  broadcaster_.AddOrUpdateSink(sink, wants);
 video_adapter_.OnSinkWants(broadcaster_.wants());
 }

void VideoFrameManager::RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink) {
  qDebug() << "in removing sink";
    broadcaster_.RemoveSink(sink);
 }
