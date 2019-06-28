#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector2D>
#include <vector>
#include <memory>
#include <QOpenGLTexture>
#include "alignedbox.hh"

class QOpenGLShaderProgram;

// Represents a gltf 2.0 model
// The gltf specification can be found:
// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md
class Model
{
public:
  static std::unique_ptr<Model> load(const char *filename);

  // Draw scene s using program prog
  void drawScene(int s, QOpenGLShaderProgram &prog);

  // Get bounding box of scene s
  AlignedBox sceneAlignedBox(int s);

private:
  // A Material represent the appearance of an object
  // It is composed of a set of texture and factore representing the different properties of the material
  struct Material
  {
    QVector4D base_color_factor = QVector4D(1, 1, 1, 1);
    std::unique_ptr<QOpenGLTexture> color_map;

    QVector2D metallic_roughness_values = QVector2D(1, 1);
    bool has_occlusion_map = false;
    float occlusion_strength = 1.0;
    std::unique_ptr<QOpenGLTexture> metallic_roughness_map;

    std::unique_ptr<QOpenGLTexture> normal_map;

    QVector3D emissive_factor = QVector3D(1, 1, 1);
    std::unique_ptr<QOpenGLTexture> emissive_map;
  };

  // A Primitive represents a single geometric entity
  struct Primitive
  {
    int vao_id;            // Index of the Vertex Array buffer
    int material_id;       // Index of the material
    GLenum mode;           // Type of component (triangle,triangle fan, quad,...)
    size_t count;          // Number of component
    GLenum component_type; // Type of index (unsigned int, short int, ...)
    size_t offset;         // Offset of the first index in the index buffer
    AlignedBox aabb;       // Axis Aligned bounding box
  };

  // A Node in the scene graph
  struct Node
  {
    QMatrix4x4 transform;      // Transformation matrix of this node
    std::vector<int> children; // List of children of the node
    int first_primitive = -1;  // Index of the first primitive for this node or -1 if no primitive
    int primitive_number = 0;  // Total number of primitives
  };

  struct Scene
  {
    std::vector<int> nodes;
  };

  void drawNode(const Node &node, QOpenGLShaderProgram &prog, const QMatrix4x4 &transform);
  void drawPrimitive(const Primitive &primitive, QOpenGLShaderProgram &prog, const QMatrix4x4 &transform);
  AlignedBox nodeAlignedBox(const Node &node, const QMatrix4x4 &transform);

  QString modelPathName;
  std::vector<Scene> scenes_;
  std::vector<Node> nodes_;
  std::vector<std::unique_ptr<QOpenGLBuffer>> buffers_;
  std::vector<std::unique_ptr<QOpenGLVertexArrayObject>> vertex_arrays_;
  std::vector<Primitive> primitives_;
  std::vector<Material> materials_;
};
