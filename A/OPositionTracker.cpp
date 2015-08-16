#include"stdafx.h"
#include"OPositionTracker.h"
#include <irrlicht.h>
using namespace std ;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
OPositionTracker::OPositionTracker()
{
	TrckerWorkBegin = false;
	First = true;
	j=0;
	AngleField = 5.1428;
	ScaleAR = 0.8;
	//磁跟踪器与摄像头距离的预设值
	DisPx=-3.05;
	DisPy=-0.55;
	DisPz=-0.15;

	posRRX = -0;
	posRRY = -0.3f;
	posRRZ = -0;

	posPX = 0;
	posPY = 0;
	posPZ = 0;
	//磁跟踪器与Camera的空间距离
	DistanceTrackToCameraX = ( DisPx+posPX);
	DistanceTrackToCameraY = (DisPy+posPY);
	DistanceTrackToCameraZ = (DisPz+posPZ);
//距离在各个坐标轴的投影
	PPx = 0;
	PPy = 0;
	PPz = 0;
	DistanceEulerYaw = 0;
	DistanceEulerPitch = 0;
	DistanceEulerRoll = 0;

	CameraPosTrackPx = 0;//磁跟踪器获得的实时Camera数据
	CameraPosTrackPy = 0;//磁跟踪器获得的实时Camera数据
	CameraPosTrackPz = 0;//磁跟踪器获得的实时Camera数据
	CameraEulTrackYaw = 0;
	CameraEulTrackPitch = 0;
	CameraEulTrackRoll = 0;
	CameraEulFormYaw = -0.757;
	CameraEulFormPitch = 0.6;
	CameraEulFormRoll = -1.4;
	CameraUpPx = 0;//摄像头朝上的Up向量在X轴的的分量
	CameraUpPy = 1;
	CameraUpPz = 0;
	DirectionEulerYaw = 0;
	DirectionEulerPitch = 0;
	DirectionEulerRoll = 0;

}
OPositionTracker::~OPositionTracker()
{

}
BOOL OPositionTracker::Initialize( VOID )
{
	BOOL	bRet = TRUE;

	g_pdiDev.Trace(FALSE, 5);

	//::SetConsoleTitle( "PDIconsole" );

    g_pdiMDat.Empty();
	g_pdiMDat.Append( PDI_MODATA_FRAMECOUNT );
    g_pdiMDat.Append( PDI_MODATA_POS );
    g_pdiMDat.Append( PDI_MODATA_ORI );
	g_dwFrameSize = 8+12+12+4;

	g_bCnxReady = FALSE;
	g_dwStationMap = 0;

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BOOL OPositionTracker::Connect( VOID )
{
    std::string msg;
    if (!(g_pdiDev.CnxReady()))
    {
		g_pdiDev.SetSerialIF( &g_pdiSer );

        ePiCommType eType = g_pdiDev.DiscoverCnx();
		switch (eType)
		{
		case PI_CNX_USB:
			msg = "USB Connection: " + std::string( g_pdiDev.GetLastResultStr() ) + "\r\n";
			break;
		case PI_CNX_SERIAL:
			msg = "Serial Connection: " + std::string( g_pdiDev.GetLastResultStr() ) + "\r\n";
			break;
		default:
			msg = "DiscoverCnx result: " + std::string( g_pdiDev.GetLastResultStr() ) + "\r\n";
			break;
		}
        g_bCnxReady = g_pdiDev.CnxReady();
		AddMsg( msg );

    }
    else
    {
        msg = "Already connected\r\n";
        g_bCnxReady = TRUE;
	    AddMsg( msg );
    }

	return g_bCnxReady;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
VOID OPositionTracker::Disconnect()
{
    std::string msg;
    if (!(g_pdiDev.CnxReady()))
    {
        msg = "Already disconnected\r\n";
    }
    else
    {
        g_pdiDev.Disconnect();
        msg = "Disconnect result: " + std::string(g_pdiDev.GetLastResultStr()) + "\r\n";
    }
    AddMsg( msg );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BOOL OPositionTracker::SetupDevice( VOID )
{
	FLOAT pv[3] = {.0f, .0f, 1.0f}; 
	g_pdiDev.SetSHemisphere(-1,(PDI3vec)pv);
	g_pdiDev.StartPipeExport();
	AddResultMsg("StartPipeExport");

	std::string msg;

	g_pdiDev.SetSDataList( -1, g_pdiMDat );
	AddResultMsg("SetSDataList");

	CPDIbiterr cBE;
	g_pdiDev.GetBITErrs( cBE );
	AddResultMsg("GetBITErrs");

	CHAR sz[100];
	cBE.Parse( sz, 100 );
	msg = "BIT Errors: " + std::string(sz) + "\r\n";
	AddMsg(msg);

	if (!(cBE.IsClear()))
	{
		g_pdiDev.ClearBITErrs();
		AddResultMsg("ClearBITErrs");
	}

	UpdateStationMap();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
VOID OPositionTracker::UpdateStationMap( VOID )
{
	g_pdiDev.GetStationMap( g_dwStationMap );
	AddResultMsg("GetStationMap");

	CHAR szMsg[100];
	sprintf(szMsg, "ActiveStationMap: %#x\r\n", g_dwStationMap );

	AddMsg( std::string(szMsg) );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
VOID OPositionTracker::AddMsg( std::string & csMsg )
{
	cout << csMsg ;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
VOID OPositionTracker::AddResultMsg( LPCSTR szCmd )
{
	std::string msg;

	//msg.Format("%s result: %s\r\n", szCmd, m_pdiDev.GetLastResultStr() );
	msg = std::string(szCmd) + " result: " + std::string( g_pdiDev.GetLastResultStr() ) + "\r\n";
	AddMsg( msg );
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
BOOL OPositionTracker::StartCont( VOID )
{
	BOOL bRet = FALSE;

	if (!(g_pdiDev.StartContPno(0)))
	{
	}
	else
	{
		bRet = TRUE;
		Sleep(1000);
	}
	AddResultMsg("\nStartContPno");

	return bRet;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
BOOL OPositionTracker::StopCont( VOID )
{
	BOOL bRet = FALSE;

	if (!(g_pdiDev.StopContPno()))
	{
	}
	else
	{
		bRet = TRUE;
		Sleep(1000);
	}
	AddResultMsg("StopContPno");

	return bRet;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
VOID OPositionTracker::DisplayCont( VOID )
{
	BOOL bExit = FALSE;

	PBYTE pBuf;
	DWORD dwSize;
	PBYTE pLastBuf = 0;

	do{
		if (!(g_pdiDev.LastPnoPtr(pBuf, dwSize)))
		{
			AddResultMsg("LastPnoPtr");
			bExit = TRUE;
		}
		else if ((pBuf == 0) || (dwSize == 0))
		{
		}
		else if (pBuf != pLastBuf)
		{
			pLastBuf = pBuf;
			DisplayFrame( pBuf, dwSize );
		}
	}while(1);
}

VOID OPositionTracker::DisplayFrame( PBYTE pBuf, DWORD dwSize )
{
	/*CHAR	szFrame[200];*/
	DWORD		i = 0;
	
    while ( i<dwSize)
    {
	    BYTE ucSensor = pBuf[i+2];
		SHORT shSize = pBuf[i+6];

        // skip rest of header
        i += 8;
		
		PDWORD pFC  = (PDWORD)(&pBuf[i]);
		PFLOAT pPno = (PFLOAT)(&pBuf[i+4]);
		if(ucSensor == 1)//第一个传感器采集的的数据用来处理摄像头的位置
		{
			if(First == true)
			{
				pPnoPrevious[0] = pPno[0];
				pPnoPrevious[1] = pPno[1];
				pPnoPrevious[2] = pPno[2];
			}
			else
			{
				if ((fabs(pPnoPrevious[0] - pPno[0])>=0.4)||(fabs(pPnoPrevious[1] - pPno[1])>=0.4)
				||(fabs(pPnoPrevious[2] - pPno[2])>=0.4))
				{
					pPno[0] = pPnoPrevious[0];
					pPno[1] = pPnoPrevious[1];
					pPno[2] = pPnoPrevious[2];
				}
			}		

			j ++;//连续采集8组数据
		////磁跟踪器的数据传给摄像头
			if(j<=8)
			{
				PosFromTrackX[j] = pPno[0];
				PosFromTrackY[j] = pPno[1];
				PosFromTrackZ[j] = pPno[2];

				PrevFromTrackX += PosFromTrackX[j];
				PrevFromTrackY += PosFromTrackY[j];
				PrevFromTrackZ += PosFromTrackZ[j];

			if(j==8)
			{
				CurrPosFromTrackX = PrevFromTrackX/8;
				CurrPosFromTrackY = PrevFromTrackY/8;
				CurrPosFromTrackZ = PrevFromTrackZ/8;
				//////////////////////////////////////
				
				
				//////////////////////////////////////
				SetCameraPosAndEuleAngle(CurrPosFromTrackX, CurrPosFromTrackY, CurrPosFromTrackZ, pPno[3], pPno[4], pPno[5]);
				//清零
				PrevFromTrackX = 0;
				PrevFromTrackY = 0;
				PrevFromTrackZ = 0;
				j = 0;
			}
			
			}
				
			i += shSize;
			//将当前坐标赋给之前的值
			pPnoPrevious[0] = pPno[0];
			pPnoPrevious[1] = pPno[1];
			pPnoPrevious[2] = pPno[2];
		}
		
		else if(ucSensor == 2)//第二个传感器采集的的数据用来左手的位置
		{
			TrackerLeftHandPosX = -pPno[0]/ScaleAR;
			TrackerLeftHandPosY = pPno[2]/ScaleAR;
			TrackerLeftHandPosZ = pPno[1]/ScaleAR;
			TrackerLeftHandYaw = pPno[3];
			TrackerLeftHandPitch = pPno[4];
			TrackerLeftHandRoll = pPno[5];
			i += shSize;
		}
		else if(ucSensor == 3)
		{
			TrackerRightHandPosX = -pPno[0]/ScaleAR;
			TrackerRightHandPosY = pPno[2]/ScaleAR;
			TrackerRightHandPosZ = pPno[1]/ScaleAR;

			CameraEulRightHandTrackYaw = pPno[3] + 90;
			CameraEulRightHandTrackPitch = pPno[4] ;/////////////////////////
			CameraEulRightHandTrackRoll = pPno[5] ;///////////////////////////

			CameraEulRightHandTrackYaw *= pi / 180.0;
			CameraEulRightHandTrackPitch *= pi / 180.0;
			CameraEulRightHandTrackRoll *= pi / 180.0;

			TrackerRightHandYaw  = -CameraEulRightHandTrackYaw;
			TrackerRightHandPitch  = CameraEulRightHandTrackRoll;
			TrackerRightHandRoll  = CameraEulRightHandTrackPitch;

			/////////////////////////我注释掉的高的代码/////////////////////////////////////////
			//DirectionRightHandX = 1*(cos(TrackerRightHandYaw)*sin(TrackerRightHandPitch)*sin(TrackerRightHandRoll)-
			//	sin(TrackerRightHandYaw)*cos(TrackerRightHandRoll));
			//DirectionRightHandZ = 1*(sin(TrackerRightHandYaw)*sin(TrackerRightHandPitch)*sin(TrackerRightHandRoll)+
			//	cos(TrackerRightHandYaw)*cos(TrackerRightHandRoll));
			//DirectionRightHandY = 1*(cos(TrackerRightHandPitch)*sin(TrackerRightHandRoll));
			//
			//TargetRightHandPositionPx = TrackerRightHandPosX + DirectionRightHandX*3000;//得到新的摄像头的Target
			//TargetRightHandPositionPy = TrackerRightHandPosY + DirectionRightHandY*3000;
			//TargetRightHandPositionPz = TrackerRightHandPosZ + DirectionRightHandZ*3000;
			/////////////////////////我注释掉的高的代码/////////////////////////////////////////

			i += shSize;
		}
		
	}	
	First = false;//第一次启动的标志
}

void  OPositionTracker::SetCameraPosAndEuleAngle(double Px ,double Py,double Pz,double Yaw,double Pitch,double Roll)
{	
	/*DistanceTrackToCameraX = 3 /1.35;
	CameraPosTrackPx =  Px /1.35;
	CameraPosTrackPy =  -Pz/1.35;
	CameraPosTrackPz =  -Py/1.35;*/
	CameraPosTrackPx =  -Px /ScaleAR;
	CameraPosTrackPy =  Pz/ScaleAR;
	CameraPosTrackPz =  Py/ScaleAR;

	////////////////////////////////////////////
	/*CameraPositionPx = CameraPosTrackPx ;
	CameraPositionPy = CameraPosTrackPy  ;
	CameraPositionPz = CameraPosTrackPz ;*/
	//DirectionX = 0;//求出摄像头的朝向
	//DirectionY = 0;
	//DirectionZ = 1;
	//
	
	CameraEulTrackYaw = Yaw + 90;
	CameraEulTrackPitch = Pitch ;/////////////////////////
	CameraEulTrackRoll = Roll ;///////////////////////////
	
	//Camera欧拉角 实际是磁跟踪接收器获得的角度在乘以一个欧拉角校正矩阵
	CameraEulerYaw = CameraEulFormYaw + CameraEulTrackYaw;
	CameraEulerPitch = CameraEulFormPitch + CameraEulTrackPitch;
	CameraEulerRoll = CameraEulFormRoll + CameraEulTrackRoll;	

	CameraEulerYaw *= pi / 180.0;
	CameraEulerPitch *= pi / 180.0;
	CameraEulerRoll *= pi / 180.0;

		
	//摄像头位置的计算//摄像头在接收器下的位置只与接收器、摄像头本身有关，采用的计算角度应为其实际采集到得数据
	DistanceEulerYaw = (-CameraEulTrackYaw) * pi / 180.0;
	DistanceEulerPitch = CameraEulTrackRoll * pi / 180.0;
	DistanceEulerRoll = CameraEulTrackPitch * pi / 180.0;
	PPx = (DistanceTrackToCameraX*cos(DistanceEulerYaw)*cos(DistanceEulerPitch) + 
		DistanceTrackToCameraZ*(cos(DistanceEulerYaw)*sin(DistanceEulerPitch)*sin(DistanceEulerRoll)-sin(DistanceEulerYaw)*cos(DistanceEulerRoll))
		+DistanceTrackToCameraY*(cos(DistanceEulerYaw)*sin(DistanceEulerPitch)*cos(DistanceEulerRoll)+sin(DistanceEulerYaw)*sin(DistanceEulerRoll)));
	PPz = DistanceTrackToCameraX*(sin(DistanceEulerYaw)*cos(DistanceEulerPitch)) + DistanceTrackToCameraZ*(sin(DistanceEulerYaw)*sin(DistanceEulerPitch)*sin(DistanceEulerRoll)+
		cos(DistanceEulerYaw)*cos(DistanceEulerRoll)) + DistanceTrackToCameraY*(sin(DistanceEulerYaw)*sin(DistanceEulerPitch)*cos(DistanceEulerRoll)
		-cos(DistanceEulerYaw)*sin(DistanceEulerRoll));
	PPy = DistanceTrackToCameraX*(-sin(DistanceEulerPitch)) + DistanceTrackToCameraZ*(cos(DistanceEulerPitch)*sin(DistanceEulerRoll)) + 
		DistanceTrackToCameraY*(cos(DistanceEulerPitch)*cos(DistanceEulerRoll));

	//摄像头朝向的计算
	/*DirectionX = sin(CameraEulerYaw);
	DirectionZ = cos(CameraEulerYaw) * cos(CameraEulerPitch);
	DirectionY = sin(CameraEulerPitch);*/
	DirectionEulerYaw = -CameraEulerYaw;
	DirectionEulerPitch = CameraEulerRoll;
	DirectionEulerRoll = CameraEulerPitch;

	DirectionX = 1*(cos(DirectionEulerYaw)*sin(DirectionEulerPitch)*sin(DirectionEulerRoll)-
		sin(DirectionEulerYaw)*cos(DirectionEulerRoll));
	DirectionZ = 1*(sin(DirectionEulerYaw)*sin(DirectionEulerPitch)*sin(DirectionEulerRoll)+
		cos(DirectionEulerYaw)*cos(DirectionEulerRoll));
	DirectionY = 1*(cos(DirectionEulerPitch)*sin(DirectionEulerRoll));

	////摄像头的Up朝向
	/*CameraUpPx = sin(-CameraEulerYaw)*sin(CameraEulerPitch)-cos(CameraEulerPitch)*sin(-CameraEulerRoll)*cos(-CameraEulerYaw);
	CameraUpPy = cos(CameraEulerPitch)*cos(-CameraEulerRoll);
	CameraUpPz = -cos(-CameraEulerYaw)*sin(CameraEulerPitch)-cos(CameraEulerPitch)*sin(-CameraEulerRoll)*sin(-CameraEulerYaw);*/
	CameraUpPx = 1*(cos(DirectionEulerYaw)*sin(DirectionEulerPitch)*cos(DirectionEulerRoll)+sin(DirectionEulerYaw)*sin(DirectionEulerRoll));
	CameraUpPz = 1*(sin(DirectionEulerYaw)*sin(DirectionEulerPitch)*cos(DirectionEulerRoll)-cos(DirectionEulerYaw)*sin(DirectionEulerRoll));
	CameraUpPy = 1*(cos(DirectionEulerPitch)*cos(DirectionEulerRoll));

	


	
	CameraPositionPx = PPx + CameraPosTrackPx;//根据坐标变换求出Camera在世界坐标系下的位置
	CameraPositionPy = PPy + CameraPosTrackPy;
	CameraPositionPz = PPz + CameraPosTrackPz;

	//////////////////////////////////////////////////////////////////////////////
	TargetPositionPx = CameraPositionPx + DirectionX;//得到新的摄像头的Target
	TargetPositionPy = CameraPositionPy + DirectionY;
	TargetPositionPz = CameraPositionPz + DirectionZ;
	
}


//求向量旋转之后终点坐标
irr::core::vector3df OPositionTracker::getRotatePosition(irr::core::vector3df startPosition,double length)
{
	DirectionRightHandX = 1*(cos(TrackerRightHandYaw)*sin(TrackerRightHandPitch)*sin(TrackerRightHandRoll)-
				sin(TrackerRightHandYaw)*cos(TrackerRightHandRoll));
	DirectionRightHandZ = 1*(sin(TrackerRightHandYaw)*sin(TrackerRightHandPitch)*sin(TrackerRightHandRoll)+
		cos(TrackerRightHandYaw)*cos(TrackerRightHandRoll));
	DirectionRightHandY = 1*(cos(TrackerRightHandPitch)*sin(TrackerRightHandRoll));
	irr::core::vector3df endPosition;
	endPosition.X = startPosition.X + DirectionRightHandX*length;//得到新的摄像头的Target
	endPosition.Y = startPosition.Y + DirectionRightHandY*length;
	endPosition.Z = startPosition.Z + DirectionRightHandZ*length;
	return endPosition;
}