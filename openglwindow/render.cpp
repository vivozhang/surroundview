/*************************************************************************/ /*!
@File           gles2test1.c
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@License        Strictly Confidential.
*/
                                                                            /**************************************************************************/
#include <GLES3/gl3.h>
#include <EGL/egl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <vector>

#include "InitMosaic.hpp"

#include "esTransform.h"

#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION

//#include "stb_image.h"
//#include "stb_image_write.h"
#include "stb_image.h"

static int hProgramHandle[5];

int bvs2DWidth = 360;
int bvs2DHeight = 720;
int bvs2DoffsetX = 0;
int bvs2DoffsetY = 0;

int bvs3DWidth = 920;
int bvs3DHeight = 720;
int bvs3DoffsetX = 360;
int bvs3DoffsetY = 0;

GLuint textureYUYV[5];
GLuint textureRes[4];

float lumiaAve[12];
CvPoint3D32f colorAve[12];                  //$)Ad???d:?:&??!!h0????
CvPoint3D32f colorCount[AVERAGE_COUNT][12]; //$)Ad???d:?:&??!!h0????

CvPoint3D32f *verCoordPoint;
CvPoint2D32f *texCoordPoint, *imgCoordPoint;

static int verCount;

GLuint bindTexture(GLuint texture, unsigned char *buffer, GLuint w, GLuint h, GLint type)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, type /*GL_LUMINANCE*/, w, h, 0, type /*GL_LUMINANCE*/, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return texture;
}

#if 1
char vertexSource[] =
    {
        "#version 300 es						  \n"
        "layout(location = 0) in vec4 av4position;              \n"
        "layout(location = 1) in vec2 av2texcoord;              \n"
        "layout(location = 2) in float coeff;              \n"
        "uniform mat4 mvp;                        \n"
        "smooth centroid out vec2 vv2texcoord;                \n"
        "out float vCoeff;                \n"
        "void main()                            \n"
        "{                                        \n"
        "    vv2texcoord = av2texcoord;           \n"
        "    vCoeff = coeff;           \n"
        "    gl_Position = mvp * av4position;     \n"
        "}                                        \n"};

#if 0
char fragmentSource[] =
{
    "#version 300 es						  \n"
    "precision highp float;                                      \n"
    "uniform sampler2D s_textureRGBA;                               \n"
    "uniform vec3 color0Adjust;                                   \n"
    "uniform vec3 color1Adjust;                                   \n"
    "smooth centroid in vec2 vv2texcoord;                        \n"
    "in float vCoeff;                					 	 \n"
    "out vec4 fragColor;                          				 \n"
    "void main()                                                 \n"
    "{                                                           \n"
    "  	 vec3 origin = texture(s_textureRGBA, vv2texcoord).rgb;\n"
    "  	 vec3 gain = color0Adjust * (1.0 - vCoeff) + color1Adjust * vCoeff;      \n"
    "    fragColor = vec4(origin + gain, 1.0);        \n"
    "}                                                           \n"

};
#else
char fragmentSource[] =
    {
        "#version 300 es						  \n"
        "precision mediump float;                                      \n"
        "uniform sampler2D texUYVY422;                            \n"
        "uniform vec3 color0Adjust;                                   \n"
        "uniform vec3 color1Adjust;                                   \n"
        "smooth centroid in vec2 vv2texcoord;                        \n"
        "in float vCoeff;                					 	 \n"
        "out vec4 fragColor;                          				 \n"
        "void main()                                                 \n"
        "{                                                           \n"
        "   vec3 yuvColor;                                           \n"
        "   vec3 rgbColor;                                           \n"
        "   float fenmu = 2.0;                                           \n"
        "   vec2 TexCoord = vv2texcoord;                              \n"
        "   ivec2  TexSize = textureSize(texUYVY422, 0);             \n"
        "   int texCoordX = int(TexCoord.x * float(TexSize.x*2));            \n"
        "   if (bool(texCoordX%2))                           \n"
        "   yuvColor.x = texture(texUYVY422, TexCoord).z;           \n"
        "   else                                          \n"
        "   yuvColor.x = texture(texUYVY422, TexCoord).x;           \n"
        "   yuvColor.y = texture(texUYVY422, TexCoord).w-0.5;         \n"
        "   yuvColor.z = texture(texUYVY422, TexCoord).y-0.5;          \n"
        "   rgbColor = yuvColor * mat3(1, 1.779, 0,		\n"
        "				1, -0.3455, -0.7169,		\n"
        "				1, 0, 1.4075);			\n"
        "  	 vec3 gain = color0Adjust * (1.0 - vCoeff) + color1Adjust * vCoeff;      \n"
        "    fragColor = vec4(rgbColor + gain, 1.0);        \n"
        "}                                                           \n"

};
#endif

char vertexMosaicSource[] =
    {
        "#version 300 es						  \n"
        "layout(location = 0) in vec4 av4position;                      \n"
        "layout(location = 1) in vec2 av2texcoord1;                     \n"
        "layout(location = 2) in vec2 av2texcoord2;                     \n"
        "layout(location = 3) in float avalpha;                         \n"
        "uniform mat4 mvp;                                              \n"
        "smooth centroid out vec2 vv2texcoord0;                         \n"
        "smooth centroid out vec2 vv2texcoord1;                         \n"
        "out float vvalpha;                                         	\n"
        "void main()                                                    \n"
        "{                                                              \n"
        "    vv2texcoord0 = av2texcoord1;                               \n"
        "    vv2texcoord1 = av2texcoord2;                               \n"
        "    vvalpha = avalpha;                                         \n"
        "    gl_Position = mvp * av4position;                           \n"
        "}                                                              \n"};

#if 0
char fragmentMosaicSource[] =
{
    "#version 300 es						  \n"
    "precision highp float;                                         \n"
    "uniform sampler2D s_texture1RGBA;                                 \n"
    "uniform sampler2D s_texture2RGBA;                                 \n"
    "uniform vec3 color0Adjust;                                   \n"
    "uniform vec3 color1Adjust;                                   \n"
    "smooth centroid in vec2 vv2texcoord1;                          \n"
    "smooth centroid in vec2 vv2texcoord2;                          \n"
    "in float vvalpha;                                         		\n"
    "out vec4 fragColor;                          					\n"
    "void main()                                                    \n"
    "{                                                              \n"
    "    vec3 rgb1 = texture(s_texture1RGBA, vv2texcoord1).rgb;        \n"
    "    vec3 rgb2 = texture(s_texture2RGBA, vv2texcoord2).rgb;        \n"
    "    fragColor = vec4(mix(rgb1 + color0Adjust, rgb2 + color1Adjust, vvalpha), 1.0);\n"
    //"    fragColor = vec4(mix(rgb1 + color0Adjust, rgb2 + color1Adjust, 0.0), 1.0);	\n"
    "}                                                              \n"
};
#else
char fragmentMosaicSource[] =
    {
        "#version 300 es						  \n"
        "precision mediump float;                                         \n"
        "uniform sampler2D tex0UYVY422;                                 \n"
        "uniform sampler2D tex1UYVY422;                                 \n"
        "uniform vec3 color0Adjust;                                   \n"
        "uniform vec3 color1Adjust;                                   \n"
        "smooth centroid in vec2 vv2texcoord0;                          \n"
        "smooth centroid in vec2 vv2texcoord1;                          \n"
        "in float vvalpha;                                         		\n"
        "out vec4 fragColor;                          					\n"
        "void main()                                                    \n"
        "{                                                              \n"
        "   vec3 yuvColor0, yuvColor1;                                           \n"
        "   vec3 rgbColor0, rgbColor1;                                           \n"
        "   float fenmu = 2.0;                                           \n"
        "   vec2 TexCoord0 = vv2texcoord0;                              \n"
        "   ivec2  TexSize0 = textureSize(tex0UYVY422, 0);             \n"
        "   int texCoordX0 = int(TexCoord0.x * float(TexSize0.x*2));            \n"
        "   if (bool(texCoordX0%2))                           \n"
        "   yuvColor0.x = texture(tex0UYVY422, TexCoord0).z;           \n"
        "   else                                          \n"
        "   yuvColor0.x = texture(tex0UYVY422, TexCoord0).x;           \n"
        "   yuvColor0.y = texture(tex0UYVY422, TexCoord0).w-0.5;         \n"
        "   yuvColor0.z = texture(tex0UYVY422, TexCoord0).y-0.5;          \n"
        "   rgbColor0 = yuvColor0 * mat3(1, 1.779, 0,		\n"
        "				1, -0.3455, -0.7169,		\n"
        "				1, 0, 1.4075);			\n"
        "   vec2 TexCoord1 = vv2texcoord1;                              \n"
        "   ivec2  TexSize1 = textureSize(tex1UYVY422, 0);             \n"
        "   int texCoordX1 = int(TexCoord1.x * float(TexSize1.x*2));            \n"
        "   if (bool(texCoordX1%2))                           \n"
        "   yuvColor1.x = texture(tex1UYVY422, TexCoord1).z;           \n"
        "   else                                          \n"
        "   yuvColor1.x = texture(tex1UYVY422, TexCoord1).x;           \n"
        "   yuvColor1.y = texture(tex1UYVY422, TexCoord1).w-0.5;         \n"
        "   yuvColor1.z = texture(tex1UYVY422, TexCoord1).y-0.5;          \n"
        "   rgbColor1 = yuvColor1 * mat3(1, 1.779, 0,		\n"
        "				1, -0.3455, -0.7169,		\n"
        "				1, 0, 1.4075);			\n"
        "    fragColor = vec4(mix(rgbColor0 + color0Adjust, rgbColor1 + color1Adjust, vvalpha), 1.0);\n"
        "}                                                              \n"

};
#endif

