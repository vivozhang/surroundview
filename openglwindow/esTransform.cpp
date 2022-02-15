#include "esTransform.h"
#include <math.h>
#include <string.h>

/*====================================================================
函数名称:   esScale
函数功能:矩阵缩放
全局变量:
输入参数:   result：操作矩阵  sx，sy，sz：三个方向的缩放比例
返回结果:   无
修改记录：
====================================================================*/
void esScale(ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz)
{
   result->m[0][0] *= sx;
   result->m[0][1] *= sx;
   result->m[0][2] *= sx;
   result->m[0][3] *= sx;

   result->m[1][0] *= sy;
   result->m[1][1] *= sy;
   result->m[1][2] *= sy;
   result->m[1][3] *= sy;

   result->m[2][0] *= sz;
   result->m[2][1] *= sz;
   result->m[2][2] *= sz;
   result->m[2][3] *= sz;
}

/*====================================================================
函数名称:   esTranslate
函数功能:矩阵平移
全局变量:
输入参数:   result：操作矩阵   tx，ty，tz：三个方向的平移值
返回结果:   无
修改记录：
====================================================================*/
void esTranslate(ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz)
{
   result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
   result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
   result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
   result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

/*====================================================================
函数名称:   esRotate
函数功能:矩阵旋转
全局变量:
输入参数:   result：angle：旋转角度，操作矩阵   x，y，z：三个方向的旋转
返回结果:   无
修改记录：
====================================================================*/
void esRotate(ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
   GLfloat sinAngle, cosAngle;
   GLfloat mag = sqrtf(x * x + y * y + z * z);

   sinAngle = sin(angle * PI / 180.0f);
   cosAngle = cos(angle * PI / 180.0f);

   if (mag > 0.0f)
   {
      GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
      GLfloat oneMinusCos;
      ESMatrix rotMat;

      x /= mag;
      y /= mag;
      z /= mag;

      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * sinAngle;
      ys = y * sinAngle;
      zs = z * sinAngle;
      oneMinusCos = 1.0f - cosAngle;

      rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
      rotMat.m[0][1] = (oneMinusCos * xy) - zs;
      rotMat.m[0][2] = (oneMinusCos * zx) + ys;
      rotMat.m[0][3] = 0.0F;

      rotMat.m[1][0] = (oneMinusCos * xy) + zs;
      rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
      rotMat.m[1][2] = (oneMinusCos * yz) - xs;
      rotMat.m[1][3] = 0.0F;

      rotMat.m[2][0] = (oneMinusCos * zx) - ys;
      rotMat.m[2][1] = (oneMinusCos * yz) + xs;
      rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
      rotMat.m[2][3] = 0.0F;

      rotMat.m[3][0] = 0.0F;
      rotMat.m[3][1] = 0.0F;
      rotMat.m[3][2] = 0.0F;
      rotMat.m[3][3] = 1.0F;

      esMatrixMultiply(result, &rotMat, result);
   }
}

/*====================================================================
函数名称:   esFrustum
函数功能:设置平头截体视图矩阵
全局变量:
输入参数:   
返回结果:   无
修改记录：
====================================================================*/
void esFrustum(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
   float deltaX = right - left;
   float deltaY = top - bottom;
   float deltaZ = farZ - nearZ;
   ESMatrix frust;

   if ((nearZ <= 0.0f) || (farZ <= 0.0f) ||
       (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f))
   {
      return;
   }

   frust.m[0][0] = 2.0f * nearZ / deltaX;
   frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

   frust.m[1][1] = 2.0f * nearZ / deltaY;
   frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

   frust.m[2][0] = (right + left) / deltaX;
   frust.m[2][1] = (top + bottom) / deltaY;
   frust.m[2][2] = -(nearZ + farZ) / deltaZ;
   frust.m[2][3] = -1.0f;

   frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
   frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

   esMatrixMultiply(result, &frust, result);
}

/*====================================================================
函数名称:   esPerspective
函数功能:设置透视矩阵
全局变量:
输入参数:   
返回结果:   无
修改记录：
====================================================================*/
void esPerspective(ESMatrix *result, float fovy, float aspect, float nearZ, float farZ)
{
   GLfloat frustumW, frustumH;

   frustumH = tan(fovy / 360.0f * PI) * nearZ;
   frustumW = frustumH * aspect;

   esFrustum(result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
}

/*====================================================================
函数名称:   esOrtho
函数功能:设置正交矩阵
全局变量:
输入参数:   
返回结果:   无
修改记录：
====================================================================*/
void esOrtho(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
   float deltaX = right - left;
   float deltaY = top - bottom;
   float deltaZ = farZ - nearZ;
   ESMatrix ortho;

   if ((deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f))
   {
      return;
   }

   esMatrixLoadIdentity(&ortho);
   ortho.m[0][0] = 2.0f / deltaX;
   ortho.m[3][0] = -(right + left) / deltaX;
   ortho.m[1][1] = 2.0f / deltaY;
   ortho.m[3][1] = -(top + bottom) / deltaY;
   ortho.m[2][2] = -2.0f / deltaZ;
   ortho.m[3][2] = -(nearZ + farZ) / deltaZ;

   esMatrixMultiply(result, &ortho, result);
}

/*====================================================================
函数名称:   esMatrixMultiply
函数功能:矩阵相乘
全局变量:
输入参数:   result：输出结果，srcA和srcB两个乘数
返回结果:   无
修改记录：
====================================================================*/
void esMatrixMultiply(ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB)
{
   ESMatrix tmp;
   int i;

   for (i = 0; i < 4; i++)
   {
      tmp.m[i][0] = (srcA->m[i][0] * srcB->m[0][0]) +
                    (srcA->m[i][1] * srcB->m[1][0]) +
                    (srcA->m[i][2] * srcB->m[2][0]) +
                    (srcA->m[i][3] * srcB->m[3][0]);

      tmp.m[i][1] = (srcA->m[i][0] * srcB->m[0][1]) +
                    (srcA->m[i][1] * srcB->m[1][1]) +
                    (srcA->m[i][2] * srcB->m[2][1]) +
                    (srcA->m[i][3] * srcB->m[3][1]);

      tmp.m[i][2] = (srcA->m[i][0] * srcB->m[0][2]) +
                    (srcA->m[i][1] * srcB->m[1][2]) +
                    (srcA->m[i][2] * srcB->m[2][2]) +
                    (srcA->m[i][3] * srcB->m[3][2]);

      tmp.m[i][3] = (srcA->m[i][0] * srcB->m[0][3]) +
                    (srcA->m[i][1] * srcB->m[1][3]) +
                    (srcA->m[i][2] * srcB->m[2][3]) +
                    (srcA->m[i][3] * srcB->m[3][3]);
   }

   memcpy(result, &tmp, sizeof(ESMatrix));
}

/*====================================================================
函数名称:   esMatrixLoadIdentity
函数功能:生成单位矩阵
全局变量:
输入参数:   result：输出结果
返回结果:   无
修改记录：
====================================================================*/
void esMatrixLoadIdentity(ESMatrix *result)
{
   memset(result, 0x0, sizeof(ESMatrix));
   result->m[0][0] = 1.0f;
   result->m[1][1] = 1.0f;
   result->m[2][2] = 1.0f;
   result->m[3][3] = 1.0f;
}

/*====================================================================
函数名称:   esMatrixLookAt
函数功能:生成视图矩阵
全局变量:
输入参数:   result：输出结果
返回结果:   无
修改记录：
====================================================================*/
void esMatrixLookAt(ESMatrix *result,
                    float posX, float posY, float posZ,
                    float lookAtX, float lookAtY, float lookAtZ,
                    float upX, float upY, float upZ)
{
   float axisX[3], axisY[3], axisZ[3];
   float length;

   // axisZ = lookAt - pos
   axisZ[0] = lookAtX - posX;
   axisZ[1] = lookAtY - posY;
   axisZ[2] = lookAtZ - posZ;

   // normalize axisZ
   length = sqrtf(axisZ[0] * axisZ[0] + axisZ[1] * axisZ[1] + axisZ[2] * axisZ[2]);

   if (length != 0.0f)
   {
      axisZ[0] /= length;
      axisZ[1] /= length;
      axisZ[2] /= length;
   }

   // axisX = up X axisZ
   axisX[0] = upY * axisZ[2] - upZ * axisZ[1];
   axisX[1] = upZ * axisZ[0] - upX * axisZ[2];
   axisX[2] = upX * axisZ[1] - upY * axisZ[0];

   // normalize axisX
   length = sqrtf(axisX[0] * axisX[0] + axisX[1] * axisX[1] + axisX[2] * axisX[2]);

   if (length != 0.0f)
   {
      axisX[0] /= length;
      axisX[1] /= length;
      axisX[2] /= length;
   }

   // axisY = axisZ x axisX
   axisY[0] = axisZ[1] * axisX[2] - axisZ[2] * axisX[1];
   axisY[1] = axisZ[2] * axisX[0] - axisZ[0] * axisX[2];
   axisY[2] = axisZ[0] * axisX[1] - axisZ[1] * axisX[0];

   // normalize axisY
   length = sqrtf(axisY[0] * axisY[0] + axisY[1] * axisY[1] + axisY[2] * axisY[2]);

   if (length != 0.0f)
   {
      axisY[0] /= length;
      axisY[1] /= length;
      axisY[2] /= length;
   }

   memset(result, 0x0, sizeof(ESMatrix));

   result->m[0][0] = -axisX[0];
   result->m[0][1] = axisY[0];
   result->m[0][2] = -axisZ[0];

   result->m[1][0] = -axisX[1];
   result->m[1][1] = axisY[1];
   result->m[1][2] = -axisZ[1];

   result->m[2][0] = -axisX[2];
   result->m[2][1] = axisY[2];
   result->m[2][2] = -axisZ[2];

   // translate (-posX, -posY, -posZ)
   result->m[3][0] = axisX[0] * posX + axisX[1] * posY + axisX[2] * posZ;
   result->m[3][1] = -axisY[0] * posX - axisY[1] * posY - axisY[2] * posZ;
   result->m[3][2] = axisZ[0] * posX + axisZ[1] * posY + axisZ[2] * posZ;
   result->m[3][3] = 1.0f;
}

#if 1
float matrix3x3Determinant(float *matrix)
{
   float result = 0.0f;

   result = matrix[0] * (matrix[4] * matrix[8] - matrix[7] * matrix[5]);
   result -= matrix[3] * (matrix[1] * matrix[8] - matrix[7] * matrix[2]);
   result += matrix[6] * (matrix[1] * matrix[5] - matrix[4] * matrix[2]);

   return result;
}

float matrixDeterminant(ESMatrix *matrix)
{
   float matrix3x3[9];
   float determinant3x3 = 0.0f;
   float result = 0.0f;
   float *elements;

   elements = &matrix->m[0][0];
   /* Remove (i, j) (1, 1) to form new 3x3 matrix. */
   matrix3x3[0] = elements[5];
   matrix3x3[1] = elements[6];
   matrix3x3[2] = elements[7];
   matrix3x3[3] = elements[9];
   matrix3x3[4] = elements[10];
   matrix3x3[5] = elements[11];
   matrix3x3[6] = elements[13];
   matrix3x3[7] = elements[14];
   matrix3x3[8] = elements[15];
   determinant3x3 = matrix3x3Determinant(matrix3x3);
   result += elements[0] * determinant3x3;

   /* Remove (i, j) (1, 2) to form new 3x3 matrix. */
   matrix3x3[0] = elements[1];
   matrix3x3[1] = elements[2];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[9];
   matrix3x3[4] = elements[10];
   matrix3x3[5] = elements[11];
   matrix3x3[6] = elements[13];
   matrix3x3[7] = elements[14];
   matrix3x3[8] = elements[15];
   determinant3x3 = matrix3x3Determinant(matrix3x3);
   result -= elements[4] * determinant3x3;

   /* Remove (i, j) (1, 3) to form new 3x3 matrix. */
   matrix3x3[0] = elements[1];
   matrix3x3[1] = elements[2];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[5];
   matrix3x3[4] = elements[6];
   matrix3x3[5] = elements[7];
   matrix3x3[6] = elements[13];
   matrix3x3[7] = elements[14];
   matrix3x3[8] = elements[15];
   determinant3x3 = matrix3x3Determinant(matrix3x3);
   result += elements[8] * determinant3x3;

   /* Remove (i, j) (1, 4) to form new 3x3 matrix. */
   matrix3x3[0] = elements[1];
   matrix3x3[1] = elements[2];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[5];
   matrix3x3[4] = elements[6];
   matrix3x3[5] = elements[7];
   matrix3x3[6] = elements[9];
   matrix3x3[7] = elements[10];
   matrix3x3[8] = elements[11];
   determinant3x3 = matrix3x3Determinant(matrix3x3);
   result -= elements[12] * determinant3x3;

   return result;
}

/*====================================================================
函数名称:   esMatrixTranspose
函数功能:矩阵转置
全局变量:
输入参数:   result：操作矩阵  
返回结果:   无
修改记录：
====================================================================*/
void esMatrixTranspose(ESMatrix *matrix)
{
   float temp;
   float *elements;

   elements = &matrix->m[0][0];

   temp = elements[1];
   elements[1] = elements[4];
   elements[4] = temp;

   temp = elements[2];
   elements[2] = elements[8];
   elements[8] = temp;

   temp = elements[3];
   elements[3] = elements[12];
   elements[12] = temp;

   temp = elements[6];
   elements[6] = elements[9];
   elements[9] = temp;

   temp = elements[7];
   elements[7] = elements[13];
   elements[13] = temp;

   temp = elements[11];
   elements[11] = elements[14];
   elements[14] = temp;
}

ESMatrix esMatrixScale(ESMatrix *matrix, float scale)
{
   ESMatrix result;
   int allElements;

   for (allElements = 0; allElements < 16; allElements++)
   {
      result.m[allElements / 4][allElements % 4] = matrix->m[allElements / 4][allElements % 4] * scale;
   }

   return result;
}

/*====================================================================
函数名称:   esMatrixInvert
函数功能:求逆矩阵
全局变量:
输入参数:   result：源矩阵  
返回结果:   逆矩阵
修改记录：
====================================================================*/
ESMatrix esMatrixInvert(ESMatrix *matrix)
{
   ESMatrix result;
   float matrix3x3[9];
   float *elements;

   elements = &matrix->m[0][0];

   /* Find the cofactor of each element. */
   /* Element (i, j) (1, 1) */
   matrix3x3[0] = elements[5];
   matrix3x3[1] = elements[6];
   matrix3x3[2] = elements[7];
   matrix3x3[3] = elements[9];
   matrix3x3[4] = elements[10];
   matrix3x3[5] = elements[11];
   matrix3x3[6] = elements[13];
   matrix3x3[7] = elements[14];
   matrix3x3[8] = elements[15];
   result.m[0][0] = matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (1, 2) */
   matrix3x3[0] = elements[1];
   matrix3x3[1] = elements[2];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[9];
   matrix3x3[4] = elements[10];
   matrix3x3[5] = elements[11];
   matrix3x3[6] = elements[13];
   matrix3x3[7] = elements[14];
   matrix3x3[8] = elements[15];
   result.m[1][0] = -matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (1, 3) */
   matrix3x3[0] = elements[1];
   matrix3x3[1] = elements[2];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[5];
   matrix3x3[4] = elements[6];
   matrix3x3[5] = elements[7];
   matrix3x3[6] = elements[13];
   matrix3x3[7] = elements[14];
   matrix3x3[8] = elements[15];
   result.m[2][0] = matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (1, 4) */
   matrix3x3[0] = elements[1];
   matrix3x3[1] = elements[2];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[5];
   matrix3x3[4] = elements[6];
   matrix3x3[5] = elements[7];
   matrix3x3[6] = elements[9];
   matrix3x3[7] = elements[10];
   matrix3x3[8] = elements[11];
   result.m[3][0] = -matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (2, 1) */
   matrix3x3[0] = elements[4];
   matrix3x3[1] = elements[6];
   matrix3x3[2] = elements[7];
   matrix3x3[3] = elements[8];
   matrix3x3[4] = elements[10];
   matrix3x3[5] = elements[11];
   matrix3x3[6] = elements[12];
   matrix3x3[7] = elements[14];
   matrix3x3[8] = elements[15];
   result.m[0][1] = -matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (2, 2) */
   matrix3x3[0] = elements[0];
   matrix3x3[1] = elements[2];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[8];
   matrix3x3[4] = elements[10];
   matrix3x3[5] = elements[11];
   matrix3x3[6] = elements[12];
   matrix3x3[7] = elements[14];
   matrix3x3[8] = elements[15];
   result.m[1][1] = matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (2, 3) */
   matrix3x3[0] = elements[0];
   matrix3x3[1] = elements[2];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[4];
   matrix3x3[4] = elements[6];
   matrix3x3[5] = elements[7];
   matrix3x3[6] = elements[12];
   matrix3x3[7] = elements[14];
   matrix3x3[8] = elements[15];
   result.m[2][1] = -matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (2, 4) */
   matrix3x3[0] = elements[0];
   matrix3x3[1] = elements[2];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[4];
   matrix3x3[4] = elements[6];
   matrix3x3[5] = elements[7];
   matrix3x3[6] = elements[8];
   matrix3x3[7] = elements[10];
   matrix3x3[8] = elements[11];
   result.m[3][1] = matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (3, 1) */
   matrix3x3[0] = elements[4];
   matrix3x3[1] = elements[5];
   matrix3x3[2] = elements[7];
   matrix3x3[3] = elements[8];
   matrix3x3[4] = elements[9];
   matrix3x3[5] = elements[11];
   matrix3x3[6] = elements[12];
   matrix3x3[7] = elements[13];
   matrix3x3[8] = elements[15];
   result.m[0][2] = matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (3, 2) */
   matrix3x3[0] = elements[0];
   matrix3x3[1] = elements[1];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[8];
   matrix3x3[4] = elements[9];
   matrix3x3[5] = elements[11];
   matrix3x3[6] = elements[12];
   matrix3x3[7] = elements[13];
   matrix3x3[8] = elements[15];
   result.m[1][2] = -matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (3, 3) */
   matrix3x3[0] = elements[0];
   matrix3x3[1] = elements[1];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[4];
   matrix3x3[4] = elements[5];
   matrix3x3[5] = elements[7];
   matrix3x3[6] = elements[12];
   matrix3x3[7] = elements[13];
   matrix3x3[8] = elements[15];
   result.m[2][2] = matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (3, 4) */
   matrix3x3[0] = elements[0];
   matrix3x3[1] = elements[1];
   matrix3x3[2] = elements[3];
   matrix3x3[3] = elements[4];
   matrix3x3[4] = elements[5];
   matrix3x3[5] = elements[7];
   matrix3x3[6] = elements[8];
   matrix3x3[7] = elements[9];
   matrix3x3[8] = elements[11];
   result.m[3][2] = -matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (4, 1) */
   matrix3x3[0] = elements[4];
   matrix3x3[1] = elements[5];
   matrix3x3[2] = elements[6];
   matrix3x3[3] = elements[8];
   matrix3x3[4] = elements[9];
   matrix3x3[5] = elements[10];
   matrix3x3[6] = elements[12];
   matrix3x3[7] = elements[13];
   matrix3x3[8] = elements[14];
   result.m[0][3] = -matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (4, 2) */
   matrix3x3[0] = elements[0];
   matrix3x3[1] = elements[1];
   matrix3x3[2] = elements[2];
   matrix3x3[3] = elements[8];
   matrix3x3[4] = elements[9];
   matrix3x3[5] = elements[10];
   matrix3x3[6] = elements[12];
   matrix3x3[7] = elements[13];
   matrix3x3[8] = elements[14];
   result.m[1][3] = matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (4, 3) */
   matrix3x3[0] = elements[0];
   matrix3x3[1] = elements[1];
   matrix3x3[2] = elements[2];
   matrix3x3[3] = elements[4];
   matrix3x3[4] = elements[5];
   matrix3x3[5] = elements[6];
   matrix3x3[6] = elements[12];
   matrix3x3[7] = elements[13];
   matrix3x3[8] = elements[14];
   result.m[2][3] = -matrix3x3Determinant(matrix3x3);

   /* Element (i, j) (4, 4) */
   matrix3x3[0] = elements[0];
   matrix3x3[1] = elements[1];
   matrix3x3[2] = elements[2];
   matrix3x3[3] = elements[4];
   matrix3x3[4] = elements[5];
   matrix3x3[5] = elements[6];
   matrix3x3[6] = elements[8];
   matrix3x3[7] = elements[9];
   matrix3x3[8] = elements[10];
   result.m[3][3] = matrix3x3Determinant(matrix3x3);

   /* The adjoint is the transpose of the cofactor matrix. */
   esMatrixTranspose(&result);

   /* The inverse is the adjoint divided by the determinant. */
   result = esMatrixScale(&result, 1.0f / matrixDeterminant(matrix));

   return result;
}
#endif
