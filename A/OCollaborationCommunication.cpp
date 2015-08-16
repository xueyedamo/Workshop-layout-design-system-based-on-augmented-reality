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
#include "StdAfx.h"
#include "OCollaborationCommunication.h"

/*******************************************************
    Function : 协同通讯类构造函数
	Parameter: 
             1   In    string   p_strIp   主机IP地址
	Return   : 无
	Used Var : 无
    Author   : 张帆 Eli zhang
*******************************************************/
OCollaborationCommunication::OCollaborationCommunication(std::string p_strIp)
{
	WSAStartup(MAKEWORD(2,2),&wsaData);
	//初始化套接字
	m_kSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_kSocket == INVALID_SOCKET)
     {
         printf(" Failed socket() \n");
     }
	else
	{
		cout<<"已经创建好套节字"<<endl;
	}
	// 填写远程地址信息
     servAddr.sin_family = AF_INET;
     servAddr.sin_port = htons(4567);
     // 注意，这里要填写服务器程序（TCPServer程序）所在机器的IP地址
     // 如果你的计算机没有联网，直接使用127.0.0.1即可
     servAddr.sin_addr.S_un.S_addr = inet_addr(p_strIp.c_str());

	 //请求登录函数
	 logion();
}

/*******************************************************
	Function : 向服务器请求登录
	Parameter: 无
	Return   : bool true=登录成功 false=登录失败
	Used Var : 无
	Author   : 张帆 Eli zhang
*******************************************************/
bool OCollaborationCommunication::logion()
{
	//向服务器申请登录循环
	 while(true)
	 {
		 m_kSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		 if(::connect(m_kSocket, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
		 {
			 printf(" Failed connect() \n");
		 }
		 else
		 {
			 cout<<"socket连接成功"<<endl;
		 }
		cout<<"请输入用户名： \n";
		cin>>m_strUsrName;
		cout<<"请输入密码： \n";
		cin>>m_strUsrPassword;
		string message=m_strUsrName+"*"+m_strUsrPassword;
		const char* szText;
		szText=message.c_str();
		//向服务器发送输入的用户名和密码
		::send(m_kSocket, szText, strlen(szText), 0) ;
		char recText[256];
		//接收服务器返回的值
		int nRecv = ::recv(m_kSocket, recText, 256, 0);
         if(nRecv > 0)
         {
             recText[nRecv] = '\0';
             string strRec(recText);
			 //登录成功
			 if(strRec=="success logion")
			 {
				 cout<<strRec<<endl;
				 break;
			 }
			 //登录失败
			 else
			 {
				 cout<<strRec<<endl;
				 // 关闭套节字
                 ::closesocket(m_kSocket);
				 cout<<"请重新登录！"<<endl;
				 continue;
			 }
         }
	 }
	 return true;
}

/*******************************************************
	Function : 向服务器发送信息
	Parameter: 
             1   In    string   p_strSendMessage   要发送的字符串
	Return   : bool true=发送成功 false=发送失败
	Used Var : 无
	Author   : 张帆 Eli zhang
*******************************************************/
bool OCollaborationCommunication::sendMessage(std::string p_strSendMessage)
{
	// 向服务器端发送数据
	const char* szText;
	szText=p_strSendMessage.c_str();
    ::send(m_kSocket, szText, strlen(szText), 0) ;
	cout<<"socket: "<<m_kSocket<<" send message"<<endl;
	return true;
}


OCollaborationCommunication::~OCollaborationCommunication(void)
{
	// 关闭套节字
    ::closesocket(m_kSocket);
}
