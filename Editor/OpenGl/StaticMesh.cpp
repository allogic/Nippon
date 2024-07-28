#include <Editor/Glad/glad.h>

#include <Editor/OpenGl/StaticMesh.h>

namespace Nippon
{
	StaticMesh::StaticMesh(MeshType MeshType)
	{
		glGenVertexArrays(1, &mVao);

		glGenBuffers(1, &mVbo);
		glGenBuffers(1, &mEbo);

		glBindVertexArray(mVao);

		glBindBuffer(GL_ARRAY_BUFFER, mVbo);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_READ | GL_STATIC_DRAW);

		switch (MeshType)
		{
			case MeshType::eMeshTypeDefault:
			{
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
				glEnableVertexAttribArray(3);
				glEnableVertexAttribArray(4);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void*)(0));
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void*)(sizeof(R32V3)));
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void*)(sizeof(R32V3) + sizeof(R32V2)));
				glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void*)(sizeof(R32V3) + sizeof(R32V2) + sizeof(R32V2)));
				glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(DefaultVertex), (void*)(sizeof(R32V3) + sizeof(R32V2) + sizeof(R32V2) + sizeof(R32V4)));

				break;
			}
			case MeshType::eMeshTypeDebug:
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

	StaticMesh::~StaticMesh()
	{
		glDeleteBuffers(1, &mVbo);
		glDeleteBuffers(1, &mEbo);

		glDeleteVertexArrays(1, &mVao);
	}

	void StaticMesh::Bind()
	{
		glBindVertexArray(mVao);
	}

	void StaticMesh::Render(RenderMode RenderMode)
	{
		switch (RenderMode)
		{
			case RenderMode::eRenderModeLines: glDrawElements(GL_LINES, (I32)mIndexBufferCount, GL_UNSIGNED_INT, nullptr); break;
			case RenderMode::eRenderModeTriangles: glDrawElements(GL_TRIANGLES, (I32)mIndexBufferCount, GL_UNSIGNED_INT, nullptr); break;
		}
	}

	void StaticMesh::UnBind()
	{
		glBindVertexArray(0);
	}

	void StaticMesh::GetVertices(void* VertexBuffer, U64 VertexStructSize, U64 VertexBufferCount)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVbo);
		glGetBufferSubData(GL_ARRAY_BUFFER, 0, VertexStructSize * VertexBufferCount, VertexBuffer);
	}

	void StaticMesh::GetIndices(void* IndexBuffer, U64 IndexStructSize, U64 IndexBufferCount)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
		glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, IndexStructSize * IndexBufferCount, IndexBuffer);
	}

	void StaticMesh::SetVertices(void* VertexBuffer, U64 VertexStructSize, U64 VertexBufferCount)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVbo);
		glBufferData(GL_ARRAY_BUFFER, VertexStructSize * VertexBufferCount, VertexBuffer, GL_STATIC_READ | GL_STATIC_DRAW);

		mVertexBufferCount = VertexBufferCount;
	}

	void StaticMesh::SetIndices(void* IndexBuffer, U64 IndexStructSize, U64 IndexBufferCount)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexStructSize * IndexBufferCount, IndexBuffer, GL_STATIC_READ | GL_STATIC_DRAW);

		mIndexBufferCount = IndexBufferCount;
	}
}