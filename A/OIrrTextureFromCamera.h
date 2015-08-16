/*******************************************************
  Function : // ͼƬ�ɼ��࣬������ͷ�ɼ�ͼƬ��ת��ΪIrrlicht��ʽ���ʣ�����Oculus Riftͷ����ʾ����HMD����Ⱦ�ı���ͼ��
  CopyRight: // ��Ȩ����(Public)
  Author   : // �ŷ� Eli zhang
  ================= Modified list ====================  
  Version    : // v1.2.0
  Modify Date: // 2015.02.20
  Mender     : // zhangxuanyu0904@163.com
  Why        : // ������ͷ�ɼ���������ͼƬ
  How        : // ������ͼƬת��Ϊ����֧�ֵĲ���ͼƬ
  Attention  : // ע������
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
	Function : ��ȡת��������֧�ֵĲ����ļ�
	Parameter: 
             1   Out   ITexture*     images     ת���������ͼƬ�����ļ� 
	Return   : ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
    void getTexture(ITexture* images[]);

	/*******************************************************
	Function : ����ͼƬ���ʣ��Ӷ�������Ⱦ����ͷ��ó�������
	Parameter:�� 
	Return   : ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
      int UpdateTexture();
	  
   private:
      //needed to manipulate irr textures
      IVideoDriver* driver;
      
      //Irrlicht�����texture
      ITexture* irr_texture0;
      ITexture* irr_texture1;

      //cv captureͨ������ͷ��ȡͼƬ
      CvCapture* cv_capture0;
	   CvCapture* cv_capture1;
      
	/*******************************************************
	Function : ��������ͷ�ɼ���ͼƬת��ΪIrrlicht֧�ֵĸ�ʽ�ļ�
	Parameter: 
             1   Out   ITexture*     irr_texture     ת���������ͼƬ�����ļ�
			 2   In    IplImage*     cv_img          ����ͷ�ɼ���ԭʼͼƬ  
	Return   : ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
      void copy_over_image(ITexture* irr_texture,IplImage* cv_img);
};
