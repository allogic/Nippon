#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Vertex.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////

namespace ark
{
  enum RenderMode
  {
    eRenderModeLines = 1,
    eRenderModeTriangles = 4,
  };

  template<typename V, typename E>
  class Mesh
  {
  public:

    Mesh();
    virtual ~Mesh();

  public:

    void Bind() const;
    void Render(RenderMode RenderMode) const;
    void UnBind() const;

  public:

    void UploadVertices(V* VertexBuffer, U32 VertexBufferSize);
    void UploadElements(E* ElementBuffer, U32 ElementBufferSize);

  private:

    U32 mVao;
    U32 mVbo;
    U32 mEbo;

    std::vector<V> mVertexBuffer = {};
    std::vector<E> mElementBuffer = {};
  };
}

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  template<typename V, typename E>
  Mesh<V, E>::Mesh()
  {
    glGenVertexArrays(1, &mVao);

    glGenBuffers(1, &mVbo);
    glGenBuffers(1, &mEbo);

    glBindVertexArray(mVao);

    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_READ | GL_STATIC_DRAW);

    switch (V::Type)
    {
      case VertexType::eVertexTypeDebug:
      {
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)(0));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)(sizeof(R32V3)));
        break;
      }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_READ | GL_STATIC_DRAW);

    glBindVertexArray(0);
  }

  template<typename V, typename E>
  Mesh<V, E>::~Mesh()
  {
    glDeleteBuffers(1, &mVbo);
    glDeleteBuffers(1, &mEbo);

    glDeleteVertexArrays(1, &mVao);
  }

  template<typename V, typename E>
  void Mesh<V, E>::Bind() const
  {
    glBindVertexArray(mVao);
  }

  template<typename V, typename E>
  void Mesh<V, E>::Render(RenderMode renderMode) const
  {
    glDrawElements(renderMode, (U32)mElementBuffer.size(), GL_UNSIGNED_INT, NULL);
  }

  template<typename V, typename E>
  void Mesh<V, E>::UnBind() const
  {
    glBindVertexArray(0);
  }

  template<typename V, typename E>
  void Mesh<V, E>::UploadVertices(V* VertexBuffer, U32 VertexBufferSize)
  {
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, VertexBufferSize * sizeof(V), VertexBuffer, GL_STATIC_READ | GL_STATIC_DRAW);
  }

  template<typename V, typename E>
  void Mesh<V, E>::UploadElements(E* ElementBuffer, U32 ElementBufferSize)
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementBufferSize * sizeof(E), ElementBuffer, GL_STATIC_READ | GL_STATIC_DRAW);
  }
}