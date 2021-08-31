#include <GLES3/gl3.h>
#include <EGL/egl.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <vector>
#include <string>

#include "InitMosaic.hpp"

TexCoords texCoords2D[2];
ObjPoints objPoints2D[2];
VertexCoords vertexCoords2D[2];
BlendAlpha blendAlpha2D[2];
LumiaAdjust lumiaAdjust2D[2];

ObjPointsStatistics objPointsStatistics2D[2];
TexCoordsStatistics texCoordsStatistics2D[2];

VBO3DMosaicImage VBO2DMosaicImageParams[2];

GLuint curveVerticesPoints[2];
GLuint cameraVerTexCoord[2];

SimplifyCamParams SimplifyfrontCamParams[2];
SimplifyCamParams SimplifyrearCamParams[2];
SimplifyCamParams SimplifyleftCamParams[2];
SimplifyCamParams SimplifyrightCamParams[2];
SimplifyCamParams backWardCamParams[2];

PARA_FIELD para_field[2];

CvPoint3D32f verticesRearTrajLinePoint[11][LENGTH * 2];

GLfloat glVertices2DCar[2][12] =
	{
		-1.0f, -1.0f, 0.0f, // left-buttom
		1.0f, -1.0f, 0.0f,	// right- buttom
		-1.0f, 1.0f, 0.0f,	// right-top
		1.0f, 1.0f, 0.0f,	// left-top

		-1.0f, -1.0f, 0.0f, // left-buttom
		1.0f, -1.0f, 0.0f,	// right- buttom
		-1.0f, 1.0f, 0.0f,	// right-top
		1.0f, 1.0f, 0.0f,	// left-top
};

GLfloat glTexCoordCar[] =
	{
		0.0f, 1.0f, // left-top
		1.0f, 1.0f, // right-top
		0.0f, 0.0f, // left-buttom
		1.0f, 0.0f, // right- buttom
};

static float getDistance(float x1, float y1, float x2, float y2, float x, float y)
{
	float temp1, temp2;
	temp1 = (y2 - y1) * x + (x1 - x2) * y + (y1 - y2) * x1 - (x1 - x2) * y1;
	temp2 = (float)sqrt((y2 - y1) * (y2 - y1) + (x1 - x2) * (x1 - x2));
	return (float)(temp1 / temp2);
}

static float getPixelDistance(int x1, int y1, int x2, int y2)
{
	/*(x,y)??(x1,y1)(x2,y2)??????????*/
	float temp1;
	temp1 = (float)sqrt((y2 - y1) * (y2 - y1) + (x1 - x2) * (x1 - x2));
	return temp1;
}

