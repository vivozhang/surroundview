/*********************************************************************************
* 版权所有 (C) 三一专汽
*
* 文件名称： esTransform.h
* 文件标识：
* 内容摘要： 数学变换操作函数声明
* 其它说明：
* 当前版本：
* 作    者：
* 完成日期：
* --------------------------------------------------------------------------------
* 修改记录1：
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容：
**********************************************************************************/
#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*==================================   常量定义  =================================*/
#define PI 3.1415926535897932384626433832795f

/*===============================    全局数据类型  ================================*/
typedef struct
{
    GLfloat   m[4][4];
} ESMatrix;

#if 1
/*===============================  函数声明  ================================*/
extern void esScale ( ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz );

extern void esTranslate ( ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz );

extern void esRotate( ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z );

extern void esFrustum ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );

extern void esPerspective ( ESMatrix *result, float fovy, float aspect, float nearZ, float farZ );

extern void esOrtho ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );

extern void esMatrixMultiply ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );

extern void esMatrixLoadIdentity ( ESMatrix *result );

extern void esMatrixLookAt ( ESMatrix *result,
                      float posX,    float posY,    float posZ,
                      float lookAtX, float lookAtY, float lookAtZ,
                      float upX,     float upY,     float upZ );

extern void esMatrixTranspose(ESMatrix *matrix);

extern ESMatrix esMatrixScale(ESMatrix *matrix, float scale);

extern ESMatrix esMatrixInvert(ESMatrix *matrix);
#endif