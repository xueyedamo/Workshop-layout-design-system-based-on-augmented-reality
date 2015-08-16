#pragma once
#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<winsock2.h> 

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <irrlicht.h>
#include <math.h>
#include "PDI.h"

#include <conio.h>
#include <wincon.h>

#include <process.h>


#define pi 3.1415926535897932384626433832795
using namespace std ;
using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class OPositionTracker
{
	public:
		//磁跟踪器部分
		OPositionTracker();
		~OPositionTracker();

		BOOL		Initialize			( VOID );
		BOOL		Connect				( VOID );
		VOID		Disconnect			( VOID );
		BOOL		SetupDevice			( VOID );
		VOID		UpdateStationMap	( VOID );
		VOID		AddMsg				( std::string & );
		VOID		AddResultMsg		( LPCSTR );
		//VOID		ShowMenu			( VOID );	
		//eMenuChoice Prompt				( VOID );
		BOOL		StartCont			( VOID );
		BOOL		StopCont			( VOID );	
		VOID		DisplayCont			( VOID );
		VOID		DisplayFrame		( PBYTE, DWORD );
		//VOID		TrackWorkStart		( VOID );

		//设置摄像头的位置与欧拉角
		void		 SetCameraPosAndEuleAngle(double,double,double,double,double,double);
		BOOL		TrckerWorkBegin ;
		bool		First ;

		//磁跟踪器为240HZ，OPENCV为30帧，每8个数据处理后传递给Opencv
		double pPnoPrevious[6];
		//分别存储位置和欧拉角
		double PosFromTrackX[10];
		double PosFromTrackY[10];
		double PosFromTrackZ[10];
		double EulerFromTrackX[10];
		double EulerFromTrackY[10];
		double EulerFromTrackZ[10];
		//磁跟踪器获得的当期与之前的位置与角度
		double CurrPosFromTrackX;
		double PrevFromTrackX;
		double CurrPosFromTrackY;
		double PrevFromTrackY;
		double CurrPosFromTrackZ;
		double PrevFromTrackZ;

		double DistanceTrackToCameraX ;//磁跟踪器与Camera的空间距离
		double DistanceTrackToCameraY;
		double DistanceTrackToCameraZ ;
		//距离在各个坐标轴的投影
		float PPx ;
		float PPy ;
		float PPz ;
		double DistanceEulerYaw ;
		double DistanceEulerPitch ;
		double DistanceEulerRoll ;

		double CameraPosTrackPx ;//磁跟踪器获得的实时Camera数据
		double CameraPosTrackPy ;//磁跟踪器获得的实时Camera数据
		double CameraPosTrackPz ;//磁跟踪器获得的实时Camera数据
		double CameraEulTrackYaw ;
		double CameraEulTrackPitch ;
		double CameraEulTrackRoll ;
		double CameraEulRightHandTrackYaw ;
		double CameraEulRightHandTrackPitch ;
		double CameraEulRightHandTrackRoll ;

		///////////////////////////////Camera部分		
		double CameraPositionPx ;//摄像头的位置坐标X轴
		double CameraPositionPy ;
		double CameraPositionPz ;
		double CameraEulerYaw ;//摄像头的旋转欧拉角
		double CameraEulerPitch ;
		double CameraEulerRoll ;
		double TargetPositionPx ;//摄像头的Target坐标
		double TargetPositionPy ;
		double TargetPositionPz ;
		double TargetRightHandPositionPx ;//track2的Target坐标
		double TargetRightHandPositionPy ;
		double TargetRightHandPositionPz ;
		double DirectionX;
		double DirectionY;
		double DirectionZ;
		double DirectionRightHandX;
		double DirectionRightHandY;
		double DirectionRightHandZ;
		//第二个传感器放在左手上
		double TrackerLeftHandPosX;
		double TrackerLeftHandPosY;
		double TrackerLeftHandPosZ;
		double TrackerLeftHandYaw;
		double TrackerLeftHandPitch;
		double TrackerLeftHandRoll;
		//第三个传感器放在右手上		
		double TrackerRightHandPosX;
		double TrackerRightHandPosY;
		double TrackerRightHandPosZ;
		double TrackerRightHandYaw;
		double TrackerRightHandPitch;
		double TrackerRightHandRoll;

		double CameraEulFormYaw ;
		double CameraEulFormPitch ;
		double CameraEulFormRoll ;
		double CameraUpPx ;//摄像头朝上的Up向量在X轴的的分量
		double CameraUpPy ;
		double CameraUpPz ;
		double DirectionEulerYaw ;
		double DirectionEulerPitch ;
		double DirectionEulerRoll ;
		double AngleField ;
		double ScaleAR ;

		//磁跟踪器与摄像头距离的预设值
		float DisPx;
		float DisPy;
		float DisPz;

		float posRRX;
		float posRRY;
		float posRRZ;

		float  posPX ;
		float  posPY ;
		float  posPZ ;

		//我添加的方法
		irr::core::vector3df getRotatePosition(irr::core::vector3df startPosition,double length);
	private:
		////////////////////////变量
		CPDIdev		g_pdiDev;
		CPDImdat    g_pdiMDat;
		CPDIser		g_pdiSer; 

		DWORD		g_dwFrameSize;
		BOOL		g_bCnxReady;
		DWORD		g_dwStationMap;
		int			j;
		
};