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
#define MSG_LENGTH 1024 //数组的长度

using namespace std;

 class OMobileTextComment
{
public:
	OMobileTextComment(void);
	void communitionBegin();//开始通信
	void communitionEnd();//结束通信
	void setModelSelected(int ID);//设置当前选择的模型名字
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

	int selectID;//选择的模型ID
	sqlHelper mySqlHelper;//声明数据库操作助手类
	std::map<int,std::string> mapIDMessage;
};