#if 1
char vertexBlendSource[] =
    {
        "#version 300 es										\n"
        "layout(location = 0) in vec4 vPosition;			  \n"
        "uniform mat4 mvp;										\n"
        "void main()											\n"
        "{														\n"
        "	 gl_Position = mvp * vPosition;		 \n"
        "}														\n"};

char fragmentBlendSource[] =
    {
        "#version 300 es											\n"
        "precision mediump float; 									\n"
        "uniform vec4 outColor; 					  \n"
        "out vec4 fragColor;										\n"
        "void main()												\n"
        "{															\n"
        "	 fragColor = outColor;			\n"
        "}															\n"};

#endif

#if 0
char vertexConvertSource[] =
{
    "#version 300 es						  \n"
    "layout(location = 0) in vec4 av4position;              \n"
    "layout(location = 1) in vec2 av2texcoord;              \n"
    "uniform mat4 mvp;                        \n"
    "smooth centroid out vec2 vv2texcoord;    \n"
    "void main()                              \n"
    "{                                        \n"
    "    vv2texcoord = av2texcoord;           \n"
    "    gl_Position = mvp * av4position;     \n"
    "}                                        \n"
};

char fragmentConvertSource[] =
{
    "#version 300 es						  \n"
    "precision highp float;                                      \n"
    "uniform sampler2D s_textureY;                               \n"
    "uniform sampler2D s_textureUV;                              \n"
    "smooth centroid in vec2 vv2texcoord;                        \n"
    "out vec4 fragColor;                          				\n"
    "void main()                                                 \n"
    "{                                                           \n"
    "    mediump vec3 yuv;                                       \n"
    "    mediump vec3 rgb;                                       \n"
    "    float lumia = texture(s_textureY, vv2texcoord).r;     \n"
    "    float cb = texture(s_textureUV, vv2texcoord).a - 0.5; \n"
    "	 float cr = texture(s_textureUV, vv2texcoord).r - 0.5; \n"
    "	 yuv = vec3(lumia, cb, cr);                              \n"
    "	 rgb = yuv * mat3(1, 1.779, 0,                           \n"
    "                     1, -0.3455, -0.7169,                   \n"
    "                     1, 0, 1.4075);                         \n"
    "    fragColor = vec4(rgb, 1.0);                          \n"
    "}                                                           \n"

};
#endif

char vertexBmpShowSource[] =
    {
        "#version 300 es						  				\n"
        "layout(location = 0) in vec4 av4position;              \n"
        "layout(location = 1) in vec2 av2texcoord;              \n"
        "uniform mat4 mvp;                        				\n"
        "smooth centroid out vec2 vv2texcoord;                	\n"
        "void main()                              				\n"
        "{                                        				\n"
        "    vv2texcoord = av2texcoord;           				\n"
        "    gl_Position = mvp * av4position;     				\n"
        "}                                        				\n"};

char fragmentBmpShowSource[] =
    {
        "#version 300 es						  					\n"
        "precision mediump float;                                     \n"
        "uniform sampler2D textureImg;                            \n"
        "smooth centroid in vec2 vv2texcoord;                       \n"
        "out vec4 fragColor;                          				\n"
        "void main()                                                \n"
        "{                                                          \n"
        "    fragColor = texture(textureImg, vv2texcoord);       	\n"
        "}                                                          \n"};

#endif

char vertexCameraSource[] =
    {
        "#version 300 es						  \n"
        "layout(location = 0) in vec4 av4position;              \n"
        "layout(location = 1) in vec2 av2texcoord;              \n"
        "uniform mat4 mvp;                        \n"
        "smooth centroid out vec2 vv2texcoord;                \n"
        "void main()                            \n"
        "{                                        \n"
        "    vv2texcoord = av2texcoord;           \n"
        "    gl_Position = mvp * av4position;     \n"
        "}                                        \n"};

#if 0
char fragmentCameraSource[] =
{
    "#version 300 es						  \n"
    "precision highp float;                                      \n"
    "uniform sampler2D s_textureRGBA;                               \n"
    "smooth centroid in vec2 vv2texcoord;                        \n"
    "out vec4 fragColor;                          				 \n"
    "void main()                                                 \n"
    "{                                                           \n"
    "    fragColor = texture(s_textureRGBA, vv2texcoord);        \n"
    "}                                                           \n"

};
#else
char fragmentCameraSource[] =
    {
        "#version 300 es						  \n"
        "precision mediump float;                                      \n"
        "uniform sampler2D texUYVY422;                            \n"
        "in vec2 vv2texcoord;                       \n"
        "out vec4 fragColor;                          \n"
        "void main()                                  \n"
        "{                                            \n"
        "   vec3 yuvColor;                                           \n"
        "   vec3 rgbColor;                                           \n"
        "   float fenmu = 2.0;                                           \n"
        "   vec2 TexCoord = vv2texcoord;                              \n"
        "   ivec2  TexSize = textureSize(texUYVY422, 0);             \n"
        "   int texCoordX = int(TexCoord.x * float(TexSize.x*2));            \n"
        "   if (bool(texCoordX%2))                           \n"
        "   yuvColor.x = texture(texUYVY422, TexCoord).z;           \n"
        "   else                                          \n"
        "   yuvColor.x = texture(texUYVY422, TexCoord).x;           \n"
        "   yuvColor.y = texture(texUYVY422, TexCoord).w-0.5;         \n"
        "   yuvColor.z = texture(texUYVY422, TexCoord).y-0.5;          \n"
        "   rgbColor = yuvColor * mat3(1, 1.779, 0,		\n"
        "				1, -0.3455, -0.7169,		\n"
        "				1, 0, 1.4075);			\n"
        "   fragColor = vec4(rgbColor, 1); \n"
        "}                                            \n"

};
#endif

