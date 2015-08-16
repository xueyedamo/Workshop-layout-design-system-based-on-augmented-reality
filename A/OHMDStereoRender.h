/*******************************************************
  Function : // ������ʾ�࣬����Oculus Riftͷ����ʾ����HMD����ʾ���峡��ͼ��
  CopyRight: // ��Ȩ����(Public)
  Author   : // �ŷ� Eli zhang
  ================= Modified list ====================  
  Version    : // v1.2.0
  Modify Date: // 2015.02.20
  Mender     : // zhangxuanyu0904@163.com
  Why        : // ������ʾ�࣬���ɾ�������ƫ�������ͼ��
  How        : // ���з���������Ⱦ
  Attention  : // ע������
*******************************************************/

#pragma once

#ifndef __HMD_STEREO_RENDER__
#define __HMD_STEREO_RENDER__

#include <irrlicht.h>

/*******************************************************
	Function : HMD�Ĳ����ṹ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
struct HMDDescriptor 
{
  int hResolution;//����ֱ���
  int vResolution;//����ֱ���
  float hScreenSize;//������Ļ�ߴ�
  float vScreenSize;//������Ļ�ߴ�
  float interpupillaryDistance;//ͫ�׾���
  float lensSeparationDistance;//��ͷ���
  float eyeToScreenDistance;//�۵���Ļ����
  float distortionK[4];//������
};

class OHMDStereoRender
{
public:
	/*******************************************************
	Function : OHMDStereoRender�๹�캯��
	Parameter: 
             1   In    irr::IrrlichtDevice *   device     ������豸ָ��
			 2   In    HMDDescriptor           HMD        ͷ����ʾ���Ĳ����ṹ��  
			 3   In    irr::f32                worldScale ��������
	Return   : ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
  OHMDStereoRender(irr::IrrlichtDevice *device, HMDDescriptor HMD, irr::f32 worldScale = 1.0);
  ~OHMDStereoRender();

  HMDDescriptor getHMD(); 

  /*******************************************************
	Function : �趨ͷ����ʾ���Ĳ����ṹ��
	Parameter:
			 1   In    HMDDescriptor   HMD   ͷ����ʾ���Ĳ����ṹ��  
	Return   : ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
  void setHMD(HMDDescriptor HMD);

  irr::f32 getWorldScale(); 
  void setWorldScale(irr::f32 worldScale);

  /*******************************************************
	Function : ��Ⱦ�������г���ģ�ͼ�����
	Parameter: 
             1   In    scene::ISceneManager*   smgr       ���泡��������ָ��
			 2   In    video::ITexture*        images     ���ұ���ͼƬ���� 
	Return   : ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
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
