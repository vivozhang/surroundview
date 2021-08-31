#include "camera.h"
#include <sys/ioctl.h>
#include <time.h>

int status = 1;          //(0:正常, 1:出图 , 2:释放图)
pthread_mutex_t g_mutex; //互斥锁
pthread_cond_t g_cond;   //条件锁
pthread_t g_thread;      //线程
int g_camera = 0;        //相机状态
int length = 0;          //图像开辟空间

//  thread
void *get(void *args)
{
    while (1)
    {
        pthread_mutex_lock(&g_mutex);
        // printf("status=%d,camera=%d\n",status,g_camera);
        if (status == 1)
        {
            // struct timeval ts, tf;
            // gettimeofday(&ts, NULL);
            // double start=ts.tv_sec * 1000.0 + ts.tv_usec/1000.0;
            int camera_index = -1;
            for (size_t i = 0; i < CAMERA_NUMS; i++)
            {
                bool b = readframe(&vct[i], &wholeimages[i]);
                if (!b)
                {
                    g_camera = -1;
                    break;
                }

                //若存在取相机数据index不相同时，相机出现故障问题
                if (i == 0)
                    camera_index = wholeimages[i].index;
                if (camera_index != wholeimages[i].index)
                {
                    printf("camera error\n");
                    g_camera = -1;
                    break;
                }

                unsigned char *ig = vct[i].buffers[wholeimages[i].index].start;
                memcpy(yuyv[i], ig, length);
            }
            status = 0;
            // gettimeofday(&tf, NULL);
            // double end = tf.tv_sec * 1000.0 + tf.tv_usec/1000.0;
            // printf("thread readframe: %f mseconds\n",(end-start));
        }
        else if (status == 2)
        {
            // struct timeval ts, tf;
            // gettimeofday(&ts, NULL);
            // double start=ts.tv_sec * 1000.0 + ts.tv_usec/1000.0;
            for (size_t i = 0; i < CAMERA_NUMS; i++)
            {
                bool b = releaseframe(&vct[i], &wholeimages[i]);
                if (!b)
                {
                    g_camera = -1;
                    break;
                }
            }
            status = 0;
            // gettimeofday(&tf, NULL);
            // double end = tf.tv_sec * 1000.0 + tf.tv_usec/1000.0;
            // printf("thread releaseframe: %f mseconds\n",(end-start));
        }
        else if (status == 3) //退出线程
        {
            printf("thread close\n");
            pthread_exit(0);
        }

        while (status == 0)
        {
            //printf("wait\n");
            pthread_cond_wait(&g_cond, &g_mutex);
        }

        pthread_mutex_unlock(&g_mutex);
        usleep(500);
    }
}

int InitCameras()
{
    struct timeval ts, tf;
    gettimeofday(&ts, NULL);
    double start = ts.tv_sec * 1000.0 + ts.tv_usec / 1000.0;
    // Initialize the V4L2 device
    for (size_t i = 0; i < CAMERA_NUMS; i++)
    {
        vct[i].dev_name = dev_names[i];
        vct[i].fd = -1;
        vct[i].w = PIX_W;
        vct[i].h = PIX_H;
        vct[i].buffers_cnt = BUF_CNT;
        bool res = initcamera(&vct[i], &length);
        if (!res)
        {
            printf("[initialize_camera] Could not initialize the %s V4L2 device.\n", dev_names[i]);
            return -1;
        }

        CLEAR(wholeimages[i]);
        wholeimages[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        wholeimages[i].memory = V4L2_MEMORY_MMAP;
    }

    for (int i = 0; i < CAMERA_NUMS; ++i)
        yuyv[i] = (unsigned char *)calloc(1, sizeof(unsigned char) * length);

    //initiate mutex
    pthread_mutex_init(&g_mutex, NULL);
    pthread_cond_init(&g_cond, NULL);
    pthread_create(&g_thread, NULL, get, NULL);
    pthread_detach(g_thread);

    gettimeofday(&tf, NULL);
    double end = tf.tv_sec * 1000.0 + tf.tv_usec / 1000.0;
    printf("initialize_camera: %f mseconds\n", (end - start));
    return 0;
}

//获取数据
int GetPixels(unsigned char **image)
{
    struct timeval ts, tf;
    gettimeofday(&ts, NULL);
    double start = ts.tv_sec * 1000.0 + ts.tv_usec / 1000.0;

    //等待恢复状态
    while (status != 0)
    {
        usleep(500);
    };

    pthread_mutex_lock(&g_mutex);
    if (g_camera >= 0) //camera no error
    {
        for (size_t i = 0; i < CAMERA_NUMS; i++)
        {
            // bool b=readframe(&vct[i],&wholeimages[i]);
            // if (!b) {
            //     printf("[GetPixels] Could not GetPixels the %s V4L2 device.\n", vct[i].dev_name);;
            // }
            // image[i]=vct[i].buffers[wholeimages[i].index].start;
            image[i] = yuyv[i];
            // printf("index=%d\n",wholeimages[i].index);
            // usleep(500);
        }
        status = 2;
        pthread_cond_signal(&g_cond);
    }
    pthread_mutex_unlock(&g_mutex);

    gettimeofday(&tf, NULL);
    double end = tf.tv_sec * 1000.0 + tf.tv_usec / 1000.0;
    printf("GetPixels: %f mseconds\n", (end - start));
    return g_camera;
}

int ReleasePixels()
{
    struct timeval ts, tf;
    gettimeofday(&ts, NULL);
    double start = ts.tv_sec * 1000.0 + ts.tv_usec / 1000.0;

    //等待恢复状态
    while (status != 0)
    {
        usleep(500);
    };

    pthread_mutex_lock(&g_mutex);
    if (g_camera >= 0) //camera no error
    {
        status = 1;
        pthread_cond_signal(&g_cond);
    }
    pthread_mutex_unlock(&g_mutex);

    gettimeofday(&tf, NULL);
    double end = tf.tv_sec * 1000.0 + tf.tv_usec / 1000.0;
    printf("ReleasePixels: %f mseconds\n", (end - start));
    // usleep(5000);
    return g_camera;
}

//destory camera
int DestoryCameras()
{
    pthread_mutex_lock(&g_mutex);
    status = 3;
    pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_mutex);

    pthread_mutex_destroy(&g_mutex);
    pthread_cond_destroy(&g_cond);
    for (size_t i = 0; i < CAMERA_NUMS; i++)
    {
        stop(&vct[i]);
        if (yuyv[i])
            free(yuyv[i]);
    }
    return 0;
}