#if 0
/*====================================================================
?$)A=f???'0:   caculateColorCoeff2D
?$)A=f????:   h.!g???8*?:e?????2h??4e??
$)Ag.??e.??:
?$)A(e????:
$)Ah>?????:   imageBuffer:??7/???e$4e????i&????
$)Ah???g;??:   ??
$)Ad???h.0e?o<?
====================================================================*/
void caculateColorCoeff2D(unsigned char **imageBuffer)
{

    int i, j;
    unsigned int rSum0, rSum1;
    unsigned int gSum0, gSum1;
    unsigned int bSum0, bSum1;

    int index0, index1;
	CvPoint3D32f rgbColor[12];

	unsigned int count;
	static unsigned int runCount = 0;
	int index;
	CvPoint3D32f addSum[8];

	index = runCount % AVERAGE_COUNT;
	
    rSum0 = 0;
    rSum1 = 0;
    gSum0 = 0;
    gSum1 = 0;
    bSum0 = 0;
    bSum1 = 0;

    for(i = 0; i < texCoordsStatistics2D.glTexCoord_FL_F.size(); i++)
    {
    	index0 = texCoordsStatistics2D.glTexCoord_FL_F[i] * 4;
        rSum0 += imageBuffer[0][index0 + 0];
        gSum0 += imageBuffer[0][index0 + 1];
        bSum0 += imageBuffer[0][index0 + 2];


        index1 = texCoordsStatistics2D.glTexCoord_FL_L[i] * 4;
        rSum1 += imageBuffer[2][index1 + 0];
        gSum1 += imageBuffer[2][index1 + 1];
        bSum1 += imageBuffer[2][index1 + 2];

	}

	count = texCoordsStatistics2D.glTexCoord_FL_F.size();
	rgbColor[0].x = 1.0 * rSum0 / count;
	rgbColor[0].y = 1.0 * gSum0 / count;
	rgbColor[0].z = 1.0 * bSum0 / count;

	rgbColor[1].x = 1.0 * rSum1 / count;
	rgbColor[1].y = 1.0 * gSum1 / count;
	rgbColor[1].z = 1.0 * bSum1 / count;


    rSum0 = 0;
    rSum1 = 0;
    gSum0 = 0;
    gSum1 = 0;
    bSum0 = 0;
    bSum1 = 0;

    for(i = 0; i < texCoordsStatistics2D.glTexCoord_FR_F.size(); i++)
    {
    	index0 = texCoordsStatistics2D.glTexCoord_FR_F[i] * 4;
        rSum0 += imageBuffer[0][index0 + 0];
        gSum0 += imageBuffer[0][index0 + 1];
        bSum0 += imageBuffer[0][index0 + 2];



        index1 = texCoordsStatistics2D.glTexCoord_FR_R[i] * 4;
        rSum1 += imageBuffer[3][index1 + 0];
        gSum1 += imageBuffer[3][index1 + 1];
        bSum1 += imageBuffer[3][index1 + 2];

    }

	count = texCoordsStatistics2D.glTexCoord_FR_F.size();

	rgbColor[2].x = 1.0 * rSum0 / count;
	rgbColor[2].y = 1.0 * gSum0 / count;
	rgbColor[2].z = 1.0 * bSum0 / count;

	rgbColor[3].x = 1.0 * rSum1 / count;
	rgbColor[3].y = 1.0 * gSum1 / count;
	rgbColor[3].z = 1.0 * bSum1 / count;


    rSum0 = 0;
    rSum1 = 0;
    gSum0 = 0;
    gSum1 = 0;
    bSum0 = 0;
    bSum1 = 0;
    
    for(i = 0; i < texCoordsStatistics2D.glTexCoord_BL_B.size(); i++)
    {
    	index0 = texCoordsStatistics2D.glTexCoord_BL_B[i] * 4;
        rSum0 += imageBuffer[1][index0 + 0];
        gSum0 += imageBuffer[1][index0 + 1];
        bSum0 += imageBuffer[1][index0 + 2];



        index1 = texCoordsStatistics2D.glTexCoord_BL_L[i] * 4;
        rSum1 += imageBuffer[2][index1 + 0];
        gSum1 += imageBuffer[2][index1 + 1];
        bSum1 += imageBuffer[2][index1 + 2];

    }

	count = texCoordsStatistics2D.glTexCoord_BL_B.size();

	rgbColor[4].x = 1.0 * rSum0 / count;
	rgbColor[4].y = 1.0 * gSum0 / count;
	rgbColor[4].z = 1.0 * bSum0 / count;

	rgbColor[5].x = 1.0 * rSum1 / count;
	rgbColor[5].y = 1.0 * gSum1 / count;
	rgbColor[5].z = 1.0 * bSum1 / count;


    rSum0 = 0;
    rSum1 = 0;
    gSum0 = 0;
    gSum1 = 0;
    bSum0 = 0;
    bSum1 = 0;
    
    for(i = 0; i < texCoordsStatistics2D.glTexCoord_BR_B.size(); i++)
    {
    	index0 = texCoordsStatistics2D.glTexCoord_BR_B[i] * 4;
        rSum0 += imageBuffer[1][index0 + 0];
        gSum0 += imageBuffer[1][index0 + 1];
        bSum0 += imageBuffer[1][index0 + 2];


        index1 = texCoordsStatistics2D.glTexCoord_BR_R[i] * 4;
        rSum1 += imageBuffer[3][index1 + 0];
        gSum1 += imageBuffer[3][index1 + 1];
        bSum1 += imageBuffer[3][index1 + 2];
    }

	count = texCoordsStatistics2D.glTexCoord_BR_B.size();

    rgbColor[6].x = 1.0 * rSum0 / count;
	rgbColor[6].y = 1.0 * gSum0 / count;
	rgbColor[6].z = 1.0 * bSum0 / count;

	rgbColor[7].x = 1.0 * rSum1 / count;
	rgbColor[7].y = 1.0 * gSum1 / count;
	rgbColor[7].z = 1.0 * bSum1 / count;

    for(i = 0; i < 8; i ++)
    {
		colorCount[index][i].x = rgbColor[i].x / 255; 
		colorCount[index][i].y = rgbColor[i].y / 255; 
		colorCount[index][i].z = rgbColor[i].z / 255;
	}
	
	runCount++;

	if(runCount < AVERAGE_COUNT)
	{
		memset(colorAve, 0, 4*3*12);
		return;
	}
	else
	{
		for(i=0; i<8; i++)
		{
			addSum[i].x = 0;
			addSum[i].y = 0;
			addSum[i].z = 0;
			for(j=0; j<AVERAGE_COUNT; j++)
			{
				addSum[i].x += colorCount[j][i].x;
				addSum[i].y += colorCount[j][i].y;
				addSum[i].z += colorCount[j][i].z;
			}
			colorAve[i].x = addSum[i].x / AVERAGE_COUNT;
			colorAve[i].y = addSum[i].y / AVERAGE_COUNT;
			colorAve[i].z = addSum[i].z / AVERAGE_COUNT;
		}

		colorAve[8].x = (colorAve[0].x + colorAve[1].x) / 2;
	    colorAve[8].y = (colorAve[0].y + colorAve[1].y) / 2;
	    colorAve[8].z = (colorAve[0].z + colorAve[1].z) / 2;
	    
	    colorAve[9].x = (colorAve[2].x + colorAve[3].x) / 2;
	    colorAve[9].y = (colorAve[2].y + colorAve[3].y) / 2;
	    colorAve[9].z = (colorAve[2].z + colorAve[3].z) / 2;
	    
	    colorAve[10].x = (colorAve[4].x + colorAve[5].x) / 2;
	    colorAve[10].y = (colorAve[4].y + colorAve[5].y) / 2;
	    colorAve[10].z = (colorAve[4].z + colorAve[5].z) / 2;
    
	    colorAve[11].x = (colorAve[6].x + colorAve[7].x) / 2;
		colorAve[11].y = (colorAve[6].y + colorAve[7].y) / 2;
	    colorAve[11].z = (colorAve[6].z + colorAve[7].z) / 2;
	}

	//for(i=0; i<12; i++)
    	//printf("count = %d, idx=%d ===== %f %f %f\n", runCount, i, colorAve[i].x, colorAve[i].y, colorAve[i].z);
}
#else
void caculateColorCoeff2D(unsigned char **imageBuffer, int flag)
{

    int i, j;
    unsigned int rSum0, rSum1;
    unsigned int gSum0, gSum1;
    unsigned int bSum0, bSum1;

    unsigned int ySum0, ySum1;
    unsigned int uSum0, uSum1;
    unsigned int vSum0, vSum1;

    int index0, index1;
    CvPoint3D32f rgbColor[12];
    CvPoint3D32f yuvColor[12];

    unsigned int count;
    static unsigned int runCount = 0;
    int index;
    CvPoint3D32f addSum[8];
    float tmp;

    index = runCount % AVERAGE_COUNT;

    ySum0 = 0;
    ySum1 = 0;
    uSum0 = 0;
    uSum1 = 0;
    vSum0 = 0;
    vSum1 = 0;

    for (i = 0; i < texCoordsStatistics2D[flag].glTexCoord_FL_F.size(); i++)
    {
        index0 = texCoordsStatistics2D[flag].glTexCoord_FL_F[i];
        if (index0 % 4 == 0)
        {
            ySum0 += imageBuffer[0][index0 + 0];
            uSum0 += imageBuffer[0][index0 + 1];
            vSum0 += imageBuffer[0][index0 + 3];
        }
        else
        {
            ySum0 += imageBuffer[0][index0 + 0];
            uSum0 += imageBuffer[0][index0 - 1];
            vSum0 += imageBuffer[0][index0 + 1];
        }

        index1 = texCoordsStatistics2D[flag].glTexCoord_FL_L[i];
        if (index0 % 4 == 0)
        {
            ySum1 += imageBuffer[2][index1 + 0];
            uSum1 += imageBuffer[2][index1 + 1];
            vSum1 += imageBuffer[2][index1 + 3];
        }
        else
        {
            ySum1 += imageBuffer[2][index1 + 0];
            uSum1 += imageBuffer[2][index1 - 1];
            vSum1 += imageBuffer[2][index1 + 1];
        }
    }

    count = texCoordsStatistics2D[flag].glTexCoord_FL_F.size();
    yuvColor[0].x = 1.0 * ySum0 / count;
    yuvColor[0].y = 1.0 * uSum0 / count;
    yuvColor[0].z = 1.0 * vSum0 / count;

    yuvColor[1].x = 1.0 * ySum1 / count;
    yuvColor[1].y = 1.0 * uSum1 / count;
    yuvColor[1].z = 1.0 * vSum1 / count;

    ySum0 = 0;
    ySum1 = 0;
    uSum0 = 0;
    uSum1 = 0;
    vSum0 = 0;
    vSum1 = 0;

    for (i = 0; i < texCoordsStatistics2D[flag].glTexCoord_FR_F.size(); i++)
    {
        index0 = texCoordsStatistics2D[flag].glTexCoord_FR_F[i];
        if (index0 % 4 == 0)
        {
            ySum0 += imageBuffer[0][index0 + 0];
            uSum0 += imageBuffer[0][index0 + 1];
            vSum0 += imageBuffer[0][index0 + 3];
        }
        else
        {
            ySum0 += imageBuffer[0][index0 + 0];
            uSum0 += imageBuffer[0][index0 - 1];
            vSum0 += imageBuffer[0][index0 + 1];
        }

        index1 = texCoordsStatistics2D[flag].glTexCoord_FR_R[i];
        if (index0 % 4 == 0)
        {
            ySum1 += imageBuffer[3][index1 + 0];
            uSum1 += imageBuffer[3][index1 + 1];
            vSum1 += imageBuffer[3][index1 + 3];
        }
        else
        {
            ySum1 += imageBuffer[3][index1 + 0];
            uSum1 += imageBuffer[3][index1 - 1];
            vSum1 += imageBuffer[3][index1 + 1];
        }
    }

    count = texCoordsStatistics2D[flag].glTexCoord_FR_F.size();

    yuvColor[2].x = 1.0 * ySum0 / count;
    yuvColor[2].y = 1.0 * uSum0 / count;
    yuvColor[2].z = 1.0 * vSum0 / count;

    yuvColor[3].x = 1.0 * ySum1 / count;
    yuvColor[3].y = 1.0 * uSum1 / count;
    yuvColor[3].z = 1.0 * vSum1 / count;

    ySum0 = 0;
    ySum1 = 0;
    uSum0 = 0;
    uSum1 = 0;
    vSum0 = 0;
    vSum1 = 0;

    for (i = 0; i < texCoordsStatistics2D[flag].glTexCoord_BL_B.size(); i++)
    {
        index0 = texCoordsStatistics2D[flag].glTexCoord_BL_B[i];
        if (index0 % 4 == 0)
        {
            ySum0 += imageBuffer[1][index0 + 0];
            uSum0 += imageBuffer[1][index0 + 1];
            vSum0 += imageBuffer[1][index0 + 3];
        }
        else
        {
            ySum0 += imageBuffer[1][index0 + 0];
            uSum0 += imageBuffer[1][index0 - 1];
            vSum0 += imageBuffer[1][index0 + 1];
        }

        index1 = texCoordsStatistics2D[flag].glTexCoord_BL_L[i];
        if (index0 % 4 == 0)
        {
            ySum1 += imageBuffer[2][index1 + 0];
            uSum1 += imageBuffer[2][index1 + 1];
            vSum1 += imageBuffer[2][index1 + 3];
        }
        else
        {
            ySum1 += imageBuffer[2][index1 + 0];
            uSum1 += imageBuffer[2][index1 - 1];
            vSum1 += imageBuffer[2][index1 + 1];
        }
    }

    count = texCoordsStatistics2D[flag].glTexCoord_BL_B.size();

    yuvColor[4].x = 1.0 * ySum0 / count;
    yuvColor[4].y = 1.0 * uSum0 / count;
    yuvColor[4].z = 1.0 * vSum0 / count;

    yuvColor[5].x = 1.0 * ySum1 / count;
    yuvColor[5].y = 1.0 * uSum1 / count;
    yuvColor[5].z = 1.0 * vSum1 / count;

    ySum0 = 0;
    ySum1 = 0;
    uSum0 = 0;
    uSum1 = 0;
    vSum0 = 0;
    vSum1 = 0;

    for (i = 0; i < texCoordsStatistics2D[flag].glTexCoord_BR_B.size(); i++)
    {
        index0 = texCoordsStatistics2D[flag].glTexCoord_BR_B[i];
        if (index0 % 4 == 0)
        {
            ySum0 += imageBuffer[1][index0 + 0];
            uSum0 += imageBuffer[1][index0 + 1];
            vSum0 += imageBuffer[1][index0 + 3];
        }
        else
        {
            ySum0 += imageBuffer[1][index0 + 0];
            uSum0 += imageBuffer[1][index0 - 1];
            vSum0 += imageBuffer[1][index0 + 1];
        }

        index1 = texCoordsStatistics2D[flag].glTexCoord_BR_R[i];
        if (index0 % 4 == 0)
        {
            ySum1 += imageBuffer[3][index1 + 0];
            uSum1 += imageBuffer[3][index1 + 1];
            vSum1 += imageBuffer[3][index1 + 3];
        }
        else
        {
            ySum1 += imageBuffer[3][index1 + 0];
            uSum1 += imageBuffer[3][index1 - 1];
            vSum1 += imageBuffer[3][index1 + 1];
        }
    }

    count = texCoordsStatistics2D[flag].glTexCoord_BR_B.size();

    yuvColor[6].x = 1.0 * ySum0 / count;
    yuvColor[6].y = 1.0 * uSum0 / count;
    yuvColor[6].z = 1.0 * vSum0 / count;

    yuvColor[7].x = 1.0 * ySum1 / count;
    yuvColor[7].y = 1.0 * uSum1 / count;
    yuvColor[7].z = 1.0 * vSum1 / count;

    for (i = 0; i < 8; i++)
    {
        tmp = (yuvColor[i].x + 1.4075 * (yuvColor[i].y - 128)) / 255;
        tmp = tmp > 1 ? 1 : tmp < 0 ? 0 : tmp;
        colorCount[index][i].x = tmp;
        tmp = (yuvColor[i].x - 0.3455 * (yuvColor[i].z - 128) - 0.7169 * (yuvColor[i].y - 128)) / 255;
        tmp = tmp > 1 ? 1 : tmp < 0 ? 0 : tmp;
        colorCount[index][i].y = tmp;
        tmp = (yuvColor[i].x + 1.779 * (yuvColor[i].z - 128)) / 255;
        tmp = tmp > 1 ? 1 : tmp < 0 ? 0 : tmp;
        colorCount[index][i].z = tmp;

        tmp = (yuvColor[i + 1].x + 1.4075 * (yuvColor[i + 1].y - 128)) / 255;
        tmp = tmp > 1 ? 1 : tmp < 0 ? 0 : tmp;
        colorCount[index][i + 1].x = tmp;
        tmp = (yuvColor[i + 1].x - 0.3455 * (yuvColor[i + 1].z - 128) - 0.7169 * (yuvColor[i + 1].y - 128)) / 255;
        tmp = tmp > 1 ? 1 : tmp < 0 ? 0 : tmp;
        colorCount[index][i + 1].y = tmp;
        tmp = (yuvColor[i + 1].x + 1.779 * (yuvColor[i + 1].z - 128)) / 255;
        tmp = tmp > 1 ? 1 : tmp < 0 ? 0 : tmp;
        colorCount[index][i + 1].z = tmp;
    }

    runCount++;

    if (runCount < AVERAGE_COUNT)
    {
        memset(colorAve, 0, 4 * 3 * 12);
        return;
    }
    else
    {
        for (i = 0; i < 8; i++)
        {
            addSum[i].x = 0;
            addSum[i].y = 0;
            addSum[i].z = 0;
            for (j = 0; j < AVERAGE_COUNT; j++)
            {
                addSum[i].x += colorCount[j][i].x;
                addSum[i].y += colorCount[j][i].y;
                addSum[i].z += colorCount[j][i].z;
            }
            colorAve[i].x = addSum[i].x / AVERAGE_COUNT;
            colorAve[i].y = addSum[i].y / AVERAGE_COUNT;
            colorAve[i].z = addSum[i].z / AVERAGE_COUNT;
        }

        colorAve[8].x = (colorAve[0].x + colorAve[1].x) / 2;
        colorAve[8].y = (colorAve[0].y + colorAve[1].y) / 2;
        colorAve[8].z = (colorAve[0].z + colorAve[1].z) / 2;

        colorAve[9].x = (colorAve[2].x + colorAve[3].x) / 2;
        colorAve[9].y = (colorAve[2].y + colorAve[3].y) / 2;
        colorAve[9].z = (colorAve[2].z + colorAve[3].z) / 2;

        colorAve[10].x = (colorAve[4].x + colorAve[5].x) / 2;
        colorAve[10].y = (colorAve[4].y + colorAve[5].y) / 2;
        colorAve[10].z = (colorAve[4].z + colorAve[5].z) / 2;

        colorAve[11].x = (colorAve[6].x + colorAve[7].x) / 2;
        colorAve[11].y = (colorAve[6].y + colorAve[7].y) / 2;
        colorAve[11].z = (colorAve[6].z + colorAve[7].z) / 2;
    }

    //for(i=0; i<12; i++)
    //printf("count = %d, idx=%d ===== %f %f %f\n", runCount, i, colorAve[i].x, colorAve[i].y, colorAve[i].z);
}

