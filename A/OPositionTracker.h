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
		//�Ÿ���������
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

		//��������ͷ��λ����ŷ����
		void		 SetCameraPosAndEuleAngle(double,double,double,double,double,double);
		BOOL		TrckerWorkBegin ;
		bool		First ;

		//�Ÿ�����Ϊ240HZ��OPENCVΪ30֡��ÿ8�����ݴ���󴫵ݸ�Opencv
		double pPnoPrevious[6];
		//�ֱ�洢λ�ú�ŷ����
		double PosFromTrackX[10];
		double PosFromTrackY[10];
		double PosFromTrackZ[10];
		double EulerFromTrackX[10];
		double EulerFromTrackY[10];
		double EulerFromTrackZ[10];
		//�Ÿ�������õĵ�����֮ǰ��λ����Ƕ�
		double CurrPosFromTrackX;
		double PrevFromTrackX;
		double CurrPosFromTrackY;
		double PrevFromTrackY;
		double CurrPosFromTrackZ;
		double PrevFromTrackZ;

		double DistanceTrackToCameraX ;//�Ÿ�������Camera�Ŀռ����
		double DistanceTrackToCameraY;
		double DistanceTrackToCameraZ ;
		//�����ڸ����������ͶӰ
		float PPx ;
		float PPy ;
		float PPz ;
		double DistanceEulerYaw ;
		double DistanceEulerPitch ;
		double DistanceEulerRoll ;

		double CameraPosTrackPx ;//�Ÿ�������õ�ʵʱCamera����
		double CameraPosTrackPy ;//�Ÿ�������õ�ʵʱCamera����
		double CameraPosTrackPz ;//�Ÿ�������õ�ʵʱCamera����
		double CameraEulTrackYaw ;
		double CameraEulTrackPitch ;
		double CameraEulTrackRoll ;
		double CameraEulRightHandTrackYaw ;
		double CameraEulRightHandTrackPitch ;
		double CameraEulRightHandTrackRoll ;

		///////////////////////////////Camera����		
		double CameraPositionPx ;//����ͷ��λ������X��
		double CameraPositionPy ;
		double CameraPositionPz ;
		double CameraEulerYaw ;//����ͷ����תŷ����
		double CameraEulerPitch ;
		double CameraEulerRoll ;
		double TargetPositionPx ;//����ͷ��Target����
		double TargetPositionPy ;
		double TargetPositionPz ;
		double TargetRightHandPositionPx ;//track2��Target����
		double TargetRightHandPositionPy ;
		double TargetRightHandPositionPz ;
		double DirectionX;
		double DirectionY;
		double DirectionZ;
		double DirectionRightHandX;
		double DirectionRightHandY;
		double DirectionRightHandZ;
		//�ڶ�������������������
		double TrackerLeftHandPosX;
		double TrackerLeftHandPosY;
		double TrackerLeftHandPosZ;
		double TrackerLeftHandYaw;
		double TrackerLeftHandPitch;
		double TrackerLeftHandRoll;
		//����������������������		
		double TrackerRightHandPosX;
		double TrackerRightHandPosY;
		double TrackerRightHandPosZ;
		double TrackerRightHandYaw;
		double TrackerRightHandPitch;
		double TrackerRightHandRoll;

		double CameraEulFormYaw ;
		double CameraEulFormPitch ;
		double CameraEulFormRoll ;
		double CameraUpPx ;//����ͷ���ϵ�Up������X��ĵķ���
		double CameraUpPy ;
		double CameraUpPz ;
		double DirectionEulerYaw ;
		double DirectionEulerPitch ;
		double DirectionEulerRoll ;
		double AngleField ;
		double ScaleAR ;

		//�Ÿ�����������ͷ�����Ԥ��ֵ
		float DisPx;
		float DisPy;
		float DisPz;

		float posRRX;
		float posRRY;
		float posRRZ;

		float  posPX ;
		float  posPY ;
		float  posPZ ;

		//����ӵķ���
		irr::core::vector3df getRotatePosition(irr::core::vector3df startPosition,double length);
	private:
		////////////////////////����
		CPDIdev		g_pdiDev;
		CPDImdat    g_pdiMDat;
		CPDIser		g_pdiSer; 

		DWORD		g_dwFrameSize;
		BOOL		g_bCnxReady;
		DWORD		g_dwStationMap;
		int			j;
		
};