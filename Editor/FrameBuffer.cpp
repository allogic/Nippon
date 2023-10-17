#include <Editor/FrameBuffer.h>

namespace ark
{
	FrameBuffer::FrameBuffer(const std::vector<AttachmentDescription>& AttachmentDescriptions, const AttachmentDescription& DepthAttachmentDescription)
		: mAttachmentCount{ (U32)AttachmentDescriptions.size() }
	{
		mColorTextures.resize(mAttachmentCount);
		mAttachmentIndices.resize(mAttachmentCount);

		for (U32 i = 0; i < mAttachmentCount; i++)
		{
			mColorTextures[i] = new RenderTexture{ AttachmentDescriptions[i].Wrap, AttachmentDescriptions[i].Filter, AttachmentDescriptions[i].Format, AttachmentDescriptions[i].FormatInternal, AttachmentDescriptions[i].Type };
			mAttachmentIndices[i] = GL_COLOR_ATTACHMENT0 + i;
		}

		mDepthStencilTexture = new DepthStencilTexture{ DepthAttachmentDescription.Wrap, DepthAttachmentDescription.Filter, DepthAttachmentDescription.Format, DepthAttachmentDescription.FormatInternal, DepthAttachmentDescription.Type };

		glGenFramebuffers(1, &mFbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFbo);

		for (U32 i = 0; i < mAttachmentCount; i++)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mColorTextures[i]->GetId(), 0);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilTexture->GetId(), 0);

		glDrawBuffers(mAttachmentCount, mAttachmentIndices.data());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &mFbo);

		delete mDepthStencilTexture;

		for (auto& colorTexture : mColorTextures)
		{
			delete colorTexture;
			colorTexture = nullptr;
		}
	}

	void FrameBuffer::Resize(U32 Width, U32 Height)
	{
		for (U32 i = 0; i < mAttachmentCount; i++)
		{
			mColorTextures[i]->Bind();
			mColorTextures[i]->Resize(Width, Height);
			mColorTextures[i]->UnBind();
		}

		mDepthStencilTexture->Bind();
		mDepthStencilTexture->Resize(Width, Height);
		mDepthStencilTexture->UnBind();
	}
}