#endif

/*====================================================================
?$)A=f???'0:   loadImage
?$)A=f????:   ??==????????g;??g:9g?
$)Ag.??e.??:
?$)A(e????:
$)Ah>?????:g:9g????i+?
$)Ah???g;??:   ??
$)Ad???h.0e?o<?
====================================================================*/
void loadImage(int w, int h)
{
    int i, j;

    int width, height, channel;
    unsigned char *imageBuffer[5];
    unsigned char *resBuffer[4];
    FILE *fp;
    char filePath[256] = {0};

    w = 1280;
    h = 720;

    for (i = 0; i < 5; i++)
    {
        //sprintf(filePath, "../test/jiaobanche/%d.yuyv", i);
        //imageBuffer[i] = stbi_load(filePath, &width, &height, &channel, 0);
        imageBuffer[i] = (unsigned char *)malloc(w * h * 2);
        /*fp = fopen(filePath, "rb");
		fread(imageBuffer[i], 1, w*h*2, fp);
		fclose(fp);*/
        //printf("image %d %d %d\n", width, height, channel);
        glGenTextures(1, &textureYUYV[i]);
        bindTexture(textureYUYV[i], imageBuffer[i], w / 2, h, GL_RGBA);
    }

    stbi_set_flip_vertically_on_load(0);

#if 1
    resBuffer[0] = stbi_load("../test/Car.png", &width, &height, &channel, 0);
    glGenTextures(1, &textureRes[0]);
    bindTexture(textureRes[0], resBuffer[0], width, height, GL_RGBA);

    // resBuffer[1] = stbi_load("../test/zebra.bmp", &width, &height, &channel, 0);
    resBuffer[1] = stbi_load("../test/car_new.png", &width, &height, &channel, 0);

    for (i = 0; i < width * height * channel; i += channel)
    {
        if (resBuffer[1][i] >= 250 &&
            resBuffer[1][i + 1] >= 250 &&
            resBuffer[1][i + 2] >= 250)
        {
            resBuffer[1][i + 3] = 0;
        }
    }
    glGenTextures(1, &textureRes[1]);
    bindTexture(textureRes[1], resBuffer[1], width, height, GL_RGBA);
#endif

    //for(i=0;i<22;i++)
    //caculateColorCoeff2D(&imageBuffer[0], 0);
}