void init2DModel(int flag)
{
	float pxv, pyv, pzv;
	float pxw, pyw, pzw;
	float pxt, pyt, pzt;
	CvPoint3D32f pt3d_w, pt3d_w0, pt3d_w1;
	CvPoint3D32f pt3d_v;

	int halfWorldWidth, halfWorldHeight, worldWidth, worldHeight;
	float i, j, fp;
	float p = 64.0;
	unsigned char directionFlag = 0;
	float x, y;
	float adjustCoeff;

	CvPoint2D32f boundary[4];
	float dist1, dist2;
	float theta, theta1;
	float weight;
	float Thresh = 0; //sqrt(0);
	float frontXfuse, frontYfuse, rearXfuse, rearYfuse;
	CvPoint2D32f f1, f2, f3;

	worldWidth = para_field[flag].carWorldX + para_field[flag].car_width + para_field[flag].carWorldX2;
	worldHeight = para_field[flag].carWorldY + para_field[flag].car_length + para_field[flag].carWorldY2;

	halfWorldWidth = worldWidth / 2;
	halfWorldHeight = worldHeight / 2;

	fp = 1.0 / p;

#if 1
	for (i = 0; i < 1; i += fp)
	{
		switch (directionFlag)
		{
		case 0:
			for (j = 0; j <= 1; j += fp)
			{
				pxt = j * para_field[flag].car_width + para_field[flag].carWorldX;
				pyt = i * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_F.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_F.push_back(pt3d_v);

				adjustCoeff = 1.0 * (pxt - para_field[flag].carWorldX) / (para_field[flag].car_width);
				lumiaAdjust2D[flag].glLumiaAdjust_F.push_back(adjustCoeff);

				pxt = j * para_field[flag].car_width + para_field[flag].carWorldX;
				pyt = (i + fp) * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_F.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_F.push_back(pt3d_v);

				adjustCoeff = 1.0 * (pxt - para_field[flag].carWorldX) / (para_field[flag].car_width);
				lumiaAdjust2D[flag].glLumiaAdjust_F.push_back(adjustCoeff);
			}
			//objPoints2D[flag].glObjPoints_F.pop_back();
			//vertexCoords2D[flag].glVertex_F.pop_back();
			break;
		case 1:
			for (j = 1; j >= 0; j -= fp)
			{
				pxt = j * para_field[flag].car_width + para_field[flag].carWorldX;
				pyt = i * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_F.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_F.push_back(pt3d_v);

				adjustCoeff = 1.0 * (pxt - para_field[flag].carWorldX) / (para_field[flag].car_width);
				lumiaAdjust2D[flag].glLumiaAdjust_F.push_back(adjustCoeff);

				pxt = j * para_field[flag].car_width + para_field[flag].carWorldX;
				pyt = (i + fp) * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_F.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_F.push_back(pt3d_v);

				adjustCoeff = 1.0 * (pxt - para_field[flag].carWorldX) / (para_field[flag].car_width);
				lumiaAdjust2D[flag].glLumiaAdjust_F.push_back(adjustCoeff);
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
	printf("VTX_NUM_F: %lu\n", objPoints2D[flag].glObjPoints_F.size());
	texCoords2D[flag].glTexCoord_F.resize(objPoints2D[flag].glObjPoints_F.size());
#endif

	directionFlag = 0;

#if 1
	for (i = 0; i < 1; i += fp)
	{
		switch (directionFlag)
		{
		case 0:
			for (j = 0; j <= 1; j += fp)
			{
				pxt = j * para_field[flag].car_width + para_field[flag].carWorldX;
				pyt = i * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY + para_field[flag].car_length)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_B.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_B.push_back(pt3d_v);

				adjustCoeff = 1.0 * (pxt - para_field[flag].carWorldX) / (para_field[flag].car_width);
				lumiaAdjust2D[flag].glLumiaAdjust_B.push_back(adjustCoeff);

				pxt = j * para_field[flag].car_width + para_field[flag].carWorldX;
				pyt = (i + fp) * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY + para_field[flag].car_length)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_B.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_B.push_back(pt3d_v);

				adjustCoeff = 1.0 * (pxt - para_field[flag].carWorldX) / (para_field[flag].car_width);
				lumiaAdjust2D[flag].glLumiaAdjust_B.push_back(adjustCoeff);
			}
			//objPoints2D[flag].glObjPoints_F.pop_back();
			//vertexCoords2D[flag].glVertex_F.pop_back();
			break;
		case 1:
			for (j = 1; j >= 0; j -= fp)
			{
				pxt = j * para_field[flag].car_width + para_field[flag].carWorldX;
				pyt = i * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY + para_field[flag].car_length)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_B.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_B.push_back(pt3d_v);

				adjustCoeff = 1.0 * (pxt - para_field[flag].carWorldX) / (para_field[flag].car_width);
				lumiaAdjust2D[flag].glLumiaAdjust_B.push_back(adjustCoeff);

				pxt = j * para_field[flag].car_width + para_field[flag].carWorldX;
				pyt = (i + fp) * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY + para_field[flag].car_length)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_B.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_B.push_back(pt3d_v);

				adjustCoeff = 1.0 * (pxt - para_field[flag].carWorldX) / (para_field[flag].car_width);
				lumiaAdjust2D[flag].glLumiaAdjust_B.push_back(adjustCoeff);
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
	printf("VTX_NUM_B: %lu\n", objPoints2D[flag].glObjPoints_B.size());
	texCoords2D[flag].glTexCoord_B.resize(objPoints2D[flag].glObjPoints_B.size());
#endif

#if 1
	for (i = 0; i < 1; i += fp)
	{
		switch (directionFlag)
		{
		case 0:
			for (j = 0; j <= 1; j += fp)
			{
				pxt = j * para_field[flag].carWorldX;
				pyt = i * para_field[flag].car_length;

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_L.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_L.push_back(pt3d_v);

				adjustCoeff = 1.0 * pyt / para_field[flag].car_length;
				lumiaAdjust2D[flag].glLumiaAdjust_L.push_back(adjustCoeff);

				pxt = j * para_field[flag].carWorldX;
				pyt = (i + fp) * para_field[flag].car_length;

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_L.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_L.push_back(pt3d_v);

				adjustCoeff = 1.0 * pyt / para_field[flag].car_length;

				lumiaAdjust2D[flag].glLumiaAdjust_L.push_back(adjustCoeff);
			}
			//objPoints2D[flag].glObjPoints_F.pop_back();
			//vertexCoords2D[flag].glVertex_F.pop_back();
			break;
		case 1:
			for (j = 1; j >= 0; j -= fp)
			{
				pxt = j * para_field[flag].carWorldX;
				pyt = i * para_field[flag].car_length;

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_L.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_L.push_back(pt3d_v);

				adjustCoeff = 1.0 * pyt / para_field[flag].car_length;

				lumiaAdjust2D[flag].glLumiaAdjust_L.push_back(adjustCoeff);

				pxt = j * para_field[flag].carWorldX;
				pyt = (i + fp) * para_field[flag].car_length;

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_L.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_L.push_back(pt3d_v);

				adjustCoeff = 1.0 * pyt / para_field[flag].car_length;

				lumiaAdjust2D[flag].glLumiaAdjust_L.push_back(adjustCoeff);
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
	printf("VTX_NUM_L: %lu\n", objPoints2D[flag].glObjPoints_L.size());
	texCoords2D[flag].glTexCoord_L.resize(objPoints2D[flag].glObjPoints_L.size());
#endif

#if 1
	for (i = 0; i < 1; i += fp)
	{
		switch (directionFlag)
		{
		case 0:
			for (j = 0; j <= 1; j += fp)
			{
				pxt = j * para_field[flag].carWorldX2;
				pyt = i * para_field[flag].car_length;

				pxw = pxt;
				pyw = pyt - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pxv = (para_field[flag].carWorldX + para_field[flag].car_width + pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_R.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_R.push_back(pt3d_v);

				adjustCoeff = 1.0 * pyt / para_field[flag].car_length;

				lumiaAdjust2D[flag].glLumiaAdjust_R.push_back(adjustCoeff);

				pxt = j * para_field[flag].carWorldX2;
				pyt = (i + fp) * para_field[flag].car_length;

				pxw = pxt;
				pyw = pyt - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pxv = (para_field[flag].carWorldX + para_field[flag].car_width + pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_R.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_R.push_back(pt3d_v);

				adjustCoeff = 1.0 * pyt / para_field[flag].car_length;

				lumiaAdjust2D[flag].glLumiaAdjust_R.push_back(adjustCoeff);
			}
			//objPoints2D[flag].glObjPoints_F.pop_back();
			//vertexCoords2D[flag].glVertex_F.pop_back();
			break;
		case 1:
			for (j = 1; j >= 0; j -= fp)
			{
				pxt = j * para_field[flag].carWorldX2;
				pyt = i * para_field[flag].car_length;

				pxw = pxt;
				pyw = pyt - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pxv = (para_field[flag].carWorldX + para_field[flag].car_width + pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_R.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_R.push_back(pt3d_v);

				adjustCoeff = 1.0 * pyt / para_field[flag].car_length;

				lumiaAdjust2D[flag].glLumiaAdjust_R.push_back(adjustCoeff);

				pxt = j * para_field[flag].carWorldX2;
				pyt = (i + fp) * para_field[flag].car_length;

				pxw = pxt;
				pyw = pyt - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pxv = (para_field[flag].carWorldX + para_field[flag].car_width + pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - (pyt + para_field[flag].carWorldY)) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_R.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_R.push_back(pt3d_v);

				adjustCoeff = 1.0 * pyt / para_field[flag].car_length;

				lumiaAdjust2D[flag].glLumiaAdjust_R.push_back(adjustCoeff);
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
	printf("VTX_NUM_R: %lu\n", objPoints2D[flag].glObjPoints_R.size());
	texCoords2D[flag].glTexCoord_R.resize(objPoints2D[flag].glObjPoints_R.size());
#endif

#if 1
	frontXfuse = para_field[flag].carWorldX * 0.5;
	frontYfuse = para_field[flag].carWorldY * 0.5;

	f1.x = para_field[flag].carWorldX;
	f1.y = para_field[flag].carWorldY;

	f2.x = 0;
	f2.y = para_field[flag].carWorldY - frontYfuse;

	f3.x = para_field[flag].carWorldX - frontXfuse;
	f3.y = 0;

	theta = atan((float)(f1.y - f3.y) / (f1.x - f3.x)) - atan((float)(f1.y - f2.y) / (f1.x - f2.x));

	for (i = 0; i <= 1; i += fp)
	{
		switch (directionFlag)
		{
		case 0:
			for (j = 0; j <= 1; j += fp)
			{
				pxt = j * para_field[flag].carWorldX;
				pyt = i * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w0 = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_FL_F.push_back(pt3d_w0);
				vertexCoords2D[flag].glVertex_FL.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w1 = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_FL_L.push_back(pt3d_w1);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_FL.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_FL.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_FL.push_back(1.0 - theta1 / theta);

					if ((pxt > para_field[flag].carWorldX / 2) && (pyt > para_field[flag].carWorldY / 2))
					{
						objPointsStatistics2D[flag].glObjPoints_FL_F.push_back(pt3d_w0);
						objPointsStatistics2D[flag].glObjPoints_FL_L.push_back(pt3d_w1);
					}
				}

				pxt = j * para_field[flag].carWorldX;
				pyt = (i + fp) * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_FL_F.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_FL.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_FL_L.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_FL.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_FL.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_FL.push_back(1.0 - theta1 / theta);
				}
			}
			//objPoints2D[flag].glObjPoints_F.pop_back();
			//vertexCoords2D[flag].glVertex_F.pop_back();
			break;
		case 1:
			for (j = 1; j >= 0; j -= fp)
			{
				pxt = j * para_field[flag].carWorldX;
				pyt = i * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_FL_F.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_FL.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_FL_L.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_FL.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_FL.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_FL.push_back(1.0 - theta1 / theta);
				}

				pxt = j * para_field[flag].carWorldX;
				pyt = (i + fp) * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_FL_F.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_FL.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_FL_L.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_FL.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_FL.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_FL.push_back(1.0 - theta1 / theta);
				}
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
	printf("VTX_NUM_FL: %lu %lu\n", objPoints2D[flag].glObjPoints_FL_F.size(), objPoints2D[flag].glObjPoints_FL_L.size());
	texCoords2D[flag].glTexCoord_FL_F.resize(objPoints2D[flag].glObjPoints_FL_F.size());
	texCoords2D[flag].glTexCoord_FL_L.resize(objPoints2D[flag].glObjPoints_FL_L.size());
	texCoordsStatistics2D[flag].glTexCoord_FL_F.resize(objPointsStatistics2D[flag].glObjPoints_FL_F.size());
	texCoordsStatistics2D[flag].glTexCoord_FL_L.resize(objPointsStatistics2D[flag].glObjPoints_FL_L.size());

#endif

#if 1
	f1.x = para_field[flag].carWorldX + para_field[flag].car_width;
	f1.y = para_field[flag].carWorldY;

	f2.x = worldWidth;
	f2.y = para_field[flag].carWorldY - frontYfuse;

	f3.x = para_field[flag].carWorldX + para_field[flag].car_width + frontXfuse;
	f3.y = 0;

	theta = atan((float)(f1.y - f3.y) / (f3.x - f1.x)) - atan((float)(f1.y - f2.y) / (f2.x - f1.x));

	printf("FR theta = %f\n", theta);

	for (i = 0; i <= 1; i += fp)
	{
		switch (directionFlag)
		{
		case 0:
			for (j = 0; j <= 1; j += fp)
			{
				pxt = para_field[flag].carWorldX + para_field[flag].car_width + j * para_field[flag].carWorldX2;
				pyt = i * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w0 = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_FR_F.push_back(pt3d_w0);
				vertexCoords2D[flag].glVertex_FR.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX - para_field[flag].car_width;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w1 = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_FR_R.push_back(pt3d_w1);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_FR.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_FR.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_FR.push_back(1.0 - theta1 / theta);

					if ((pxt < para_field[flag].carWorldX + para_field[flag].car_width + para_field[flag].carWorldX2 / 2) && (pyt > para_field[flag].carWorldY / 2))
					{
						objPointsStatistics2D[flag].glObjPoints_FR_F.push_back(pt3d_w0);
						objPointsStatistics2D[flag].glObjPoints_FR_R.push_back(pt3d_w1);
					}
				}

				pxt = para_field[flag].carWorldX + para_field[flag].car_width + j * para_field[flag].carWorldX2;
				pyt = (i + fp) * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_FR_F.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_FR.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX - para_field[flag].car_width;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_FR_R.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_FR.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_FR.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_FR.push_back(1.0 - theta1 / theta);
				}
			}
			//objPoints2D[flag].glObjPoints_F.pop_back();
			//vertexCoords2D[flag].glVertex_F.pop_back();
			break;
		case 1:
			for (j = 1; j >= 0; j -= fp)
			{
				pxt = para_field[flag].carWorldX + para_field[flag].car_width + j * para_field[flag].carWorldX2;
				pyt = i * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_FR_F.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_FR.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX - para_field[flag].car_width;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_FR_R.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_FR.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_FR.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_FR.push_back(1.0 - theta1 / theta);
				}

				pxt = para_field[flag].carWorldX + para_field[flag].car_width + j * para_field[flag].carWorldX2;
				pyt = (i + fp) * para_field[flag].carWorldY;

				pxw = pxt - worldWidth / 2 + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = para_field[flag].chessboard_width_corners * para_field[flag].square_size + (pyt - para_field[flag].carWorldY);
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_FR_F.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_FR.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX - para_field[flag].car_width;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_FR_R.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_FR.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_FR.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_FR.push_back(1.0 - theta1 / theta);
				}
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
	printf("VTX_NUM_FR: %lu %lu\n", objPoints2D[flag].glObjPoints_FR_F.size(), objPoints2D[flag].glObjPoints_FR_R.size());
	texCoords2D[flag].glTexCoord_FR_F.resize(objPoints2D[flag].glObjPoints_FR_F.size());
	texCoords2D[flag].glTexCoord_FR_R.resize(objPoints2D[flag].glObjPoints_FR_R.size());
	texCoordsStatistics2D[flag].glTexCoord_FR_F.resize(objPointsStatistics2D[flag].glObjPoints_FR_F.size());
	texCoordsStatistics2D[flag].glTexCoord_FR_R.resize(objPointsStatistics2D[flag].glObjPoints_FR_R.size());
#endif

#if 1
	rearXfuse = para_field[flag].carWorldX * 0.8;
	rearYfuse = (worldHeight - para_field[flag].carWorldY - para_field[flag].car_length) * 0.2;

	f1.x = para_field[flag].carWorldX;
	f1.y = para_field[flag].carWorldY + para_field[flag].car_length;

	f2.x = 0;
	f2.y = para_field[flag].carWorldY + para_field[flag].car_length + rearYfuse;

	f3.x = para_field[flag].carWorldX - rearXfuse;
	f3.y = worldHeight;

	theta = atan((float)((f3.y - f1.y)) / (f1.x - f3.x)) - atan((float)((f2.y - f1.y)) / (f1.x - f2.x));

	printf("BL theta = %f\n", theta);

	for (i = 0; i <= 1; i += fp)
	{
		switch (directionFlag)
		{
		case 0:
			for (j = 0; j <= 1; j += fp)
			{
				pxt = j * para_field[flag].carWorldX;
				pyt = para_field[flag].carWorldY + para_field[flag].car_length + i * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].car_length;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w0 = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_BL_B.push_back(pt3d_w0);
				vertexCoords2D[flag].glVertex_BL.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w1 = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_BL_L.push_back(pt3d_w1);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_BL.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_BL.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					weight = theta1 / theta;
					blendAlpha2D[flag].glAlpha_BL.push_back(1.0 - theta1 / theta);

					if ((pxt > para_field[flag].carWorldX / 2) && (pyt < para_field[flag].carWorldY + para_field[flag].car_length + para_field[flag].carWorldY2 / 2))
					{
						objPointsStatistics2D[flag].glObjPoints_BL_B.push_back(pt3d_w0);
						objPointsStatistics2D[flag].glObjPoints_BL_L.push_back(pt3d_w1);
					}
				}

				pxt = j * para_field[flag].carWorldX;
				pyt = para_field[flag].carWorldY + para_field[flag].car_length + (i + fp) * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].car_length;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_BL_B.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_BL.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_BL_L.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_BL.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_BL.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					weight = theta1 / theta;
					blendAlpha2D[flag].glAlpha_BL.push_back(1.0 - theta1 / theta);
				}
			}
			//objPoints2D[flag].glObjPoints_F.pop_back();
			//vertexCoords2D[flag].glVertex_F.pop_back();
			break;
		case 1:
			for (j = 1; j >= 0; j -= fp)
			{
				pxt = j * para_field[flag].carWorldX;
				pyt = para_field[flag].carWorldY + para_field[flag].car_length + i * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].car_length;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_BL_B.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_BL.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_BL_L.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_BL.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_BL.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					weight = theta1 / theta;
					blendAlpha2D[flag].glAlpha_BL.push_back(1.0 - theta1 / theta);
				}

				pxt = j * para_field[flag].carWorldX;
				pyt = para_field[flag].carWorldY + para_field[flag].car_length + (i + fp) * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].car_length;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_BL_B.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_BL.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX + para_field[flag].chessboard_width_corners * para_field[flag].square_size;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_BL_L.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_BL.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_BL.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					weight = theta1 / theta;
					blendAlpha2D[flag].glAlpha_BL.push_back(1.0 - theta1 / theta);
				}
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
	printf("VTX_NUM_BL: %lu %lu\n", objPoints2D[flag].glObjPoints_BL_B.size(), objPoints2D[flag].glObjPoints_BL_L.size());
	texCoords2D[flag].glTexCoord_BL_B.resize(objPoints2D[flag].glObjPoints_BL_B.size());
	texCoords2D[flag].glTexCoord_BL_L.resize(objPoints2D[flag].glObjPoints_BL_L.size());
	texCoordsStatistics2D[flag].glTexCoord_BL_B.resize(objPointsStatistics2D[flag].glObjPoints_BL_B.size());
	texCoordsStatistics2D[flag].glTexCoord_BL_L.resize(objPointsStatistics2D[flag].glObjPoints_BL_L.size());
#endif

#if 1
	f1.x = para_field[flag].carWorldX + para_field[flag].car_width;
	f1.y = para_field[flag].carWorldY + para_field[flag].car_length;

	f2.x = worldHeight;
	f2.y = para_field[flag].carWorldY + para_field[flag].car_length + rearYfuse;

	f3.x = para_field[flag].carWorldX + para_field[flag].car_width + rearXfuse;
	f3.y = worldHeight;

	theta = atan((float)((f3.y - f1.y)) / (f3.x - f1.x)) - atan((float)((f2.y - f1.y)) / (f2.x - f1.x));

	for (i = 0; i <= 1; i += fp)
	{
		switch (directionFlag)
		{
		case 0:
			for (j = 0; j <= 1; j += fp)
			{
				pxt = para_field[flag].carWorldX + para_field[flag].car_width + j * para_field[flag].carWorldX2;
				pyt = para_field[flag].carWorldY + para_field[flag].car_length + i * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].car_length;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w0 = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_BR_B.push_back(pt3d_w0);
				vertexCoords2D[flag].glVertex_BR.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX - para_field[flag].car_width;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w1 = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_BR_R.push_back(pt3d_w1);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_BR.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_BR.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_BR.push_back(1.0 - theta1 / theta);

					if ((pxt < para_field[flag].carWorldX + para_field[flag].car_width + para_field[flag].carWorldX2 / 2) && (pyt < para_field[flag].carWorldY + para_field[flag].car_length + para_field[flag].carWorldY2 / 2))
					{
						objPointsStatistics2D[flag].glObjPoints_BR_B.push_back(pt3d_w0);
						objPointsStatistics2D[flag].glObjPoints_BR_R.push_back(pt3d_w1);
					}
				}

				pxt = para_field[flag].carWorldX + para_field[flag].car_width + j * para_field[flag].carWorldX2;
				pyt = para_field[flag].carWorldY + para_field[flag].car_length + (i + fp) * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].car_length;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_BR_B.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_BR.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX - para_field[flag].car_width;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_BR_R.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_BR.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_BR.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_BR.push_back(1.0 - theta1 / theta);
				}
			}
			//objPoints2D[flag].glObjPoints_F.pop_back();
			//vertexCoords2D[flag].glVertex_F.pop_back();
			break;
		case 1:
			for (j = 1; j >= 0; j -= fp)
			{
				pxt = para_field[flag].carWorldX + para_field[flag].car_width + j * para_field[flag].carWorldX2;
				pyt = para_field[flag].carWorldY + para_field[flag].car_length + i * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].car_length;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_BR_B.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_BR.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX - para_field[flag].car_width;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_BR_R.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_BR.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_BR.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_BR.push_back(1.0 - theta1 / theta);
				}

				pxt = para_field[flag].carWorldX + para_field[flag].car_width + j * para_field[flag].carWorldX2;
				pyt = para_field[flag].carWorldY + para_field[flag].car_length + (i + fp) * para_field[flag].carWorldY2;

				pxw = pxt - halfWorldWidth + para_field[flag].chessboard_length_corners * para_field[flag].square_size / 2;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].car_length;
				pzw = 0;

				pxv = (pxt - halfWorldWidth) / halfWorldWidth;
				pyv = (halfWorldHeight - pyt) / halfWorldHeight;
				pzv = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);
				pt3d_v = cvPoint3D32f(pxv, pyv, pzv);

				objPoints2D[flag].glObjPoints_BR_B.push_back(pt3d_w);
				vertexCoords2D[flag].glVertex_BR.push_back(pt3d_v);

				pxw = pxt - para_field[flag].carWorldX - para_field[flag].car_width;
				pyw = pyt - para_field[flag].carWorldY - para_field[flag].LRchess2carFront_distance;
				pzw = 0;

				pt3d_w = cvPoint3D32f(pxw, pyw, pzw);

				objPoints2D[flag].glObjPoints_BR_R.push_back(pt3d_w);

				if (getDistance(f1.x, f1.y, f3.x, f3.y, pxt, pyt) < 0)
				{
					blendAlpha2D[flag].glAlpha_BR.push_back(0.0);
				}
				else if (getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt) > 0)
				{
					blendAlpha2D[flag].glAlpha_BR.push_back(1.0);
				}
				else
				{
					dist1 = fabs(getDistance(f1.x, f1.y, f2.x, f2.y, pxt, pyt));
					dist2 = getPixelDistance(f1.x, f1.y, pxt, pyt);

					theta1 = asin(dist1 / dist2);

					blendAlpha2D[flag].glAlpha_BR.push_back(1.0 - theta1 / theta);
				}
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
	printf("VTX_NUM_BR: %lu %lu\n", objPoints2D[flag].glObjPoints_BR_B.size(), objPoints2D[flag].glObjPoints_BR_R.size());
	texCoords2D[flag].glTexCoord_BR_B.resize(objPoints2D[flag].glObjPoints_BR_B.size());
	texCoords2D[flag].glTexCoord_BR_R.resize(objPoints2D[flag].glObjPoints_BR_R.size());
	texCoordsStatistics2D[flag].glTexCoord_BR_B.resize(objPointsStatistics2D[flag].glObjPoints_BR_B.size());
	texCoordsStatistics2D[flag].glTexCoord_BR_R.resize(objPointsStatistics2D[flag].glObjPoints_BR_R.size());
