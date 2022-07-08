#include "videotrack.h"
#include <QDebug>
#include "test/vcm_capturer.h"
#include "modules/video_capture/video_capture_factory.h"
#include <webrtcvideocapturer.h>
using namespace webrtc;

VideoTrack::~VideoTrack() { qDebug() << "vi idf dfdestrif f "<<this;
}

VideoTrack::VideoTrack(std::unique_ptr<WebrtcVideoCapturer> capturer):webrtc::VideoTrackSource(false),mVideosource(std::move(capturer))
{

}

rtc::scoped_refptr<VideoTrack> VideoTrack::create() {
  const size_t kWidth = 640;
  const size_t kHeight = 480;
  const size_t kFps = 30;

  std::unique_ptr<WebrtcVideoCapturer> capturer;

  std::unique_ptr<VideoCaptureModule::DeviceInfo> deviceInfo(
      VideoCaptureFactory::CreateDeviceInfo());

  if (!deviceInfo) {
    qWarning() << "Failed to create video device ";
    return nullptr;
  }

  int number_of_devices = deviceInfo->NumberOfDevices();

  qDebug() << "My number of devices is " << number_of_devices;

  for (int i = 0; i < number_of_devices; i++) {
    capturer =
        absl::WrapUnique(WebrtcVideoCapturer ::Create(kWidth, kHeight, kFps, i));
    if (capturer) {
      qDebug() << "Capturer created ";
      return rtc::make_ref_counted<VideoTrack>(std::move(capturer));
    } else {
      qDebug() << "Failed to create capturer";
    }
  }
  return nullptr;
}



rtc::VideoSourceInterface<webrtc::VideoFrame> *VideoTrack::source()
{
  qDebug() << "In get seouer ert ert --------------------------------- ";
  return mVideosource.get();
}
