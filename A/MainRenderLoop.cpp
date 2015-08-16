// MainRenderLoop.cpp: ����Ŀ�ļ���

#include "stdafx.h"

#ifdef _MSC_VER
// We'll also define this to stop MSVC complaining about sprintf().

#define span 140
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")//����Irrlicht���涯̬���ӿ�
#endif

#include <irrlicht.h>
#include "driverChoice.h"
#include <iostream> 
#include <string>   
#include <io.h>
#include<vector>
#include <sstream>
#include<map>
#include<deque>
#include <math.h> 
#include "OIrrTextureFromCamera.h"
#include "OMobileTextComment.h"
#include "ODynamicGestureDistinguish.h"
#include "OStaticGestureDistinguish.h"
#include "OCollaborationCommunication.h"
#include "OHMDStereoRender.h"
#include"Tracker.h"
#include "PDI.h"
#include <process.h>

using namespace std;//��Irrlicht����������ռ����ַ�������Щ��ͻ  
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

/********************ͷ����ʾ�������趨*************************/
  //���ͬ��������ѡ��1280*800,800*600,640*480
#define SCREEN_WIDTH   1280
#define SCREEN_HEIGHT  800
#define FULLSCREEN     1


//��������
//�ӷ��������յ��������
deque<std::string> m_deqStrComand;
//�������ͨ����ʵ����
OCollaborationCommunication *m_colCommunication = new OCollaborationCommunication("127.0.0.1");
//���ֻ�ͨ����Ķ���ʵ����
OMobileTextComment  *m_OMobileTextComment = new OMobileTextComment();
//��̬����ʶ����ʵ����
OStaticGestureDistinguish *m_staticGestureDis = new OStaticGestureDistinguish(60);
//��̬����ʶ�����ʵ����
ODynamicGestureDistinguish *m_dynamicGestureDis = new ODynamicGestureDistinguish();
//�Ÿ�����������
Tracker* m_tracker == new Tracker();

//������Ķ���ʵ����
IrrlichtDevice*   m_IrrDevice;//�����豸ʵ��
IVideoDriver*     m_IrrDriver;//��������ʵ��
ISceneManager*    m_IrrSmgr;//���������ʵ��
scene::ITriangleSelector* m_ItriElector = 0;//����������ײ��������ѡ����
scene::ICameraSceneNode* m_ICamera ;//��������������ͷ

///////////////////////////���������߼�����////////////////////////////////////
bool   m_bModelStore=false;//����ģ�Ϳ�
bool   m_bNodeCapture=false;//ץȡģ��
bool   m_bNodeDelete=false ;//ɾ��ģ��
bool   m_bNodeRotate=false;//��תģ��
bool   m_bUseDataGlove=false;//ʹ����������
int    m_iStaticGestureNumber=-1;//��̬�������
int    m_iGloveDataCount=0;//�ɼ�������������
double m_aadGloveData[90][15];//����ɼ��Ķ�̬��������
double m_aadPreGloveData[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//�洢ǰһ֡����
double m_aadPrePreGloveData[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//�洢��ǰһ֡����
int    m_bRelativeDistance=0;//ץȡģ��ʱ����ģ�;���
vector3df m_vec3RelativePosition;//ץȡ֮ǰ������ײ���ģ�����������λ��
//��ת�õ��ı���
vector2df m_vec2PrePositionXY,m_vec2PrePositionXZ,m_vec2PrePositionYZ,m_vec2CurPositionXY,m_vec2CurPositionYZ,m_vec2CurPositionXZ;//������ת����ʱ��¼��ʼ��������
bool m_bFirstRotate=true;//�жϵ�һ��������ת������ת
core::vector3df m_vec3Rotation;//ģ��ԭʼ�ĽǶ�

///////////////////////////////////��ģ�Ϳ���ر�������///////////////////////////////////////
vector<std::string> m_vecStrModelName;//�����ȡÿ��ģ���ļ�������
std::map<std::string,IAnimatedMeshSceneNode*> m_mapNodeStore;//ģ�Ϳ�����ֺͽڵ��
//��ǰ��ʾ��ģ�Ϳ�ҳ��
int m_iCurModelPage=0;
//ģ�Ϳ����ҳ��
int m_iTotalPageNumber=0;
//ÿҳ��ʾģ�͵�����
int m_iPageModelCount=3;
//��һ��������ҳ��־ʱ��ҳ
bool m_bPgUpDnFirst=true;

//�볡���ڵ���ص�һЩ����
int m_iModelNodeID=0;//���ģ�͵�ID�ţ���0������
std::map<int,std::string> m_mapIdNameStore;//��ų����ڵ�ID��ģ������ӳ���ϵ
//��ڵ���ص�һЩ����
//��ǰ�ͻ���ѡ�е�ģ�ͽڵ�ID
int m_iSelectedModelID=-1;

//����������ײ��������
core::line3d<f32> m_line3TestRay;
//�������ߵĲ���
video::SMaterial m_SMmaterial;
bool m_bIsDrawLine = false;

//���������¼��࣬��׽���������¼�
class IrrEventReceiver  : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
		{
			KeyIsDown[0][event.KeyInput.Key]=KeyIsDown[1][event.KeyInput.Key];
			KeyIsDown[1][event.KeyInput.Key] = event.KeyInput.PressedDown;
		}
		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[1][keyCode];
	}
	virtual bool IsKeyUp(EKEY_CODE keyCode) 
	{
		bool keyUp=KeyIsDown[0][keyCode]&&!KeyIsDown[1][keyCode];
		KeyIsDown[0][keyCode]=KeyIsDown[1][keyCode];
		return keyUp;
	}
	IrrEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			for (u32 j=0;j<2;j++)
			{
			   KeyIsDown[j][i] = false;
			}
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[2][KEY_KEY_CODES_COUNT];
};

////////////////////////////////�߳�ִ�к�������///////////////////////////////////////////
//�����ֻ�ͨ�ŵ��߳�
void TheadFunOMobileTextComment();
//ʶ��̬���Ƶ��߳�ִ�к���
void ThreadFunDynGesRec();
//���շ�������Ϣ�̴߳�����
void ThreadFunCmdRecvProc(SOCKET mySocket);
//��ȡ�Ÿ����������ݽ���
void Thread_tracker(void *t1);

////////////////////////////////////////��������////////////////////////////////////////////
//����ģ�ͺ���
bool LoadModels(ISceneNode* m_ICamera,std::map<std::string,IAnimatedMeshSceneNode*>& myNode);
//��̬����ʶ����
bool StaticGestureRecognition(IrrEventReceiver& receiver);
//��תģ�ͺ���
void RotateModel(scene::ISceneNode * pickdSceneNode);