#endif

#if 1
	glVertices2DCar[flag][0] = 1.0 * (para_field[flag].carWorldX - halfWorldWidth) / halfWorldWidth - SHADOW_X_OFFSET;
	glVertices2DCar[flag][1] = 1.0 * (halfWorldHeight - para_field[flag].carWorldY - para_field[flag].car_length) / halfWorldHeight - SHADOW_Y_OFFSET;

	glVertices2DCar[flag][3] = 1.0 * (para_field[flag].carWorldX + para_field[flag].car_width - halfWorldWidth) / halfWorldWidth + SHADOW_X_OFFSET;
	glVertices2DCar[flag][4] = 1.0 * (halfWorldHeight - para_field[flag].carWorldY - para_field[flag].car_length) / halfWorldHeight - SHADOW_Y_OFFSET;

	glVertices2DCar[flag][6] = 1.0 * (para_field[flag].carWorldX - halfWorldWidth) / halfWorldWidth - SHADOW_X_OFFSET;
	glVertices2DCar[flag][7] = 1.0 * (halfWorldHeight - para_field[flag].carWorldY) / halfWorldHeight + SHADOW_Y_OFFSET;

	glVertices2DCar[flag][9] = 1.0 * (para_field[flag].carWorldX + para_field[flag].car_width - halfWorldWidth) / halfWorldWidth + SHADOW_X_OFFSET;
	glVertices2DCar[flag][10] = 1.0 * (halfWorldHeight - para_field[flag].carWorldY) / halfWorldHeight + SHADOW_Y_OFFSET;
#endif
}

void projectPoints(const CvMat *obj_points,
				   const CvMat *r_vec,
				   const CvMat *t_vec,
				   const CvMat *A,
				   const CvMat *dist_coeffs,
				   CvMat *img_points)
{
	float a[9], t[3], R[9], k[4];
	float fx, fy, cx, cy;
	CvPoint3D32f *M;
	CvPoint2D32f *m;
	int i, j, count;
	CvPoint2D32f imgPoints;
	CvMat _rr;

	M = (CvPoint3D32f *)(obj_points->data.ptr);
	m = (CvPoint2D32f *)(img_points->data.ptr);
	count = MAX(obj_points->rows, obj_points->cols);

	//printf(" %s *************************************************\n", __FUNCTION__);
	//printf("OBJ POINTS counts [%d] \n", count);

	_rr = cvMat(3, 3, CV_32F, R);

	cvRodrigues2(r_vec, &_rr, NULL);

	for (i = 0; i < 3; i++)
	{
		t[i] = cvmGet(t_vec, 0, i);

		for (j = 0; j < 3; j++)
		{
			a[i * 3 + j] = cvmGet(A, i, j);
			//R[i*3 + j] = (float)cvmGet(r_vec,i,j);
		}
	}

	for (i = 0; i < 4; i++)
	{
		if (!dist_coeffs)
		{
			k[i] = 0;
		}
		else
		{
			k[i] = cvmGet(dist_coeffs, 0, i);
		}
	}

	fx = 2 * a[0];
	fy = 2 * a[4];
	cx = 2 * a[2];
	cy = 2 * a[5];

	//printf("fx fy cx cy [%f %f %f %f]\n", a[0], a[4], a[2], a[5]);
	//printf("k [%f %f %f %f]\n", k[0], k[1], k[2], k[3]);
	//printf("rotate [%f %f %f]\n", cvmGet(r_vec, 0, 0),
	//cvmGet(r_vec, 0, 1), cvmGet(r_vec, 0, 2));
	//printf("trans  [%f %f %f]\n", t[0], t[1], t[2]);

	for (i = 0; i < count; i++)
	{
		double X = M[i].x;
		double Y = M[i].y;
		double Z = M[i].z;
		double x = R[0] * X + R[1] * Y + R[2] * Z + t[0];
		double y = R[3] * X + R[4] * Y + R[5] * Z + t[1];
		double z = R[6] * X + R[7] * Y + R[8] * Z + t[2];
		double r, r2, xd, yd, theta, theta2, theta4, theta6, theta8, theta_d;

		if (z < 0)
		{
			imgPoints.x = 0.0001;
			imgPoints.y = 0.0001;
		}
		else
		{
			if (!dist_coeffs)
			{
				xd = x;
				yd = y;
			}
			else
			{
				z = z ? 1 / z : 1;
				x = x * z;
				y = y * z;
				r2 = x * x + y * y;
				r = sqrt(r2);
				theta = atan(r);
				theta2 = theta * theta;
				theta4 = theta2 * theta2;
				theta6 = theta4 * theta2;
				theta8 = theta6 * theta2;

				theta_d = theta * (1 + k[0] * theta2 + k[1] * theta4 + k[2] * theta6 + k[3] * theta8);

				if (r < 0.00001f)
				{
					xd = 0;
					yd = 0;
				}
				else
				{
					xd = (x * theta_d) / r;
					yd = (y * theta_d) / r;
				}
			}
			imgPoints.x = fx * xd + cx;
			imgPoints.y = fy * yd + cy;
#if 0
            if (imgPoints.x < 2)
            {
                imgPoints.x = 0.0001;
                imgPoints.y = 0.0001;
                //printf("1 %f\n",imgPoints.x);
            }
            else if (imgPoints.x > IMGWIDTH - 4)
            {
                imgPoints.x = 0.0001;//img_width - 4;
                imgPoints.y = 0.0001;
                //printf("2 %f\n",imgPoints.x);
            }

            if (imgPoints.y < 2)
            {
                imgPoints.x = 0.0001;
                imgPoints.y = 0.0001;
                //printf("3 %f\n",imgPoints.y);
            }
            else if (imgPoints.y > IMGHEIGHT - 4)
            {
                imgPoints.x = 0.0001;
                imgPoints.y = 0.0001;//img_height - 4;
                //printf("4 %f\n",imgPoints.y);
            }
#endif

			if (imgPoints.x < 0 || imgPoints.x > IMGWIDTH || imgPoints.y < 0 || imgPoints.y > IMGHEIGHT)
			{
				imgPoints.x = 0.0001;
				imgPoints.y = 0.0001;
			}
			else
			{
				m[i].x = imgPoints.x / IMGWIDTH;
				m[i].y = imgPoints.y / IMGHEIGHT;
			}
		}
	}
}

