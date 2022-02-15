/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "opengl_common.h"
#include "openglwindow.h"
#include <QtGui/QGuiApplication>
#include <QApplication>
#include <QtWidgets/QApplication>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "tools.h"

extern int initMosaic(safImgRect allView, safImgRect singleView);
extern int updateTexture(unsigned char **src);
extern void runRender(int viewMode, float steeringWheelAngle, int flag);

//! [1]
class TriangleWindow : public OpenGLWindow
{
public:
    TriangleWindow();
    ~TriangleWindow();

    void initialize() override;
    void render() override;

private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    GLuint m_program;
public:
    safImgRect allView, singleView;
};

TriangleWindow::TriangleWindow()
    : m_program(0)
{
}

TriangleWindow::~TriangleWindow()
{
}

//yuyv的数据保存
unsigned char *outbuf[5] = {nullptr};
void TriangleWindow::initialize()
{
    initMosaic(allView, singleView);
    loadImage(outbuf);
}

void TriangleWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    glClear(GL_COLOR_BUFFER_BIT);
    updateTexture(outbuf);
    int scm = 1;                             //车身状态
    float phsical_angle = 0;       //转向角
    int heavy_status = 0;            //载重
    runRender(scm, phsical_angle, heavy_status);
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    QSurfaceFormat format;
    format.setSamples(16);
    TriangleWindow window;
    window.setFormat(format);

    // unsigned int tmp_w=2160,tmp_h=1440, l=3,r=7;    //笔记本屏幕分辨率大小,左右两栏比例大小
    unsigned int tmp_w=1280,tmp_h=720, l=3,r=7;          //中控平屏幕分辨率大小,左右两栏比例大小
    // unsigned int tmp_w = 1920, tmp_h = 1080, l = 4, r = 6; //米文计算平台屏幕分辨率大小,左右两栏比例大小
    // unsigned int tmp_w=1920,tmp_h=1200, l=4,r=6;    //tianzhun计算平台屏幕分辨率大小,左右两栏比例大小
    window.allView.x = 0;
    window.allView.y = 0;
    window.allView.height = tmp_h;
    window.allView.width = tmp_w * l / (l + r);

    window.singleView.x = tmp_w * l / (l + r);
    window.singleView.y = 0;
    window.singleView.width = tmp_w * r / (l + r);
    window.singleView.height = tmp_h;

    window.resize(1280, 720);
    window.show();
    // window.showFullScreen(); //全屏显示
    window.setAnimating(true);
    return app.exec();
}
