#pragma once

#include <vector>

#include <Common/Types.h>

#include <Editor/Texture.h>

#include <Editor/Glad/glad.h>

namespace ark
{
	struct AttachmentDescription
	{
		U32 Wrap;
		U32 Filter;
		U32 Format;
		U32 FormatInternal;
		U32 Type;
	};

	class FrameBuffer
	{
	public:

		FrameBuffer(const std::vector<AttachmentDescription>& ColorAttachmentDescriptions, const AttachmentDescription& DepthAttachmentDescription);
		virtual ~FrameBuffer();

	public:

		inline const auto& GetId() const { return mFbo; }
		inline const auto& GetColorTextures() const { return mColorTextures; }
		inline const auto& GetColorTexture(U32 Index) const { return mColorTextures[Index]; }
		inline const auto& GetDepthStencilTexture() const { return mDepthStencilTexture; }

	public:

		void Resize(U32 Width, U32 Height);
		
	public:

		template<typename T>
		void Clear(const T& Color, U32 Index) const;

		template<typename T>
		T ReadPixel(U32 X, U32 Y, U32 Index) const;

	private:

		U32 mFbo = 0;
		U32 mAttachmentCount = 0;

		std::vector<RenderTexture*> mColorTextures = {};
		std::vector<U32> mAttachmentIndices = {};

		DepthStencilTexture* mDepthStencilTexture = nullptr;
	};
}

namespace ark
{
	template<typename T>
	void FrameBuffer::Clear(const T& Color, U32 Index) const
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + Index);
		glClearTexImage(mColorTextures[Index]->GetId(), 0, mColorTextures[Index]->GetFormat(), mColorTextures[Index]->GetType(), &Color);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	template<typename T>
	T FrameBuffer::ReadPixel(U32 X, U32 Y, U32 Index) const
	{
		T value = {};

		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + Index);
		glReadPixels((I32)X, (I32)Y, 1, 1, mColorTextures[Index]->GetFormat(), mColorTextures[Index]->GetType(), &value);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		return value;
	}
}