#if 1
/*====================================================================
?$)A=f???'0:   esLoadShader
?$)A=f????:   ??==???2e?
$)Ag.??e.??:
?$)A(e????:
$)Ah>?????:   type:???2e?g1;e?o<?haderSrc:???2e?f:??
$)Ah???g;??:   g<??e%=g????2e?
$)Ad???h.0e?o<?
====================================================================*/
GLuint esLoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;
    char *infoLog;

    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0)
    {
        return 0;
    }

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            infoLog = (char *)malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            printf("Error compiling shader:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

/*====================================================================
?$)A=f???'0:   esLoadProgram
?$)A=f????:   ??;:???2e??%f?
$)Ag.??e.??:
?$)A(e????:
$)Ah>?????:   vertShaderSrc:i!6g????2e?f:??o<?ragShaderSrc:??????2e?f:??
$)Ah???g;??:   ??;:e%=g??%f?
$)Ad???h.0e?o<?
====================================================================*/
GLuint esLoadProgram(const char *vertShaderSrc, const char *fragShaderSrc)
{
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;
    char *infoLog;

    // Load the vertex/fragment shaders
    vertexShader = esLoadShader(GL_VERTEX_SHADER, vertShaderSrc);

    if (vertexShader == 0)
    {
        return 0;
    }

    fragmentShader = esLoadShader(GL_FRAGMENT_SHADER, fragShaderSrc);

    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return 0;
    }

    // Create the program object
    programObject = glCreateProgram();

    if (programObject == 0)
    {
        return 0;
    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Link the program
    glLinkProgram(programObject);

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        GLint infoLen = 0;

        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            infoLog = (char *)malloc(sizeof(char) * infoLen);

            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            printf("Error linking program:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteProgram(programObject);
        return 0;
    }

    // Free up no longer needed shader resources
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programObject;
}

#endif

/*====================================================================
?$)A=f???'0:   setBool
?$)A=f????:   g;???2e??????88?????
$)Ag.??e.??:
?$)A(e????:
$)Ah>?????:   programId:???2e??%f?o<?ame:?????'0o<?alue:???????
$)Ah???g;??:   ??
$)Ad???h.0e?o<?
====================================================================*/
static void setBool(GLuint programId, const char *name, bool value)
{
    glUniform1i(glGetUniformLocation(programId, name), (int)value);
}

static void setInt(GLuint programId, const char *name, int value)
{
    glUniform1i(glGetUniformLocation(programId, name), value);
}

static void setFloat(GLuint programId, const char *name, float value)
{
    glUniform1f(glGetUniformLocation(programId, name), value);
}

static void setVec2(GLuint programId, const char *name, const vec2 value)
{
    glUniform2fv(glGetUniformLocation(programId, name), 1, &value.x);
}

static void setVec2(GLuint programId, const char *name, float x, float y)
{
    glUniform2f(glGetUniformLocation(programId, name), x, y);
}

static void setVec3(GLuint programId, const char *name, const vec3 value)
{
    glUniform3fv(glGetUniformLocation(programId, name), 1, &value.x);
}

static void setVec3(GLuint programId, const char *name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(programId, name), x, y, z);
}

static void setVec4(GLuint programId, const char *name, const vec4 value)
{
    glUniform4fv(glGetUniformLocation(programId, name), 1, &value.r);
}

static void setVec4(GLuint programId, const char *name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(programId, name), x, y, z, w);
}

static void setMat4(GLuint programId, const char *name, ESMatrix mat)
{
    glUniformMatrix4fv(glGetUniformLocation(programId, name), 1, GL_FALSE, &mat.m[0][0]);
}

/*====================================================================
?$)A=f???'0:   rotationMatrixToEulerAngles
?$)A=f????:   ??=,?)i?h=????????
$)Ag.??e.??:
?$)A(e????:
$)Ah>?????:   Ro<??h=???5o?alpha\beta\gamma h>??d8?8*f,'f?h'?
$)Ah???g;??:   ??
$)Ad???h.0e?o<?
====================================================================*/
void rotationMatrixToEulerAngle(float *R, float *alpha, float *beta, float *gamma)
{
    float sy = sqrt(R[7] * R[7] + R[8] * R[8]);

    int singular = sy < 1e-6; // If

    float x, y, z;
    if (!singular)
    {
        x = atan2(R[7], R[8]);
        y = atan2(-R[6], sy);
        z = atan2(R[3], R[0]);
    }
    else
    {
        x = atan2(-R[5], R[4]);
        y = atan2(-R[6], sy);
        z = 0;
    }

    *alpha = x;
    *beta = y;
    *gamma = z;
}

/*====================================================================
?$)A=f???'0:   initShader
?$)A=f????:   ??;:???2e?
$)Ag.??e.??:
?$)A(e????:
$)Ah>?????:   ??
$)Ah???g;??:   ??
$)Ad???h.0e?o<?
====================================================================*/
static int initShader(void)
{
    hProgramHandle[0] = esLoadProgram(vertexSource, fragmentSource);
    hProgramHandle[1] = esLoadProgram(vertexMosaicSource, fragmentMosaicSource);
    hProgramHandle[2] = esLoadProgram(vertexBmpShowSource, fragmentBmpShowSource);
    hProgramHandle[3] = esLoadProgram(vertexBlendSource, fragmentBlendSource);
    hProgramHandle[4] = esLoadProgram(vertexCameraSource, fragmentCameraSource);

    printf("%d %d %d %d %d\n", hProgramHandle[0], hProgramHandle[1], hProgramHandle[2], hProgramHandle[3], hProgramHandle[4]);

    return GL_TRUE;
}

