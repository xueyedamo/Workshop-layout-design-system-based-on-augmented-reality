// MainRenderLoop.cpp: 主项目文件。

#include "stdafx.h"

#ifdef _MSC_VER
// We'll also define this to stop MSVC complaining about sprintf().

#define span 140
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")//加载Irrlicht引擎动态链接库
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

using namespace std;//和Irrlicht引擎的命名空间在字符串上有些冲突  
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

/********************头盔显示器参数设定*************************/
  //宽高同样有三种选择1280*800,800*600,640*480
#define SCREEN_WIDTH   1280
#define SCREEN_HEIGHT  800
#define FULLSCREEN     1


//声明变量
//从服务器接收的命令队列
deque<std::string> m_deqStrComand;
//与服务器通信类实例化
OCollaborationCommunication *m_colCommunication = new OCollaborationCommunication("127.0.0.1");
//与手机通信类的对象实例化
OMobileTextComment  *m_OMobileTextComment = new OMobileTextComment();
//静态手势识别类实例化
OStaticGestureDistinguish *m_staticGestureDis = new OStaticGestureDistinguish(60);
//动态手势识别类的实例化
ODynamicGestureDistinguish *m_dynamicGestureDis = new ODynamicGestureDistinguish();
//磁跟踪器类声明
Tracker* m_tracker == new Tracker();

//引擎类的对象实例化
IrrlichtDevice*   m_IrrDevice;//引擎设备实例
IVideoDriver*     m_IrrDriver;//引擎驱动实例
ISceneManager*    m_IrrSmgr;//引擎管理器实例
scene::ITriangleSelector* m_ItriElector = 0;//引擎用于碰撞检测的三角选择器
scene::ICameraSceneNode* m_ICamera ;//引擎中虚拟摄像头

