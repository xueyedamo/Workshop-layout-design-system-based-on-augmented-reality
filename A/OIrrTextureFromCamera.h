/*******************************************************
  Function : // 图片采集类，从摄像头采集图片并转换为Irrlicht格式材质，用于Oculus Rift头盔显示器（HMD）渲染的背景图像
  CopyRight: // 版权公开(Public)
  Author   : // 张帆 Eli zhang
  ================= Modified list ====================  
  Version    : // v1.2.0
  Modify Date: // 2015.02.20
  Mender     : // zhangxuanyu0904@163.com
  Why        : // 从摄像头采集场景背景图片
  How        : // 将背景图片转换为引擎支持的材质图片
  Attention  : // 注意事项
*******************************************************/
#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class OIrrTextureFromCamera
{
   public:
      OIrrTextureFromCamera(IVideoDriver* driver, int cvflags = 0, int use_rgba_texture = 0);
      ~OIrrTextureFromCamera();

	/*******************************************************
	Function : 获取转换后引擎支持的材质文件
	Parameter: 
             1   Out   ITexture*     images     转换后的引擎图片材质文件 
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
    void getTexture(ITexture* images[]);

	/*******************************************************
	Function : 更新图片材质，从而不断渲染摄像头获得场景背景
	Parameter:无 
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
      int UpdateTexture();
	  
   private:
      //needed to manipulate irr textures
      IVideoDriver* driver;
      
      //Irrlicht引擎的texture
      ITexture* irr_texture0;
      ITexture* irr_texture1;

      //cv capture通过摄像头获取图片
      CvCapture* cv_capture0;
	   CvCapture* cv_capture1;
      
	/*******************************************************
	Function : 将从摄像头采集的图片转换为Irrlicht支持的格式文件
	Parameter: 
             1   Out   ITexture*     irr_texture     转换后的引擎图片材质文件
			 2   In    IplImage*     cv_img          摄像头采集的原始图片  
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
      void copy_over_image(ITexture* irr_texture,IplImage* cv_img);
};