/*====================================================================
?$)A=f???'0:   show2DCar
?$)A=f????:   3Df82f??;i??>g$:
$)Ag.??e.??:
?$)A(e????:
$)Ah>?????:   DisplayChannelID  ?>g$:f(!e?
$)Ah???g;??:   ??
$)Ad???h.0e?o<?
====================================================================*/
void show2DCar(int flag)
{
    vec3 colorAdjust[2];
    int i, j;
    ESMatrix matrixMVP;

    esMatrixLoadIdentity(&matrixMVP);

    glViewport(bvs2DoffsetX, bvs2DoffsetY, bvs2DWidth, bvs2DHeight);

#if 1
    glUseProgram(hProgramHandle[0]);
    /* Enable attributes for position, color and texture coordinates etc. */
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    setMat4(hProgramHandle[0], "mvp", matrixMVP);

    //Front
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].CamVerticesPoints[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].CamImagePoints[0]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].LumiaBalance[0]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);

    colorAdjust[0].x = colorAve[8].x - colorAve[0].x;
    colorAdjust[0].y = colorAve[8].y - colorAve[0].y;
    colorAdjust[0].z = colorAve[8].z - colorAve[0].z;

    colorAdjust[1].x = colorAve[9].x - colorAve[2].x;
    colorAdjust[1].y = colorAve[9].y - colorAve[2].y;
    colorAdjust[1].z = colorAve[9].z - colorAve[2].z;

    setVec3(hProgramHandle[0], "color0Adjust", colorAdjust[0]);
    setVec3(hProgramHandle[0], "color1Adjust", colorAdjust[1]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[0]);
    setInt(hProgramHandle[0], "texUYVY422", 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, texCoords2D[flag].glTexCoord_F.size()); //GL_TRIANGLE_STRIP

    //back
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].CamVerticesPoints[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].CamImagePoints[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].LumiaBalance[1]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);

    colorAdjust[0].x = colorAve[10].x - colorAve[4].x;
    colorAdjust[0].y = colorAve[10].y - colorAve[4].y;
    colorAdjust[0].z = colorAve[10].z - colorAve[4].z;

    colorAdjust[1].x = colorAve[11].x - colorAve[6].x;
    colorAdjust[1].y = colorAve[11].y - colorAve[6].y;
    colorAdjust[1].z = colorAve[11].z - colorAve[6].z;

    setVec3(hProgramHandle[0], "color0Adjust", colorAdjust[0]);
    setVec3(hProgramHandle[0], "color1Adjust", colorAdjust[1]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[1]);
    setInt(hProgramHandle[0], "texUYVY422", 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, texCoords2D[flag].glTexCoord_B.size());

    //left
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].CamVerticesPoints[2]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].CamImagePoints[2]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].LumiaBalance[2]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);

    colorAdjust[0].x = colorAve[8].x - colorAve[1].x;
    colorAdjust[0].y = colorAve[8].y - colorAve[1].y;
    colorAdjust[0].z = colorAve[8].z - colorAve[1].z;

    colorAdjust[1].x = colorAve[10].x - colorAve[5].x;
    colorAdjust[1].y = colorAve[10].y - colorAve[5].y;
    colorAdjust[1].z = colorAve[10].z - colorAve[5].z;

    setVec3(hProgramHandle[0], "color0Adjust", colorAdjust[0]);
    setVec3(hProgramHandle[0], "color1Adjust", colorAdjust[1]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[2]);
    setInt(hProgramHandle[0], "texUYVY422", 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, texCoords2D[flag].glTexCoord_L.size());

    //right
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].CamVerticesPoints[3]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].CamImagePoints[3]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].LumiaBalance[3]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);

    colorAdjust[0].x = colorAve[9].x - colorAve[3].x;
    colorAdjust[0].y = colorAve[9].y - colorAve[3].y;
    colorAdjust[0].z = colorAve[9].z - colorAve[3].z;

    colorAdjust[1].x = colorAve[11].x - colorAve[7].x;
    colorAdjust[1].y = colorAve[11].y - colorAve[7].y;
    colorAdjust[1].z = colorAve[11].z - colorAve[7].z;

    setVec3(hProgramHandle[0], "color0Adjust", colorAdjust[0]);
    setVec3(hProgramHandle[0], "color1Adjust", colorAdjust[1]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[3]);
    setInt(hProgramHandle[0], "texUYVY422", 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, texCoords2D[flag].glTexCoord_R.size());
#endif

    //printf("run independ\n");
#if 1
    //?$)A;h??????
    glUseProgram(hProgramHandle[1]);

    /* Enable attributes for position, color and texture coordinates etc. */
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    //glUniformMatrix4fv(mvp_pos[1], 1, GL_FALSE, &matrixMVP.m[0][0]);
    setMat4(hProgramHandle[1], "mvp", matrixMVP);

    //front  left
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].Alpha[0]);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicCamVerticesPoints[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicFLCamImagePoints[0]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicFLCamImagePoints[1]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    colorAdjust[0].x = colorAve[8].x - colorAve[0].x;
    colorAdjust[0].y = colorAve[8].y - colorAve[0].y;
    colorAdjust[0].z = colorAve[8].z - colorAve[0].z;

    colorAdjust[1].x = colorAve[8].x - colorAve[1].x;
    colorAdjust[1].y = colorAve[8].y - colorAve[1].y;
    colorAdjust[1].z = colorAve[8].z - colorAve[1].z;

    setVec3(hProgramHandle[1], "color0Adjust", colorAdjust[0]);
    setVec3(hProgramHandle[1], "color1Adjust", colorAdjust[1]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[0]);
    setInt(hProgramHandle[1], "tex0UYVY422", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[2]);
    setInt(hProgramHandle[1], "tex1UYVY422", 1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCoords2D[flag].glVertex_FL.size());

    //front  right
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].Alpha[1]);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicCamVerticesPoints[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicFRCamImagePoints[0]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicFRCamImagePoints[1]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    colorAdjust[0].x = colorAve[9].x - colorAve[2].x;
    colorAdjust[0].y = colorAve[9].y - colorAve[2].y;
    colorAdjust[0].z = colorAve[9].z - colorAve[2].z;

    colorAdjust[1].x = colorAve[9].x - colorAve[3].x;
    colorAdjust[1].y = colorAve[9].y - colorAve[3].y;
    colorAdjust[1].z = colorAve[9].z - colorAve[3].z;

    setVec3(hProgramHandle[1], "color0Adjust", colorAdjust[0]);
    setVec3(hProgramHandle[1], "color1Adjust", colorAdjust[1]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[0]);
    setInt(hProgramHandle[1], "tex0UYVY422", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[3]);
    setInt(hProgramHandle[1], "tex1UYVY422", 1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCoords2D[flag].glVertex_FR.size());

    //back  left
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].Alpha[2]);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicCamVerticesPoints[2]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicBLCamImagePoints[0]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicBLCamImagePoints[1]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    colorAdjust[0].x = colorAve[10].x - colorAve[4].x;
    colorAdjust[0].y = colorAve[10].y - colorAve[4].y;
    colorAdjust[0].z = colorAve[10].z - colorAve[4].z;

    colorAdjust[1].x = colorAve[10].x - colorAve[5].x;
    colorAdjust[1].y = colorAve[10].y - colorAve[5].y;
    colorAdjust[1].z = colorAve[10].z - colorAve[5].z;

    setVec3(hProgramHandle[1], "color0Adjust", colorAdjust[0]);
    setVec3(hProgramHandle[1], "color1Adjust", colorAdjust[1]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[1]);
    setInt(hProgramHandle[1], "tex0UYVY422", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[2]);
    setInt(hProgramHandle[1], "tex1UYVY422", 1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCoords2D[flag].glVertex_BL.size());

    //back  right
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].Alpha[3]);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicCamVerticesPoints[3]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicBRCamImagePoints[0]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].MosaicBRCamImagePoints[1]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    colorAdjust[0].x = colorAve[11].x - colorAve[6].x;
    colorAdjust[0].y = colorAve[11].y - colorAve[6].y;
    colorAdjust[0].z = colorAve[11].z - colorAve[6].z;

    colorAdjust[1].x = colorAve[11].x - colorAve[7].x;
    colorAdjust[1].y = colorAve[11].y - colorAve[7].y;
    colorAdjust[1].z = colorAve[11].z - colorAve[7].z;

    setVec3(hProgramHandle[1], "color0Adjust", colorAdjust[0]);
    setVec3(hProgramHandle[1], "color1Adjust", colorAdjust[1]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[1]);
    setInt(hProgramHandle[1], "tex0UYVY422", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[3]);
    setInt(hProgramHandle[1], "tex1UYVY422", 1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCoords2D[flag].glVertex_BR.size());
#endif

