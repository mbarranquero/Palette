#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <QVector>
#include <QQuaternion>
#include <QMatrix4x4>

class TrackBall
{
public:
    // Looks at target from eye position with given up vector
    void lookAt(const QVector3D &eye, const QVector3D &target, const QVector3D &up);

    // Rotate around target according to newPoint2D in camera space
    // This function have no effect if the trackball is disable
    bool track(const QPointF &newPoint2D);

    // Zooms in direction of target settup by lookAt
    void zoom(float val, float scale = 0.9);

    // Enables the trackball
    void start();

    // Disables the trackball
    // When the trackball is disable, the track function
    void stop();

    // Returns if the trackball is enable
    bool isActivate() const { return is_active_; }

    // Returns the view matrix
    const QMatrix4x4 &viewMatrix() const { return view_; }

    // Extracts camera position from the view matrix
    QVector3D position() const;

private:
    bool mapToSphere(const QPointF &p2, QVector3D &v3);
    void rotateAroundTarget(float angle, const QVector3D &axis);

    QMatrix4x4 view_;
    QVector3D target_;
    QVector3D last_point_;
    bool last_point_valid_ = false;
    bool is_active_ = false;
};