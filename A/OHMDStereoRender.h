/*******************************************************
  Function : // 立体显示类，用于Oculus Rift头盔显示器（HMD）显示立体场景图像
  CopyRight: // 版权公开(Public)
  Author   : // 张帆 Eli zhang
  ================= Modified list ====================  
  Version    : // v1.2.0
  Modify Date: // 2015.02.20
  Mender     : // zhangxuanyu0904@163.com
  Why        : // 立体显示类，生成具有立体偏差的两个图像
  How        : // 进行分屏两次渲染
  Attention  : // 注意事项
*******************************************************/

#pragma once

#ifndef __HMD_STEREO_RENDER__
#define __HMD_STEREO_RENDER__

#include <irrlicht.h>

/*******************************************************
	Function : HMD的参数结构体
	Author   : 张帆 Eli zhang
	*******************************************************/
struct HMDDescriptor 
{
  int hResolution;//竖向分辨率
  int vResolution;//横向分辨率
  float hScreenSize;//竖向屏幕尺寸
  float vScreenSize;//横向屏幕尺寸
  float interpupillaryDistance;//瞳孔距离
  float lensSeparationDistance;//镜头间距
  float eyeToScreenDistance;//眼到屏幕距离
  float distortionK[4];//畸变率
};

class OHMDStereoRender
{
public:
	/*******************************************************
	Function : OHMDStereoRender类构造函数
	Parameter: 
             1   In    irr::IrrlichtDevice *   device     引擎的设备指针
			 2   In    HMDDescriptor           HMD        头盔显示器的参数结构体  
			 3   In    irr::f32                worldScale 场景比率
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
  OHMDStereoRender(irr::IrrlichtDevice *device, HMDDescriptor HMD, irr::f32 worldScale = 1.0);
  ~OHMDStereoRender();

  HMDDescriptor getHMD(); 

  /*******************************************************
	Function : 设定头盔显示器的参数结构体
	Parameter:
			 1   In    HMDDescriptor   HMD   头盔显示器的参数结构体  
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
  void setHMD(HMDDescriptor HMD);

  irr::f32 getWorldScale(); 
  void setWorldScale(irr::f32 worldScale);

  /*******************************************************
	Function : 渲染绘制所有场景模型及背景
	Parameter: 
             1   In    scene::ISceneManager*   smgr       引擎场景管理器指针
			 2   In    video::ITexture*        images     左右背景图片数组 
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
  void drawAll(irr::scene::ISceneManager* smgr,irr::video::ITexture* images[]);

private:  
  irr::video::IVideoDriver* _driver;
  irr::video::ITexture* _renderTexture;
  irr::scene::ISceneManager* _smgr;

  HMDDescriptor _HMD;
  irr::f32 _worldScale;
  irr::core::matrix4 _projectionLeft;
  irr::core::matrix4 _projectionRight;
  irr::f32 _eyeSeparation;
  irr::f32 _lensShift;

  irr::core::rect<irr::s32> _viewportLeft;
  irr::core::rect<irr::s32> _viewportRight;

  irr::scene::ICameraSceneNode* _pCamera;

  irr::video::SMaterial _renderMaterial;
  irr::video::S3DVertex _planeVertices[4];
  irr::u16 _planeIndices[6];
  irr::ITimer* _timer;

  class OculusDistorsionCallback: public irr::video::IShaderConstantSetCallBack 
  { 
  public:
    irr::f32 scale[2];
    irr::f32 scaleIn[2];
    irr::f32 lensCenter[2];
    irr::f32 hmdWarpParam[4];
    virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) 
    { 
      irr::video::IVideoDriver* driver = services->getVideoDriver();
      services->setPixelShaderConstant("scale", scale, 2);
      services->setPixelShaderConstant("scaleIn", scaleIn ,2);
      services->setPixelShaderConstant("lensCenter", lensCenter ,2);
      services->setPixelShaderConstant("hmdWarpParam", hmdWarpParam ,4);
    }
  };
  OculusDistorsionCallback _distortionCB;
};
#endif
