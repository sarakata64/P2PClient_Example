#include "clientview.h"
#include <i420_texture_cache.h>
#include <gl_video_shader.h>
#include <QDebug>
#include <QLabel>
ClientView::ClientView(QWidget *parent)


{

  QSurfaceFormat format;
  format.setProfile(QSurfaceFormat::CompatibilityProfile);
  format.setVersion(3, 3);
  setFormat(format);
  makeCurrent();
  qDebug() << "is done ";
  mPeerLabel = new QLabel(this);

  mPeerLabel->setGeometry((this->width() - 200) + 16,
                          this->height() - mPeerLabel->height() - 16, 200, 56);

  auto font = mPeerLabel->font();
  mPeerLabel->setText("32");
  font.setPixelSize(32);
  mPeerLabel->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
  mPeerLabel->setFont(font);
}

ClientView::~ClientView()
{

}

void ClientView::renderFrame(const webrtc::VideoFrame &frame) {


  auto videeoFrame = std::make_shared<webrtc::VideoFrame>(frame);

  if (mFramesQueue.size() >= 300) {
    for (int i = 0; i < 100; ++i) {
      std::shared_ptr<webrtc::VideoFrame> dropFrame;
      dropFrame = mFramesQueue.dequeue();
      if (!dropFrame) {
        qDebug() << "try dequeue failed.";

      }
    }
  }
  mFramesQueue.enqueue(videeoFrame);
  update();
//  static int counter = 0;
}

void ClientView::initializeGL()
{
  qDebug() << "Initialising gl ";
  F=context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
  F->glClearColor(0.3f,0.5f,0.2f,1.0f);
  F->glClear(GL_COLOR_BUFFER_BIT);


  F->glEnable(GL_DEPTH_TEST);

  F->glEnable(GL_TEXTURE_2D);

  mi420TextureCache = std::make_shared<I420TextureCache>(F);
 mi420TextureCache->init();

 mVideoShader = std::make_shared<GLVideoShader>(F);

}
void ClientView::resizeGL(int, int) { qDebug() << "iseze  ntigl "; }

QLabel *ClientView::peerLabel() const
{
  return mPeerLabel;
}

void ClientView::paintGL() {


  if (mCacheFrame) {

    float imageRatio =
        (float)mCacheFrame->width() / (float)mCacheFrame->height();
    float canvasRatio = (float)width() / (float)height();

    int32_t viewportX = 0;
    int32_t viewportY = 0;

    int32_t viewportW = 0;
    int32_t viewportH = 0;

    if (canvasRatio >= imageRatio) {
      viewportH = height();
      viewportW = viewportH * imageRatio;
      viewportX = (float)(width() - viewportW) / 2.0f;
    } else {
      viewportW = width();
      viewportH = viewportW / imageRatio;
      viewportY = (float)(height() - viewportH) / 2.0f;
    }

    glViewport(viewportX * devicePixelRatio(), viewportY * devicePixelRatio(),
               viewportW * devicePixelRatio(), viewportH * devicePixelRatio());
  }

  glClear(GL_COLOR_BUFFER_BIT);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);



  if(!mFramesQueue.isEmpty()){
     std::shared_ptr<webrtc::VideoFrame> frame;
  frame = mFramesQueue.dequeue();

  if (frame.get()) {

    mCacheFrame = frame;
  } else {

    qDebug() << "try dequeue failed.";
   }

  if (mCacheFrame) {

    if(mi420TextureCache.get()){
 mi420TextureCache->uploadFrameToTextures(*mCacheFrame );

    mVideoShader->applyShadingForFrame(mCacheFrame ->width(),
                                       mCacheFrame ->height(),
                                       mCacheFrame ->rotation(),
                                       mi420TextureCache->yTexture(),
                                       mi420TextureCache->uTexture(),
                                       mi420TextureCache->vTexture());
    }
  }
  }

}

void ClientView::resizeEvent(QResizeEvent *) {}