//������
int main(int argc, char* argv[])
{
	InitializeCriticalSection(&g_cs);
    /////////////////////////////////////////���������߳�///////////////////////////////////////////
    HANDLE hThread;
    DWORD dThreadID;
	//���������������ͨ�ŵ��߳�
    hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadFunCmdRecvProc,(void*)(m_colCommunication->mySocket),0,&dThreadID);

    //�������ֻ�ͨ�ŵ��߳�
    hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TheadFunOMobileTextComment,NULL,0,&dThreadID);

	//��ʼ���Ÿ�������,�����̣߳���ʼ��ȡ�Ÿ�����������;
	if(m_tracker->TrackerInitialize())
	{
		_beginthread(Thread_tracker,0,NULL);
	}
	else
	{
		return 0;
	}

   //��������ʶ��̬���Ƶ��߳�
   if(m_bUseDataGlove)
   {
       hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadFunDynGesRec,NULL,0,&dThreadID);
   }
	
	//////////////////////////////////Irrlicht�����ʼ��/////////////////////////////////////
    IrrEventReceiver receiver;//�����¼�
	//���洴���豸
    SIrrlichtCreationParameters params = SIrrlichtCreationParameters();
    params.AntiAlias = 50;
	params.Fullscreen=FULLSCREEN;
    params.DriverType = video::EDT_OPENGL;
    params.WindowSize = core::dimension2d<u32>(SCREEN_WIDTH, SCREEN_HEIGHT);
	params.EventReceiver=&receiver;
    m_IrrDevice = createDeviceEx(params);
    m_IrrDriver = m_IrrDevice->getVideoDriver();
    m_IrrSmgr = m_IrrDevice->getSceneManager();
    IGUIEnvironment* guienv = m_IrrDevice->getGUIEnvironment();
    m_IrrDevice->setWindowCaption(L"���䲼�ֲ��԰�");

 //////////////////////////////////ͷ��������ʾ���ʼ��//////////////////////////////////////
   HMDDescriptor HMD;
  // Parameters from the Oculus Rift DK1
  //���ͬ��������ѡ��1280*800,800*600,640*480
  HMD.hResolution = SCREEN_WIDTH;
  HMD.vResolution = SCREEN_HEIGHT;
  HMD.hScreenSize = 0.14976;
  HMD.vScreenSize = 0.0936;
  HMD.interpupillaryDistance = 0.064;
  HMD.lensSeparationDistance = 0.064;
  HMD.eyeToScreenDistance = 0.041;
  HMD.distortionK[0] = 1.0;
  HMD.distortionK[1] = 0.22;
  HMD.distortionK[2] = 0.24;
  HMD.distortionK[3] = 0.0;

  OHMDStereoRender l_HMDrender(m_IrrDevice, HMD, 10);

    //��������ͷ
   video::ITexture* l_aITexImages[2];//��������ָ��
   OIrrTextureFromCamera* l_TextureFromCam = new OIrrTextureFromCamera(m_IrrDriver,0,0);
   l_TextureFromCam->getTexture(l_aITexImages);

   //��ײ����������
   scene::ISceneNode*  l_PickedStoreNode=0;//������ײ����ģ�Ϳ�ڵ�
   scene::ISceneNode*  l_PickedSceneNode=0;//������ײ���ĳ����ڵ�
   scene::ISceneNode*  l_PreSelectNode=0;//��¼֮ǰѡ�еĽڵ�
   scene::ISceneNode*  l_SelectedSceneNode = 0;//ѡ�е�ģ�ͽڵ�
   scene::ISceneCollisionManager* l_collManager = m_IrrSmgr->getSceneCollisionManager();
   // ��ǰ������ײ��λ��
   core::vector3df l_vec3IntersectionPos;
   // ������ײ��������Ƭ
   core::triangle3df l_tri3HitTriangle;
   ////////////////////////////////////////���ؽڵ�////////////////////////////////////////////////////////
   //������������ͷ�ڵ㼰�趨����ͷ����
   	m_ICamera = m_IrrSmgr->addCameraSceneNode();
    m_ICamera->setPosition(vector3df(m_tracker->CameraPositionPx, m_tracker->CameraPositionPy, m_tracker->CameraPositionPz));
    m_ICamera->setTarget(vector3df(m_tracker->TargetPositionPx,m_tracker->TargetPositionPy,m_tracker->TargetPositionPz));
	m_ICamera->setFarValue(20000.0);//�����������Զ�ļ�����
	m_ICamera->setAspectRatio(4.0f/3.0f);
	m_ICamera->setFOV(PI / m_tracker->AngleField);
	m_ICamera->setUpVector(vector3df(0,1,0));
	

   //����ģ�Ϳ���ģ��
	LoadModels(m_ICamera,m_mapNodeStore);

	//��ӵƹ⼰����ƽڵ�
	scene::IBillboardSceneNode * bill = m_IrrSmgr->addBillboardSceneNode();
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
	bill->setMaterialTexture(0, m_IrrDriver->getTexture("particle.bmp"));
	bill->setMaterialFlag(video::EMF_LIGHTING, true);
	bill->setMaterialFlag(video::EMF_ZBUFFER, false);
	bill->setSize(core::dimension2d<f32>(40.0f, 40.0f));
	bill->setVisible(false);

	//��ӵƹ���
	scene::ILightSceneNode* nodeLight1=m_IrrSmgr->addLightSceneNode(m_ICamera,core::vector3df(50, 100, -10),video::SColorf(1,1,1),100);
	nodeLight1->setLightType(video::ELT_DIRECTIONAL);
   //////////////////////////////////��ʱ���ʱ���������///////////////////////////////////////////////////////////////////////////
	// ����������֮֡���ʱ��
   u32 uThenTime = m_IrrDevice->getTimer()->getTime();
   // ÿ���ƶ�������
   const f32 MOVEMENT_SPEED = 0.5f;
   //�����ʱ��ʱ�����
   u32 timeBegin= m_IrrDevice->getTimer()->getTime();
   //���巭ҳʱ�������
   u32 pgTimeBegin=m_IrrDevice->getTimer()->getTime();
   u32 pgUpDnDelay=0;
   vector3df test;
   //////////////////////////////////////////������Ⱦ��ѭ��//////////////////////////////////////////////////
   while(m_IrrDevice->run())
   {
	   //����������
	   if (m_bUseDataGlove)
	   {
		   wchar_t wchar[10]={0};
		   //�����ٽ���
		   EnterCriticalSection(&g_cs);
		   int len=swprintf( wchar ,9, L"%d", m_iStaticGestureNumber );
		   LeaveCriticalSection(&g_cs);
		   m_IrrDevice->setWindowCaption(wchar);
	   }

	   //���ݴŸ��������¹��շ���
	   nodeLight1->setRotation(m_ICamera->getTarget()-m_ICamera->getPosition());
	   //��������ͷ��λ��
	   m_ICamera->setUpVector(core::vector3df(m_tracker->CameraUpPx,m_tracker->CameraUpPy,m_tracker->CameraUpPz));
	   m_ICamera->setPosition(vector3df(m_tracker->CameraPositionPx, m_tracker->CameraPositionPy, m_tracker->CameraPositionPz));
	   m_ICamera->setTarget(vector3df(m_tracker->TargetPositionPx,m_tracker->TargetPositionPy,m_tracker->TargetPositionPz));	   
	   m_ICamera->setRotation(vector3df(-m_tracker->CameraEulerPitch/3.14*180.0,m_tracker->CameraEulerYaw/3.14*180.0,m_tracker->CameraEulerRoll/3.14*180.0));
	  
	   //��������ͷ��������
      l_TextureFromCam->UpdateTexture();
	  // �����֡ʱ���
	  const u32 uNowTime = m_IrrDevice->getTimer()->getTime();
	  const f32 fFrameDeltaTime = (f32)(uNowTime - uThenTime) / 1000.f; // Time in seconds
	  uThenTime = uNowTime;
      //��ʼ��Ⱦ
      m_IrrDriver->beginScene(true, true, SColor(255,100,101,140));
	  //���Ʊ���ͼƬ��ģ��
      l_HMDrender.drawAll(m_IrrSmgr,l_aITexImages,m_line3TestRay,m_bIsDrawLine);
	  //��̬����ʶ��
	  StaticGestureRecognition(receiver);

	  //////////////////////////////////////////�������������������////////////////////////////////////////
	  //��ŷָ����ַ���������
	  vector<std::string> vectorStr;
	  EnterCriticalSection(&g_cs);//����m_deqStrComand

	  if(m_deqStrComand.size()>0)
	  {
		  char buf[1024];
		  std::string str=m_deqStrComand.front();
		  //���ַ���ת���ַ�����
		  for(int i=0;i<str.length();i++)
		  {
			  buf[i] = str[i];
		  }
		  buf[str.length()] = '\0';
		  //�����֮��Ҫɾ����
		  m_deqStrComand.pop_front();
		  //���ڷָ��ַ�������ַ�
		  const char *d = "@";
		  char *p;
		  p = strtok(buf,d);
		  //�ַ�����ķָ�
		  while(p)
		  {
			  std::string strRec(p);
			  //printf("%s\n",p);
			  vectorStr.push_back(strRec);
			  p=strtok(NULL,d);
		  }
	  }

	  LeaveCriticalSection(&g_cs);//�ͷ�m_deqStrComand
	  //������������ʱ
	  if(vectorStr.size()>1)
	  {
		    //��Ϊ���ģ������ʱ
			if(vectorStr[1]=="addModel")
			{
				//���ģ�ͽڵ�
				std::string strModelName=vectorStr[2];
				IAnimatedMeshSceneNode* addNode=m_mapNodeStore[strModelName];
				ISceneNode* modelNodeAdd=addNode->clone(0,m_IrrSmgr);//������ӵĽ��
				m_mapIdNameStore[m_iModelNodeID]=strModelName;//����ID���ּ�ֵ��
				modelNodeAdd->setID(m_iModelNodeID);//���ý��ID��

				m_iModelNodeID++;//ID��һΪ�´μ���ģ�ͽ����׼��

				modelNodeAdd->setVisible(true);//ʹģ����ʾ
				//ģ�Ͱ�Χ������
				modelNodeAdd->setDebugDataVisible(irr::scene::EDS_OFF);

				//Ϊ��ӽ�㴴������ѡ����
				m_ItriElector = m_IrrSmgr->createTriangleSelector((IAnimatedMeshSceneNode *)modelNodeAdd);
				modelNodeAdd->setTriangleSelector(m_ItriElector);
				modelNodeAdd->setParent(m_IrrSmgr->getRootSceneNode());
				m_ItriElector->drop(); // We're done with this m_ItriElector, so drop it now.
				modelNodeAdd->setPosition(vector3df(0,-50,150));
				m_ICamera->removeChild(modelNodeAdd);
			}

			//��Ϊѡ��ģ������ʱ
			if(vectorStr[1]=="selectModel")
			{
				//ͨ��ID���ģ�ͽڵ�
				int selectID=atoi(vectorStr[2].c_str());
				//�Ѹñ�ѡ��ģ������Ϊ��͸��
				m_IrrSmgr->getSceneNodeFromId(selectID)->setDebugDataVisible(irr::scene::EDS_HALF_TRANSPARENCY);
			}

			//��Ϊȡ��ѡ������ʱ
			if(vectorStr[1]=="cancelSelectModel")
			{
				//ͨ��ID���ģ�ͽڵ�
				int cancelSelectID=atoi(vectorStr[2].c_str());
				//�Ѹ�ȡ��ѡ��ģ��ѡ�б�־ȥ��
				m_IrrSmgr->getSceneNodeFromId(cancelSelectID)->setDebugDataVisible(irr::scene::EDS_OFF);
			}

			//��Ϊ�ƶ�����תģ������ʱ(��ģ���ƶ���ʱ��Ҳ����ģ�͵�ת�Ǳ仯)
			if(vectorStr[1]=="moveModel"&&vectorStr.size()==7)
			{
				//ͨ��ID���ģ�ͽڵ�
				int selectID=atoi(vectorStr[2].c_str());
				double X=atoi(vectorStr[3].c_str());
				double Y=atoi(vectorStr[4].c_str());
				double Z=atoi(vectorStr[5].c_str());
				double rotateY=atoi(vectorStr[6].c_str());

				//ģ�͵���ת�ǣ�������һ����ת�ǣ�
				core::vector3df m_vec3Rotation(0,rotateY,0);
				//�趨ģ�͵���ת��
				(m_IrrSmgr->getSceneNodeFromId(selectID))->setRotation(m_vec3Rotation);
				
				//��÷��������͵�ģ��λ��
				core::vector3df nodePosition(X,Y,Z);
				//�趨ģ�͵�λ��
				(m_IrrSmgr->getSceneNodeFromId(selectID))->setPosition(nodePosition);;
			}

			//��Ϊɾ��ģ������ʱ
			if(vectorStr[1]=="deleteModel")
			{
				int deleteID=atoi(vectorStr[2].c_str());
				//ͨ��ID���Ҫɾ���Ľڵ�
				m_IrrSmgr->getSceneNodeFromId(deleteID)->remove();
			}

			//��Ϊ���ز�����Ϣ����ʱ
			if(vectorStr[0]=="layoutData")
			{
			    //�ѱ������е�ģ��ȫ��ɾ��
				for(int i=0;i<m_iModelNodeID;++i)
				{
					//ͨ��ID���Ҫɾ���Ľڵ�
					if(m_IrrSmgr->getSceneNodeFromId(i))
					{
						m_IrrSmgr->getSceneNodeFromId(i)->remove();
					}	
				}
				//��ģ�͵���ʼID��Ϊ0
				m_iModelNodeID=0;

				//��ò�����ģ�͵�����
				int modelCount=(vectorStr.size()-1)/5;
				//�Ѳ����е�ģ����ӵ�������ȥ
				for(int i=1;i<=modelCount;i++)
				{
					//���ģ�͵�����
					std::string strModelName=vectorStr[(i-1)*5+1];
					//���ģ�͵�λ��
					core::vector3df modelPosition;
					modelPosition.X=atoi(vectorStr[(i-1)*5+2].c_str());
					modelPosition.Y=atoi(vectorStr[(i-1)*5+3].c_str());
					modelPosition.Z=atoi(vectorStr[(i-1)*5+4].c_str());
					//���ģ�͵���ת�Ƕ�
					core::vector3df modelRotation;
					modelRotation.X=90;
					modelRotation.Y=atoi(vectorStr[(i-1)*5+5].c_str());
					modelRotation.Z=0;
					//���ģ�ͽڵ�
					IAnimatedMeshSceneNode* addNode=m_mapNodeStore[strModelName];
					ISceneNode* modelNodeAdd=addNode->clone(0,m_IrrSmgr);//������ӵĽ��
					m_mapIdNameStore[m_iModelNodeID]=strModelName;//����ID���ּ�ֵ��
					modelNodeAdd->setID(m_iModelNodeID);//���ý��ID��

					m_iModelNodeID++;//ID��һΪ�´μ���ģ�ͽ����׼��

					modelNodeAdd->setVisible(true);//ʹģ����ʾ
					//ģ�Ͱ�Χ������
					modelNodeAdd->setDebugDataVisible(irr::scene::EDS_OFF);

					//Ϊ��ӽ�㴴������ѡ����
					m_ItriElector = m_IrrSmgr->createTriangleSelector((IAnimatedMeshSceneNode *)modelNodeAdd);
					modelNodeAdd->setTriangleSelector(m_ItriElector);
					modelNodeAdd->setParent(m_IrrSmgr->getRootSceneNode());
					m_ItriElector->drop(); // We're done with this m_ItriElector, so drop it now.
					modelNodeAdd->setPosition(vector3df(0,-50,150));
					m_ICamera->removeChild(modelNodeAdd);

					//�������ģ�ͽڵ��λ�ü��Ƕ�
					modelNodeAdd->setPosition(modelPosition);
					modelNodeAdd->setRotation(modelRotation);
				}
			}
	  }


///////////////////////////////////////////�����߼�/////////////////////////////////////////////
	 //��ײ���ߵ�λ�ø���
	 m_line3TestRay.start = vector3df(m_tracker->TrackerRightHandPosX, m_tracker->TrackerRightHandPosY, m_tracker->TrackerRightHandPosZ);
	 m_line3TestRay.end =m_tracker->getRotatePosition(m_line3TestRay.start,3000);
	   //����ģ�Ϳ�
	  if (m_bModelStore)
	  {
		  bill->setVisible(false);

		  //��ʾ��ҳ����
		  ISceneNode* PgUpNode=m_IrrSmgr->getSceneNodeFromId(2000);
		  ISceneNode* PgDnNode=m_IrrSmgr->getSceneNodeFromId(2001);
		  PgUpNode->setVisible(true);
		  PgDnNode->setVisible(true);
          PgUpNode->setPosition(vector3df(-50,50,200));
		  PgDnNode->setPosition(vector3df(50,50,200));
		   
		  //��ģ�Ϳ⼰��Χ������
		  for(auto iter(m_mapNodeStore.begin());iter!=m_mapNodeStore.end();iter++)
		  {
			  iter->second->setVisible(false);
			  //ģ�Ͱ�Χ������
			  iter->second->setDebugDataVisible(irr::scene::EDS_OFF);
		  }
		  //ģ�Ϳ��ҳ��ʾ
		  int modelNum=0;//����ģ�ͼ�ľ���
		  for(int i=m_iPageModelCount*m_iCurModelPage;i<m_vecStrModelName.size()&&i<m_iPageModelCount*m_iCurModelPage+m_iPageModelCount;i++)
		  {
			  //ģ�Ϳ���ʾ
			  IAnimatedMeshSceneNode* storeModel=m_mapNodeStore[m_vecStrModelName[i]];
			  storeModel->setVisible(true);
			  storeModel->updateAbsolutePosition();

			  //����ģ�ͼ����
              storeModel->setPosition(vector3df(span*modelNum-span,-50,200));
			  storeModel->updateAbsolutePosition();
			  modelNum++;
		  }

		  //����ѡ��ģ�Ϳ�ģ������
		  m_bIsDrawLine=true;

		  //���������ײģ�Ϳ�ڵ�		  
		  l_PickdedStoreNode =l_collManager->getSceneNodeAndCollisionPointFromRay(m_line3TestRay,l_vec3IntersectionPos, l_tri3HitTriangle, 0, 0); 
			//ѡ��������ģ�ͷ�������ײ
			u32 pgtTimeEnd= m_IrrDevice->getTimer()->getTime();; //��õ�ǰ��ʱ��
			pgUpDnDelay=pgtTimeEnd-pgTimeBegin;//����ʱ����
			if (l_PickdedStoreNode) 
			{
				//�����ʾ��ײ
				bill->setVisible(true);
				bill->setPosition(l_vec3IntersectionPos); 
				//��ײģ�͵İ�Χ����ʾ
				l_PickdedStoreNode->setDebugDataVisible(irr::scene::EDS_BBOX);
				//��ҳ��־��Χ�в���ʾ
		        m_IrrSmgr->getSceneNodeFromId(2000)->setDebugDataVisible(irr::scene::EDS_OFF);
		        m_IrrSmgr->getSceneNodeFromId(2001)->setDebugDataVisible(irr::scene::EDS_OFF);

				//ÿ��2�뷭ҳһ�Σ�ʹ��������ģ�Ϳⷭҳ��
				//��ײ���Ϸ���־
				if(l_PickdedStoreNode->getID()==2000&&m_iCurModelPage>0&&(m_bPgUpDnFirst||pgUpDnDelay>2000))
				{
					//��ҳ��ʱ������0
			        pgTimeBegin=pgtTimeEnd;
					m_bPgUpDnFirst=false;
					m_iCurModelPage-=1;
				}
				//��ײ���·���־
				else if(l_PickdedStoreNode->getID()==2001&&m_iCurModelPage<m_iTotalPageNumber-1&&(m_bPgUpDnFirst||pgUpDnDelay>2000))
				{
					//��ҳ��ʱ������0
			        pgTimeBegin=pgtTimeEnd;
					m_bPgUpDnFirst=false;
					m_iCurModelPage+=1;
				}
				//��ײ��ģ�Ϳ�ģ�ͱ�־
				else
				{
					//ץȡ���ģ��
					if (m_bNodeCapture)
					{
						//ץȡ���Ƹ�λ��ģ�Ϳ�״̬��ץȡ����Ϊ�㶯���ƣ�Ҫ��λ��
						m_bNodeCapture=false;
						//ע���������λ���������������ͬһ��ģ�ͣ���������������
						m_bModelStore=false;

						//���ģ�ͽڵ�
						ISceneNode* modelNodeAdd=l_PickdedStoreNode->clone(0,m_IrrSmgr);//������ӵĽ��
						std::string storeModelName=m_vecStrModelName[l_PickdedStoreNode->getID()-1000];//�õ����ģ������
						m_mapIdNameStore[m_iModelNodeID]=storeModelName;//���볡��ID���ּ�ֵ��
						modelNodeAdd->setID(m_iModelNodeID);//���ý��ID��

						m_iModelNodeID++;//ID��һΪ�´μ���ģ�ͽ����׼��

						//֪ͨ���������ģ��
						m_colCommunication->sendMessage("addModel@"+storeModelName);

						//ģ�Ͱ�Χ������
						modelNodeAdd->setDebugDataVisible(irr::scene::EDS_OFF);

						//Ϊ��ӽ�㴴������ѡ����
						m_ItriElector = m_IrrSmgr->createTriangleSelector((IAnimatedMeshSceneNode *)modelNodeAdd);
						modelNodeAdd->setTriangleSelector(m_ItriElector);
						modelNodeAdd->setParent(m_IrrSmgr->getRootSceneNode());
						m_ItriElector->drop(); // We're done with this m_ItriElector, so drop it now.
						modelNodeAdd->setPosition(vector3df(80,-44,-60));
						modelNodeAdd->setRotation(vector3df(90,90,0));
						//m_ICamera->removeChild(modelNodeAdd);	

						//����ģ�Ϳ�
						for(auto iter(m_mapNodeStore.begin());iter!=m_mapNodeStore.end();iter++)
						{
							iter->second->setVisible(false);
						}
						//���ط�ҳ��
						m_IrrSmgr->getSceneNodeFromId(2000)->setVisible(false);
						m_IrrSmgr->getSceneNodeFromId(2001)->setVisible(false);
					}
				}

				//��ѡ��Ľڵ��ÿ�
				l_PickdedStoreNode=0;
			} 
			//û�������ڵ�ѷ�ҳ��ر�����λ
			else
			{
				//��ҳ��ʱ������0
			    pgTimeBegin=pgtTimeEnd;
				m_bPgUpDnFirst=true;
			}
	  }
	  //��ģ�Ϳ�״̬
	  else
	  {
		  //����ģ�Ϳ�
		  for(auto iter(m_mapNodeStore.begin());iter!=m_mapNodeStore.end();iter++)
		  {
			  iter->second->setVisible(false);
		  }

			//����ѡ������
		    m_bIsDrawLine=true;

			//���������ײ��ģ�ͽڵ�
			l_PickedSceneNode =l_collManager->getSceneNodeAndCollisionPointFromRay(m_line3TestRay,l_vec3IntersectionPos, l_tri3HitTriangle,0, 0);
		    
			//�����߷�����ײ����ʾѡ��ģ���Ҹ�ģ��û�б�����ģ��ѡ��ʱ
			if(l_PickedSceneNode && (l_PickedSceneNode->isDebugDataVisible()!=irr::scene::EDS_HALF_TRANSPARENCY))
			{
				//��¼��ѡ�е�ģ��
				l_PreSelectNode=l_PickedSceneNode;
				// We need to reset the transform before doing our own rendering.
				m_IrrDriver->setTransform(video::ETS_WORLD, core::matrix4());
				int modelID=l_PickedSceneNode->getID();
				//��ʾ��Χ��ͻ��ѡ�е�ģ��
				l_PickedSceneNode->setDebugDataVisible(irr::scene::EDS_BBOX);
				//�Ѹýڵ��ID���Ϊ��ѡ�е�ID
				m_iSelectedModelID=modelID;
				bill->setVisible(false);
				//�������֪ͨѡ�и�ģ��(ȷ��ֻ֪ͨһ��)
				if(l_PickedSceneNode->isDebugDataVisible()==irr::scene::EDS_OFF)
				{
					//����ֻ����ID��ʾ�����ֲ�����Ψһ��
					m_colCommunication->sendMessage("selectModel@"+strModelID);
				}

				//�ƶ�ģ�ͼ���תģ��
				if(m_bNodeCapture)
				{
					//��ʾ��Χ��ͻ��ѡ�е�ģ��
					l_PickedSceneNode->setPosition(m_tracker->getRotatePosition(m_line3TestRay.start,m_bRelativeDistance)+m_vec3RelativePosition);
					//�����������ģ�͵�λ�ü���ת�Ƕȣ���ת���ƶ���Ҫȫ�����ͣ�
					u32 timeEnd= m_IrrDevice->getTimer()->getTime();; //��õ�ǰ��ʱ��
					//ÿ��0.1�������������һ��λ��
					//*********************ע��***********************
					//���͵�̫���ʹ���������������գ����ڲ��õ������ݷ��ͻ�������ݴ���һ��
					if(timeEnd-timeBegin>100)
					{
						//��õ�ǰģ�͵�λ��
						core::vector3df position=l_PickedSceneNode->getPosition();
						double X=position.X;
						double Y=position.Y;
						double Z=position.Z;

						//���ģ�͵���ת�ǣ�������һ����ת�ǣ�
						core::vector3df m_vec3Rotation=l_PickedSceneNode->getRotation();
						double rotateY=m_vec3Rotation.Y;

						//��doubleת��string
						char buf[10];
						sprintf(buf, "%f", X);
						std::string strX=buf;
						sprintf(buf, "%f", Y);
						std::string strY=buf;
						sprintf(buf, "%f", Z);
						std::string strZ=buf;
						sprintf(buf, "%f", rotateY);
						std::string strRotateY=buf;
						//֪ͨ������ģ���ƶ�
						m_colCommunication->sendMessage("moveModel@"+strModelID+"@"+strX+"@"+strY+"@"+strZ+"@"+strRotateY);
						timeBegin=timeEnd;
					}
				}
				//��¼��ץȡģ��֮ǰ��ײ���λ�ü������ģ��ԭ���ƫ���������ƶ���ʱ�������㣩
				else
				{
					//��¼��������ģ�͵ľ���
					//��¼ģ��ԭ���������ײ���λ�ã����ƶ���ʱ����벻��
					m_bRelativeDistance=(f32)m_line3TestRay.start.getDistanceFrom(l_vec3IntersectionPos);
					m_vec3RelativePosition=l_PickedSceneNode->getPosition()-l_vec3IntersectionPos;
				}

				//ɾ��ģ��
				if (m_bNodeDelete)
				{
					l_PickedSceneNode->remove();
					l_PickedSceneNode=0;
					//ɾ�����Ƹ�λ��ɾ������Ϊ�㶯���ƣ�
					m_bNodeDelete=false;

					//ʹ��ѡ��ID��־Ϊ��
					m_iSelectedModelID=-1;
					//֪ͨ������ģ��ɾ��
					//����ֻ����ID��ʾ�����ֲ�����Ψһ��
					m_colCommunication->sendMessage("deleteModel@"+strModelID);
				}
				//��ѡ�еĽڵ�������ֱ�ע
				//��ֹδѡ��ģ�͵��´���
				if(l_PickedSceneNode)
				{
					//��ͨ�������õ�ǰѡ�е�ģ��ID
					myOMobileTextComment->setModelSelected(modelID);
				}


			}
			//û�нڵ�����߷�����ײ
			else
			{
				//������λ
		        l_PickedSceneNode=0;
				//��ͨ����ID��λ��δѡ���κ�ģ�ͣ�
		        m_OMobileTextComment->setModelSelected(-1);
				//��λʱ��֮ǰѡ�нڵ��ѡ�б�־ȥ��
				if(m_iSelectedModelID!=-1)
				{
					m_IrrSmgr->getSceneNodeFromId(m_iSelectedModelID)->setDebugDataVisible(irr::scene::EDS_OFF);
					//�������֪ͨȡ����ģ�͵�ѡ�У�ȷ��ֻ����һ����Ϣ��
					 //��intת��string
					 char buf[10];
					 sprintf(buf, "%d", m_iSelectedModelID);
					 std::string strSelectedID=buf;
					 m_colCommunication->sendMessage("cancelSelectModel@"+strSelectedID);
					//�ѱ�ѡ�е�ID�ÿ�
					m_iSelectedModelID=-1;
				}
			}

			//��תģ��(����Ҫʵʱѡ�нڵ�)
			if(l_PreSelectNode&&m_bNodeRotate)
			{
				//��ʾ��Χ��ͻ��ѡ�е�ģ��
				l_PreSelectNode->setDebugDataVisible(irr::scene::EDS_BBOX);
				RotateModel(l_PreSelectNode);
				//�����������ģ�͵�λ�ü���ת�Ƕȣ���ת���ƶ���Ҫȫ�����ͣ�
				u32 timeEnd= m_IrrDevice->getTimer()->getTime();; //��õ�ǰ��ʱ��
				//ÿ��0.1�������������һ��λ��
				//*********************ע��***********************
				//���͵�̫���ʹ���������������գ����ڲ��õ������ݷ��ͻ�������ݴ���һ��
				if(timeEnd-timeBegin>100)
				{
					//��õ�ǰģ�͵�λ��
					core::vector3df position=l_PickedSceneNode->getPosition();
					double X=position.X;
					double Y=position.Y;
					double Z=position.Z;

					//���ģ�͵���ת�ǣ�������һ����ת�ǣ�
					core::vector3df m_vec3Rotation=l_PickedSceneNode->getRotation();
					double rotateY=m_vec3Rotation.Y;

					//��doubleת��string
					char buf[10];
					sprintf(buf, "%f", X);
					std::string strX=buf;
					sprintf(buf, "%f", Y);
					std::string strY=buf;
					sprintf(buf, "%f", Z);
					std::string strZ=buf;
					sprintf(buf, "%f", rotateY);
					std::string strRotateY=buf;
					//֪ͨ������ģ���ƶ�
					m_colCommunication->sendMessage("moveModel@"+strModelID+"@"+strX+"@"+strY+"@"+strZ+"@"+strRotateY);
					timeBegin=timeEnd;
				}
			}
			else
			{
				l_PreSelectNode=0;
				m_bFirstRotate=true;
			}
	   }

	   //������Ⱦ
       m_IrrDriver->endScene();

	   //��������������ز��ַ�������
	   if(receiver.IsKeyUp(irr::KEY_KEY_Y))
	   {
		   //����������������ò�����Ϣ
		   m_colCommunication->sendMessage("loadLayoutData");
	   }
	   //����������ͱ��沼�ַ�������
	   if(receiver.IsKeyUp(irr::KEY_KEY_T))
	   {
		   m_colCommunication->sendMessage("saveLayoutData");
	   }
   }
   m_IrrDevice->drop();
   return 0;
}
///////////////////////////////////////////����ʵ��////////////////////////////////////////////////
bool LoadModels(ISceneNode* m_ICamera,std::map<std::string,IAnimatedMeshSceneNode*>& myNode)
{
	_finddata_t file; 
	long longf;   
  
    //��ȡ�ļ������ļ����� 
    if((longf = _findfirst("..\\model\\*.3ds", &file))==-1l)   
    {   
        std::cout<<"û���ҵ�ģ��!\n";   
    }   
    else  
    {   
        std::cout<<"\n����ģ��\n";   
		std::string tempName;
		tempName = "";   
        tempName = file.name;
		m_vecStrModelName.push_back(tempName);//���ص�һ��ģ������
        while( _findnext( longf, &file ) == 0 )   
        {     
			tempName = "";   
            tempName = file.name; 
            if (tempName == "..")   
            {   
                continue;   
            }   
			m_vecStrModelName.push_back(tempName);
        }   
    }   
  
    _findclose(longf); 

	//ģ�Ϳ�ģ�ͽڵ��ID(��1000��ʼ)
	int storeModelID=1000;
	//����ģ�ʹ��������ڵ�
   auto iter(m_vecStrModelName.begin());
   while(iter!=m_vecStrModelName.end())
   {
	   std::string myPath=*iter;
	   myPath="..\\model\\"+myPath;
	   scene::IAnimatedMesh* mesh = m_IrrSmgr->getMesh((char*)myPath.c_str());

	   //ģ����Ϊ����ͷ���ӽڵ�
       IAnimatedMeshSceneNode* modelNode = m_IrrSmgr->addAnimatedMeshSceneNode(mesh,m_ICamera);

	   // �����Է���ǿ�ȴ�С����������Ⱦ�е����ȴ�С��
       modelNode->getMaterial(0).Shininess = 20.0f; 

	   modelNode->setMaterialFlag(EMF_LIGHTING, true);

	   modelNode->setRotation(vector3df(90, 0, 0));
	   modelNode->setPosition(vector3df(90, 0, 0));

	   //��������ѡ����
	   m_ItriElector = m_IrrSmgr->createTriangleSelector(modelNode);
	   modelNode->setTriangleSelector(m_ItriElector);
	   m_ItriElector->drop(); // We're done with this m_ItriElector, so drop it now.

	   myNode[(*iter)]=modelNode;//����ģ�����ֺ�ģ�Ϳ����
	   iter++;
	   //����ģ��Ϊ���ɼ�
	   modelNode->setVisible(false);

	   //����ģ�Ϳ�ģ��ID
	   modelNode->setID(storeModelID);
	   ++storeModelID;
   }

   //����ģ�Ϳ��ҳ��(����ģ��һҳ)
   m_iTotalPageNumber=m_vecStrModelName.size()/m_iPageModelCount;
   if(m_vecStrModelName.size()%m_iPageModelCount!=0)
   {
	   m_iTotalPageNumber+=1;
   }

   //��ӷ�ҳ��־ģ��
   for(int i=0;i<2;i++)
   {
	   scene::IAnimatedMesh* mesh;
	   if(0==i)
	   {
		   mesh = m_IrrSmgr->getMesh("left.3ds");
	   }
	   else
	   {
		   mesh = m_IrrSmgr->getMesh("right.3ds");
	   }
	   //ģ����Ϊ����ͷ���ӽڵ�
		IAnimatedMeshSceneNode* modelNode = m_IrrSmgr->addAnimatedMeshSceneNode(mesh,m_ICamera);

		// �����Է���ǿ�ȴ�С����������Ⱦ�е����ȴ�С��
		modelNode->getMaterial(0).Shininess = 20.0f; 

		modelNode->setMaterialFlag(EMF_LIGHTING, true);

		modelNode->setRotation(vector3df(90, 0, 0));

		//��������ѡ����
		m_ItriElector = m_IrrSmgr->createTriangleSelector(modelNode);
		modelNode->setTriangleSelector(m_ItriElector);
		m_ItriElector->drop(); // We're done with this m_ItriElector, so drop it now.

		//����ģ��Ϊ���ɼ�
		modelNode->setVisible(false);

		//����ģ�Ϳ�ģ��ID
		if(0==i)
		{
			modelNode->setID(2000);
		}
		else
		{
			modelNode->setID(2001);
		}
   }
   
   return true;
}

