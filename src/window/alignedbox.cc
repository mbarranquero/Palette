#include "alignedbox.hh"

AlignedBox::AlignedBox()
    : min_pos_(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()), max_pos_(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min())
{
}

void AlignedBox::extend(const QVector3D &v)
{
    for (size_t i = 0; i < 3; i++)
    {
        min_pos_[i] = std::min(v[i], min_pos_[i]);
        max_pos_[i] = std::max(v[i], max_pos_[i]);
    }
}

void AlignedBox::extend(const AlignedBox &b)
{
    for (size_t i = 0; i < 3; i++)
    {
        min_pos_[i] = std::min(b.min_pos_[i], min_pos_[i]);
        max_pos_[i] = std::max(b.max_pos_[i], max_pos_[i]);
    }
}

AlignedBox AlignedBox::transform(const QMatrix4x4 &m) const
{
    AlignedBox aabb;
    QVector3D corner;
    for (size_t i = 0; i < 2; i++)
    {
        corner[0] = i == 0 ? min_pos_[0] : max_pos_[0];
        for (size_t j = 0; j < 2; j++)
        {
            corner[1] = j == 0 ? min_pos_[1] : max_pos_[1];
            for (size_t k = 0; k < 2; k++)
            {
                corner[2] = k == 0 ? min_pos_[2] : max_pos_[2];
                aabb.extend(m.map(corner));
            }
        }
    }

    return aabb;
}
