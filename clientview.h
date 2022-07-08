#ifndef CLIENTVIEW_H
#define CLIENTVIEW_H

#include <QObject>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QTimer>
#include "api/video/video_frame.h"
#include <QOpenGLShaderProgram>
#include <QQueue>

class I420TextureCache;
class GLVideoShader;
class QLabel;
class ClientView : public QOpenGLWidget
{
  Q_OBJECT

public:
  ClientView(QWidget *parent = nullptr);
  ~ClientView();
  void renderFrame(const webrtc::VideoFrame &frame);

  QLabel *peerLabel() const;

protected:
  void resizeEvent(QResizeEvent *event) override;
  void paintGL() override;
  void initializeGL() override;
  void resizeGL(int w, int h) override;

private:
  QOpenGLFunctions_3_3_Core *F;
  std::shared_ptr<I420TextureCache> mi420TextureCache;

  std::shared_ptr<GLVideoShader> mVideoShader;
  std::shared_ptr<webrtc::VideoFrame> mCacheFrame;
  QQueue<std::shared_ptr<webrtc::VideoFrame>> mFramesQueue ;
  QLabel *mPeerLabel;
};
#endif // CLIENTVIEW_H
