#pragma once

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QDateTime>
#include "model.hh"
#include "trackball.hh"

class GLView : public QOpenGLWidget
{
  Q_OBJECT

public:
  GLView(QWidget *parent = 0);
  ~GLView();

  virtual QSize sizeHint() const { return QSize(1600, 900); }
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);

protected:
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

private slots:
  // Reloadeds shaders if they have been modified since the last compilation
  void checkShaders();

  // Toggle pbr/color map rendering
  void toggleColorMap(bool);

private:
  bool loadProgram();

  QOpenGLShaderProgram program_;
  QOpenGLShaderProgram additionalProgram_;
  QDateTime last_link_;

  TrackBall trackball_;
  QMatrix4x4 proj_mat_;

  QVector3D light_directions[3];
  QVector3D light_colors[3];
  bool only_color_map;

  float width_, height_;
  float fov_ = 45.0;

  std::unique_ptr<Model> model;
};
