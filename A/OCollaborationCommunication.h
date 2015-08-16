/*******************************************************
  Function : // 与服务器进行协同设计的类，接收发送各种命令
  CopyRight: // 版权公开(Public)
  Author   : // 张帆 Eli zhang
  ================= Modified list ====================  
  Version    : // v1.2.0
  Modify Date: // 2015.01.20
  Mender     : // zhangxuanyu0904@163.com
  Why        : // 偶然漏掉命令信息
  How        : // 添加命令队列
  Attention  : // 注意事项
*******************************************************/
#pragma once
 #include <stdio.h>
 #include <WINSOCK2.H> 
 #include <iostream>
 #include <string>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")


class OCollaborationCommunication
{
private:
	// 初始化WS2_32.dll
     WSADATA wsaData;
	 //填写远程地址信息
	 sockaddr_in servAddr;
	 //
	/*******************************************************
	Function : 向服务器请求登录
	Parameter: 无
	Return   : bool true=登录成功 false=登录失败
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
	 bool logion();
	 
public:
	//创建套节字
	SOCKET m_kSocket;
	//保存登录的用户名，密码
	std::string m_strUsrName;
	std::string m_strUsrPassword;
	 /*******************************************************
    Function : 协同通讯类构造函数
	Parameter: 
             1   In    string   p_strIp   主机IP地址
	Return   : 无
	Used Var : 无
    Author   : 张帆 Eli zhang
	*******************************************************/
	OCollaborationCommunication(std::string p_strIp);

	
	/*******************************************************
	Function : 向服务器发送信息
	Parameter: 
             1   In    string   p_strSendMessage   要发送的字符串
	Return   : bool true=发送成功 false=发送失败
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
	 bool sendMessage(std::string p_strSendMessage);
	~OCollaborationCommunication(void);
};

