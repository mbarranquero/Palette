#include "model.hh"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
#include <QDir>
#include <QDebug>
#include <string>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QOpenGLFunctions_4_5_Core>
#include <map>
#include <iostream>
#include <QtOpenGL>

#define TEST_ARTHUR

struct cmp_str
{
    bool operator()(char const *a, char const *b) const
    {
        return std::strcmp(a, b) < 0;
    }
};
const std::map<const char *, std::pair<int, const char *>, cmp_str> attribute_mapping = {{"POSITION", {0, "vtx_position"}}, {"NORMAL", {1, "vtx_normal"}}, {"TEXCOORD_0", {2, "vtx_texcoord"}}};

size_t compSizeFromType(int comp)
{
    switch (comp)
    {
    case TINYGLTF_PARAMETER_TYPE_BYTE:
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
        return sizeof(char);

    case TINYGLTF_PARAMETER_TYPE_SHORT:
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
        return sizeof(short);

    case TINYGLTF_PARAMETER_TYPE_INT:
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
        return sizeof(int);
    case TINYGLTF_PARAMETER_TYPE_FLOAT:
        return sizeof(float);
    }
}

std::unique_ptr<Model> Model::load(const char *filename)
{
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();
    QFileInfo path = QFileInfo(filename);
    auto ext = path.completeSuffix();
    if (ext == "glb" && ext == "gltf")
    {
        qDebug() << "Error: unsupported file extension " << ext;
        return std::unique_ptr<Model>();
    }

    tinygltf::Model gltf_model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    bool ret;
    if (ext == ".glb")
        ret = loader.LoadBinaryFromFile(&gltf_model, &err, &warn, std::string(filename));
    else
        ret = loader.LoadASCIIFromFile(&gltf_model, &err, &warn, std::string(filename));

    if (!warn.empty())
    {
        qDebug() << "Warning when loading file " << filename;
        qDebug() << warn.c_str();
    }

    if (!err.empty())
    {
        qDebug() << "Error when loading file " << filename;
        qDebug() << err.c_str();
    }

    if (!ret)
        return std::unique_ptr<Model>();

    std::unique_ptr<Model> model = std::unique_ptr<Model>(new Model);

    // Load Buffers
    for (const auto &bv : gltf_model.bufferViews)
    {
        QOpenGLBuffer::Type type;
        switch (bv.target)
        {
        case TINYGLTF_TARGET_ARRAY_BUFFER:
            type = QOpenGLBuffer::VertexBuffer;
            break;
        case TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER:
            type = QOpenGLBuffer::IndexBuffer;
            break;
        }
        model->buffers_.push_back(std::unique_ptr<QOpenGLBuffer>(new QOpenGLBuffer(type)));
        QOpenGLBuffer &buf = *model->buffers_.back();
        const auto &vec = gltf_model.buffers[bv.buffer].data;
        buf.create();
        buf.bind();
        buf.allocate(vec.data() + bv.byteOffset, bv.byteLength * sizeof(vec[0]));
    }

#ifdef TEST_ARTHUR

#endif

    //Load Materials
    model->materials_.reserve(gltf_model.materials.size());
    for (const auto &m : gltf_model.materials)
    {
        model->materials_.resize(model->materials_.size() + 1);
        auto &mat = model->materials_.back();
        auto it = m.values.find("baseColorTexture");
        if (it != m.values.end())
        {
            const auto &name = it->first;
            const auto &val = it->second;
            auto &t = gltf_model.textures[val.TextureIndex()];
            auto &img = gltf_model.images[t.source];
            mat.color_map = std::unique_ptr<QOpenGLTexture>(new QOpenGLTexture(QOpenGLTexture::Target2D));
            mat.color_map->setSize(img.width, img.height);
            mat.color_map->setFormat(QOpenGLTexture::SRGB8_Alpha8);
            mat.color_map->allocateStorage();
            mat.color_map->setData(QOpenGLTexture::RGBA, QOpenGLTexture::PixelType(img.pixel_type), img.image.data());
            if (t.sampler != -1)
            {
                auto &sampler = gltf_model.samplers[t.sampler];
                mat.color_map->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::WrapMode(sampler.wrapS));
                mat.color_map->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::WrapMode(sampler.wrapT));
                mat.color_map->setMinMagFilters(QOpenGLTexture::Filter(sampler.minFilter), QOpenGLTexture::Filter(sampler.magFilter));
            }
        }

        it = m.values.find("metallicRoughnessTexture");
        if (it != m.values.end())
        {
            const auto &name = it->first;
            const auto &val = it->second;
            auto &t = gltf_model.textures[val.TextureIndex()];
            auto &img = gltf_model.images[t.source];
            mat.metallic_roughness_map = std::unique_ptr<QOpenGLTexture>(new QOpenGLTexture(QOpenGLTexture::Target2D));
            mat.metallic_roughness_map->setSize(img.width, img.height);
            mat.metallic_roughness_map->setFormat(QOpenGLTexture::RGBA8_UNorm);
            mat.metallic_roughness_map->allocateStorage();
            mat.metallic_roughness_map->setData(QOpenGLTexture::RGBA, QOpenGLTexture::PixelType(img.pixel_type), img.image.data());

            if (t.sampler != -1)
            {
                auto &sampler = gltf_model.samplers[t.sampler];
                mat.metallic_roughness_map->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::WrapMode(sampler.wrapS));
                mat.metallic_roughness_map->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::WrapMode(sampler.wrapT));
                mat.metallic_roughness_map->setMinMagFilters(QOpenGLTexture::Filter(sampler.minFilter), QOpenGLTexture::Filter(sampler.magFilter));
            }
        }

        it = m.values.find("baseColorFactor");
        if (it != m.values.end())
        {
            const auto &name = it->first;
            const auto &val = it->second;
            auto factor = val.ColorFactor();
            mat.base_color_factor = QVector4D(factor[0], factor[1], factor[2], factor[3]);
        }

        it = m.values.find("metallicFactor");
        if (it != m.values.end())
        {
            const auto &name = it->first;
            const auto &val = it->second;
            mat.metallic_roughness_values.setX((float)val.Factor());
        }

        it = m.values.find("roughnessFactor");
        if (it != m.values.end())
        {
            const auto &name = it->first;
            const auto &val = it->second;
            mat.metallic_roughness_values.setY((float)val.Factor());
        }

        it = m.additionalValues.find("normalTexture");
        if (it != m.additionalValues.end())
        {
            const auto &name = it->first;
            const auto &val = it->second;
            auto &t = gltf_model.textures[val.TextureIndex()];
            auto &img = gltf_model.images[t.source];
            mat.normal_map = std::unique_ptr<QOpenGLTexture>(new QOpenGLTexture(QOpenGLTexture::Target2D));
            mat.normal_map->setSize(img.width, img.height);
            mat.normal_map->setFormat(QOpenGLTexture::RGBA8_UNorm);
            mat.normal_map->allocateStorage();
            mat.normal_map->setData(QOpenGLTexture::RGBA, QOpenGLTexture::PixelType(img.pixel_type), img.image.data());

            if (t.sampler != -1)
            {
                auto &sampler = gltf_model.samplers[t.sampler];
                mat.normal_map->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::WrapMode(sampler.wrapS));
                mat.normal_map->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::WrapMode(sampler.wrapT));
                mat.normal_map->setMinMagFilters(QOpenGLTexture::Filter(sampler.minFilter), QOpenGLTexture::Filter(sampler.magFilter));
            }
        }

        it = m.additionalValues.find("occlusionTexture");
        if (it != m.additionalValues.end())
        {
            mat.has_occlusion_map = true;
        }

        it = m.additionalValues.find("emissiveTexture");
        if (it != m.additionalValues.end())
        {
            const auto &name = it->first;
            const auto &val = it->second;
            auto &t = gltf_model.textures[val.TextureIndex()];
            auto &img = gltf_model.images[t.source];
            mat.emissive_map = std::unique_ptr<QOpenGLTexture>(new QOpenGLTexture(QOpenGLTexture::Target2D));
            mat.emissive_map->setSize(img.width, img.height);
            mat.emissive_map->setFormat(QOpenGLTexture::RGBA8_UNorm);
            mat.emissive_map->allocateStorage();
            mat.emissive_map->setData(QOpenGLTexture::RGBA, QOpenGLTexture::PixelType(img.pixel_type), img.image.data());

            if (t.sampler != -1)
            {
                auto &sampler = gltf_model.samplers[t.sampler];
                mat.emissive_map->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::WrapMode(sampler.wrapS));
                mat.emissive_map->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::WrapMode(sampler.wrapT));
                mat.emissive_map->setMinMagFilters(QOpenGLTexture::Filter(sampler.minFilter), QOpenGLTexture::Filter(sampler.magFilter));
            }
        }

        it = m.additionalValues.find("emissiveFactor");
        if (it != m.additionalValues.end())
        {
            const auto &name = it->first;
            const auto &val = it->second;
            auto factor = val.ColorFactor();
            mat.base_color_factor = QVector3D(factor[0], factor[1], factor[2]);
        }
    }

    // Load Meshes
    std::vector<int> primitive_offsets;
    for (const auto &m : gltf_model.meshes)
    {
        primitive_offsets.push_back(model->primitives_.size());
        for (const auto &p : m.primitives)
        {
            Primitive primitive;
            primitive.vao_id = model->vertex_arrays_.size();
            model->vertex_arrays_.push_back(std::unique_ptr<QOpenGLVertexArrayObject>(new QOpenGLVertexArrayObject));
            QOpenGLVertexArrayObject &vao = *model->vertex_arrays_.back();
            vao.create();
            vao.bind();

            for (const auto &it : p.attributes)
            {
                const auto &name = it.first;
                const auto &a = it.second;
                if (attribute_mapping.count(name.c_str()))
                {
                    const auto &acc = gltf_model.accessors[a];
                    const auto &bv = gltf_model.bufferViews[acc.bufferView];

                    int attrib_index = attribute_mapping.at(name.c_str()).first;
                    f->glVertexArrayVertexBuffer(vao.objectId(),
                                                 attrib_index,
                                                 model->buffers_[acc.bufferView]->bufferId(), 0,
                                                 bv.byteStride != 0 ? bv.byteStride : acc.type * compSizeFromType(acc.componentType));
                    f->glVertexArrayAttribFormat(vao.objectId(),
                                                 attrib_index,
                                                 acc.type,
                                                 acc.componentType,
                                                 acc.normalized,
                                                 acc.byteOffset);

                    //if it is a position attributes
                    if (attrib_index == 0)
                    {
                        primitive.aabb.extend(QVector3D(acc.minValues[0], acc.minValues[1], acc.minValues[2]));
                        primitive.aabb.extend(QVector3D(acc.maxValues[0], acc.maxValues[1], acc.maxValues[2]));
                    }
                }
            }

            const auto &acc = gltf_model.accessors[p.indices];
            f->glVertexArrayElementBuffer(vao.objectId(), model->buffers_[acc.bufferView]->bufferId());
            primitive.material_id = p.material;
            primitive.mode = p.mode;
            primitive.count = acc.count;
            primitive.component_type = acc.componentType;
            primitive.offset = acc.byteOffset;
            model->primitives_.push_back(primitive);
        }
    }

    // Load Nodes
    model->nodes_.reserve(gltf_model.nodes.size());
    for (const auto &n : gltf_model.nodes)
    {
        Node node;
        node.children = n.children;
        if (n.matrix.size() == 16)
        {
            std::vector<float> fvec;
            for (auto const &d : n.matrix)
                fvec.push_back(d);
            node.transform *= QMatrix4x4(fvec.data());
        }
        else
        {
            if (n.translation.size() == 3)
            {
                node.transform.translate(n.translation[0], n.translation[1], n.translation[2]);
            }
            if (n.rotation.size() == 4)
            {
                node.transform.rotate(QQuaternion(n.rotation[3], n.rotation[0], n.rotation[1], n.rotation[2]));
            }
            if (n.scale.size() == 3)
            {
                node.transform.scale(n.scale[0], n.scale[1], n.scale[2]);
            }
        }
        if (n.mesh != -1)
        {
            node.primitive_number = gltf_model.meshes[n.mesh].primitives.size();
            node.first_primitive = primitive_offsets[n.mesh];
        }
        model->nodes_.push_back(node);
    }

    // Load Scenes
    model->scenes_.reserve(gltf_model.scenes.size());
    int i = 0;
    for (const auto &s : gltf_model.scenes)
    {
        Scene scene;
        for (auto &n : s.nodes)
            scene.nodes.push_back(n);
        model->scenes_.push_back(scene);
    }

    return model;
}

