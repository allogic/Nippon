#include <Editor/FrameBuffer.h>
#include <Editor/Texture.h>

#include <Vendor/GLAD/glad.h>

///////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////

namespace ark
{
  FrameBuffer::FrameBuffer()
  {
    mColorTexture = new RenderTexture{ TextureWrap::ClampToEdge, TextureFilter::Linear };
    mDepthStencilTexture = new DepthStencilTexture{ TextureWrap::ClampToEdge, TextureFilter::Linear };

    glGenFramebuffers(1, &mFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture->GetId(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilTexture->GetId(), 0);

    U32 attachments[1] = { GL_COLOR_ATTACHMENT0 };

    glDrawBuffers(1, attachments);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }

  FrameBuffer::~FrameBuffer()
  {
    glDeleteFramebuffers(1, &mFbo);
  }

  void FrameBuffer::Resize(U32 Width, U32 Height)
  {
    mColorTexture->Bind();
    mColorTexture->Resize(Width, Height);
    mColorTexture->Unbind();

    mDepthStencilTexture->Bind();
    mDepthStencilTexture->Resize(Width, Height);
    mDepthStencilTexture->Unbind();
  }
}