void projectPoints1(const CvMat *obj_points,
					const CvMat *r_vec,
					const CvMat *t_vec,
					const CvMat *A,
					const CvMat *dist_coeffs,
					CvMat *img_points)
{
	float a[9], t[3], R[9], k[4];
	float fx, fy, cx, cy;
	CvPoint3D32f *M;
	int *m;
	int i, j, count;
	CvPoint2D32f imgPoints;
	CvMat _rr;

	M = (CvPoint3D32f *)(obj_points->data.ptr);
	m = (int *)(img_points->data.ptr);
	count = MAX(obj_points->rows, obj_points->cols);

	//printf(" %s *************************************************\n", __FUNCTION__);
	//printf("OBJ POINTS counts [%d] \n", count);

	_rr = cvMat(3, 3, CV_32F, R);

	cvRodrigues2(r_vec, &_rr, NULL);

	for (i = 0; i < 3; i++)
	{
		t[i] = cvmGet(t_vec, 0, i);

		for (j = 0; j < 3; j++)
		{
			a[i * 3 + j] = cvmGet(A, i, j);
			//R[i*3 + j] = (float)cvmGet(r_vec,i,j);
		}
	}

	for (i = 0; i < 4; i++)
	{
		if (!dist_coeffs)
		{
			k[i] = 0;
		}
		else
		{
			k[i] = cvmGet(dist_coeffs, 0, i);
		}
	}

	fx = 2 * a[0];
	fy = 2 * a[4];
	cx = 2 * a[2];
	cy = 2 * a[5];

	//printf("fx fy cx cy [%f %f %f %f]\n", a[0], a[4], a[2], a[5]);
	//printf("k [%f %f %f %f]\n", k[0], k[1], k[2], k[3]);
	//printf("rotate [%f %f %f]\n", cvmGet(r_vec, 0, 0),
	//cvmGet(r_vec, 0, 1), cvmGet(r_vec, 0, 2));
	//printf("trans  [%f %f %f]\n", t[0], t[1], t[2]);

	for (i = 0; i < count; i++)
	{
		double X = M[i].x;
		double Y = M[i].y;
		double Z = M[i].z;
		double x = R[0] * X + R[1] * Y + R[2] * Z + t[0];
		double y = R[3] * X + R[4] * Y + R[5] * Z + t[1];
		double z = R[6] * X + R[7] * Y + R[8] * Z + t[2];
		double r, r2, xd, yd, theta, theta2, theta4, theta6, theta8, theta_d;

		if (!dist_coeffs)
		{
			xd = x;
			yd = y;
		}
		else
		{
			z = z ? 1 / z : 1;
			x = x * z;
			y = y * z;
			r2 = x * x + y * y;
			r = sqrt(r2);
			theta = atan(r);
			theta2 = theta * theta;
			theta4 = theta2 * theta2;
			theta6 = theta4 * theta2;
			theta8 = theta6 * theta2;

			theta_d = theta * (1 + k[0] * theta2 + k[1] * theta4 + k[2] * theta6 + k[3] * theta8);

			if (r < 0.00001f)
			{
				xd = 0;
				yd = 0;
			}
			else
			{
				xd = (x * theta_d) / r;
				yd = (y * theta_d) / r;
			}
		}
		imgPoints.x = fx * xd + cx;
		imgPoints.y = fy * yd + cy;

		m[i] = cvRound(imgPoints.y) * IMGWIDTH * 2 + cvRound(imgPoints.x) * 2;

		//printf("%d\n",m[i]);
	}
}

CvPoint2D32f projectPoints3(const CvPoint3D32f obj_points,
							const float *r_vec,
							const float *t_vec)
{
	CvPoint2D32f imgPoints;
	double X, Y, Z, x, y, z;

	X = obj_points.x;
	Y = obj_points.y;
	Z = obj_points.z;
	x = r_vec[0] * X + r_vec[1] * Y + r_vec[2] * Z + t_vec[0];
	y = r_vec[3] * X + r_vec[4] * Y + r_vec[5] * Z + t_vec[1];
	z = r_vec[6] * X + r_vec[7] * Y + r_vec[8] * Z + t_vec[2];

	z = z ? 1 / z : 1;
	imgPoints.x = x * z;
	imgPoints.y = y * z;

	return imgPoints;
}

CvPoint2D32f function(float a1, float b1, float c1, float a2, float b2, float c2)
{
	float a3, b3, c3;
	float a4, b4, c4;
	CvPoint2D32f res;

	a3 = a1 * a2;
	a4 = a2 * a1;
	b3 = b1 * a2;
	b4 = b2 * a1;
	c3 = c1 * a2;
	c4 = c2 * a1;

	res.y = (c3 - c4) / (b3 - b4);

	a3 = a1 * b2;
	a4 = a2 * b1;
	b3 = b1 * b2;
	b4 = b2 * b1;
	c3 = c1 * b2;
	c4 = c2 * b1;

	res.x = (c3 - c4) / (a3 - a4);

	return res;
}

CvPoint2D32f function1(CvPoint2D32f dist, const CvMat *A, undistortParams params, double *invR)
{
	float a[9];
	float fx, fy, cx, cy;
	int i, j, count;
	CvPoint2D32f imgPoints;
	float angle = 0;
	float xr, yr;
	float a1, b1, c1, a2, b2, c2;
	CvPoint2D32f len, point;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			a[i * 3 + j] = cvmGet(A, i, j);
		}
	}

	fx = 2 * a[0];
	fy = 2 * a[4];
	cx = 2 * a[2];
	cy = 2 * a[5];

	//xr = (sin(angle) + dist.x * cos(angle)) / (cos(angle) - dist.x * sin(angle));
	//yr = dist.y * (xr * sin(angle) + cos(angle));

	a1 = dist.x * invR[6] - invR[0];
	b1 = dist.x * invR[7] - invR[1];
	c1 = invR[2] - dist.x * invR[8];

	a2 = dist.y * invR[6] - invR[3];
	b2 = dist.y * invR[7] - invR[4];
	c2 = invR[5] - dist.y * invR[8];

	point = function(a1, b1, c1, a2, b2, c2);

	imgPoints.x = (cx + point.x * fx - params.x) / params.xZoom;
	imgPoints.y = (cy + point.y * fy - params.y) / params.yZoom;

	return imgPoints;
}

CvPoint2D32f function3(CvPoint2D32f dist, const float *intrinsic_matrix, float *distortTable, undistortParams params, double *invR)
{
	float a[9], k[4];
	float fx, fy, cx, cy;
	int i, j, count;
	CvPoint2D32f imgPoints, point;
	//float angle = params.angle + degree * RADIAN;
	//float xr, yr;
	float a1, b1, c1, a2, b2, c2;
	double r, r2, r4, r6, xd, yd, x, y;
	double distort, undistort;
	float k1, k2, p1, p2, k3;

	cx = intrinsic_matrix[0];
	cy = intrinsic_matrix[1];
	fx = intrinsic_matrix[2];
	fy = intrinsic_matrix[3];

	k1 = distortTable[0];
	k2 = distortTable[1];
	p1 = distortTable[2];
	p2 = distortTable[3];
	k3 = distortTable[4];

	a1 = invR[0] - dist.x * invR[6];
	b1 = invR[1] - dist.x * invR[7];
	c1 = dist.x * invR[8] - invR[2];
	a2 = invR[3] - dist.y * invR[6];
	b2 = invR[4] - dist.y * invR[7];
	c2 = dist.y * invR[8] - invR[5];

	point = function(a1, b1, c1, a2, b2, c2);
	x = point.x;
	y = point.y;
	r2 = x * x + y * y;
	r4 = r2 * r2;
	r6 = r4 * r2;

	//xd = x * (1 + k1 * r2 + k2 * r4 + k3 * r6);// + 2 * p1 * x * y + p2 * (r2 + 2 * x2);
	//yd = y * (1 + k1 * r2 + k2 * r4 + k3 * r6);// + 2 * p2 * x * y + p1 * (r2 + 2 * y2);
	xd = x * (1 + k1 * r2 + k2 * r4); // + 2 * p1 * x * y + p2 * (r2 + 2 * x2);
	yd = y * (1 + k1 * r2 + k2 * r4); // + 2 * p2 * x * y + p1 * (r2 + 2 * y2);
									  //xd = x * (1 + k1 * r2 + k2 * r4) + 2 * p1 * x * y + p2 * (r2 + 2 * x * x);
									  //yd = y * (1 + k1 * r2 + k2 * r4) + 2 * p2 * x * y + p1 * (r2 + 2 * y * y);

	/*$)A5C5=OqKXWx1j*/
	imgPoints.x = (cx + x * fx - params.x) / params.xZoom;
	imgPoints.y = (cy + y * fy - params.y) / params.yZoom;

	return imgPoints;
}