///////////////////////////交互控制逻辑变量////////////////////////////////////
bool   m_bModelStore=false;//调出模型库
bool   m_bNodeCapture=false;//抓取模型
bool   m_bNodeDelete=false ;//删除模型
bool   m_bNodeRotate=false;//旋转模型
bool   m_bUseDataGlove=false;//使用数据手套
int    m_iStaticGestureNumber=-1;//静态手势序号
int    m_iGloveDataCount=0;//采集手套数据总数
double m_aadGloveData[90][15];//保存采集的动态手势数据
double m_aadPreGloveData[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//存储前一帧数据
double m_aadPrePreGloveData[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//存储再前一帧数据
int    m_bRelativeDistance=0;//抓取模型时距离模型距离
vector3df m_vec3RelativePosition;//抓取之前射线碰撞点和模型坐标点的相对位置
//旋转用到的变量
vector2df m_vec2PrePositionXY,m_vec2PrePositionXZ,m_vec2PrePositionYZ,m_vec2CurPositionXY,m_vec2CurPositionYZ,m_vec2CurPositionXZ;//做出旋转手势时记录初始方向向量
bool m_bFirstRotate=true;//判断第一次做出旋转手势旋转
core::vector3df m_vec3Rotation;//模型原始的角度

///////////////////////////////////与模型库相关变量声明///////////////////////////////////////
vector<std::string> m_vecStrModelName;//保存读取每个模型文件的名字
std::map<std::string,IAnimatedMeshSceneNode*> m_mapNodeStore;//模型库的名字和节点对
//当前显示的模型库页面
int m_iCurModelPage=0;
//模型库的总页数
int m_iTotalPageNumber=0;
//每页显示模型的总数
int m_iPageModelCount=3;
//第一次碰到翻页标志时翻页
bool m_bPgUpDnFirst=true;

//与场景节点相关的一些变量
int m_iModelNodeID=0;//添加模型的ID号（从0递增）
std::map<int,std::string> m_mapIdNameStore;//存放场景节点ID和模型名字映射关系
//与节点相关的一些变量
//当前客户端选中的模型节点ID
int m_iSelectedModelID=-1;

//定义用于碰撞检测的射线
core::line3d<f32> m_line3TestRay;
//绘制射线的材质
video::SMaterial m_SMmaterial;
bool m_bIsDrawLine = false;

//定义引擎事件类，捕捉如键盘鼠标事件
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

////////////////////////////////线程执行函数声明///////////////////////////////////////////
//用于手机通信的线程
void TheadFunOMobileTextComment();
//识别动态手势的线程执行函数
void ThreadFunDynGesRec();
//接收服务器信息线程处理函数
void ThreadFunCmdRecvProc(SOCKET mySocket);
//读取磁跟踪器的数据进程
void Thread_tracker(void *t1);

////////////////////////////////////////声明函数////////////////////////////////////////////
//加载模型函数
bool LoadModels(ISceneNode* m_ICamera,std::map<std::string,IAnimatedMeshSceneNode*>& myNode);
//静态手势识别函数
bool StaticGestureRecognition(IrrEventReceiver& receiver);
//旋转模型函数
void RotateModel(scene::ISceneNode * pickdSceneNode);

//主函数
int main(int argc, char* argv[])
{
	InitializeCriticalSection(&g_cs);
    /////////////////////////////////////////创建工作线程///////////////////////////////////////////
    HANDLE hThread;
    DWORD dThreadID;
	//创建用于与服务器通信的线程
    hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadFunCmdRecvProc,(void*)(m_colCommunication->mySocket),0,&dThreadID);

    //创建与手机通信的线程
    hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TheadFunOMobileTextComment,NULL,0,&dThreadID);

	//初始化磁跟踪器类,开启线程，开始读取磁跟踪器的数据;
	if(m_tracker->TrackerInitialize())
	{
		_beginthread(Thread_tracker,0,NULL);
	}
	else
	{
		return 0;
	}

   //创建用于识别动态手势的线程
   if(m_bUseDataGlove)
   {
       hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadFunDynGesRec,NULL,0,&dThreadID);
   }
	
	//////////////////////////////////Irrlicht引擎初始化/////////////////////////////////////
    IrrEventReceiver receiver;//声明事件
	//引擎创建设备
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
    m_IrrDevice->setWindowCaption(L"车间布局测试版");

 //////////////////////////////////头盔立体显示类初始化//////////////////////////////////////
   HMDDescriptor HMD;
  // Parameters from the Oculus Rift DK1
  //宽高同样有三种选择1280*800,800*600,640*480
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

    //开启摄像头
   video::ITexture* l_aITexImages[2];//定义纹理指针
   OIrrTextureFromCamera* l_TextureFromCam = new OIrrTextureFromCamera(m_IrrDriver,0,0);
   l_TextureFromCam->getTexture(l_aITexImages);

   //碰撞检测变量定义
   scene::ISceneNode*  l_PickedStoreNode=0;//射线碰撞到的模型库节点
   scene::ISceneNode*  l_PickedSceneNode=0;//射线碰撞到的场景节点
   scene::ISceneNode*  l_PreSelectNode=0;//记录之前选中的节点
   scene::ISceneNode*  l_SelectedSceneNode = 0;//选中的模型节点
   scene::ISceneCollisionManager* l_collManager = m_IrrSmgr->getSceneCollisionManager();
   // 当前射线碰撞点位置
   core::vector3df l_vec3IntersectionPos;
   // 射线碰撞的三角面片
   core::triangle3df l_tri3HitTriangle;
   ////////////////////////////////////////加载节点////////////////////////////////////////////////////////
   //加载引擎摄像头节点及设定摄像头参数
   	m_ICamera = m_IrrSmgr->addCameraSceneNode();
    m_ICamera->setPosition(vector3df(m_tracker->CameraPositionPx, m_tracker->CameraPositionPy, m_tracker->CameraPositionPz));
    m_ICamera->setTarget(vector3df(m_tracker->TargetPositionPx,m_tracker->TargetPositionPy,m_tracker->TargetPositionPz));
	m_ICamera->setFarValue(20000.0);//设置摄像机最远的剪切面
	m_ICamera->setAspectRatio(4.0f/3.0f);
	m_ICamera->setFOV(PI / m_tracker->AngleField);
	m_ICamera->setUpVector(vector3df(0,1,0));
	

   //加载模型库中模型
	LoadModels(m_ICamera,m_mapNodeStore);

	//添加灯光及广告牌节点
	scene::IBillboardSceneNode * bill = m_IrrSmgr->addBillboardSceneNode();
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
	bill->setMaterialTexture(0, m_IrrDriver->getTexture("particle.bmp"));
	bill->setMaterialFlag(video::EMF_LIGHTING, true);
	bill->setMaterialFlag(video::EMF_ZBUFFER, false);
	bill->setSize(core::dimension2d<f32>(40.0f, 40.0f));
	bill->setVisible(false);

	//添加灯光结点
	scene::ILightSceneNode* nodeLight1=m_IrrSmgr->addLightSceneNode(m_ICamera,core::vector3df(50, 100, -10),video::SColorf(1,1,1),100);
	nodeLight1->setLightType(video::ELT_DIRECTIONAL);
   //////////////////////////////////计时相关时间变量声明///////////////////////////////////////////////////////////////////////////
	// 测量相临两帧之间的时间
   u32 uThenTime = m_IrrDevice->getTimer()->getTime();
   // 每秒移动的速率
   const f32 MOVEMENT_SPEED = 0.5f;
   //定义计时的时间变量
   u32 timeBegin= m_IrrDevice->getTimer()->getTime();
   //定义翻页时间检测变量
   u32 pgTimeBegin=m_IrrDevice->getTimer()->getTime();
   u32 pgUpDnDelay=0;
   vector3df test;
   //////////////////////////////////////////引擎渲染主循环//////////////////////////////////////////////////
   while(m_IrrDevice->run())
   {
	   //输出手势序号
	   if (m_bUseDataGlove)
	   {
		   wchar_t wchar[10]={0};
		   //采用临界区
		   EnterCriticalSection(&g_cs);
		   int len=swprintf( wchar ,9, L"%d", m_iStaticGestureNumber );
		   LeaveCriticalSection(&g_cs);
		   m_IrrDevice->setWindowCaption(wchar);
	   }

	   //根据磁跟踪器更新光照方向
	   nodeLight1->setRotation(m_ICamera->getTarget()-m_ICamera->getPosition());
	   //更新摄像头的位置
	   m_ICamera->setUpVector(core::vector3df(m_tracker->CameraUpPx,m_tracker->CameraUpPy,m_tracker->CameraUpPz));
	   m_ICamera->setPosition(vector3df(m_tracker->CameraPositionPx, m_tracker->CameraPositionPy, m_tracker->CameraPositionPz));
	   m_ICamera->setTarget(vector3df(m_tracker->TargetPositionPx,m_tracker->TargetPositionPy,m_tracker->TargetPositionPz));	   
	   m_ICamera->setRotation(vector3df(-m_tracker->CameraEulerPitch/3.14*180.0,m_tracker->CameraEulerYaw/3.14*180.0,m_tracker->CameraEulerRoll/3.14*180.0));
	  
	   //更新摄像头背景纹理
      l_TextureFromCam->UpdateTexture();
	  // 算出两帧时间差
	  const u32 uNowTime = m_IrrDevice->getTimer()->getTime();
	  const f32 fFrameDeltaTime = (f32)(uNowTime - uThenTime) / 1000.f; // Time in seconds
	  uThenTime = uNowTime;
      //开始渲染
      m_IrrDriver->beginScene(true, true, SColor(255,100,101,140));
	  //绘制背景图片和模型
      l_HMDrender.drawAll(m_IrrSmgr,l_aITexImages,m_line3TestRay,m_bIsDrawLine);
	  //静态手势识别
	  StaticGestureRecognition(receiver);

	  //////////////////////////////////////////解析及处理服务器命令////////////////////////////////////////
	  //存放分割后的字符串的容器
	  vector<std::string> vectorStr;
	  EnterCriticalSection(&g_cs);//锁定m_deqStrComand

	  if(m_deqStrComand.size()>0)
	  {
		  char buf[1024];
		  std::string str=m_deqStrComand.front();
		  //把字符串转成字符数组
		  for(int i=0;i<str.length();i++)
		  {
			  buf[i] = str[i];
		  }
		  buf[str.length()] = '\0';
		  //命令处理之后要删除掉
		  m_deqStrComand.pop_front();
		  //用于分割字符数组的字符
		  const char *d = "@";
		  char *p;
		  p = strtok(buf,d);
		  //字符数组的分割
		  while(p)
		  {
			  std::string strRec(p);
			  //printf("%s\n",p);
			  vectorStr.push_back(strRec);
			  p=strtok(NULL,d);
		  }
	  }

	  LeaveCriticalSection(&g_cs);//释放m_deqStrComand
	  //当解析到命令时
	  if(vectorStr.size()>1)
	  {
		    //当为添加模型命令时
			if(vectorStr[1]=="addModel")
			{
				//添加模型节点
				std::string strModelName=vectorStr[2];
				IAnimatedMeshSceneNode* addNode=m_mapNodeStore[strModelName];
				ISceneNode* modelNodeAdd=addNode->clone(0,m_IrrSmgr);//复制添加的结点
				m_mapIdNameStore[m_iModelNodeID]=strModelName;//插入ID名字键值对
				modelNodeAdd->setID(m_iModelNodeID);//设置结点ID号

				m_iModelNodeID++;//ID加一为下次加载模型结点做准备

				modelNodeAdd->setVisible(true);//使模型显示
				//模型包围盒隐藏
				modelNodeAdd->setDebugDataVisible(irr::scene::EDS_OFF);

				//为添加结点创建三角选择器
				m_ItriElector = m_IrrSmgr->createTriangleSelector((IAnimatedMeshSceneNode *)modelNodeAdd);
				modelNodeAdd->setTriangleSelector(m_ItriElector);
				modelNodeAdd->setParent(m_IrrSmgr->getRootSceneNode());
				m_ItriElector->drop(); // We're done with this m_ItriElector, so drop it now.
				modelNodeAdd->setPosition(vector3df(0,-50,150));
				m_ICamera->removeChild(modelNodeAdd);
			}

			//当为选择模型命令时
			if(vectorStr[1]=="selectModel")
			{
				//通过ID获得模型节点
				int selectID=atoi(vectorStr[2].c_str());
				//把该被选中模型设置为半透明
				m_IrrSmgr->getSceneNodeFromId(selectID)->setDebugDataVisible(irr::scene::EDS_HALF_TRANSPARENCY);
			}

			//当为取消选中命令时
			if(vectorStr[1]=="cancelSelectModel")
			{
				//通过ID获得模型节点
				int cancelSelectID=atoi(vectorStr[2].c_str());
				//把该取消选中模型选中标志去掉
				m_IrrSmgr->getSceneNodeFromId(cancelSelectID)->setDebugDataVisible(irr::scene::EDS_OFF);
			}

			//当为移动及旋转模型命令时(在模型移动的时候也接收模型的转角变化)
			if(vectorStr[1]=="moveModel"&&vectorStr.size()==7)
			{
				//通过ID获得模型节点
				int selectID=atoi(vectorStr[2].c_str());
				double X=atoi(vectorStr[3].c_str());
				double Y=atoi(vectorStr[4].c_str());
				double Z=atoi(vectorStr[5].c_str());
				double rotateY=atoi(vectorStr[6].c_str());

				//模型的旋转角（增加了一个旋转角）
				core::vector3df m_vec3Rotation(0,rotateY,0);
				//设定模型的旋转角
				(m_IrrSmgr->getSceneNodeFromId(selectID))->setRotation(m_vec3Rotation);
				
				//获得服务器发送的模型位置
				core::vector3df nodePosition(X,Y,Z);
				//设定模型的位置
				(m_IrrSmgr->getSceneNodeFromId(selectID))->setPosition(nodePosition);;
			}

			//当为删除模型命令时
			if(vectorStr[1]=="deleteModel")
			{
				int deleteID=atoi(vectorStr[2].c_str());
				//通过ID获得要删除的节点
				m_IrrSmgr->getSceneNodeFromId(deleteID)->remove();
			}

			//当为下载布局信息命令时
			if(vectorStr[0]=="layoutData")
			{
			    //把本场景中的模型全部删除
				for(int i=0;i<m_iModelNodeID;++i)
				{
					//通过ID获得要删除的节点
					if(m_IrrSmgr->getSceneNodeFromId(i))
					{
						m_IrrSmgr->getSceneNodeFromId(i)->remove();
					}	
				}
				//把模型的起始ID设为0
				m_iModelNodeID=0;

				//获得布局中模型的总数
				int modelCount=(vectorStr.size()-1)/5;
				//把布局中的模型添加到场景中去
				for(int i=1;i<=modelCount;i++)
				{
					//获得模型的名字
					std::string strModelName=vectorStr[(i-1)*5+1];
					//获得模型的位置
					core::vector3df modelPosition;
					modelPosition.X=atoi(vectorStr[(i-1)*5+2].c_str());
					modelPosition.Y=atoi(vectorStr[(i-1)*5+3].c_str());
					modelPosition.Z=atoi(vectorStr[(i-1)*5+4].c_str());
					//获得模型的旋转角度
					core::vector3df modelRotation;
					modelRotation.X=90;
					modelRotation.Y=atoi(vectorStr[(i-1)*5+5].c_str());
					modelRotation.Z=0;
					//添加模型节点
					IAnimatedMeshSceneNode* addNode=m_mapNodeStore[strModelName];
					ISceneNode* modelNodeAdd=addNode->clone(0,m_IrrSmgr);//复制添加的结点
					m_mapIdNameStore[m_iModelNodeID]=strModelName;//插入ID名字键值对
					modelNodeAdd->setID(m_iModelNodeID);//设置结点ID号

					m_iModelNodeID++;//ID加一为下次加载模型结点做准备

					modelNodeAdd->setVisible(true);//使模型显示
					//模型包围盒隐藏
					modelNodeAdd->setDebugDataVisible(irr::scene::EDS_OFF);

					//为添加结点创建三角选择器
					m_ItriElector = m_IrrSmgr->createTriangleSelector((IAnimatedMeshSceneNode *)modelNodeAdd);
					modelNodeAdd->setTriangleSelector(m_ItriElector);
					modelNodeAdd->setParent(m_IrrSmgr->getRootSceneNode());
					m_ItriElector->drop(); // We're done with this m_ItriElector, so drop it now.
					modelNodeAdd->setPosition(vector3df(0,-50,150));
					m_ICamera->removeChild(modelNodeAdd);

					//设置添加模型节点的位置及角度
					modelNodeAdd->setPosition(modelPosition);
					modelNodeAdd->setRotation(modelRotation);
				}
			}
	  }


///////////////////////////////////////////交互逻辑/////////////////////////////////////////////
	 //碰撞射线的位置更新
	 m_line3TestRay.start = vector3df(m_tracker->TrackerRightHandPosX, m_tracker->TrackerRightHandPosY, m_tracker->TrackerRightHandPosZ);
	 m_line3TestRay.end =m_tracker->getRotatePosition(m_line3TestRay.start,3000);
	   //调出模型库
	  if (m_bModelStore)
	  {
		  bill->setVisible(false);

		  //显示翻页按键
		  ISceneNode* PgUpNode=m_IrrSmgr->getSceneNodeFromId(2000);
		  ISceneNode* PgDnNode=m_IrrSmgr->getSceneNodeFromId(2001);
		  PgUpNode->setVisible(true);
		  PgDnNode->setVisible(true);
          PgUpNode->setPosition(vector3df(-50,50,200));
		  PgDnNode->setPosition(vector3df(50,50,200));
		   
		  //让模型库及包围盒隐藏
		  for(auto iter(m_mapNodeStore.begin());iter!=m_mapNodeStore.end();iter++)
		  {
			  iter->second->setVisible(false);
			  //模型包围盒隐藏
			  iter->second->setDebugDataVisible(irr::scene::EDS_OFF);
		  }
		  //模型库分页显示
		  int modelNum=0;//调整模型间的距离
		  for(int i=m_iPageModelCount*m_iCurModelPage;i<m_vecStrModelName.size()&&i<m_iPageModelCount*m_iCurModelPage+m_iPageModelCount;i++)
		  {
			  //模型库显示
			  IAnimatedMeshSceneNode* storeModel=m_mapNodeStore[m_vecStrModelName[i]];
			  storeModel->setVisible(true);
			  storeModel->updateAbsolutePosition();

			  //设置模型间距离
              storeModel->setPosition(vector3df(span*modelNum-span,-50,200));
			  storeModel->updateAbsolutePosition();
			  modelNum++;
		  }

		  //画出选择模型库模型射线
		  m_bIsDrawLine=true;

		  //获得射线碰撞模型库节点		  
		  l_PickdedStoreNode =l_collManager->getSceneNodeAndCollisionPointFromRay(m_line3TestRay,l_vec3IntersectionPos, l_tri3HitTriangle, 0, 0); 
			//选择射线与模型发生了碰撞
			u32 pgtTimeEnd= m_IrrDevice->getTimer()->getTime();; //获得当前的时间
			pgUpDnDelay=pgtTimeEnd-pgTimeBegin;//计算时间间隔
			if (l_PickdedStoreNode) 
			{
				//亮点标示碰撞
				bill->setVisible(true);
				bill->setPosition(l_vec3IntersectionPos); 
				//碰撞模型的包围盒显示
				l_PickdedStoreNode->setDebugDataVisible(irr::scene::EDS_BBOX);
				//翻页标志包围盒不显示
		        m_IrrSmgr->getSceneNodeFromId(2000)->setDebugDataVisible(irr::scene::EDS_OFF);
		        m_IrrSmgr->getSceneNodeFromId(2001)->setDebugDataVisible(irr::scene::EDS_OFF);

				//每隔2秒翻页一次（使用射线让模型库翻页）
				//碰撞到上翻标志
				if(l_PickdedStoreNode->getID()==2000&&m_iCurModelPage>0&&(m_bPgUpDnFirst||pgUpDnDelay>2000))
				{
					//翻页计时变量置0
			        pgTimeBegin=pgtTimeEnd;
					m_bPgUpDnFirst=false;
					m_iCurModelPage-=1;
				}
				//碰撞到下翻标志
				else if(l_PickdedStoreNode->getID()==2001&&m_iCurModelPage<m_iTotalPageNumber-1&&(m_bPgUpDnFirst||pgUpDnDelay>2000))
				{
					//翻页计时变量置0
			        pgTimeBegin=pgtTimeEnd;
					m_bPgUpDnFirst=false;
					m_iCurModelPage+=1;
				}
				//碰撞到模型库模型标志
				else
				{
					//抓取添加模型
					if (m_bNodeCapture)
					{
						//抓取手势复位（模型库状态下抓取手势为点动手势，要复位）
						m_bNodeCapture=false;
						//注意这个不复位会造成连续多次添加同一个模型！！！！！！！！
						m_bModelStore=false;

						//添加模型节点
						ISceneNode* modelNodeAdd=l_PickdedStoreNode->clone(0,m_IrrSmgr);//复制添加的结点
						std::string storeModelName=m_vecStrModelName[l_PickdedStoreNode->getID()-1000];//得到添加模型名字
						m_mapIdNameStore[m_iModelNodeID]=storeModelName;//插入场景ID名字键值对
						modelNodeAdd->setID(m_iModelNodeID);//设置结点ID号

						m_iModelNodeID++;//ID加一为下次加载模型结点做准备

						//通知服务器添加模型
						m_colCommunication->sendMessage("addModel@"+storeModelName);

						//模型包围盒隐藏
						modelNodeAdd->setDebugDataVisible(irr::scene::EDS_OFF);

						//为添加结点创建三角选择器
						m_ItriElector = m_IrrSmgr->createTriangleSelector((IAnimatedMeshSceneNode *)modelNodeAdd);
						modelNodeAdd->setTriangleSelector(m_ItriElector);
						modelNodeAdd->setParent(m_IrrSmgr->getRootSceneNode());
						m_ItriElector->drop(); // We're done with this m_ItriElector, so drop it now.
						modelNodeAdd->setPosition(vector3df(80,-44,-60));
						modelNodeAdd->setRotation(vector3df(90,90,0));
						//m_ICamera->removeChild(modelNodeAdd);	

						//隐藏模型库
						for(auto iter(m_mapNodeStore.begin());iter!=m_mapNodeStore.end();iter++)
						{
							iter->second->setVisible(false);
						}
						//隐藏翻页键
						m_IrrSmgr->getSceneNodeFromId(2000)->setVisible(false);
						m_IrrSmgr->getSceneNodeFromId(2001)->setVisible(false);
					}
				}

				//把选择的节点置空
				l_PickdedStoreNode=0;
			} 
			//没有碰到节点把翻页相关变量复位
			else
			{
				//翻页计时变量置0
			    pgTimeBegin=pgtTimeEnd;
				m_bPgUpDnFirst=true;
			}
	  }
	  //非模型库状态
	  else
	  {
		  //隐藏模型库
		  for(auto iter(m_mapNodeStore.begin());iter!=m_mapNodeStore.end();iter++)
		  {
			  iter->second->setVisible(false);
		  }

			//画出选择射线
		    m_bIsDrawLine=true;

			//获得射线碰撞的模型节点
			l_PickedSceneNode =l_collManager->getSceneNodeAndCollisionPointFromRay(m_line3TestRay,l_vec3IntersectionPos, l_tri3HitTriangle,0, 0);
		    
			//和射线发生碰撞即表示选中模型且该模型没有被其他模型选中时
			if(l_PickedSceneNode && (l_PickedSceneNode->isDebugDataVisible()!=irr::scene::EDS_HALF_TRANSPARENCY))
			{
				//记录下选中的模型
				l_PreSelectNode=l_PickedSceneNode;
				// We need to reset the transform before doing our own rendering.
				m_IrrDriver->setTransform(video::ETS_WORLD, core::matrix4());
				int modelID=l_PickedSceneNode->getID();
				//显示包围盒突出选中的模型
				l_PickedSceneNode->setDebugDataVisible(irr::scene::EDS_BBOX);
				//把该节点的ID标记为被选中的ID
				m_iSelectedModelID=modelID;
				bill->setVisible(false);
				//向服务器通知选中该模型(确保只通知一次)
				if(l_PickedSceneNode->isDebugDataVisible()==irr::scene::EDS_OFF)
				{
					//这里只能用ID标示，名字不具有唯一性
					m_colCommunication->sendMessage("selectModel@"+strModelID);
				}

				//移动模型即旋转模型
				if(m_bNodeCapture)
				{
					//显示包围盒突出选中的模型
					l_PickedSceneNode->setPosition(m_tracker->getRotatePosition(m_line3TestRay.start,m_bRelativeDistance)+m_vec3RelativePosition);
					//向服务器传送模型的位置及旋转角度（旋转或移动都要全部传送）
					u32 timeEnd= m_IrrDevice->getTimer()->getTime();; //获得当前的时间
					//每隔0.1秒向服务器发送一下位置
					//*********************注意***********************
					//发送的太快会使服务器来不及接收，由于采用的流数据发送会造成数据串在一起
					if(timeEnd-timeBegin>100)
					{
						//获得当前模型的位置
						core::vector3df position=l_PickedSceneNode->getPosition();
						double X=position.X;
						double Y=position.Y;
						double Z=position.Z;

						//获得模型的旋转角（增加了一个旋转角）
						core::vector3df m_vec3Rotation=l_PickedSceneNode->getRotation();
						double rotateY=m_vec3Rotation.Y;

						//将double转成string
						char buf[10];
						sprintf(buf, "%f", X);
						std::string strX=buf;
						sprintf(buf, "%f", Y);
						std::string strY=buf;
						sprintf(buf, "%f", Z);
						std::string strZ=buf;
						sprintf(buf, "%f", rotateY);
						std::string strRotateY=buf;
						//通知服务器模型移动
						m_colCommunication->sendMessage("moveModel@"+strModelID+"@"+strX+"@"+strY+"@"+strZ+"@"+strRotateY);
						timeBegin=timeEnd;
					}
				}
				//记录在抓取模型之前碰撞点的位置及相对于模型原点的偏移量（在移动的时候做计算）
				else
				{
					//记录操作者与模型的距离
					//记录模型原点相对于碰撞点的位置，在移动的时候距离不变
					m_bRelativeDistance=(f32)m_line3TestRay.start.getDistanceFrom(l_vec3IntersectionPos);
					m_vec3RelativePosition=l_PickedSceneNode->getPosition()-l_vec3IntersectionPos;
				}

				//删除模型
				if (m_bNodeDelete)
				{
					l_PickedSceneNode->remove();
					l_PickedSceneNode=0;
					//删除手势复位（删除手势为点动手势）
					m_bNodeDelete=false;

					//使被选中ID标志为空
					m_iSelectedModelID=-1;
					//通知服务器模型删除
					//这里只能用ID标示，名字不具有唯一性
					m_colCommunication->sendMessage("deleteModel@"+strModelID);
				}
				//对选中的节点进行文字备注
				//防止未选中模型导致错误
				if(l_PickedSceneNode)
				{
					//给通信类设置当前选中的模型ID
					myOMobileTextComment->setModelSelected(modelID);
				}


			}
			//没有节点和射线发生碰撞
			else
			{
				//变量复位
		        l_PickedSceneNode=0;
				//给通信类ID复位（未选中任何模型）
		        m_OMobileTextComment->setModelSelected(-1);
				//复位时把之前选中节点的选中标志去掉
				if(m_iSelectedModelID!=-1)
				{
					m_IrrSmgr->getSceneNodeFromId(m_iSelectedModelID)->setDebugDataVisible(irr::scene::EDS_OFF);
					//向服务器通知取消该模型的选中（确保只发送一条信息）
					 //将int转成string
					 char buf[10];
					 sprintf(buf, "%d", m_iSelectedModelID);
					 std::string strSelectedID=buf;
					 m_colCommunication->sendMessage("cancelSelectModel@"+strSelectedID);
					//把被选中的ID置空
					m_iSelectedModelID=-1;
				}
			}

			//旋转模型(不需要实时选中节点)
			if(l_PreSelectNode&&m_bNodeRotate)
			{
				//显示包围盒突出选中的模型
				l_PreSelectNode->setDebugDataVisible(irr::scene::EDS_BBOX);
				RotateModel(l_PreSelectNode);
				//向服务器传送模型的位置及旋转角度（旋转或移动都要全部传送）
				u32 timeEnd= m_IrrDevice->getTimer()->getTime();; //获得当前的时间
				//每隔0.1秒向服务器发送一下位置
				//*********************注意***********************
				//发送的太快会使服务器来不及接收，由于采用的流数据发送会造成数据串在一起
				if(timeEnd-timeBegin>100)
				{
					//获得当前模型的位置
					core::vector3df position=l_PickedSceneNode->getPosition();
					double X=position.X;
					double Y=position.Y;
					double Z=position.Z;

					//获得模型的旋转角（增加了一个旋转角）
					core::vector3df m_vec3Rotation=l_PickedSceneNode->getRotation();
					double rotateY=m_vec3Rotation.Y;

					//将double转成string
					char buf[10];
					sprintf(buf, "%f", X);
					std::string strX=buf;
					sprintf(buf, "%f", Y);
					std::string strY=buf;
					sprintf(buf, "%f", Z);
					std::string strZ=buf;
					sprintf(buf, "%f", rotateY);
					std::string strRotateY=buf;
					//通知服务器模型移动
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

	   //结束渲染
       m_IrrDriver->endScene();

	   //向服务器发送下载布局方案命令
	   if(receiver.IsKeyUp(irr::KEY_KEY_Y))
	   {
		   //向服务器发送请求获得布局信息
		   m_colCommunication->sendMessage("loadLayoutData");
	   }
	   //向服务器发送保存布局方案命令
	   if(receiver.IsKeyUp(irr::KEY_KEY_T))
	   {
		   m_colCommunication->sendMessage("saveLayoutData");
	   }
   }
   m_IrrDevice->drop();
   return 0;
}
///////////////////////////////////////////函数实现////////////////////////////////////////////////
bool LoadModels(ISceneNode* m_ICamera,std::map<std::string,IAnimatedMeshSceneNode*>& myNode)
{
	_finddata_t file; 
	long longf;   
  
    //读取文件夹下文件名字 
    if((longf = _findfirst("..\\model\\*.3ds", &file))==-1l)   
    {   
        std::cout<<"没有找到模型!\n";   
    }   
    else  
    {   
        std::cout<<"\n加载模型\n";   
		std::string tempName;
		tempName = "";   
        tempName = file.name;
		m_vecStrModelName.push_back(tempName);//加载第一个模型名字
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

	//模型库模型节点的ID(从1000开始)
	int storeModelID=1000;
	//加载模型创建场景节点
   auto iter(m_vecStrModelName.begin());
   while(iter!=m_vecStrModelName.end())
   {
	   std::string myPath=*iter;
	   myPath="..\\model\\"+myPath;
	   scene::IAnimatedMesh* mesh = m_IrrSmgr->getMesh((char*)myPath.c_str());

	   //模型设为摄像头的子节点
       IAnimatedMeshSceneNode* modelNode = m_IrrSmgr->addAnimatedMeshSceneNode(mesh,m_ICamera);

	   // 设置自发光强度大小（即镜面渲染中的亮度大小）
       modelNode->getMaterial(0).Shininess = 20.0f; 

	   modelNode->setMaterialFlag(EMF_LIGHTING, true);

	   modelNode->setRotation(vector3df(90, 0, 0));
	   modelNode->setPosition(vector3df(90, 0, 0));

	   //创建三角选择器
	   m_ItriElector = m_IrrSmgr->createTriangleSelector(modelNode);
	   modelNode->setTriangleSelector(m_ItriElector);
	   m_ItriElector->drop(); // We're done with this m_ItriElector, so drop it now.

	   myNode[(*iter)]=modelNode;//插入模型名字和模型库结点对
	   iter++;
	   //设置模型为不可见
	   modelNode->setVisible(false);

	   //设置模型库模型ID
	   modelNode->setID(storeModelID);
	   ++storeModelID;
   }

   //计算模型库的页数(三个模型一页)
   m_iTotalPageNumber=m_vecStrModelName.size()/m_iPageModelCount;
   if(m_vecStrModelName.size()%m_iPageModelCount!=0)
   {
	   m_iTotalPageNumber+=1;
   }

   //添加翻页标志模型
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
	   //模型设为摄像头的子节点
		IAnimatedMeshSceneNode* modelNode = m_IrrSmgr->addAnimatedMeshSceneNode(mesh,m_ICamera);

		// 设置自发光强度大小（即镜面渲染中的亮度大小）
		modelNode->getMaterial(0).Shininess = 20.0f; 

		modelNode->setMaterialFlag(EMF_LIGHTING, true);

		modelNode->setRotation(vector3df(90, 0, 0));

		//创建三角选择器
		m_ItriElector = m_IrrSmgr->createTriangleSelector(modelNode);
		modelNode->setTriangleSelector(m_ItriElector);
		m_ItriElector->drop(); // We're done with this m_ItriElector, so drop it now.

		//设置模型为不可见
		modelNode->setVisible(false);

		//设置模型库模型ID
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

//静态手势识别函数
bool StaticGestureRecognition(IrrEventReceiver& receiver)
{
	//数据手套交互
	if (m_bUseDataGlove)
	{
		//静态的手势识别
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
	//键盘交互
	else
	{
		//模型库手势
		if(receiver.IsKeyDown(irr::KEY_KEY_2))
		{
			m_bModelStore=true;
		}
		//顺时针旋转手势
		else if(receiver.IsKeyDown(irr::KEY_KEY_M))
		{
			m_bNodeRotate=true;
		}
		//抓取手势
		else if(receiver.IsKeyDown(irr::KEY_KEY_4))
		{	
			m_bNodeCapture=true;
		}
		//删除手势
		else if(receiver.IsKeyDown(irr::KEY_KEY_5))
		{	
			m_bNodeDelete=true;
		}		
		//复位手势
		else if(receiver.IsKeyDown(irr::KEY_KEY_0))
		{
			m_bModelStore=false;
			m_bNodeCapture=false;
			m_bNodeDelete=false ;
			m_bNodeRotate=false;
		}
		//当手势处于无动作状态
		//这个时候m_bNodeCapture,m_bNodeDelete,m_bNodeRotate均要复位
		//（如果没有复位在由抓取转为模型库的时候会出错，m_bNodeCapture没有得到复位）
		else 
		{
			m_bNodeCapture=false;
			nodeDelete=false;
			m_bNodeRotate=false;
		}
	}
	//手势互锁关系（手势分为模型库状态和非模型库状态
	//非模型库状态又分为选中模型状态和非选中模型状态）
	//模型库调出时
	if(m_bModelStore)
	{
		//只允许m_bNodeCapture为true，且其为点动（即在一个循环之后要自动复位为false）
		nodeDelete=false;
		m_bNodeRotate=false;
	}
	return true;
}

//双手控制旋转模型函数
void RotateModel(scene::ISceneNode * preSelectNode)
{
	//获得当前模型的位置
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
		m_vec3Rotation=preSelectNode->getRotation();//得到原始的旋转角度
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

//磁跟踪数据读取线程执行函数
void Thread_tracker(void *t1)
{
    //线程		
	m_tracker->DisplayCont(); 
}

//动态手势识别线程执行函数
void ThreadFunDynGesRec()
{
    //定义开始计时的时间
	time_t clockBegin=clock();   
	while(true)
	{  
	  //获得目前的时间
	  clock_t clockEnd=clock();   
	  //当达到时间间隔时采样
	  if (m_bUseDataGlove&&(clockEnd-clockBegin>=0.02))
	  {
		  //静态的手势识别
		  EnterCriticalSection(&g_cs);
	      m_iStaticGestureNumber=m_staticGestureDis->gestureDistinguish();
	      LeaveCriticalSection(&g_cs);

		  //动态的手势识别
          bool bPreIsODynamicGestureDistinguish=false;//进行动态手势识别
		  bool bCurIsODynamicGestureDistinguish=false;//进行动态手势识别

		  //采集数据
		  m_staticGestureDis->getGloveData(m_aadGloveData[m_iGloveDataCount]);

		  //存储当前的采样数据
		  double aadCurGloveData[15];
		  //保护数组放置越界
		  if(m_iGloveDataCount>80)
		  {
			  m_iGloveDataCount=0;
		  }
		  for(int i=0;i<15;++i)
		  {
			  aadCurGloveData[i]=m_aadGloveData[m_iGloveDataCount][i];
		  }
		  ++m_iGloveDataCount;

		  //求上一帧数据相对斜率
		  int iPrePositiveSlope=0;//上一帧数据相对斜率
		  int iPreNegativeCount=0,iPrePositiveCount=0;//待测试八个数据的变化率为正负总数
		  //求出八个数据变化率分别为正负的总数
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
		  //如果8个变化率中有7个为正则总的变化率为正
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
		  //求当前帧数据相对斜率
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
		  //循环更新记录的数据
		  for(int i=0;i<15;++i)
		  {
			  m_aadPrePreGloveData[i]=m_aadPreGloveData[i];
			  m_aadPreGloveData[i]=aadCurGloveData[i];
		  }
		  //进行动态手势识别
		  //比较相邻两帧变化率确定是否出现拐点
		  if(bPreIsODynamicGestureDistinguish||bCurIsODynamicGestureDistinguish||(iPrePositiveSlope*iCurPositiveSlope==2))
		  {
			  int dynamicGestureNum=-1;
			  if(m_iGloveDataCount>10)
			  {
			      dynamicGestureNum=m_dynamicGestureDis->gestureDistinguish(m_aadGloveData,m_iGloveDataCount);
			      cout<<dynamicGestureNum<<endl;

				  //生成模板数据的测试函数

				  保存训练好的模板
				  fstream f("d://dynamicGesture.txt",ios::out);
				  f<<m_iGloveDataCount<<' ';
				  for(int i=0;i<m_iGloveDataCount;i++)
				  {
				  	for(int k=0;k<15;k++)
				  	{
				  		f<<m_aadGloveData[i][k]<<' '; //写入数据
				  	}
				  }
				  f.close();
			  }
			  m_iGloveDataCount=0;//重新开始采集数据 
		  }
		 clockBegin=clockEnd;
	  }
	}
}

//处理主机命令线程执行函数
void ThreadFunCmdRecvProc(SOCKET mySocket)
{
	//从服务器端接收数据
	 while(true)
	 {
	     char buff[1024];
	     int nRecv = ::recv(mySocket, buff, 1024, 0);
         if(nRecv > 0)
         {
             buff[nRecv] = '\0';
			 std::string myComand(buff);
			 //把收到的命令排在命令队列末尾
			 //***********************注意**************************
			 //注意（由于登录时候有延迟所以不能把接收到的一些‘#..’放到命令队列）
			 if(myComand[0]!='#')
			 {
				 EnterCriticalSection(&g_cs);
				 m_deqStrComand.push_back(myComand);
				 LeaveCriticalSection(&g_cs);
			 }
             printf("接收到数据：%s\n", buff);
         }
	 }
}

//用于手机通信的线程
void TheadFunOMobileTextComment()
{
   m_OMobileTextComment->communitionBegin();
}