void Model::drawScene(int s, QOpenGLShaderProgram &prog)
{
    if (scenes_.size() > s)
        for (const auto &n : scenes_[s].nodes)
            drawNode(nodes_[n], prog, QMatrix4x4());
}

void Model::drawNode(const Node &node, QOpenGLShaderProgram &prog, const QMatrix4x4 &transform)
{
    QMatrix4x4 node_trans = transform * node.transform;
    for (const auto &c : node.children)
        drawNode(nodes_[c], prog, node_trans);

    if (node.first_primitive != -1)
        for (int i = 0; i < node.primitive_number; i++)
            drawPrimitive(primitives_[node.first_primitive + i], prog, node_trans);
}

void Model::drawPrimitive(const Primitive &primitive, QOpenGLShaderProgram &prog, const QMatrix4x4 &transform)
{
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();
    // Activate Materials
    auto &mat = materials_[primitive.material_id];
    int tex_unit_count = 0;

    prog.setUniformValue("base_color_factor", mat.base_color_factor);
    bool has_color_map = mat.color_map.operator bool();
    prog.setUniformValue("has_color_map", has_color_map);
    if (has_color_map)
    {
        mat.color_map->bind(tex_unit_count);
        prog.setUniformValue("color_map", tex_unit_count++);
    }

    prog.setUniformValue("metallic_roughness_values", mat.metallic_roughness_values);
    prog.setUniformValue("occlusion_strength", mat.occlusion_strength);
    bool has_metallic_roughness_map = mat.metallic_roughness_map.operator bool();
    prog.setUniformValue("has_metallic_roughness_map", has_metallic_roughness_map);
    prog.setUniformValue("has_occlusion_map", mat.has_occlusion_map);
    if (has_metallic_roughness_map)
    {
        mat.metallic_roughness_map->bind(tex_unit_count);
        prog.setUniformValue("metallic_roughness_map", tex_unit_count++);
    }

    bool has_normal_map = mat.normal_map.operator bool();
    prog.setUniformValue("has_color_map", has_normal_map);
    if (has_normal_map)
    {
        mat.normal_map->bind(tex_unit_count);
        prog.setUniformValue("normal_map", tex_unit_count++);
    }

    bool has_emissive_map = mat.emissive_map.operator bool();
    prog.setUniformValue("emissive_factor", mat.emissive_factor);
    prog.setUniformValue("has_emissive_map", has_emissive_map);
    if (has_emissive_map)
    {
        mat.emissive_map->bind(tex_unit_count);
        prog.setUniformValue("emissive_map", tex_unit_count++);
    }

    prog.setUniformValue("mat_model", transform);
    prog.setUniformValue("mat_normal", transform.normalMatrix());

    // Active attributes
    for (const auto &it : attribute_mapping)
    {
        const auto &key = it.first;
        const auto &val = it.second;
        const auto &index = val.first;
        const auto &name = val.second;
        auto loc = prog.attributeLocation(name);
        if (loc != -1)
        {
            f->glEnableVertexArrayAttrib(vertex_arrays_[primitive.vao_id]->objectId(), index);
            f->glVertexArrayAttribBinding(vertex_arrays_[primitive.vao_id]->objectId(), loc, index);
        }
        else
        {
            f->glDisableVertexArrayAttrib(vertex_arrays_[primitive.vao_id]->objectId(), index);
        }
    }
    vertex_arrays_[primitive.vao_id]->bind();
    f->glDrawElements(primitive.mode,
                      primitive.count,
                      primitive.component_type, reinterpret_cast<void *>(primitive.offset));

    if (has_color_map)
        mat.color_map->release();
    if (has_metallic_roughness_map)
        mat.metallic_roughness_map->release();
    if (has_normal_map)
        mat.normal_map->release();
}

AlignedBox Model::sceneAlignedBox(int s)
{
    AlignedBox aabb;
    ;
    if (scenes_.size() > s)
        for (const auto &n : scenes_[s].nodes)
            aabb.extend(nodeAlignedBox(nodes_[n], QMatrix4x4()));

    return aabb;
}

AlignedBox Model::nodeAlignedBox(const Node &node, const QMatrix4x4 &transform)
{
    QMatrix4x4 node_trans = transform * node.transform;
    AlignedBox aabb;
    for (const auto &c : node.children)
        aabb.extend(nodeAlignedBox(nodes_[c], transform));

    if (node.first_primitive != -1)
        for (int i = 0; i < node.primitive_number; i++)
            aabb.extend(primitives_[node.first_primitive + i].aabb.transform(node_trans));

    return aabb;
}