void generateTriangleStrip(std::vector<CvPoint2D32f> &triagnle, int width, int height)
{
	float p = 16.0;
	float i, j, fp;
	unsigned char directionFlag;
	CvPoint2D32f point;
	fp = 1.0 / p;

	directionFlag = 0;

	for (i = 0; i < 1; i += fp)
	{
		switch (directionFlag)
		{
		case 0:
			for (j = 0; j <= 1; j += fp)
			{
				point.x = j * width;
				point.y = i * height;

				triagnle.push_back(point);

				point.x = j * width;
				point.y = (i + fp) * height;

				triagnle.push_back(point);
			}
			//objPoints2D.glObjPoints_F.pop_back();
			//vertexCoords2D.glVertex_F.pop_back();
			break;
		case 1:
			for (j = 1; j >= 0; j -= fp)
			{
				point.x = j * width;
				point.y = i * height;

				triagnle.push_back(point);

				point.x = j * width;
				point.y = (i + fp) * height;

				triagnle.push_back(point);
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
}

#if 1
double towFac(double *b)
{
	return b[0] * b[3] - b[1] * b[2];
}

void copy3To2(double *a, double *b, int i, int j)
{
	int m = 0, n = 0;
	int count = 0;
	for (m = 0; m < 3; m++)
	{
		for (n = 0; n < 3; n++)
		{
			if (m != i && n != j)
			{
				count++;
				b[((count - 1) / 2) * 2 + (count - 1) % 2] = a[m * 3 + n];
			}
		}
	}
}

double threeSum(double *a, double *b)
{
	double sum = 0;
	for (int i = 0; i < 3; i++)
	{
		copy3To2(a, b, 0, i);
		if (i % 2 == 0)
		{
			sum += a[i] * towFac(b);
		}
		else
		{
			sum -= a[i] * towFac(b);
		}
	}
	return sum;
}

void calCArray(double *a, double *b, double *c)
{
	int i = 0;
	int j = 0;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			copy3To2(a, b, i, j);
			if ((i + j) % 2 == 0)
			{
				c[j * 3 + i] = towFac(b);
			}
			else
			{
				c[j * 3 + i] = -towFac(b);
			}
		}
	}
}

void niArray(double *c, double A)
{
	int i = 0;
	int j = 0;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			c[i * 3 + j] /= A;
		}
	}
}

void getInvertMatrix(double *src, double *dst)
{
	int i, n = 0;
	long sum = 0;

	//printf("$)AT-@4>XUsHgOB#:\n");
	//printArray3(src, 3, 3);
	double b[2][2];
	double t = threeSum(src, &b[0][0]); //$)AGs|A|
	//printf("|A|=%lf\n", t);
	calCArray(src, &b[0][0], dst); //$)AGsA*
	niArray(dst, t);			   //$)AGsDf>XUs
}
#endif

void rotationMatrixToEulerAngles(CvMat *rVec, float *alpha, float *beta, float *gamma)
{
	float a[9], t[3], R[9], k[4];
	float fx, fy, cx, cy;
	CvPoint3D32f *M;
	CvPoint2D32f *m;
	int i, j, count;
	CvPoint2D32f imgPoints;
	CvMat _rr;

	_rr = cvMat(3, 3, CV_32F, R);

	cvRodrigues2(rVec, &_rr, NULL);
	//cvTranspose(&_rr, &_rr);

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
	//printf("%f %f %f\n", x, y, z);
	//return Vec3f(x, y, z);
}

void matrixMul(double *srcA, double *srcB, double *dst)
{
	dst[0] = srcA[0] * srcB[0] + srcA[1] * srcB[3] + srcA[2] * srcB[6];
	dst[1] = srcA[0] * srcB[1] + srcA[1] * srcB[4] + srcA[2] * srcB[7];
	dst[2] = srcA[0] * srcB[2] + srcA[1] * srcB[5] + srcA[2] * srcB[8];

	dst[3] = srcA[3] * srcB[0] + srcA[4] * srcB[3] + srcA[5] * srcB[6];
	dst[4] = srcA[3] * srcB[1] + srcA[4] * srcB[4] + srcA[5] * srcB[7];
	dst[5] = srcA[3] * srcB[2] + srcA[4] * srcB[5] + srcA[5] * srcB[8];

	dst[6] = srcA[6] * srcB[0] + srcA[7] * srcB[3] + srcA[8] * srcB[6];
	dst[7] = srcA[6] * srcB[1] + srcA[7] * srcB[4] + srcA[8] * srcB[7];
	dst[8] = srcA[6] * srcB[2] + srcA[7] * srcB[5] + srcA[8] * srcB[8];
}