//��̬����ʶ����
bool StaticGestureRecognition(IrrEventReceiver& receiver)
{
	//�������׽���
	if (m_bUseDataGlove)
	{
		//��̬������ʶ��
		m_iStaticGestureNumber=m_staticGestureDis->gestureDistinguish();
		int gestureNum=m_iStaticGestureNumber;
		switch(gestureNum)
		{
		case 1:
			//m_bNodeRotate=true;
			break;
		case 2:
			m_bModelStore=true;
			break;
		case 3:
			
			break;
		case 4:
			m_bNodeCapture=true;
			break;
		case 0:
			m_bModelStore=false;
			m_bNodeCapture=false;
			m_bNodeDelete=false ;
			//m_bNodeRotate=false;
			break;
		default:
			m_bNodeCapture=false;
			m_bNodeDelete=false;
			//m_bNodeRotate=false;
		}
	}
	//���̽���
	else
	{
		//ģ�Ϳ�����
		if(receiver.IsKeyDown(irr::KEY_KEY_2))
		{
			m_bModelStore=true;
		}
		//˳ʱ����ת����
		else if(receiver.IsKeyDown(irr::KEY_KEY_M))
		{
			m_bNodeRotate=true;
		}
		//ץȡ����
		else if(receiver.IsKeyDown(irr::KEY_KEY_4))
		{	
			m_bNodeCapture=true;
		}
		//ɾ������
		else if(receiver.IsKeyDown(irr::KEY_KEY_5))
		{	
			m_bNodeDelete=true;
		}		
		//��λ����
		else if(receiver.IsKeyDown(irr::KEY_KEY_0))
		{
			m_bModelStore=false;
			m_bNodeCapture=false;
			m_bNodeDelete=false ;
			m_bNodeRotate=false;
		}
		//�����ƴ����޶���״̬
		//���ʱ��m_bNodeCapture,m_bNodeDelete,m_bNodeRotate��Ҫ��λ
		//�����û�и�λ����ץȡתΪģ�Ϳ��ʱ������m_bNodeCaptureû�еõ���λ��
		else 
		{
			m_bNodeCapture=false;
			nodeDelete=false;
			m_bNodeRotate=false;
		}
	}
	//���ƻ�����ϵ�����Ʒ�Ϊģ�Ϳ�״̬�ͷ�ģ�Ϳ�״̬
	//��ģ�Ϳ�״̬�ַ�Ϊѡ��ģ��״̬�ͷ�ѡ��ģ��״̬��
	//ģ�Ϳ����ʱ
	if(m_bModelStore)
	{
		//ֻ����m_bNodeCaptureΪtrue������Ϊ�㶯������һ��ѭ��֮��Ҫ�Զ���λΪfalse��
		nodeDelete=false;
		m_bNodeRotate=false;
	}
	return true;
}

