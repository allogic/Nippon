#include <Editor/FrameBuffer.h>
#include <Editor/Texture2D.h>

namespace ark
{
	U32 FrameBuffer::Create(U32 Width, U32 Height)
	{
		U32 frameBuffer = 0;

		glGenFramebuffers(1, &frameBuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, Width);
		glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, Height);

		U32 colorAttachment0 = Texture2D::Create(Width, Height, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_RGBA, GL_RGBA32F, GL_FLOAT, nullptr);
		U32 colorAttachment1 = Texture2D::Create(Width, Height, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_RED_INTEGER, GL_R32UI, GL_UNSIGNED_INT, nullptr);
		U32 depthStencilAttachment = Texture2D::Create(Width, Height, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_DEPTH_STENCIL, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment0, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, colorAttachment1, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilAttachment, 0);

		U32 attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

		glDrawBuffers(2, attachments);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return frameBuffer;
	}

	void FrameBuffer::Destroy(U32 Id)
	{
		I32 colorAttachment0 = 0;
		I32 colorAttachment1 = 0;
		I32 depthStencilAttachment = 0;

		glBindFramebuffer(GL_FRAMEBUFFER, Id);

		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &colorAttachment0);
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &colorAttachment1);
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &depthStencilAttachment);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Texture2D::Destroy(colorAttachment0);
		Texture2D::Destroy(colorAttachment1);
		Texture2D::Destroy(depthStencilAttachment);

		glDeleteFramebuffers(1, &Id);
	}

	U32 FrameBuffer::GetWidth(U32 Id)
	{
		I32 value = 0;
		
		glBindFramebuffer(GL_FRAMEBUFFER, Id);
		glGetFramebufferParameteriv(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, &value);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return value;
	}

	U32 FrameBuffer::GetHeight(U32 Id)
	{
		I32 value = 0;

		glBindFramebuffer(GL_FRAMEBUFFER, Id);
		glGetFramebufferParameteriv(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, &value);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return value;
	}

	U32 FrameBuffer::GetColorTexture(U32 Id, U32 Index)
	{
		I32 texture = 0;

		glBindFramebuffer(GL_FRAMEBUFFER, Id);
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Index, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &texture);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return texture;
	}

	U32 FrameBuffer::GetDepthStencilTexture(U32 Id)
	{
		I32 texture = 0;

		glBindFramebuffer(GL_FRAMEBUFFER, Id);
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &texture);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return texture;
	}

	U32 FrameBuffer::ReadIntegerAt(U32 Id, U32 X, U32 Y, U32 Index)
	{
		U32 value = 0;

		glBindFramebuffer(GL_READ_FRAMEBUFFER, Id);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + Index);
		glReadPixels((I32)X, (I32)Y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &value);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		return value;
	}

	std::vector<U8> FrameBuffer::CopyRGB(U32 Id)
	{
		std::vector<U8> bytes = {};

		I32 width = 0;
		I32 height = 0;

		glBindFramebuffer(GL_FRAMEBUFFER, Id);

		glGetFramebufferParameteriv(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, &width);
		glGetFramebufferParameteriv(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, &height);

		bytes.resize(width * height * 3);

		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, bytes.data());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return bytes;
	}

	std::vector<U8> FrameBuffer::CopyRGBA(U32 Id)
	{
		std::vector<U8> bytes = {};

		I32 width = 0;
		I32 height = 0;

		glBindFramebuffer(GL_FRAMEBUFFER, Id);

		glGetFramebufferParameteriv(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, &width);
		glGetFramebufferParameteriv(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, &height);

		bytes.resize(width * height * 4);

		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, bytes.data());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return bytes;
	}
}
