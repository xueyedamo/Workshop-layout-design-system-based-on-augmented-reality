#pragma once
#include "stdafx.h"
#include "stdio.h"  
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <WINSOCK2.H>  

#include"Helper.h"

#pragma comment(lib, "Ws2_32.lib")   
#define MSG_LENGTH 1024 //����ĳ���

using namespace std;

 class OMobileTextComment
{
public:
	OMobileTextComment(void);
	void communitionBegin();//��ʼͨ��
	void communitionEnd();//����ͨ��
	void setModelSelected(int ID);//���õ�ǰѡ���ģ������
private:
	WSADATA       m_WSADATA;      
	char          buf[MSG_LENGTH];   
	int           m_nRecvBytes,m_nSendBytes;      
	SOCKET        Listening;
	SOCKET        NewConnection;   
	SOCKADDR_IN   ServerAddr;      
	SOCKADDR_IN   ClientAddr;     
	int ClientAddrLen;
	int m_nPort;

	int selectID;//ѡ���ģ��ID
	sqlHelper mySqlHelper;//�������ݿ����������
	std::map<int,std::string> mapIDMessage;
};

