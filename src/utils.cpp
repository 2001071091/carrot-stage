#include "utils.h"
#include <FreeImage.h>

using com_yoekey_3d::GLBITMAP;

namespace com_yoekey_3d{
	GLBITMAP* FIBitmap2GLBitmap(FIBITMAP *fibmp){
		int i, j, k;
		int pitch = FreeImage_GetPitch(fibmp);
		unsigned char *bits = FreeImage_GetBits(fibmp);
		int bpp = FreeImage_GetBPP(fibmp);
		GLBITMAP *glbmp = (GLBITMAP *)malloc(sizeof(GLBITMAP));
		RGBQUAD *palette = NULL;

		if (!glbmp) return NULL;

		glbmp->w = FreeImage_GetWidth(fibmp);
		glbmp->h = FreeImage_GetHeight(fibmp);

		switch (bpp) {
		case 8:
			if (!(palette = FreeImage_GetPalette(fibmp))) return NULL;
			if (!(glbmp->buf = (unsigned char *)malloc(glbmp->w*glbmp->h * 3))) return NULL;
			glbmp->rgb_mode = GL_RGB;
			for (i = 0; i < glbmp->h; ++i) {
				for (j = 0; j < glbmp->w; ++j) {
					k = bits[i*pitch + j];
					glbmp->buf[(i*glbmp->w + j) * 3 + 0] = palette[k].rgbRed;
					glbmp->buf[(i*glbmp->w + j) * 3 + 1] = palette[k].rgbGreen;
					glbmp->buf[(i*glbmp->w + j) * 3 + 2] = palette[k].rgbBlue;
				}
			}
			break;
		case 24:
			if (!(glbmp->buf = (unsigned char *)malloc(glbmp->w*glbmp->h * 3))) return NULL;
			glbmp->rgb_mode = GL_RGB;
			for (i = 0; i < glbmp->h; ++i) {
				for (j = 0; j < glbmp->w; ++j) {
					glbmp->buf[(i*glbmp->w + j) * 3 + 0] = bits[i*pitch + j * 3 + 2];
					glbmp->buf[(i*glbmp->w + j) * 3 + 1] = bits[i*pitch + j * 3 + 1];
					glbmp->buf[(i*glbmp->w + j) * 3 + 2] = bits[i*pitch + j * 3 + 0];
				}
			}
			break;
		case 32:
			if (!(glbmp->buf = (unsigned char *)malloc(glbmp->w*glbmp->h * 4))) return NULL;
			glbmp->rgb_mode = GL_RGBA;
			for (i = 0; i < glbmp->h; ++i) {
				for (j = 0; j < glbmp->w; ++j) {
					glbmp->buf[(i*glbmp->w + j) * 4 + 0] = bits[i*pitch + j * 4 + 2];
					glbmp->buf[(i*glbmp->w + j) * 4 + 1] = bits[i*pitch + j * 4 + 1];
					glbmp->buf[(i*glbmp->w + j) * 4 + 2] = bits[i*pitch + j * 4 + 0];
					glbmp->buf[(i*glbmp->w + j) * 4 + 3] = bits[i*pitch + j * 4 + 3];
				}
			}
			break;
		default: return NULL;
		}

		return glbmp;
	}

}
void com_yoekey_3d::obj_split_face_cell(char* cell, GLint* is){
	is[0] = atoi(strtok(cell, "/"));
	is[1] = atoi(strtok(NULL, "/"));
	is[2] = atoi(strtok(NULL, "/"));
}

void com_yoekey_3d::FreeGLBitmap(GLBITMAP *glbmp)
{
	if (glbmp) {
		if (glbmp->buf) free(glbmp->buf);
		free(glbmp);
	}
}


GLuint com_yoekey_3d::loadTexture(const char *filename)
{
	#ifdef FREEIMAGE_LIB
		FreeImage_Initialise(FALSE);
	#endif  
	GLuint tex = 0;
	FIBITMAP *bitmap = NULL;
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	
	GLBITMAP *glbmp = NULL;

	fif = FreeImage_GetFileType(filename, 0);
	if (FIF_UNKNOWN == fif) {
		fif = FreeImage_GetFIFFromFilename(filename);
		if (FIF_UNKNOWN == fif)
			return 0;
	}
	if (FreeImage_FIFSupportsReading(fif))
		bitmap = FreeImage_Load(fif, filename, 0);

	if (!bitmap)
		return 0;

	//printf("bit: %d\n", FreeImage_GetBPP(bitmap));

	glbmp = FIBitmap2GLBitmap(bitmap);
	if (!glbmp)
		return 0;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, glbmp->rgb_mode, glbmp->w, glbmp->h, 0, glbmp->rgb_mode, GL_UNSIGNED_BYTE, glbmp->buf);

	FreeGLBitmap(glbmp);
	FreeImage_Unload(bitmap);

	#ifdef FREEIMAGE_LIB
		FreeImage_DeInitialise();
	#endif  
	return tex;
}