//˫�ֿ�����תģ�ͺ���
void RotateModel(scene::ISceneNode * preSelectNode)
{
	//��õ�ǰģ�͵�λ��
	if(m_bFirstRotate)
	{
		m_bFirstRotate=false;
		vector3df preNodeRight=m_line3TestRay.start;
		vector3df preNodeLeft(m_tracker->TrackerLeftHandPosX, m_tracker->TrackerLeftHandPosY, m_tracker->TrackerLeftHandPosZ);
		m_vec2PrePositionYZ.X=preNodeRight.X-preNodeLeft.X;
		m_vec2PrePositionYZ.Y=preNodeRight.Z-preNodeLeft.Z;
		m_vec2PrePositionXY.X=preNodeRight.Y-preNodeLeft.Y;
		m_vec2PrePositionXY.Y=preNodeRight.Z-preNodeLeft.Z;
		m_vec2PrePositionXZ.X=preNodeRight.X-preNodeLeft.X;
		m_vec2PrePositionXZ.Y=preNodeRight.Y-preNodeLeft.Y;
		m_vec3Rotation=preSelectNode->getRotation();//�õ�ԭʼ����ת�Ƕ�
	}
	vector3df curNodeRight=m_line3TestRay.start;
	vector3df curNodeLeft(m_tracker->TrackerLeftHandPosX, m_tracker->TrackerLeftHandPosY, m_tracker->TrackerLeftHandPosZ);
	m_vec2CurPositionYZ.X=curNodeRight.X-curNodeLeft.X;
	m_vec2CurPositionYZ.Y=curNodeRight.Z-curNodeLeft.Z;
	m_vec2CurPositionXY.X=curNodeRight.Y-curNodeLeft.Y;
	m_vec2CurPositionXY.Y=curNodeRight.Z-curNodeLeft.Z;
	m_vec2CurPositionXZ.X=curNodeRight.X-curNodeLeft.X;
	m_vec2CurPositionXZ.Y=curNodeRight.Y-curNodeLeft.Y;
	f64 rotateYZ=m_vec2PrePositionYZ.getAngleWith(m_vec2CurPositionYZ);
	f64 rotateXY=m_vec2PrePositionXY.getAngleWith(m_vec2CurPositionXY);
	f64 rotateXZ=m_vec2PrePositionXZ.getAngleWith(m_vec2CurPositionXZ);
	cout<<rotateYZ<<endl;
	//cout<<curNodeLeft.X<<endl;
	preSelectNode->setRotation(m_vec3Rotation+core::vector3df(0,rotateYZ,0));
}

