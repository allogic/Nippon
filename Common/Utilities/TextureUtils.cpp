#include <Common/Utilities/TextureUtils.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Vendor/StbImage/stb_image_write.h>

namespace ark
{
	void TextureUtils::WriteJPG(U32 Width, U32 Height, std::vector<U8>& Bytes, const fs::path& File)
	{
		U32 channels = 3;
		U32 quality = 100;

		stbi_flip_vertically_on_write(true);
		stbi_write_jpg(File.string().c_str(), Width, Height, channels, &Bytes[0], quality);
	}

	void TextureUtils::WritePNG(U32 Width, U32 Height, std::vector<U8>& Bytes, const fs::path& File)
	{
		U32 channels = 4;
		U32 stride = channels * Width;

		stride += (stride % 4) ? (4 - (stride % 4)) : 0;

		stbi_flip_vertically_on_write(true);
		stbi_write_png(File.string().c_str(), Width, Height, channels, &Bytes[0], stride);
	}
}
