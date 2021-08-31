#include "camera.h"
#ifdef OPENCV3
#include <opencv2/highgui/highgui.hpp>
//opencv版本为3.2版本
int InitCameras()
{
	try {
				std::string cam_fmt="ABGR32";  //rgba
				// if(GROUP_NUM<=1)
				{
					std::string cam_devname = "/dev/video0";      // A组
					int window_num=GROUP_A_CAMERA_NUM;
					mvcam[0] = new miivii::MvGmslCamera(cam_devname,window_num,cam_w,cam_h,fps,imgWidth,imgHeight,cam_fmt);
					bool group_a_key(true);
					if(mvcam[0]) std::cout << "Camera type in group A is " << mvcam[0]->GetCameraType(group_a_key) << std::endl;
					else return -1;
				}
				if(GROUP_NUM==2)                                                      // B组
				{
					std::string cam_devname = "/dev/video1";
					int window_num=GROUP_B_CAMERA_NUM;
					mvcam[1] = new miivii::MvGmslCamera(cam_devname,window_num,cam_w,cam_h,fps,imgWidth,imgHeight,cam_fmt);
					bool group_b_key(true);
					if(mvcam[1]) std::cout << "Camera type in group B is " << mvcam[1]->GetCameraType(group_b_key) << std::endl;
					else return -1;
				}
	}
	catch(...)
	{
		std::cout<<"init err:"<<std::endl;
		return -1;
	}
	return 0;
}

int DestoryCameras()
{
	for(int i=0;i<GROUP_NUM;i++)
	{
		if(mvcam[i])
		{
			delete mvcam[i];
			mvcam[i]=NULL;
		}
	}
	std::cout<<"DestoryCameras successfully."<<std::endl;
}

void show(unsigned char ** input)
{
    std::string windowName="rgba";
    for (size_t i = 0; i < CAMERA_NUM; i++)
    {
        cv::Mat imgbuf=cv::Mat(imgHeight, imgWidth, CV_8UC4, input[i]);
        cv::Mat mrgba(imgHeight, imgWidth, CV_8UC3);
        cv::cvtColor(imgbuf, mrgba, CV_RGBA2BGR);
		cv::imwrite("./front.png",mrgba);
        cv::imshow(windowName + std::to_string(i), mrgba);
    }
    cv::waitKey(1);
}

int  GetPixels(unsigned char ** buf)
{      
	try
   {
		//std::cout<<"1111"<<std::endl;
		uint64_t timestamp=0;
		//if(GROUP_NUM<=1)    //A组
		{
			//std::cout<<"2222"<<std::endl;
			// camera : front back left right
			if (mvcam[0]&&mvcam[0]->GetImagePtr(group_a_buf, timestamp))
			{
				//buf : front back left right
				for(int i=0;i<GROUP_A_CAMERA_NUM;i++)
					buf[i]=group_a_buf[i];
				// buf[1]=group_a_buf[0];
				// buf[2]=group_a_buf[0];
				// buf[3]=group_a_buf[0];
				// buf[4]=group_a_buf[0];
			}
			else
			{
				// std::cout << "Can't get image form group A." << std::endl;
				return -1;
			}
		}
		
		if(GROUP_NUM==2)   //B组
		{
			//std::cout<<"333"<<std::endl;
			// camera : front back left right
			if (mvcam[1]&&mvcam[1]->GetImagePtr(group_b_buf, timestamp))
			{
				//outbuf : front back left right
				for(int i=0;i<GROUP_B_CAMERA_NUM;i++)
					buf[GROUP_A_CAMERA_NUM+i]=group_b_buf[i];
			}
			else
			{
				// std::cout << "Can't get image form group A." << std::endl;
				return -1;
			}
		}
   }
  catch(const std::exception& e)
  {
	  std::cout << e.what() << std::endl;
	  return -1;
  }
  return 0;
}
#endif
