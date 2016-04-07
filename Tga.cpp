#include <string.h>
#include "Tga.h"
#include "Log.h"

namespace Img
{
	CTga::CTga() : _fr(NULL)
	{

	}

	CTga::~CTga()
	{
		Cleanup();
	}

	unsigned char TGAheader[12];

	int CTga::Load(const char * fname)
	{
		_fr = IO::CFileReader::Open(fname);
		if (!_fr)
		{
			Log::CLog::Write("CTga: File open error (%s)...\r\n", fname);
			return ID_TGA_FNAM;
		}
		if (_fr->Read(TGAheader,sizeof(TGAheader)) != 1)
		{
			Cleanup();
			Log::CLog::Write("CTga: Incorrect header size...\r\n");
			return ID_TGA_FLEN;
		}
		image.Cleanup();
		int result = ParseHeader();
		if(result != ID_TGA_OK)
		{
			Log::CLog::Write("CTga: Unhandled file format...\r\n");
			result = ID_TGA_FFMT;
		}
		else if (TGAheader[2] > 9)	
			result = LoadcTGA();
		else if ((TGAheader[2] == 2) || (TGAheader[2] == 3))
			result = LoaduTGA();
		else
		{
			Log::CLog::Write("CTga(c): Unhandled file data...\r\n");
			result = ID_TGA_UDAT;
		}
		if(result != ID_TGA_OK)
		{
			Cleanup();
			return result;
		}
		return ID_TGA_OK;
	}

