#include "trackball.hh"

#include <QQuaternion>
#include <QTransform>

void TrackBall::lookAt(const QVector3D &eye, const QVector3D &target, const QVector3D &up)
{
    view_.lookAt(eye, target, up);
    target_ = target;
}

void TrackBall::start()
{
    is_active_ = true;
    last_point_valid_ = false;
}

void TrackBall::stop()
{
    is_active_ = false;
}
bool TrackBall::track(const QPointF &point2D)
{
    if (!is_active_)
        return false;
    QVector3D new_point;
    bool new_point_valid = mapToSphere(point2D, new_point);

    if (last_point_valid_ && new_point_valid)
    {
        QVector3D axis = QVector3D::crossProduct(last_point_, new_point).normalized();
        float cos_angle = QVector3D::dotProduct(last_point_, new_point);
        if (std::abs(cos_angle) < 1.0)
        {
            float angle = 2. * acos(cos_angle);
            rotateAroundTarget(angle, axis);
        }
    }

    last_point_ = new_point;
    last_point_valid_ = new_point_valid;
    return new_point_valid;
}

void TrackBall::zoom(float val, float scale)
{
    auto inv_view = view_.inverted();
    auto translation = inv_view.column(3);
    auto pos = translation.toVector3D();
    auto dir = (pos - target_).normalized();
    float dist = (pos - target_).length();
    float new_dist = (val < 0 ? scale : 1 / scale) * dist;
    auto new_pos = target_ + dir * new_dist;
    inv_view.setColumn(3, QVector4D(new_pos, translation.w()));
    view_ = inv_view.inverted();
}

bool TrackBall::mapToSphere(const QPointF &p2, QVector3D &v3)
{
    if ((p2.x() >= 0) && (p2.x() <= 1) && (p2.y() >= 0) &&
        (p2.y() <= 1))
    {
        double x = p2.x() - 0.5;
        double y = 0.5 - p2.y();
        double sinx = sin(M_PI * x * 0.5);
        double siny = sin(M_PI * y * 0.5);
        double sinx2siny2 = sinx * sinx + siny * siny;

        v3.setX(sinx);
        v3.setY(siny);
        v3.setZ(sinx2siny2 < 1.0 ? sqrt(1.0 - sinx2siny2) : 0.0);

        return true;
    }
    else
        return false;
}

void TrackBall::rotateAroundTarget(float angle, const QVector3D &axis)
{
    QVector3D t = view_ * target_;
    QMatrix4x4 t_mat1, t_mat2, q_mat;
    t_mat1.translate(t);
    t_mat2.translate(-t);
    q_mat.rotate(180 * angle / M_PI, axis);
    view_ = t_mat1 * q_mat * t_mat2 * view_;
}

QVector3D TrackBall::position() const
{
    return view_.inverted().column(3).toVector3D();
}