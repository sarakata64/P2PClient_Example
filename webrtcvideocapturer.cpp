#include "webrtcvideocapturer.h"
#include <QDebug>
#include "modules/video_capture/video_capture_factory.h"
#include <clientview.h>

using namespace webrtc;
WebrtcVideoCapturer::WebrtcVideoCapturer()
{

}

bool WebrtcVideoCapturer::Init(size_t width, size_t height, size_t target_fps,
                               size_t capture_device_index) {
  std::unique_ptr<VideoCaptureModule::DeviceInfo> deviceInfo(
      VideoCaptureFactory::CreateDeviceInfo());
  char device_name[256];
  char unique_name[256];

  int32_t result = deviceInfo->GetDeviceName(capture_device_index, device_name,
                                             sizeof(device_name), unique_name,
                                             sizeof(unique_name));
  qDebug() << "Device infoe result " << result;
  if (result != 0) {
    qDebug() << "Failed to get device name";
    return false;
  }
  mVideoCaptureModule = VideoCaptureFactory::Create(unique_name);
  qDebug() << "asdasd  " << mVideoCaptureModule;
  if (!mVideoCaptureModule) {
    qDebug() << "Failed to create video capture module";
    destroy();
    return false;
  }

  mVideoCaptureModule->RegisterCaptureDataCallback(this);

  qDebug() << "My current name " << mVideoCaptureModule->CurrentDeviceName();

  deviceInfo->GetCapability(mVideoCaptureModule->CurrentDeviceName(), 0,
                            mCapability);

  mCapability.width = static_cast<int32_t>(width);
  mCapability.height = static_cast<int32_t>(height);
  mCapability.maxFPS = static_cast<int32_t>(target_fps);
  mCapability.videoType = VideoType::kI420;

  int captureResult = mVideoCaptureModule->StartCapture(mCapability);
  qDebug() << "result " << captureResult;

  if (captureResult != 0) {
    destroy();
    return false;
  }

  bool started = mVideoCaptureModule->CaptureStarted();
  qDebug() << "is captureing " << started;
  return true;
}



WebrtcVideoCapturer *WebrtcVideoCapturer::Create( size_t width, size_t height,
                                                 size_t target_fps,
                                                 size_t capture_device_index) {

  std::unique_ptr<WebrtcVideoCapturer> mCapturer(new WebrtcVideoCapturer());

  bool ready = mCapturer->Init(width, height, target_fps, capture_device_index);
  if (!ready) {
    qDebug() << "Failed to create VcmCapturer(w = " << width
             << ", h = " << height << ", fps = " << target_fps << ")";

    return nullptr;
  }
  qDebug() << "is created "<<mCapturer.get();
  return mCapturer.release();
}


WebrtcVideoCapturer::~WebrtcVideoCapturer() { destroy();
  qDebug() << "is destroying " << this;
}

void WebrtcVideoCapturer::OnFrame(const webrtc::VideoFrame &frame) {
   VideoFrameManager::on_frame(frame);

}

void WebrtcVideoCapturer::destroy() {
  if (! mVideoCaptureModule)
    return;

 mVideoCaptureModule->StopCapture();
mVideoCaptureModule->DeRegisterCaptureDataCallback();
  // Release reference to VCM.
mVideoCaptureModule = nullptr;
}