#if 1
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(hProgramHandle[2]);

    setMat4(hProgramHandle[2], "mvp", matrixMVP);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].CarVerTexCoord[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[flag].CarVerTexCoord[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureRes[1]);
    setInt(hProgramHandle[1], "textureImg", 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif
}

void drawUndistortBackCurve(ESMatrix orthoMatrix)
{
    int i;
    vec4 color[4] = {{1.0, 0.0, 0.0, 0.5}, {1.0, 1.0, 0.0, 0.5}, {0.0, 1.0, 0.0, 0.5}, {0.0, 0.0, 0.0, 0.5}};

    float verticesView[] =
        {
            -0.5f, -0.5f, 0.0f, // left-buttom
            0.5f, -0.5f, 0.0f,  // right- buttom
            -0.5f, 0.5f, 0.0f,  // right-top
            0.5f, 0.5f, 0.0f,   // left-top
        };

    glUseProgram(hProgramHandle[3]);

    setMat4(hProgramHandle[3], "mvp", orthoMatrix);

    //?$)A3i?f71e:&f5?????e<?i"??f77e?
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(0);

#if 1
    for (i = 0; i < 6; i++)
    {
        setVec4(hProgramHandle[3], "outColor", color[i / 2]);

        glBindBuffer(GL_ARRAY_BUFFER, curveVerticesPoints[0]);
        glBufferData(GL_ARRAY_BUFFER, LENGTH * 2 * sizeof(CvPoint3D32f), verticesRearTrajLinePoint[i], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, LENGTH * 2);
    }

    glBindBuffer(GL_ARRAY_BUFFER, curveVerticesPoints[0]);
    glBufferData(GL_ARRAY_BUFFER, LENGTH * 2 * sizeof(CvPoint3D32f), verticesRearTrajLinePoint[6], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    for (i = 0; i < 3; i++)
    {
        setVec4(hProgramHandle[3], "outColor", color[i]);
        glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
    }
#endif

    setVec4(hProgramHandle[3], "outColor", color[0]);

    glBindBuffer(GL_ARRAY_BUFFER, curveVerticesPoints[0]);
    glBufferData(GL_ARRAY_BUFFER, LENGTH * 2 * sizeof(CvPoint3D32f), verticesRearTrajLinePoint[7], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //glDrawArrays(GL_TRIANGLE_STRIP, 0, LENGTH * 2);
}

CvPoint2D32f undistort(float i, float j,
                       const float *intrinsic_matrix, const float *dist_coeffs, undistortParams params, double *invR)
{
    float u, v;
    double x, y, x2, y2, r2, r, r4, r6, theta, theta2, theta4, theta6, theta8, theta_d, xd, yd, kr, _2xy;
    float xc, yc, zc, xr, yr;
    CvPoint2D32f imgPoints;
    float p3D[3], oldP3D[3];
    float z;

    float cx = intrinsic_matrix[0];
    float cy = intrinsic_matrix[1];
    float fx = intrinsic_matrix[2];
    float fy = intrinsic_matrix[3];
    //float _fx = 1.f / fx, _fy = 1.f / fy;
    float k1 = dist_coeffs[0];
    float k2 = dist_coeffs[1];
    float p1 = dist_coeffs[2];
    float p2 = dist_coeffs[3];
    float k3 = 0; //dist_coeffs[4];

    p3D[0] = (params.xZoom * j + params.x - cx) / fx;
    p3D[1] = (params.yZoom * i + params.y - cy) / fy;
    p3D[2] = 1.0;

    oldP3D[0] = invR[0] * p3D[0] + invR[1] * p3D[1] + invR[2] * p3D[2];
    oldP3D[1] = invR[3] * p3D[0] + invR[4] * p3D[1] + invR[5] * p3D[2];
    oldP3D[2] = invR[6] * p3D[0] + invR[7] * p3D[1] + invR[8] * p3D[2];

    if (oldP3D[2] > 0.001)
    {
        x = oldP3D[0] / oldP3D[2];
        y = oldP3D[1] / oldP3D[2];
        z = 1.0;

        y = y / z;
        x = x / z;

        y2 = y * y;
        x2 = x * x;
        r2 = x2 + y2;
        r4 = r2 * r2;
        r6 = r4 * r2;

        //xd = x * (1 + k1 * r2 + k2 * r4 + k3 * r6);// + 2 * p1 * x * y + p2 * (r2 + 2 * x2);
        //yd = y * (1 + k1 * r2 + k2 * r4 + k3 * r6);// + 2 * p2 * x * y + p1 * (r2 + 2 * y2);

        //x = x + 2 * p1 * x * y + p2 * (r2 + 2 * x2);
        //y = y + 2 * p2 * x * y + p1 * (r2 + 2 * y2);
#if 0
		xd = x * (1 + k1 * r2 + k2 * r4) + 2 * p1 * x * y + p2 * (r2 + 2 * x2);
		yd = y * (1 + k1 * r2 + k2 * r4) + 2 * p2 * x * y + p1 * (r2 + 2 * y2);

		/*$)Ae>????????*/
	    u  = fx * xd + cx;
	    v  = fy * yd + cy;
#endif

        _2xy = 2 * x * y;

        kr = (1 + ((k3 * r2 + k2) * r2 + k1) * r2);
        u = fx * (x * kr + p1 * _2xy + p2 * (r2 + 2 * x2)) + cx;
        v = fy * (y * kr + p1 * (r2 + 2 * y2) + p2 * _2xy) + cy;

        //u = fx*x + cx;
        //v = fy*y + cy;

        if (u < 0)
        {
            u = 0;
        }
        else if (u > IMGWIDTH)
        {
            u = IMGWIDTH; //IMGWIDTH - 2;
        }

        if (v < 0)
        {
            v = 0; //2;
        }
        else if (v > IMGHEIGHT)
        {
            v = IMGHEIGHT; //IMGHEIGHT - 2;
        }
    }
    else
    {
        u = 0;
        v = 0;
    }

    imgPoints.x = u / IMGWIDTH;
    imgPoints.y = v / IMGHEIGHT;

    return imgPoints;
}

void showSingleView(int viewMode, float wheelAngle, int flag)
{
    int whichCamera, cnt;
    float rVec[3];
    CvMat rVEC, rMAT;

    static float camera[4], distortTable[5], rMat[9], tVec[3];

    float verticesView[] =
        {
            -1.0f, -1.0f, 0.0f, // left-buttom
            1.0f, -1.0f, 0.0f,  // right- buttom
            -1.0f, 1.0f, 0.0f,  // right-top
            1.0f, 1.0f, 0.0f,   // left-top
        };

    float texCoordView[] =
        {
            0.0f, 1.0f, // left-top
            1.0f, 1.0f, // right-top
            0.0f, 0.0f, // left-buttom
            1.0f, 0.0f, // right- buttom
        };

    float *verAddr, *texAddr;

    float alpha, beta, gamma;
    double R[9], invR[9], outR[9];
    undistortParams resizer;
    int k;
    ESMatrix orthoMatrix;

    switch (viewMode)
    {
    case VIEW_FRONT:
        whichCamera = 0;
        cnt = 4;
        verAddr = verticesView;
        texAddr = texCoordView;
        break;
    case VIEW_BACK:
        whichCamera = 1;
        cnt = 4;
        verAddr = verticesView;
        texAddr = texCoordView;
        break;
    case VIEW_LEFT:
        whichCamera = 2;
        cnt = 4;
        verAddr = verticesView;
        texAddr = texCoordView;
        break;
    case VIEW_RIGHT:
        whichCamera = 3;
        cnt = 4;
        verAddr = verticesView;
        texAddr = texCoordView;
        break;
    case VIEW_BACKWARD:
        whichCamera = 4;
        cnt = verCount;
        verAddr = (float *)verCoordPoint;
        texAddr = (float *)texCoordPoint;
        break;
    default:
        whichCamera = 0;
        cnt = 4;
        verAddr = verticesView;
        texAddr = texCoordView;
        break;
    }

    esMatrixLoadIdentity(&orthoMatrix);

    if (viewMode == VIEW_BACKWARD)
    {
        camera[0] = (float)backWardCamParams[flag].mimdInt[0] / SCALE2;
        camera[1] = (float)backWardCamParams[flag].mimdInt[1] / SCALE2;
        camera[2] = (float)backWardCamParams[flag].mimdInt[2] / SCALE2;
        camera[3] = (float)backWardCamParams[flag].mimdInt[3] / SCALE2;

        distortTable[0] = (float)backWardCamParams[flag].mimdInt[4] / SCALE1;
        distortTable[1] = (float)backWardCamParams[flag].mimdInt[5] / SCALE1;
        distortTable[2] = (float)backWardCamParams[flag].mimdInt[6] / SCALE1;
        distortTable[3] = (float)backWardCamParams[flag].mimdInt[7] / SCALE1;
        distortTable[4] = 0.003176562358955857;

        rVec[0] = (float)backWardCamParams[flag].mrInt[0] / SCALE3;
        rVec[1] = (float)backWardCamParams[flag].mrInt[1] / SCALE3;
        rVec[2] = (float)backWardCamParams[flag].mrInt[2] / SCALE3;

        tVec[0] = (float)backWardCamParams[flag].mtInt[0] / SCALE2;
        tVec[1] = (float)backWardCamParams[flag].mtInt[1] / SCALE2;
        tVec[2] = (float)backWardCamParams[flag].mtInt[2] / SCALE2;

        rVEC = cvMat(1, 3, CV_32F, rVec);
        rMAT = cvMat(3, 3, CV_32F, rMat);

        cvRodrigues2(&rVEC, &rMAT, NULL);

        rotationMatrixToEulerAngle(rMat, &alpha, &beta, &gamma);

        alpha = 0 * RADIAN;

        R[0] = cos(beta) * cos(gamma);
        R[1] = cos(beta) * sin(gamma);
        R[2] = -sin(beta);
        R[3] = sin(alpha) * sin(beta) * cos(gamma) - cos(alpha) * sin(gamma);
        R[4] = sin(alpha) * sin(beta) * sin(gamma) + cos(alpha) * cos(gamma);
        R[5] = sin(alpha) * cos(beta);
        R[6] = cos(alpha) * sin(beta) * cos(gamma) + sin(alpha) * sin(gamma);
        R[7] = cos(alpha) * sin(beta) * sin(gamma) - sin(alpha) * cos(gamma);
        R[8] = cos(alpha) * cos(beta);

        getInvertMatrix(R, invR);

        resizer.x = 100;
        resizer.y = 50;
        resizer.xZoom = 1.0 * 1080 / bvs3DWidth;
        resizer.yZoom = 1.0 * 620 / bvs3DHeight;

        for (k = 0; k < verCount; k++)
        {
            texCoordPoint[k] = undistort(imgCoordPoint[k].y, imgCoordPoint[k].x,
                                         camera, distortTable, resizer, invR);
        }
    }

    glViewport(bvs3DoffsetX, bvs3DoffsetY, bvs3DWidth, bvs3DHeight);

    glUseProgram(hProgramHandle[4]);

    /* Enable attributes for position, color and texture coordinates etc. */
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    setMat4(hProgramHandle[4], "mvp", orthoMatrix);

    //Front
    glBindBuffer(GL_ARRAY_BUFFER, cameraVerTexCoord[0]);
    glBufferData(GL_ARRAY_BUFFER, cnt * sizeof(CvPoint3D32f), verAddr, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, cameraVerTexCoord[1]);
    glBufferData(GL_ARRAY_BUFFER, cnt * sizeof(CvPoint2D32f), texAddr, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[whichCamera]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, cnt);

    if (viewMode == VIEW_BACKWARD)
    {
        findRearCurve(wheelAngle, resizer, camera, distortTable, rMat, tVec, invR, bvs3DWidth, bvs3DHeight);

        drawUndistortBackCurve(orthoMatrix);
    }
}

void showFourView()
{
    vec3 colorAdjust[2] = {{0, 0, 0}, {0, 0, 0}};

    float verticesViewFront[] =
        {
            -1.0f, 0.0f, 0.0f, // left-buttom
            0.0f, 0.0f, 0.0f,  // right- buttom
            -1.0f, 1.0f, 0.0f, // right-top
            0.0f, 1.0f, 0.0f,  // left-top
        };

    float verticesViewBack[] =
        {
            0.0f, 0.0f, 0.0f, // left-buttom
            1.0f, 0.0f, 0.0f, // right- buttom
            0.0f, 1.0f, 0.0f, // right-top
            1.0f, 1.0f, 0.0f, // left-top
        };

    float verticesViewLeft[] =
        {
            -1.0f, -1.0f, 0.0f, // left-buttom
            0.0f, -1.0f, 0.0f,  // rLeftight- buttom
            -1.0f, 0.0f, 0.0f,  // right-top
            0.0f, 0.0f, 0.0f,   // left-top
        };

    float verticesViewRight[] =
        {
            0.0f, -1.0f, 0.0f, // left-buttom
            1.0f, -1.0f, 0.0f, // right- buttom
            0.0f, 0.0f, 0.0f,  // right-top
            1.0f, 0.0f, 0.0f,  // left-top
        };

    float texCoordView[] =
        {
            0.0f, 1.0f, // left-top
            1.0f, 1.0f, // right-top
            0.0f, 0.0f, // left-buttom
            1.0f, 0.0f, // right- buttom
        };

    ESMatrix orthoMatrix;
    float lumia[2] = {0.0, 0.0};
    esMatrixLoadIdentity(&orthoMatrix);

    glViewport(bvs3DoffsetX, bvs3DoffsetY, bvs3DWidth, bvs3DHeight);

    glUseProgram(hProgramHandle[4]);

    /* Enable attributes for position, color and texture coordinates etc. */
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    setMat4(hProgramHandle[4], "mvp", orthoMatrix);

    //Front
    glBindBuffer(GL_ARRAY_BUFFER, cameraVerTexCoord[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(CvPoint3D32f), verticesViewFront, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, cameraVerTexCoord[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(CvPoint2D32f), texCoordView, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //Back
    glBindBuffer(GL_ARRAY_BUFFER, cameraVerTexCoord[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(CvPoint3D32f), verticesViewBack, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, cameraVerTexCoord[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(CvPoint2D32f), texCoordView, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //Left
    glBindBuffer(GL_ARRAY_BUFFER, cameraVerTexCoord[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(CvPoint3D32f), verticesViewLeft, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, cameraVerTexCoord[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(CvPoint2D32f), texCoordView, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[2]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //Right
    glBindBuffer(GL_ARRAY_BUFFER, cameraVerTexCoord[0]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(CvPoint3D32f), verticesViewRight, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, cameraVerTexCoord[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(CvPoint2D32f), texCoordView, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureYUYV[3]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int generateTriangle(CvPoint2D32f *triagnle, int width, int height)
{
    float p = 64.0;
    float i, j, fp;
    unsigned char directionFlag;
    CvPoint2D32f point;
    int count;
    fp = 1.0 / p;

    directionFlag = 0;

    count = 0;

    for (i = 0; i < 1; i += fp)
    {
        switch (directionFlag)
        {
        case 0:
            for (j = 0; j <= 1; j += fp)
            {
                point.x = j * width;
                point.y = i * height;

                //triagnle.push_back(point);
                triagnle[count++] = point;

                point.x = j * width;
                point.y = (i + fp) * height;

                //triagnle.push_back(point);
                triagnle[count++] = point;

                //count += 2;
            }
            //objPoints2D.glObjPoints_F.pop_back();
            //vertexCoords2D.glVertex_F.pop_back();
            break;
        case 1:
            for (j = 1; j >= 0; j -= fp)
            {
                point.x = j * width;
                point.y = i * height;

                //triagnle.push_back(point);
                triagnle[count++] = point;

                point.x = j * width;
                point.y = (i + fp) * height;

                //triagnle.push_back(point);
                triagnle[count++] = point;

                //count += 2;
            }
            //objPoints.glObjPoints_F.pop_back();
            //vertexCoords.glVertex_F.pop_back();
            break;
        }
        // change direction!
        if (j - fp == 1)
        {
            directionFlag = 1;
        }
        else
        {
            directionFlag = 0;
        }
    }

    //printf("count = %d\n",count);

    return count;
}

void initBackWard(int width, int height)
{
    int k, p;
    p = 64;

    imgCoordPoint = (CvPoint2D32f *)malloc(sizeof(CvPoint2D32f) * p * (p + 1) * 2);

    verCount = generateTriangle(imgCoordPoint, width, height);

    texCoordPoint = (CvPoint2D32f *)malloc(sizeof(CvPoint2D32f) * verCount);
    verCoordPoint = (CvPoint3D32f *)malloc(sizeof(CvPoint3D32f) * verCount);

    for (k = 0; k < verCount; k++)
    {
        verCoordPoint[k].x = -(1.0 - 2 * imgCoordPoint[k].x / width);
        verCoordPoint[k].y = -(1.0 - 2 * imgCoordPoint[k].y / height);
        verCoordPoint[k].z = 0;
    }
}

int initMosaic(safImgRect allView, safImgRect singleView)
{
    int i;

    bvs2DWidth = allView.width;
    bvs2DHeight = allView.height;
    bvs2DoffsetX = allView.x;
    bvs2DoffsetY = allView.y;

    bvs3DWidth = singleView.width;
    bvs3DHeight = singleView.height;
    bvs3DoffsetX = singleView.x;
    bvs3DoffsetY = singleView.y;

    initShader();

    initCamParaData();

    for (i = 0; i < 2; i++)
    {
        init2DModel(i);

        initTextureCoords(i);

        getCamPixelPosition(i);
    }

    loadImage(IMGWIDTH, IMGHEIGHT);

    //caculateLumiaCoeff();

    initVBO();

    initBackWard(bvs3DWidth, bvs3DHeight);

    return 0;
}

extern void updateTexture(unsigned char **src)
{
    int i;
    for (i = 0; i < 5; i++)
    {
        glBindTexture(GL_TEXTURE_2D, textureYUYV[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMGWIDTH / 2, IMGHEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, src[i]);
    }

    caculateColorCoeff2D(src, 0);
    //memset(colorAve, 0, 4*3*12);
}

void runRender(int viewMode, float steeringWheelAngle, int flag)
{
    // Clear the colorbuffer and depth-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    show2DCar(flag);
    if (viewMode == VIEW_OVERALL)
    {
        showFourView();
    }
    else
    {
        showSingleView(viewMode, steeringWheelAngle, flag);
    }
}
