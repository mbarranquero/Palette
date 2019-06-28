#include "glview.hh"

#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions_4_5_Core>
#include <QTimer>
#include <QFileInfo>
#include <QMouseEvent>
#include <iostream>
#include <QOpenGLFramebufferObject>

const char *vert_filename = PROJECT_DIR "/src/shaders/simple.vert";
const char *frag_filename = PROJECT_DIR "/src/shaders/simple.frag";
const char *test_filename = PROJECT_DIR "/src/shaders/test.frag";

GLView::GLView(QWidget *parent)
    : QOpenGLWidget(parent)
{
    // Setup a timer to check periodically if the shader files have been modified
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    timer->connect(timer, SIGNAL(timeout()), this, SLOT(checkShaders()));
    timer->start();
}

GLView::~GLView()
{
}

void GLView::initializeGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.2, 0.2, 0.2, 1);
    f->glEnable(GL_DEPTH_TEST);
    loadProgram();
    model = Model::load(PROJECT_DIR "/assets/WaterBottle/WaterBottle.gltf");
    auto aabb = model->sceneAlignedBox(0);
    trackball_.lookAt(QVector3D(0, aabb.scale(), 0), aabb.center(), QVector3D(0, 0, 1));

    light_directions[0] = QVector3D(0., 0.5, -0.5);
    light_colors[0] = QVector3D(1., 0.8, 0.8);

    light_directions[1] = QVector3D(-0.4, -0.5, -0.5);
    light_colors[1] = QVector3D(0.8, 0.8, 1.);

    light_directions[2] = QVector3D(0.4, -0.5, -0.5);
    light_colors[2] = QVector3D(0.8, 1., 0.8);

    only_color_map = false;
}

void GLView::paintGL()
{
    
    
    
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    if (program_.isLinked())
//    {
//        program_.bind();
//        program_.setUniformValue("mat_view", trackball_.viewMatrix());
//        program_.setUniformValue("mat_proj", proj_mat_);
//        program_.setUniformValue("cam_pos", trackball_.position());
//        program_.setUniformValueArray("light_directions", light_directions, 3);
//        program_.setUniformValueArray("light_colors", light_colors, 3);
//        program_.setUniformValue("only_color_map", only_color_map);
//        if (model)
//        {
//            model->drawScene(0, program_);
//        }
//    }

    if (additionalProgram_.isLinked())
    {
        additionalProgram_.bind();
        additionalProgram_.setUniformValue("mat_view", trackball_.viewMatrix());
        additionalProgram_.setUniformValue("mat_proj", proj_mat_);
        additionalProgram_.setUniformValue("cam_pos", trackball_.position());
        additionalProgram_.setUniformValueArray("light_directions", light_directions, 3);
        additionalProgram_.setUniformValueArray("light_colors", light_colors, 3);
        additionalProgram_.setUniformValue("only_color_map", only_color_map);
        if (model)
        {
            model->drawScene(0, additionalProgram_);
        }
    }
}

void GLView::resizeGL(int w, int h)
{

    // Calculate aspect ratio
    float aspect = float(w) / float(h ? h : 1);

    // Choose near and far according to scene scale
    auto aabb = model->sceneAlignedBox(0);
    const float zNear = aabb.scale() * 0.1;
    const float zFar = zNear * 200.0;

    // Reset projection
    proj_mat_.setToIdentity();

    // Set perspective projection
    proj_mat_.perspective(fov_, aspect, zNear, zFar);

    width_ = w;
    height_ = h;
}

// Reload shaders if they have been modified since the last link
void GLView::checkShaders()
{
    QDateTime vdata = QFileInfo(vert_filename).lastModified();
    QDateTime fdata = QFileInfo(frag_filename).lastModified();
    if (vdata > last_link_ || fdata > last_link_)
    {
        loadProgram();
        update();
    }
}

void GLView::toggleColorMap(bool val)
{
    only_color_map = val;
    update();
}

bool GLView::loadProgram()
{
    program_.removeAllShaders();
    program_.addShaderFromSourceFile(QOpenGLShader::Vertex, vert_filename);
    program_.addShaderFromSourceFile(QOpenGLShader::Fragment, frag_filename);

    additionalProgram_.removeAllShaders();
    additionalProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, vert_filename);
    additionalProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, test_filename);

    bool link_res = program_.link() && additionalProgram_.link();

    if (link_res)
        last_link_ = QDateTime::currentDateTime();

    return link_res;
}


void GLView::mouseMoveEvent(QMouseEvent *event)
{
    auto mouse_pos = event->windowPos();
    mouse_pos.rx() /= width_;
    mouse_pos.ry() /= height_;
    if (trackball_.track(mouse_pos))
        update();
}

void GLView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        trackball_.start();
    }

    if (event->button() == Qt::LeftButton)
    {
        std::cout << "# Event detected" <<std::endl;
        QOpenGLFramebufferObject * fbo = new QOpenGLFramebufferObject(QSize(int(width_), int(height_)), GL_TEXTURE_2D);
        if (fbo->bind()) {
            std::cout << "Fbo binded" <<std::endl;
            update();
            std::cout << "Fbo rendered" <<std::endl;
            QImage textCoord = fbo->toImage();
            fbo->release();


            textCoord.save("frameBuffetTest.png");
            std::cout << "Fbo to qimage done" <<std::endl;
            QColor t = textCoord.pixelColor(event->pos());
            std::cout << "Frame buffer dim w: " << width_<< " | h: " << height_<<std::endl;
            std::cout << "Clicked pos X: " << event->pos().x()<< " | Y: " <<event->pos().y() <<std::endl;
            std::cout << "Tex coord X: " << t.red()<< " | Y: " <<t.green() << "| test:" << t.blue()<<std::endl;
            std::cout << "###" <<std::endl;

        }
        delete fbo;


    }
}

void GLView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        trackball_.stop();
    }
}

void GLView::wheelEvent(QWheelEvent *event)
{
    trackball_.zoom(-event->angleDelta().y());
    update();
}