void initTextureCoords(int flag)
{
	CvMat obj_points, img_points;
	int i;

	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_F.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_F[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_F.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_F[0]);
	projectPoints(&obj_points, SimplifyfrontCamParams[flag].r_vec, SimplifyfrontCamParams[flag].t_vec, SimplifyfrontCamParams[flag].camera, SimplifyfrontCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_F.clear();

	// front left blend front part
	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_FL_F.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_FL_F[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_FL_F.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_FL_F[0]);
	projectPoints(&obj_points, SimplifyfrontCamParams[flag].r_vec, SimplifyfrontCamParams[flag].t_vec, SimplifyfrontCamParams[flag].camera, SimplifyfrontCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_FL_F.clear();

	// front right blend front part
	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_FR_F.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_FR_F[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_FR_F.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_FR_F[0]);
	projectPoints(&obj_points, SimplifyfrontCamParams[flag].r_vec, SimplifyfrontCamParams[flag].t_vec, SimplifyfrontCamParams[flag].camera, SimplifyfrontCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_FR_F.clear();

	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_B.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_B[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_B.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_B[0]);
	projectPoints(&obj_points, SimplifyrearCamParams[flag].r_vec, SimplifyrearCamParams[flag].t_vec, SimplifyrearCamParams[flag].camera, SimplifyrearCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_B.clear();

	// rear left blend rear part
	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_BL_B.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_BL_B[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_BL_B.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_BL_B[0]);
	projectPoints(&obj_points, SimplifyrearCamParams[flag].r_vec, SimplifyrearCamParams[flag].t_vec, SimplifyrearCamParams[flag].camera, SimplifyrearCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_BL_B.clear();

	// rear right blend rear part
	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_BR_B.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_BR_B[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_BR_B.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_BR_B[0]);
	projectPoints(&obj_points, SimplifyrearCamParams[flag].r_vec, SimplifyrearCamParams[flag].t_vec, SimplifyrearCamParams[flag].camera, SimplifyrearCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_BR_B.clear();

	//left cam
	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_L.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_L[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_L.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_L[0]);
	projectPoints(&obj_points, SimplifyleftCamParams[flag].r_vec, SimplifyleftCamParams[flag].t_vec, SimplifyleftCamParams[flag].camera, SimplifyleftCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_L.clear();

	//left front blend left part
	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_FL_L.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_FL_L[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_FL_L.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_FL_L[0]);
	projectPoints(&obj_points, SimplifyleftCamParams[flag].r_vec, SimplifyleftCamParams[flag].t_vec, SimplifyleftCamParams[flag].camera, SimplifyleftCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_FL_L.clear();

	//left rear blend left part
	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_BL_L.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_BL_L[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_BL_L.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_BL_L[0]);
	projectPoints(&obj_points, SimplifyleftCamParams[flag].r_vec, SimplifyleftCamParams[flag].t_vec, SimplifyleftCamParams[flag].camera, SimplifyleftCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_BL_L.clear();

	//right cam
	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_R.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_R[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_R.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_R[0]);
	projectPoints(&obj_points, SimplifyrightCamParams[flag].r_vec, SimplifyrightCamParams[flag].t_vec, SimplifyrightCamParams[flag].camera, SimplifyrightCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_R.clear();

	//right front blend right part
	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_FR_R.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_FR_R[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_FR_R.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_FR_R[0]);
	projectPoints(&obj_points, SimplifyrightCamParams[flag].r_vec, SimplifyrightCamParams[flag].t_vec, SimplifyrightCamParams[flag].camera, SimplifyrightCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_FR_R.clear();

	//right rear blend right part
	obj_points = cvMat(1, objPoints2D[flag].glObjPoints_BR_R.size(), CV_32FC3, &objPoints2D[flag].glObjPoints_BR_R[0]);
	img_points = cvMat(1, texCoords2D[flag].glTexCoord_BR_R.size(), CV_32FC2, &texCoords2D[flag].glTexCoord_BR_R[0]);
	projectPoints(&obj_points, SimplifyrightCamParams[flag].r_vec, SimplifyrightCamParams[flag].t_vec, SimplifyrightCamParams[flag].camera, SimplifyrightCamParams[flag].dist_coeffs, &img_points);
	objPoints2D[flag].glObjPoints_BR_R.clear();
}

void getCamPixelPosition(int flag)
{
	CvMat obj_points, img_points;
	int i;

	// front left blend front part
	obj_points = cvMat(1, objPointsStatistics2D[flag].glObjPoints_FL_F.size(), CV_32FC3, &objPointsStatistics2D[flag].glObjPoints_FL_F[0]);
	img_points = cvMat(1, texCoordsStatistics2D[flag].glTexCoord_FL_F.size(), CV_32SC1, &texCoordsStatistics2D[flag].glTexCoord_FL_F[0]);
	projectPoints1(&obj_points, SimplifyfrontCamParams[flag].r_vec, SimplifyfrontCamParams[flag].t_vec, SimplifyfrontCamParams[flag].camera, SimplifyfrontCamParams[flag].dist_coeffs, &img_points);
	objPointsStatistics2D[flag].glObjPoints_FL_F.clear();

	printf("FL_F = %d\n", texCoordsStatistics2D[flag].glTexCoord_FL_F.size());

	// front right blend front part
	obj_points = cvMat(1, objPointsStatistics2D[flag].glObjPoints_FR_F.size(), CV_32FC3, &objPointsStatistics2D[flag].glObjPoints_FR_F[0]);
	img_points = cvMat(1, texCoordsStatistics2D[flag].glTexCoord_FR_F.size(), CV_32SC1, &texCoordsStatistics2D[flag].glTexCoord_FR_F[0]);
	projectPoints1(&obj_points, SimplifyfrontCamParams[flag].r_vec, SimplifyfrontCamParams[flag].t_vec, SimplifyfrontCamParams[flag].camera, SimplifyfrontCamParams[flag].dist_coeffs, &img_points);
	objPointsStatistics2D[flag].glObjPoints_FR_F.clear();

	// front left blend front part
	obj_points = cvMat(1, objPointsStatistics2D[flag].glObjPoints_BL_B.size(), CV_32FC3, &objPointsStatistics2D[flag].glObjPoints_BL_B[0]);
	img_points = cvMat(1, texCoordsStatistics2D[flag].glTexCoord_BL_B.size(), CV_32SC1, &texCoordsStatistics2D[flag].glTexCoord_BL_B[0]);
	projectPoints1(&obj_points, SimplifyrearCamParams[flag].r_vec, SimplifyrearCamParams[flag].t_vec, SimplifyrearCamParams[flag].camera, SimplifyrearCamParams[flag].dist_coeffs, &img_points);
	objPointsStatistics2D[flag].glObjPoints_BL_B.clear();

	// front right blend front part
	obj_points = cvMat(1, objPointsStatistics2D[flag].glObjPoints_BR_B.size(), CV_32FC3, &objPointsStatistics2D[flag].glObjPoints_BR_B[0]);
	img_points = cvMat(1, texCoordsStatistics2D[flag].glTexCoord_BR_B.size(), CV_32SC1, &texCoordsStatistics2D[flag].glTexCoord_BR_B[0]);
	projectPoints1(&obj_points, SimplifyrearCamParams[flag].r_vec, SimplifyrearCamParams[flag].t_vec, SimplifyrearCamParams[flag].camera, SimplifyrearCamParams[flag].dist_coeffs, &img_points);
	objPointsStatistics2D[flag].glObjPoints_BR_B.clear();

	// front left blend front part
	obj_points = cvMat(1, objPointsStatistics2D[flag].glObjPoints_FL_L.size(), CV_32FC3, &objPointsStatistics2D[flag].glObjPoints_FL_L[0]);
	img_points = cvMat(1, texCoordsStatistics2D[flag].glTexCoord_FL_L.size(), CV_32SC1, &texCoordsStatistics2D[flag].glTexCoord_FL_L[0]);
	projectPoints1(&obj_points, SimplifyleftCamParams[flag].r_vec, SimplifyleftCamParams[flag].t_vec, SimplifyleftCamParams[flag].camera, SimplifyleftCamParams[flag].dist_coeffs, &img_points);
	objPointsStatistics2D[flag].glObjPoints_FL_L.clear();

	// front right blend front part
	obj_points = cvMat(1, objPointsStatistics2D[flag].glObjPoints_BL_L.size(), CV_32FC3, &objPointsStatistics2D[flag].glObjPoints_BL_L[0]);
	img_points = cvMat(1, texCoordsStatistics2D[flag].glTexCoord_BL_L.size(), CV_32SC1, &texCoordsStatistics2D[flag].glTexCoord_BL_L[0]);
	projectPoints1(&obj_points, SimplifyleftCamParams[flag].r_vec, SimplifyleftCamParams[flag].t_vec, SimplifyleftCamParams[flag].camera, SimplifyleftCamParams[flag].dist_coeffs, &img_points);
	objPointsStatistics2D[flag].glObjPoints_BL_L.clear();

	// front left blend front part
	obj_points = cvMat(1, objPointsStatistics2D[flag].glObjPoints_FR_R.size(), CV_32FC3, &objPointsStatistics2D[flag].glObjPoints_FR_R[0]);
	img_points = cvMat(1, texCoordsStatistics2D[flag].glTexCoord_FR_R.size(), CV_32SC1, &texCoordsStatistics2D[flag].glTexCoord_FR_R[0]);
	projectPoints1(&obj_points, SimplifyrightCamParams[flag].r_vec, SimplifyrightCamParams[flag].t_vec, SimplifyrightCamParams[flag].camera, SimplifyrightCamParams[flag].dist_coeffs, &img_points);
	objPointsStatistics2D[flag].glObjPoints_FR_R.clear();

	// front right blend front part
	obj_points = cvMat(1, objPointsStatistics2D[flag].glObjPoints_BR_R.size(), CV_32FC3, &objPointsStatistics2D[flag].glObjPoints_BR_R[0]);
	img_points = cvMat(1, texCoordsStatistics2D[flag].glTexCoord_BR_R.size(), CV_32SC1, &texCoordsStatistics2D[flag].glTexCoord_BR_R[0]);
	projectPoints1(&obj_points, SimplifyrightCamParams[flag].r_vec, SimplifyrightCamParams[flag].t_vec, SimplifyrightCamParams[flag].camera, SimplifyrightCamParams[flag].dist_coeffs, &img_points);
	objPointsStatistics2D[flag].glObjPoints_BR_R.clear();
}

void findRearCurve(float wheelAngle, undistortParams params, float *camera, float *distortTable, float *rVec, float *tVec, double *invR, int width, int height)
{
	int wheel_base = 4675;
	int rear_wheel = 1375;
	float far_x;
	float ex_r, in_r, ex_r1, in_r1;
	float tmpx, tmpy;
	float angle;
	int i, k;
	float j;
	float px, py;
	CvPoint3D32f pw;
	CvPoint2D32f len, point;
	float lineWidth, halfLineWidth, otherHalfLineWidth;
	double ex_r_in, ex_r_out, in_r_in, in_r_out;
	double ex_r1_in, ex_r1_out, in_r1_in, in_r1_out;
	double baseAngle[3][4], deltaAngle[3][4], radius[4];
	float fp = 1.0 / 64;
	CvPoint3D32f worldPoints[10][LENGTH * 2];
	float startx, length;
	float startx0, length0;
	float maxAngle;
	float offsetY;
	int idx;
	int gap[3] = {7, 5, 3};
	int offset[2] = {1, 2};
	//float distance[4] = {800, 2140, 4124, 6000};
	float distance[4] = {0, 500, 1500, 4000};
	int car_width = 2556;
	float scale;

	lineWidth = 30;
	halfLineWidth = lineWidth / 2;
	otherHalfLineWidth = lineWidth - halfLineWidth;
	offsetY = 0; //1310;//+800;

	startx = -car_width / 2;
	length = car_width;

	/*$)AW*;;3I;!6HVF*/
	if (wheelAngle >= 0 && wheelAngle < 0.001)
	{
		wheelAngle = 0.001;
	}
	else if (wheelAngle < 0 && wheelAngle > -0.001)
	{
		wheelAngle = -0.001;
	}
	angle = (90 - wheelAngle) * RADIAN;

	ex_r = fabs((float)wheel_base * tan(angle)) + car_width / 2;
	in_r = ex_r - car_width;

	in_r1 = sqrt(in_r * in_r + rear_wheel * rear_wheel);
	ex_r1 = sqrt(ex_r * ex_r + rear_wheel * rear_wheel);

	in_r1_in = in_r1 - halfLineWidth;
	in_r1_out = in_r1 + otherHalfLineWidth;
	ex_r1_in = ex_r1 - halfLineWidth;
	ex_r1_out = ex_r1 + otherHalfLineWidth;

	radius[0] = in_r1_in;
	radius[1] = in_r1_out;
	radius[2] = ex_r1_in;
	radius[3] = ex_r1_out;

	/*for(idx=0; idx<3; idx++)
	{
		for(k=0; k<4; k++)
		{
			baseAngle[idx][k] = asin((rear_wheel + distance[idx]) / radius[k]);

			if(radius[k] > rear_wheel + distance[idx+1])
			{
				deltaAngle[idx][k] = asin((rear_wheel + distance[idx+1]) / radius[k]) - baseAngle[idx][k];
			}
			else
			{
				deltaAngle[idx][k] = PI / 2 - baseAngle[idx][k];
			}

			//PRINTF("angle0 %f %f\n",baseAngle[idx][k],deltaAngle[idx][k]);
		}
	}*/

	//ale = (1.0 - 0.6 * fabs(wheelAngle)/MAX_WHEEL_ANGLE);
	scale = (1.0 - 0.2 * fabs(wheelAngle) / MAX_WHEEL_ANGLE);

	for (idx = 0; idx < 3; idx++)
	{
		for (k = 0; k < 4; k++)
		{
			if (k < 2)
			{
				baseAngle[idx][k] = asin((rear_wheel + distance[idx] * scale) / radius[k]);

				if (radius[k] > rear_wheel + distance[idx + 1] * scale)
				{
					deltaAngle[idx][k] = asin((rear_wheel + distance[idx + 1] * scale) / radius[k]) - baseAngle[idx][k];
				}
				else
				{
					deltaAngle[idx][k] = PI / 2 - baseAngle[idx][k];
				}
			}
			else
			{
				baseAngle[idx][k] = asin((rear_wheel + distance[idx]) / radius[k]);

				if (radius[k] > rear_wheel + distance[idx + 1])
				{
					deltaAngle[idx][k] = asin((rear_wheel + distance[idx + 1]) / radius[k]) - baseAngle[idx][k];
				}
				else
				{
					deltaAngle[idx][k] = PI / 2 - baseAngle[idx][k];
				}
			}
		}
	}

	if (wheelAngle < 0)
	{
		for (idx = 0; idx < 3; idx++)
		{
			for (j = 0, k = 0; j <= 1; j += fp, k++)
			{
				for (i = 0; i < 2; i++)
				{
					tmpx = car_width / 2 - ex_r + radius[i * 2 + 0] * cos(baseAngle[idx][i * 2 + 0] + deltaAngle[idx][i * 2 + 0] * j);
					tmpy = radius[i * 2 + 0] * sin(baseAngle[idx][i * 2 + 0] + deltaAngle[idx][i * 2 + 0] * j) - rear_wheel;

					worldPoints[idx * 2 + i][k * 2 + 0] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);

					tmpx = car_width / 2 - ex_r + radius[i * 2 + 1] * cos(baseAngle[idx][i * 2 + 1] + deltaAngle[idx][i * 2 + 1] * j);
					tmpy = radius[i * 2 + 1] * sin(baseAngle[idx][i * 2 + 1] + deltaAngle[idx][i * 2 + 1] * j) - rear_wheel;

					worldPoints[idx * 2 + i][k * 2 + 1] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);
				}
			}
		}

		for (idx = 0; idx < 3; idx++)
		{
			tmpx = car_width / 2 - ex_r + radius[1] * cos(baseAngle[idx][1] + deltaAngle[idx][1] * 1.0);
			tmpy = radius[1] * sin(baseAngle[idx][1] + deltaAngle[idx][1] * 1.0) - rear_wheel;

			worldPoints[6][idx * 4 + 0] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);

			tmpx = car_width / 2 - ex_r + radius[1] * cos(baseAngle[idx][1] + deltaAngle[idx][1] * (1.0 - gap[idx] * fp));
			tmpy = radius[1] * sin(baseAngle[idx][1] + deltaAngle[idx][1] * (1.0 - gap[idx] * fp)) - rear_wheel;

			worldPoints[6][idx * 4 + 1] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);

			tmpx = car_width / 2 - ex_r + radius[2] * cos(baseAngle[idx][2] + deltaAngle[idx][2] * 1.0);
			tmpy = radius[2] * sin(baseAngle[idx][2] + deltaAngle[idx][2] * 1.0) - rear_wheel;

			worldPoints[6][idx * 4 + 2] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);

			tmpx = car_width / 2 - ex_r + radius[2] * cos(baseAngle[idx][2] + deltaAngle[idx][2] * (1.0 - gap[idx] * fp));
			tmpy = radius[2] * sin(baseAngle[idx][2] + deltaAngle[idx][2] * (1.0 - gap[idx] * fp)) - rear_wheel;

			worldPoints[6][idx * 4 + 3] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);
		}
	}
	else
	{
		for (idx = 0; idx < 3; idx++)
		{
			for (j = 0, k = 0; j <= 1; j += fp, k++)
			{
				for (i = 0; i < 2; i++)
				{
					tmpx = car_width / 2 + in_r - radius[i * 2 + 0] * cos(baseAngle[idx][i * 2 + 0] + deltaAngle[idx][i * 2 + 0] * j);
					tmpy = radius[i * 2 + 0] * sin(baseAngle[idx][i * 2 + 0] + deltaAngle[idx][i * 2 + 0] * j) - rear_wheel;

					worldPoints[idx * 2 + i][k * 2 + 0] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);

					tmpx = car_width / 2 + in_r - radius[i * 2 + 1] * cos(baseAngle[idx][i * 2 + 1] + deltaAngle[idx][i * 2 + 1] * j);
					tmpy = radius[i * 2 + 1] * sin(baseAngle[idx][i * 2 + 1] + deltaAngle[idx][i * 2 + 1] * j) - rear_wheel;

					worldPoints[idx * 2 + i][k * 2 + 1] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);
				}
			}
		}

		for (idx = 0; idx < 3; idx++)
		{
			tmpx = car_width / 2 + in_r - radius[1] * cos(baseAngle[idx][1] + deltaAngle[idx][1] * 1.0);
			tmpy = radius[1] * sin(baseAngle[idx][1] + deltaAngle[idx][1] * 1.0) - rear_wheel;

			worldPoints[6][idx * 4 + 0] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);

			tmpx = car_width / 2 + in_r - radius[1] * cos(baseAngle[idx][1] + deltaAngle[idx][1] * (1.0 - gap[idx] * fp));
			tmpy = radius[1] * sin(baseAngle[idx][1] + deltaAngle[idx][1] * (1.0 - gap[idx] * fp)) - rear_wheel;

			worldPoints[6][idx * 4 + 1] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);

			tmpx = car_width / 2 + in_r - radius[2] * cos(baseAngle[idx][2] + deltaAngle[idx][2] * 1.0);
			tmpy = radius[2] * sin(baseAngle[idx][2] + deltaAngle[idx][2] * 1.0) - rear_wheel;

			worldPoints[6][idx * 4 + 2] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);

			tmpx = car_width / 2 + in_r - radius[2] * cos(baseAngle[idx][2] + deltaAngle[idx][2] * (1.0 - gap[idx] * fp));
			tmpy = radius[2] * sin(baseAngle[idx][2] + deltaAngle[idx][2] * (1.0 - gap[idx] * fp)) - rear_wheel;

			worldPoints[6][idx * 4 + 3] = cvPoint3D32f(tmpx, tmpy + offsetY, 0.0);
		}
	}

	for (j = 0, k = 0; j <= 1; j += fp)
	{
		tmpx = startx + (length)*j;
		tmpy = 0 - 5;

		worldPoints[7][k * 2 + 0] = cvPoint3D32f(tmpx, tmpy, 0.0);

		tmpx = startx + (length)*j;
		tmpy = 0 + 5;

		worldPoints[7][k * 2 + 1] = cvPoint3D32f(tmpx, tmpy, 0.0);
		k++;
	}

	for (k = 0; k < 8; k++)
	{
		for (i = 0; i < LENGTH * 2; i++) /*$)A<FKcJ@=gWx1j5c*/
		{
			pw = worldPoints[k][i];
			//printf("%f %f\n",pw.x, pw.y);
			len = projectPoints3(pw, rVec, tVec);

			point = function3(len, camera, distortTable, params, invR);

			//printf("%d %d %f %f\n",k,i,point.x,point.y);

			verticesRearTrajLinePoint[k][i].x = -(1.0 - 2 * point.x / width);
			verticesRearTrajLinePoint[k][i].y = -(1.0 - 2 * point.y / height);
			verticesRearTrajLinePoint[k][i].z = 0.0;
		}
	}
}

