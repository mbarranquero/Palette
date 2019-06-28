#pragma once

#include <QMatrix4x4>
#include <QVector3D>

class AlignedBox
{
public:
    // Creates an empty axis aligned bounding box
    AlignedBox();

    // Returns the lower bound
    QVector3D minPos() { return min_pos_; }

    // Returns the upper bound
    QVector3D maxPos() { return max_pos_; }

    // Returns the box diagonal length
    float scale() { return (max_pos_ - min_pos_).length(); }

    // Returns the center
    QVector3D center() { return (max_pos_ + min_pos_) / 2.; }

    // Extends the box to contains the given point
    void extend(const QVector3D &v);

    // Extends the box to contains the given box
    void extend(const AlignedBox &b);

    // Returns a new AlignedBox containing this box transformed by m
    AlignedBox transform(const QMatrix4x4 &m) const;

private:
    QVector3D min_pos_;
    QVector3D max_pos_;
};