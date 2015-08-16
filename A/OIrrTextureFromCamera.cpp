#include "StdAfx.h"
#include "OIrrTextureFromCamera.h"

OIrrTextureFromCamera::OIrrTextureFromCamera(IVideoDriver* driver, int cvflags, int use_rgba_texture)
{
   char unique_tex_name[50];
   IplImage* cv_img0 = 0;
   IplImage* cv_img1 = 0;
   
   //set this
   this->driver = driver;
   
   //start the capture
   //this->cv_capture = cvCaptureFromCAM(cvflags);
   //if(!this->cv_capture) return;
    cv_capture1 = cvCreateCameraCapture( 1 ); 
    cv_capture0 = cvCreateCameraCapture( 0 );
	//*******************************注意**********************************************
	//头盔的摄像头分辨率有三种选择1280*720,800*600,640*480，默认的是最小的，当调成最大的时采集图像会很卡。
	//支架上的摄像头分辨率最大是640*480
	cvSetCaptureProperty(cv_capture1, CV_CAP_PROP_FPS, 30);
	cvSetCaptureProperty(cv_capture1, CV_CAP_PROP_FRAME_WIDTH, 1280);
	cvSetCaptureProperty(cv_capture1, CV_CAP_PROP_FRAME_HEIGHT, 720);
	cvSetCaptureProperty(cv_capture0, CV_CAP_PROP_FPS, 30);
	cvSetCaptureProperty(cv_capture0, CV_CAP_PROP_FRAME_WIDTH, 1280);
	cvSetCaptureProperty(cv_capture0, CV_CAP_PROP_FRAME_HEIGHT, 720);
   
   //grab an image
	cv_img0 = cvQueryFrame(this->cv_capture0);
   cv_img1 = cvQueryFrame(this->cv_capture1);
   if(!cv_img0||!cv_img1) return;
   
   //for irrlicht scheme
   sprintf(unique_tex_name, "cv_image_CAM:%d", this);
   
   //make the texture
   if(use_rgba_texture)
      this->irr_texture0 = this->driver->addTexture(core::dimension2d<u32>(cv_img0->width, cv_img0->height), unique_tex_name, video::ECF_A8R8G8B8);
   else
    if(use_rgba_texture)
      this->irr_texture1 = this->driver->addTexture(core::dimension2d<u32>(cv_img1->width, cv_img1->height), unique_tex_name, video::ECF_A8R8G8B8);
   else
      this->irr_texture1 = this->driver->addTexture(core::dimension2d<u32>(cv_img1->width, cv_img1->height), unique_tex_name, video::ECF_R8G8B8);  this->irr_texture0 = this->driver->addTexture(core::dimension2d<u32>(cv_img0->width, cv_img0->height), unique_tex_name, video::ECF_R8G8B8);
   
   //finally
   this->copy_over_image(irr_texture0,cv_img0);
   this->copy_over_image(irr_texture1,cv_img1);
}

OIrrTextureFromCamera::~OIrrTextureFromCamera()
{
   
}

	/*******************************************************
	Function : 获取转换后引擎支持的材质文件
	Parameter: 
             1   Out   ITexture*     images    转换后的引擎图片材质文件 
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
void OIrrTextureFromCamera::getTexture(ITexture* images[])
{
   images[0]=irr_texture0;
   images[1]=irr_texture1;
}

/*******************************************************
	Function : 更新图片材质，从而不断渲染摄像头获得场景背景
	Parameter:无 
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
int OIrrTextureFromCamera::UpdateTexture()
{
   IplImage* cv_img0 = 0;
   IplImage* cv_img1 = 0;
   //grab an image
   cv_img0 = cvQueryFrame(this->cv_capture0);
   cv_img1 = cvQueryFrame(this->cv_capture1);
  /* cv_img0=cvLoadImage("1.jpg", -1);
   cv_img1=cvLoadImage("2.jpg", -1);*/
   if(!cv_img0||!cv_img1) return 0;
   cvFlip(cv_img0, NULL,0);    //核心在这里 
   cvFlip(cv_img1, NULL,0);    //核心在这里 
   this->copy_over_image(irr_texture0,cv_img0);
   this->copy_over_image(irr_texture1,cv_img1);

}

	/*******************************************************
	Function : 将从摄像头采集的图片转换为Irrlicht支持的格式文件
	Parameter: 
             1   Out   ITexture*     irr_texture     转换后的引擎图片材质文件
			 2   In    IplImage*     cv_img          摄像头采集的原始图片  
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
void OIrrTextureFromCamera::copy_over_image(ITexture* irr_texture,IplImage* cv_img)
{
   // read the pixels directly into the texture
   char* pixels = (char*)(irr_texture->lock());
   char* ardata = (char*)cv_img->imageData;
   char* final_loc = cv_img->imageSize + cv_img->imageData;

   switch(irr_texture->getColorFormat())
   {
      case ECF_R8G8B8:
         while(ardata < final_loc)
         {
            *pixels = *ardata;
            pixels++; ardata++;
            *pixels = *ardata;
            pixels++; ardata++;
            *pixels = *ardata;
            pixels++; ardata++;
         }
         break;
         
      case ECF_A8R8G8B8:
         while(ardata < final_loc)
         {
            *pixels = *ardata;
            pixels++; ardata++;
            *pixels = *ardata;
            pixels++; ardata++;
            *pixels = *ardata;
            pixels++; ardata++;
            
            pixels++;
         }
         break;
   }


   irr_texture->unlock();
}