void initVBO()
{
	int i;

	for (i = 0; i < 2; i++)
	{
		glGenBuffers(4, VBO2DMosaicImageParams[i].CamVerticesPoints);
		glGenBuffers(4, VBO2DMosaicImageParams[i].CamImagePoints);

		glGenBuffers(4, VBO2DMosaicImageParams[i].MosaicCamVerticesPoints);
		glGenBuffers(2, VBO2DMosaicImageParams[i].MosaicFLCamImagePoints);
		glGenBuffers(2, VBO2DMosaicImageParams[i].MosaicFRCamImagePoints);
		glGenBuffers(2, VBO2DMosaicImageParams[i].MosaicBLCamImagePoints);
		glGenBuffers(2, VBO2DMosaicImageParams[i].MosaicBRCamImagePoints);

		glGenBuffers(4, VBO2DMosaicImageParams[i].LumiaBalance);

		glGenBuffers(4, VBO2DMosaicImageParams[i].Alpha);

		glGenBuffers(2, VBO2DMosaicImageParams[i].CarVerTexCoord);
	}

#if 1
	glGenBuffers(2, curveVerticesPoints);
	glGenBuffers(2, cameraVerTexCoord);
#endif

#if 1
	for (i = 0; i < 2; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].CamVerticesPoints[0]);
		glBufferData(GL_ARRAY_BUFFER, texCoords2D[i].glTexCoord_F.size() * sizeof(CvPoint3D32f), &vertexCoords2D[i].glVertex_F[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].CamVerticesPoints[1]);
		glBufferData(GL_ARRAY_BUFFER, texCoords2D[i].glTexCoord_B.size() * sizeof(CvPoint3D32f), &vertexCoords2D[i].glVertex_B[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].CamVerticesPoints[2]);
		glBufferData(GL_ARRAY_BUFFER, texCoords2D[i].glTexCoord_L.size() * sizeof(CvPoint3D32f), &vertexCoords2D[i].glVertex_L[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].CamVerticesPoints[3]);
		glBufferData(GL_ARRAY_BUFFER, texCoords2D[i].glTexCoord_R.size() * sizeof(CvPoint3D32f), &vertexCoords2D[i].glVertex_R[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].CamImagePoints[0]);
		glBufferData(GL_ARRAY_BUFFER, texCoords2D[i].glTexCoord_F.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_F[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].CamImagePoints[1]);
		glBufferData(GL_ARRAY_BUFFER, texCoords2D[i].glTexCoord_B.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_B[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].CamImagePoints[2]);
		glBufferData(GL_ARRAY_BUFFER, texCoords2D[i].glTexCoord_L.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_L[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].CamImagePoints[3]);
		glBufferData(GL_ARRAY_BUFFER, texCoords2D[i].glTexCoord_R.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_R[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicCamVerticesPoints[0]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_FL.size() * sizeof(CvPoint3D32f), &vertexCoords2D[i].glVertex_FL[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicCamVerticesPoints[1]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_FR.size() * sizeof(CvPoint3D32f), &vertexCoords2D[i].glVertex_FR[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicCamVerticesPoints[2]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_BL.size() * sizeof(CvPoint3D32f), &vertexCoords2D[i].glVertex_BL[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicCamVerticesPoints[3]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_BR.size() * sizeof(CvPoint3D32f), &vertexCoords2D[i].glVertex_BR[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicFLCamImagePoints[0]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_FL.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_FL_F[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicFLCamImagePoints[1]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_FL.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_FL_L[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicFRCamImagePoints[0]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_FR.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_FR_F[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicFRCamImagePoints[1]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_FR.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_FR_R[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicBLCamImagePoints[0]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_BL.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_BL_B[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicBLCamImagePoints[1]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_BL.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_BL_L[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicBRCamImagePoints[0]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_BR.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_BR_B[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].MosaicBRCamImagePoints[1]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_BR.size() * sizeof(CvPoint2D32f), &texCoords2D[i].glTexCoord_BR_R[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].LumiaBalance[0]);
		glBufferData(GL_ARRAY_BUFFER, lumiaAdjust2D[i].glLumiaAdjust_F.size() * sizeof(float), &lumiaAdjust2D[i].glLumiaAdjust_F[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].LumiaBalance[1]);
		glBufferData(GL_ARRAY_BUFFER, lumiaAdjust2D[i].glLumiaAdjust_B.size() * sizeof(float), &lumiaAdjust2D[i].glLumiaAdjust_B[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].LumiaBalance[2]);
		glBufferData(GL_ARRAY_BUFFER, lumiaAdjust2D[i].glLumiaAdjust_L.size() * sizeof(float), &lumiaAdjust2D[i].glLumiaAdjust_L[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].LumiaBalance[3]);
		glBufferData(GL_ARRAY_BUFFER, lumiaAdjust2D[i].glLumiaAdjust_R.size() * sizeof(float), &lumiaAdjust2D[i].glLumiaAdjust_R[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].Alpha[0]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_FL.size() * sizeof(float), &blendAlpha2D[i].glAlpha_FL[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].Alpha[1]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_FR.size() * sizeof(float), &blendAlpha2D[i].glAlpha_FR[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].Alpha[2]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_BL.size() * sizeof(float), &blendAlpha2D[i].glAlpha_BL[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].Alpha[3]);
		glBufferData(GL_ARRAY_BUFFER, vertexCoords2D[i].glVertex_BR.size() * sizeof(float), &blendAlpha2D[i].glAlpha_BR[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].CarVerTexCoord[0]);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), &glVertices2DCar[i][0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2DMosaicImageParams[i].CarVerTexCoord[1]);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), glTexCoordCar, GL_STATIC_DRAW);
	}
#endif

	printf("Bind Finish\n");
}

void sd3DSetCVMatChnVal(CvMat *srcMat, unsigned int row, unsigned int col, unsigned char k, void *val, int type)
{
	unsigned int elemBytes = CV_ELEM_SIZE(type);
	unsigned char chnBytes = elemBytes / CV_MAT_CN(type);
	void *addr = srcMat->data.ptr + (size_t)srcMat->step * row + elemBytes * col + chnBytes * k;

	memcpy((unsigned char *)addr, (unsigned char *)val, chnBytes);
}

void sd3DSetCVMatDefVal(CvMat *srcMat, void *val, int type)
{
	unsigned int i = 0, j = 0, k = 0;

	for (i = 0; i < srcMat->rows; i++)
	{
		for (j = 0; j < srcMat->cols; j++)
		{
			for (k = 0; k < CV_MAT_CN(type); k++)
			{
				sd3DSetCVMatChnVal(srcMat, i, j, k, val, type);
			}
		}
	}
}

void initCamParaData()
{
	int i;
	float tmpVal32F = 0;

	for (i = 0; i < 2; i++)
	{
		para_field[i].chessboard_width_corners = 5;
		para_field[i].chessboard_length_corners = 7;
		para_field[i].square_size = 20;

		readParamsXML(i);

		SimplifyfrontCamParams[i].r_vec = cvCreateMat(1, 3, CV_32FC1);
		cvmSet(SimplifyfrontCamParams[i].r_vec, 0, 0, (float)SimplifyfrontCamParams[i].mrInt[0] / SCALE3);
		cvmSet(SimplifyfrontCamParams[i].r_vec, 0, 1, (float)SimplifyfrontCamParams[i].mrInt[1] / SCALE3);
		cvmSet(SimplifyfrontCamParams[i].r_vec, 0, 2, (float)SimplifyfrontCamParams[i].mrInt[2] / SCALE3);

		SimplifyfrontCamParams[i].t_vec = cvCreateMat(1, 3, CV_32FC1);
		cvmSet(SimplifyfrontCamParams[i].t_vec, 0, 0, (float)SimplifyfrontCamParams[i].mtInt[0] / SCALE2);
		cvmSet(SimplifyfrontCamParams[i].t_vec, 0, 1, (float)SimplifyfrontCamParams[i].mtInt[1] / SCALE2);
		cvmSet(SimplifyfrontCamParams[i].t_vec, 0, 2, (float)SimplifyfrontCamParams[i].mtInt[2] / SCALE2);

		SimplifyfrontCamParams[i].camera = cvCreateMat(3, 3, CV_32FC1);
		tmpVal32F = 0;
		sd3DSetCVMatDefVal(SimplifyfrontCamParams[i].camera, &tmpVal32F, CV_32FC1);
		cvmSet(SimplifyfrontCamParams[i].camera, 0, 0, (float)SimplifyfrontCamParams[i].mimdInt[0] / SCALE2); //fx
		cvmSet(SimplifyfrontCamParams[i].camera, 1, 1, (float)SimplifyfrontCamParams[i].mimdInt[1] / SCALE2); //fy
		cvmSet(SimplifyfrontCamParams[i].camera, 0, 2, (float)SimplifyfrontCamParams[i].mimdInt[2] / SCALE2); //cx
		cvmSet(SimplifyfrontCamParams[i].camera, 1, 2, (float)SimplifyfrontCamParams[i].mimdInt[3] / SCALE2); //cy
		cvmSet(SimplifyfrontCamParams[i].camera, 2, 2, 1.0);

		SimplifyfrontCamParams[i].dist_coeffs = cvCreateMat(1, 4, CV_32FC1);
		cvmSet(SimplifyfrontCamParams[i].dist_coeffs, 0, 0, (float)SimplifyfrontCamParams[i].mimdInt[4] / SCALE1);
		cvmSet(SimplifyfrontCamParams[i].dist_coeffs, 0, 1, (float)SimplifyfrontCamParams[i].mimdInt[5] / SCALE1);
		cvmSet(SimplifyfrontCamParams[i].dist_coeffs, 0, 2, (float)SimplifyfrontCamParams[i].mimdInt[6] / SCALE1);
		cvmSet(SimplifyfrontCamParams[i].dist_coeffs, 0, 3, (float)SimplifyfrontCamParams[i].mimdInt[7] / SCALE1);

		SimplifyrearCamParams[i].r_vec = cvCreateMat(1, 3, CV_32FC1);
		cvmSet(SimplifyrearCamParams[i].r_vec, 0, 0, (float)SimplifyrearCamParams[i].mrInt[0] / SCALE3);
		cvmSet(SimplifyrearCamParams[i].r_vec, 0, 1, (float)SimplifyrearCamParams[i].mrInt[1] / SCALE3);
		cvmSet(SimplifyrearCamParams[i].r_vec, 0, 2, (float)SimplifyrearCamParams[i].mrInt[2] / SCALE3);

		SimplifyrearCamParams[i].t_vec = cvCreateMat(1, 3, CV_32FC1);
		cvmSet(SimplifyrearCamParams[i].t_vec, 0, 0, (float)SimplifyrearCamParams[i].mtInt[0] / SCALE2);
		cvmSet(SimplifyrearCamParams[i].t_vec, 0, 1, (float)SimplifyrearCamParams[i].mtInt[1] / SCALE2);
		cvmSet(SimplifyrearCamParams[i].t_vec, 0, 2, (float)SimplifyrearCamParams[i].mtInt[2] / SCALE2);

		SimplifyrearCamParams[i].camera = cvCreateMat(3, 3, CV_32FC1);
		tmpVal32F = 0;
		sd3DSetCVMatDefVal(SimplifyrearCamParams[i].camera, &tmpVal32F, CV_32FC1);
		cvmSet(SimplifyrearCamParams[i].camera, 0, 0, (float)SimplifyrearCamParams[i].mimdInt[0] / SCALE2); //fx
		cvmSet(SimplifyrearCamParams[i].camera, 1, 1, (float)SimplifyrearCamParams[i].mimdInt[1] / SCALE2); //fy
		cvmSet(SimplifyrearCamParams[i].camera, 0, 2, (float)SimplifyrearCamParams[i].mimdInt[2] / SCALE2); //cx
		cvmSet(SimplifyrearCamParams[i].camera, 1, 2, (float)SimplifyrearCamParams[i].mimdInt[3] / SCALE2); //cy
		cvmSet(SimplifyrearCamParams[i].camera, 2, 2, 1.0);

		SimplifyrearCamParams[i].dist_coeffs = cvCreateMat(1, 4, CV_32FC1);
		cvmSet(SimplifyrearCamParams[i].dist_coeffs, 0, 0, (float)SimplifyrearCamParams[i].mimdInt[4] / SCALE1);
		cvmSet(SimplifyrearCamParams[i].dist_coeffs, 0, 1, (float)SimplifyrearCamParams[i].mimdInt[5] / SCALE1);
		cvmSet(SimplifyrearCamParams[i].dist_coeffs, 0, 2, (float)SimplifyrearCamParams[i].mimdInt[6] / SCALE1);
		cvmSet(SimplifyrearCamParams[i].dist_coeffs, 0, 3, (float)SimplifyrearCamParams[i].mimdInt[7] / SCALE1);

		SimplifyleftCamParams[i].r_vec = cvCreateMat(1, 3, CV_32FC1);
		cvmSet(SimplifyleftCamParams[i].r_vec, 0, 0, (float)SimplifyleftCamParams[i].mrInt[0] / SCALE3);
		cvmSet(SimplifyleftCamParams[i].r_vec, 0, 1, (float)SimplifyleftCamParams[i].mrInt[1] / SCALE3);
		cvmSet(SimplifyleftCamParams[i].r_vec, 0, 2, (float)SimplifyleftCamParams[i].mrInt[2] / SCALE3);

		SimplifyleftCamParams[i].t_vec = cvCreateMat(1, 3, CV_32FC1);
		cvmSet(SimplifyleftCamParams[i].t_vec, 0, 0, (float)SimplifyleftCamParams[i].mtInt[0] / SCALE2);
		cvmSet(SimplifyleftCamParams[i].t_vec, 0, 1, (float)SimplifyleftCamParams[i].mtInt[1] / SCALE2);
		cvmSet(SimplifyleftCamParams[i].t_vec, 0, 2, (float)SimplifyleftCamParams[i].mtInt[2] / SCALE2);

		SimplifyleftCamParams[i].camera = cvCreateMat(3, 3, CV_32FC1);
		tmpVal32F = 0;
		sd3DSetCVMatDefVal(SimplifyleftCamParams[i].camera, &tmpVal32F, CV_32FC1);
		cvmSet(SimplifyleftCamParams[i].camera, 0, 0, (float)SimplifyleftCamParams[i].mimdInt[0] / SCALE2); //fx
		cvmSet(SimplifyleftCamParams[i].camera, 1, 1, (float)SimplifyleftCamParams[i].mimdInt[1] / SCALE2); //fy
		cvmSet(SimplifyleftCamParams[i].camera, 0, 2, (float)SimplifyleftCamParams[i].mimdInt[2] / SCALE2); //cx
		cvmSet(SimplifyleftCamParams[i].camera, 1, 2, (float)SimplifyleftCamParams[i].mimdInt[3] / SCALE2); //cy
		cvmSet(SimplifyleftCamParams[i].camera, 2, 2, 1.0);

		SimplifyleftCamParams[i].dist_coeffs = cvCreateMat(1, 4, CV_32FC1);
		cvmSet(SimplifyleftCamParams[i].dist_coeffs, 0, 0, (float)SimplifyleftCamParams[i].mimdInt[4] / SCALE1);
		cvmSet(SimplifyleftCamParams[i].dist_coeffs, 0, 1, (float)SimplifyleftCamParams[i].mimdInt[5] / SCALE1);
		cvmSet(SimplifyleftCamParams[i].dist_coeffs, 0, 2, (float)SimplifyleftCamParams[i].mimdInt[6] / SCALE1);
		cvmSet(SimplifyleftCamParams[i].dist_coeffs, 0, 3, (float)SimplifyleftCamParams[i].mimdInt[7] / SCALE1);

		SimplifyrightCamParams[i].r_vec = cvCreateMat(1, 3, CV_32FC1);
		cvmSet(SimplifyrightCamParams[i].r_vec, 0, 0, (float)SimplifyrightCamParams[i].mrInt[0] / SCALE3);
		cvmSet(SimplifyrightCamParams[i].r_vec, 0, 1, (float)SimplifyrightCamParams[i].mrInt[1] / SCALE3);
		cvmSet(SimplifyrightCamParams[i].r_vec, 0, 2, (float)SimplifyrightCamParams[i].mrInt[2] / SCALE3);

		SimplifyrightCamParams[i].t_vec = cvCreateMat(1, 3, CV_32FC1);
		cvmSet(SimplifyrightCamParams[i].t_vec, 0, 0, (float)SimplifyrightCamParams[i].mtInt[0] / SCALE2);
		cvmSet(SimplifyrightCamParams[i].t_vec, 0, 1, (float)SimplifyrightCamParams[i].mtInt[1] / SCALE2);
		cvmSet(SimplifyrightCamParams[i].t_vec, 0, 2, (float)SimplifyrightCamParams[i].mtInt[2] / SCALE2);

		SimplifyrightCamParams[i].camera = cvCreateMat(3, 3, CV_32FC1);
		tmpVal32F = 0;
		sd3DSetCVMatDefVal(SimplifyrightCamParams[i].camera, &tmpVal32F, CV_32FC1);
		cvmSet(SimplifyrightCamParams[i].camera, 0, 0, (float)SimplifyrightCamParams[i].mimdInt[0] / SCALE2); //fx
		cvmSet(SimplifyrightCamParams[i].camera, 1, 1, (float)SimplifyrightCamParams[i].mimdInt[1] / SCALE2); //fy
		cvmSet(SimplifyrightCamParams[i].camera, 0, 2, (float)SimplifyrightCamParams[i].mimdInt[2] / SCALE2); //cx
		cvmSet(SimplifyrightCamParams[i].camera, 1, 2, (float)SimplifyrightCamParams[i].mimdInt[3] / SCALE2); //cy
		cvmSet(SimplifyrightCamParams[i].camera, 2, 2, 1.0);

		SimplifyrightCamParams[i].dist_coeffs = cvCreateMat(1, 4, CV_32FC1);
		cvmSet(SimplifyrightCamParams[i].dist_coeffs, 0, 0, (float)SimplifyrightCamParams[i].mimdInt[4] / SCALE1);
		cvmSet(SimplifyrightCamParams[i].dist_coeffs, 0, 1, (float)SimplifyrightCamParams[i].mimdInt[5] / SCALE1);
		cvmSet(SimplifyrightCamParams[i].dist_coeffs, 0, 2, (float)SimplifyrightCamParams[i].mimdInt[6] / SCALE1);
		cvmSet(SimplifyrightCamParams[i].dist_coeffs, 0, 3, (float)SimplifyrightCamParams[i].mimdInt[7] / SCALE1);
	}
}
