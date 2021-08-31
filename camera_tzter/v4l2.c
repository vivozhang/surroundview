#include "v4l2.h"
static int xioctl(int fh, int request, void *arg)
{
  int r;
  int num = 0;
  do
  {
    r = ioctl(fh, request, arg);
    ++num;
  } while (-1 == r && num < 3);
  return r;
}
bool initcamera(struct v4l2_device *vd, int *length)
{
  vd->fd = open(vd->dev_name, O_RDWR, 0); //| O_NONBLOCK
  if (vd->fd < 0)
  {
    printf("[v4l2] Cannot open '%s': %d, %s\n", vd->dev_name, errno, strerror(errno));
    return false;
  }

  struct v4l2_capability cap;
  struct v4l2_format fmt;

  if (xioctl(vd->fd, VIDIOC_QUERYCAP, &cap) < 0)
  {
    printf("[v4l2] %s is no V4L2 device\n", vd->dev_name);
    close(vd->fd);
    return false;
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
  {
    printf("[v4l2] %s is no V4L2 video capturing device\n", vd->dev_name);
    close(vd->fd);
    return false;
  }
  if (!(cap.capabilities & V4L2_CAP_STREAMING))
  {
    printf("[v4l2] %s isn't capable of streaming (TODO: support reading)\n", vd->dev_name);
    close(vd->fd);
    return false;
  }

  CLEAR(fmt);
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = vd->w;
  fmt.fmt.pix.height = vd->h;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

  if (xioctl(vd->fd, VIDIOC_S_FMT, &fmt) < 0)
  {
    printf("[v4l2] Could not set data format settings of %s\n", vd->dev_name);
    close(vd->fd);
    return false;
  }

  struct v4l2_requestbuffers req;
  CLEAR(req);
  req.count = vd->buffers_cnt;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (xioctl(vd->fd, VIDIOC_REQBUFS, &req) < 0)
  {
    printf("[v4l2] %s Does not support memory mapping\n", vd->dev_name);
    close(vd->fd);
    return false;
  }

  // Allocate memory for the memory mapped buffers
  vd->buffers = (struct buffer *)calloc(req.count, sizeof(struct buffer));
  if (vd->buffers == NULL)
  {
    printf("[v4l2] Not enough memory for %s to initialize %d MMAP buffers\n", vd->dev_name, req.count);
    close(vd->fd);
    return false;
  }

  for (int i = 0; i < req.count; ++i)
  {
    struct v4l2_buffer buf;
    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;

    if (xioctl(vd->fd, VIDIOC_QUERYBUF, &buf) < 0)
    {
      printf("[v4l2] Querying buffer %d from %s failed\n", i, vd->dev_name);
      close(vd->fd);
      return false;
    }

    //  Map the buffer
    vd->buffers[i].length = buf.length;
    // printf("length:%d,%d\n",buf.length,buf.bytesused);
    *length = buf.length;
    vd->buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, vd->fd, buf.m.offset);
    if (MAP_FAILED == vd->buffers[i].start)
    {
      printf("[v4l2] Mapping buffer %d with length %d from %s failed\n", i, buf.length, vd->dev_name);
      close(vd->fd);
      return false;
    }
  }

  //start capture
  enum v4l2_buf_type type;
  struct v4l2_buffer buf;
  for (int i = 0; i < req.count; i++)
  {
    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    if (xioctl(vd->fd, VIDIOC_QBUF, &buf) < 0)
    {
      printf("[v4l2] Could not enqueue buffer %d during start capture for %s\n", buf.index, vd->dev_name);
      close(vd->fd);
      return false;
    }
  }

  //set streaming on
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (xioctl(vd->fd, VIDIOC_STREAMON, &type) < 0)
  {
    printf("[v4l2] Could not start stream of %s, %d %s\n", vd->dev_name, errno, strerror(errno));
    close(vd->fd);
    return false;
  }
  return true;
}

bool readframe(struct v4l2_device *vd, struct v4l2_buffer *buf)
{
  // struct timeval ts;
  // gettimeofday(&ts, NULL);
  // printf("\n[readframe][%lu.%lu]\t %s before get picture\n", ts.tv_sec, ts.tv_usec,vd->dev_name);
  if (-1 == xioctl(vd->fd, VIDIOC_DQBUF, buf))
  {
    printf("[v4l2_error] readframe of %s \n", vd->dev_name);
    return false;
  }
  // gettimeofday(&ts, NULL);
  // printf("\n[readframe][%lu.%lu]\t %s get image index = %d.\n", ts.tv_sec, ts.tv_usec,vd->dev_name,buf->index);
  return true;
}

bool releaseframe(struct v4l2_device *vd, struct v4l2_buffer *buf)
{
  // struct timeval ts;
  // gettimeofday(&ts, NULL);
  // printf("\n[postreadframe][%lu.%lu]\tbefore get picture\n", ts.tv_sec, ts.tv_usec);
  if (-1 == xioctl(vd->fd, VIDIOC_QBUF, buf))
  {
    printf("[v4l2_error] releaseframe of %s\n", vd->dev_name);
    return false;
  }
  // gettimeofday(&ts, NULL);
  // printf("\n[postreadframe][%lu.%lu]\tget image index = %d.\n", ts.tv_sec, ts.tv_usec,buf->index);
  return true;
}

bool stop(struct v4l2_device *vd)
{
  enum v4l2_buf_type type;
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (xioctl(vd->fd, VIDIOC_STREAMOFF, &type) < 0)
  {
    printf("[v4l2] Could not stop stream of %s\n", vd->dev_name);
    return false;
  }
  if (vd->buffers)
  {
    free(vd->buffers);
    vd->buffers = NULL;
  }
  close(vd->fd);
  return true;
}