//�Ÿ������ݶ�ȡ�߳�ִ�к���
void Thread_tracker(void *t1)
{
    //�߳�		
	m_tracker->DisplayCont(); 
}

//��̬����ʶ���߳�ִ�к���
void ThreadFunDynGesRec()
{
    //���忪ʼ��ʱ��ʱ��
	time_t clockBegin=clock();   
	while(true)
	{  
	  //���Ŀǰ��ʱ��
	  clock_t clockEnd=clock();   
	  //���ﵽʱ����ʱ����
	  if (m_bUseDataGlove&&(clockEnd-clockBegin>=0.02))
	  {
		  //��̬������ʶ��
		  EnterCriticalSection(&g_cs);
	      m_iStaticGestureNumber=m_staticGestureDis->gestureDistinguish();
	      LeaveCriticalSection(&g_cs);

		  //��̬������ʶ��
          bool bPreIsODynamicGestureDistinguish=false;//���ж�̬����ʶ��
		  bool bCurIsODynamicGestureDistinguish=false;//���ж�̬����ʶ��

		  //�ɼ�����
		  m_staticGestureDis->getGloveData(m_aadGloveData[m_iGloveDataCount]);

		  //�洢��ǰ�Ĳ�������
		  double aadCurGloveData[15];
		  //�����������Խ��
		  if(m_iGloveDataCount>80)
		  {
			  m_iGloveDataCount=0;
		  }
		  for(int i=0;i<15;++i)
		  {
			  aadCurGloveData[i]=m_aadGloveData[m_iGloveDataCount][i];
		  }
		  ++m_iGloveDataCount;

		  //����һ֡�������б��
		  int iPrePositiveSlope=0;//��һ֡�������б��
		  int iPreNegativeCount=0,iPrePositiveCount=0;//�����԰˸����ݵı仯��Ϊ��������
		  //����˸����ݱ仯�ʷֱ�Ϊ����������
		  for(int i=0;i<15;i++)
		  {
			  if(i==3||i==4||i==5||i==6||i==8||i==9||i==11||i==12)
			  {
				  if(m_aadPrePreGloveData[i]!=0)
				  {
					  if((m_aadPreGloveData[i]-m_aadPrePreGloveData[i])>0)
					  {
						  ++iPrePositiveCount;
					  }
					  if((m_aadPreGloveData[i]-m_aadPrePreGloveData[i])<0)
					  {
						  ++iPreNegativeCount;
					  }
				  }
			  }
		  }
		  //���8���仯������7��Ϊ�����ܵı仯��Ϊ��
		  if(iPrePositiveCount>=7)
		  {
			  iPrePositiveSlope=1;
		  }
		  else if(iPreNegativeCount>=7)
		  {
			  iPrePositiveSlope=2;
		  }
		  else
		  {
			  bPreIsODynamicGestureDistinguish=true;
		  }
		  //��ǰ֡�������б��
		  int iCurPositiveSlope=0;
		  int iCurNegativeCount=0,iCurPositiveCount=0; 
		  for(int i=0;i<15;i++)
		  {
			  if(i==3||i==4||i==5||i==6||i==8||i==9||i==11||i==12)
			  {
				  if(m_aadPreGloveData[i]!=0)
				  {
					  if((aadCurGloveData[i]-m_aadPreGloveData[i])>0)
					  {
						  ++iCurPositiveCount;
					  }
					  if((aadCurGloveData[i]-m_aadPreGloveData[i])<0)
					  {
						  ++iCurNegativeCount;
					  }
				  }
			  }
		  }
		  if(iCurPositiveCount>=7)
		  {
			  iCurPositiveSlope=1;
		  }
		  else if(iCurNegativeCount>=7)
		  {
			  iCurPositiveSlope=2;
		  }
		  else
		  {
			  bCurIsODynamicGestureDistinguish=true;
		  }
		  //ѭ�����¼�¼������
		  for(int i=0;i<15;++i)
		  {
			  m_aadPrePreGloveData[i]=m_aadPreGloveData[i];
			  m_aadPreGloveData[i]=aadCurGloveData[i];
		  }
		  //���ж�̬����ʶ��
		  //�Ƚ�������֡�仯��ȷ���Ƿ���ֹյ�
		  if(bPreIsODynamicGestureDistinguish||bCurIsODynamicGestureDistinguish||(iPrePositiveSlope*iCurPositiveSlope==2))
		  {
			  int dynamicGestureNum=-1;
			  if(m_iGloveDataCount>10)
			  {
			      dynamicGestureNum=m_dynamicGestureDis->gestureDistinguish(m_aadGloveData,m_iGloveDataCount);
			      cout<<dynamicGestureNum<<endl;

				  //����ģ�����ݵĲ��Ժ���

				  ����ѵ���õ�ģ��
				  fstream f("d://dynamicGesture.txt",ios::out);
				  f<<m_iGloveDataCount<<' ';
				  for(int i=0;i<m_iGloveDataCount;i++)
				  {
				  	for(int k=0;k<15;k++)
				  	{
				  		f<<m_aadGloveData[i][k]<<' '; //д������
				  	}
				  }
				  f.close();
			  }
			  m_iGloveDataCount=0;//���¿�ʼ�ɼ����� 
		  }
		 clockBegin=clockEnd;
	  }
	}
}

//�������������߳�ִ�к���
void ThreadFunCmdRecvProc(SOCKET mySocket)
{
	//�ӷ������˽�������
	 while(true)
	 {
	     char buff[1024];
	     int nRecv = ::recv(mySocket, buff, 1024, 0);
         if(nRecv > 0)
         {
             buff[nRecv] = '\0';
			 std::string myComand(buff);
			 //���յ������������������ĩβ
			 //***********************ע��**************************
			 //ע�⣨���ڵ�¼ʱ�����ӳ����Բ��ܰѽ��յ���һЩ��#..���ŵ�������У�
			 if(myComand[0]!='#')
			 {
				 EnterCriticalSection(&g_cs);
				 m_deqStrComand.push_back(myComand);
				 LeaveCriticalSection(&g_cs);
			 }
             printf("���յ����ݣ�%s\n", buff);
         }
	 }
}

//�����ֻ�ͨ�ŵ��߳�
void TheadFunOMobileTextComment()
{
   m_OMobileTextComment->communitionBegin();
}