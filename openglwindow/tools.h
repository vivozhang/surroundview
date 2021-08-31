#ifndef TOOLS_H
#define TOOLS_H
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>

void loadImage(unsigned char **outbuf)
{
    int x, y, n;
    stbi_set_flip_vertically_on_load(false);
    outbuf[0] = stbi_load("../data/front.png", &x, &y, &n, 4);
    outbuf[1] = stbi_load("../data/back.png", &x, &y, &n, 4);
    outbuf[2] = stbi_load("../data/left.png", &x, &y, &n, 4);
    outbuf[3] = stbi_load("../data/right.png", &x, &y, &n, 4);
    outbuf[4] = stbi_load("../data/daoche.png", &x, &y, &n, 4);
}

void saveImage(int err_num, unsigned char **outbuf)
{
    if (err_num < 10000)
    {
        std::string filename = "../Image/front/" + std::to_string(err_num) + ".png";
        stbi_write_png(filename.c_str(), 1280, 720, 4, outbuf[0], 0);

        filename = "../Image/back/" + std::to_string(err_num) + ".png";
        stbi_write_png(filename.c_str(), 1280, 720, 4, outbuf[1], 0);

        filename = "../Image/left/" + std::to_string(err_num) + ".png";
        stbi_write_png(filename.c_str(), 1280, 720, 4, outbuf[2], 0);

        filename = "../Image/right/" + std::to_string(err_num) + ".png";
        stbi_write_png(filename.c_str(), 1280, 720, 4, outbuf[3], 0);

        filename = "../Image/dc/" + std::to_string(err_num) + ".png";
        stbi_write_png(filename.c_str(), 1280, 720, 4, outbuf[4], 0);
    }
}

void saveYUYV(int err_num, unsigned char **outbuf, int size)
{
    if (err_num < 1000)
    {
        FILE *fp;
        std::string filename = "../Image/front" + std::to_string(err_num) + ".dat";
        fp = fopen(filename.c_str(), "w");
        fwrite(outbuf[0], sizeof(unsigned char), size, fp);
        fclose(fp);

        filename = "../Image/back" + std::to_string(err_num) + ".dat";
        fp = fopen(filename.c_str(), "w");
        fwrite(outbuf[1], sizeof(unsigned char), size, fp);
        fclose(fp);

        filename = "../Image/left" + std::to_string(err_num) + ".dat";
        fp = fopen(filename.c_str(), "w");
        fwrite(outbuf[2], sizeof(unsigned char), size, fp);
        fclose(fp);

        filename = "../Image/right" + std::to_string(err_num) + ".dat";
        fp = fopen(filename.c_str(), "w");
        fwrite(outbuf[3], sizeof(unsigned char), size, fp);
        fclose(fp);

        filename = "../Image/dc" + std::to_string(err_num) + ".dat";
        fp = fopen(filename.c_str(), "w");
        fwrite(outbuf[4], sizeof(unsigned char), size, fp);
        fclose(fp);
    }
}

void loadYUYV(unsigned char **outbuf, int size)
{
    int err_num = 5;
    for (int i = 0; i < 5; i++)
        outbuf[i] = (unsigned char *)calloc(size, sizeof(unsigned char));

    FILE *fp;
    std::string filename = "../data/front" + std::to_string(err_num) + ".dat";
    fp = fopen(filename.c_str(), "r");
    fread(outbuf[0], 1, size, fp); //读取文件的字符串
    fclose(fp);

    filename = "../data/back" + std::to_string(err_num) + ".dat";
    fp = fopen(filename.c_str(), "r");
    fread(outbuf[1], 1, size, fp); //读取文件的字符串
    fclose(fp);

    filename = "../data/left" + std::to_string(err_num) + ".dat";
    fp = fopen(filename.c_str(), "r");
    fread(outbuf[2], 1, size, fp); //读取文件的字符串
    fclose(fp);

    filename = "../data/right" + std::to_string(err_num) + ".dat";
    fp = fopen(filename.c_str(), "r");
    fread(outbuf[3], 1, size, fp); //读取文件的字符串
    fclose(fp);

    filename = "../data/dc" + std::to_string(err_num) + ".dat";
    fp = fopen(filename.c_str(), "r");
    fread(outbuf[4], 1, size, fp); //读取文件的字符串
    fclose(fp);
}
#endif