	int CTga::ParseHeader()
	{
		unsigned char header[6];
		if (_fr->Read(header,sizeof(header)) != 1)
		{
			Log::CLog::Write("CTga: Loading of header failed...\r\n");
			return ID_TGA_FLEN;
		}
		image.width = (header[1] << 8) | header[0];
		image.height = (header[3] << 8) | header[2];
		image.bpp = header[4];
		if ((image.width > 0) && (image.height > 0))
		{
			int result = PixelFormat(image.bpp, image.pf);
			if(result != ID_TGA_OK)
				return result;
			image.size = (image.bpp >> 3) * image.height * image.width;
		}
		return ID_TGA_OK;
	}
	int CTga::LoadcTGA()
	{
			/*unsigned long index = 0;
			unsigned char colorbuffer[4];*/
			unsigned char chunkheader;
			const int bytesperpixel = image.bpp >> 3;
			try
			{
				image.data = new unsigned char[image.size];
			}catch (std::bad_alloc)
			{
				Log::CLog::Write("CTga: Memory (data)allocation failed...\r\n");
				return ID_TGA_AMEM;
			}
			unsigned char *p = image.data;
			do
			{
				if (_fr->Read(&chunkheader, 1) != 1)
				{
					Log::CLog::Write("CTga(c): Compressed data error...\r\n");
					return ID_TGA_FLEN;
				}
				if (chunkheader & 0x80)
				{
					if (_fr->Read(p,  bytesperpixel) != 1)
					{
						Log::CLog::Write("CTga(c): Compressed data error...\r\n");
						return ID_TGA_FLEN;
					}
					int n = (chunkheader - 127)/* * bytesperpixel*/;
					unsigned char * val = p;
					p += bytesperpixel;
					for (int i = 1; i < n; i++)
					{
						// TODO:: refactor
						switch(bytesperpixel)
						{
						case 4:
							*(p + 3) = *(val + 3);
						case 3:
							*(p + 2) = *(val + 2);
							*(p + 1) = *(val + 1);
						case 1:
							*p = *val;
							break;
						}
						p += bytesperpixel;
					}
				}
				else
				{
					int n = (chunkheader + 1) * bytesperpixel;
					if (_fr->Read(p,  n) != 1)
					{
						long pos = _fr->Pos();
						Log::CLog::Write("CTga(c): Compressed data error...\r\n");
						return ID_TGA_FLEN;
					}
					p += n;
				}
				//if (chunkheader<128)
				//{
				//	chunkheader++;
				//	for (unsigned short i = 0;i < chunkheader;i++)
				//	{

				//		if (_fr->Read(colorbuffer, bytesperpixel) != 1)
				//		{
				//			Log::CLog::Write("CTga(c): Compressed data error...\r\n");
				//			return ID_TGA_FLEN;
				//		}
				//		if (bytesperpixel == 1)
				//			image.data[index++] = colorbuffer[0];
				//		else if(bytesperpixel == 3)
				//		{
				//			image.data[index++] = colorbuffer[2];
				//			image.data[index++] = colorbuffer[1];
				//			image.data[index++] = colorbuffer[0];
				//		}
				//		else if (bytesperpixel == 4)
				//		{
				//			image.data[index++] = colorbuffer[2];
				//			image.data[index++] = colorbuffer[1];
				//			image.data[index++] = colorbuffer[0];
				//			image.data[index++] = colorbuffer[3];
				//		}
				//			
				//			
				//	/*	image->data[index++]=(GLubyte)255;
				//		image->data[index++]=(GLubyte)255;
				//		image->data[index++]=(GLubyte)255;
				//		if (bytesperpixel==4)
				//			image->data[index++]=(GLubyte)255;*/
				//	}
				//}
				//else 
				//{
				//	chunkheader -= 127;
				//	if (_fr->Read(colorbuffer, bytesperpixel) != 1)
				//	{
				//		Log::CLog::Write("CTga(c): Uncompressed data error...\r\n");
				//		return ID_TGA_FLEN;
				//	}
				//	/*colorbuffer[0]^=colorbuffer[2]^=colorbuffer[0]^=colorbuffer[2];
				//	memcpy(image->data+index,colorbuffer,bytesperpixel);
				//	index+=bytesperpixel*chunkheader;*/
				//	for (unsigned short i = 0; i < chunkheader; i++)
				//	{
				//		if (bytesperpixel == 1)
				//			image.data[index++] = colorbuffer[0];
				//		else if(bytesperpixel == 3)
				//		{
				//			image.data[index++] = colorbuffer[2];
				//			image.data[index++] = colorbuffer[1];
				//			image.data[index++] = colorbuffer[0];
				//		}
				//		else if (bytesperpixel == 4)
				//		{
				//			image.data[index++] = colorbuffer[2];
				//			image.data[index++] = colorbuffer[1];
				//			image.data[index++] = colorbuffer[0];
				//			image.data[index++] = colorbuffer[3];
				//		}
				//	}
				//}
			}
			while (p - image.data < image.size);
			return ID_TGA_OK;
	}
	int CTga::LoaduTGA()
	{
		const int bytesperpixel = image.bpp >> 3;
		try
		{
			image.data = new unsigned char[image.size];
		}catch (std::bad_alloc)
		{
			Log::CLog::Write("CTga: Memory (data)allocation failed...\r\n");
			return ID_TGA_AMEM;
		}
		if (_fr->Read(image.data, image.size) != 1)
		{
			Log::CLog::Write("CTga: Insufficient bytes in file...\r\n");
			return ID_TGA_FLEN;
		}
		if ((bytesperpixel == 3) || (bytesperpixel == 4))
		{
			//swap byte order: BGR->RGB
			for (unsigned long i=0;i<image.size;i+=bytesperpixel)
			{
				if (image.data[i] != image.data[i+2])
					image.data[i] ^= image.data[i+2] ^= image.data[i] ^= image.data[i+2];
			}
			//swap byte order: BGR->RGB
		}
		return ID_TGA_OK;
	}

	void CTga::Cleanup(void)
	{
		delete _fr;
		_fr = NULL;
	}

	ImgData& CTga::GetImage(void)
	{
		return image;
	}
	//////////////////////////////////////////////////////////////////////
	// TGA Loader
	//////////////////////////////////////////////////////////////////////
}
