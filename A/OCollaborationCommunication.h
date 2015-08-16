/*******************************************************
  Function : // �����������Эͬ��Ƶ��࣬���շ��͸�������
  CopyRight: // ��Ȩ����(Public)
  Author   : // �ŷ� Eli zhang
  ================= Modified list ====================  
  Version    : // v1.2.0
  Modify Date: // 2015.01.20
  Mender     : // zhangxuanyu0904@163.com
  Why        : // żȻ©��������Ϣ
  How        : // ����������
  Attention  : // ע������
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
	// ��ʼ��WS2_32.dll
     WSADATA wsaData;
	 //��дԶ�̵�ַ��Ϣ
	 sockaddr_in servAddr;
	 //
	/*******************************************************
	Function : ������������¼
	Parameter: ��
	Return   : bool true=��¼�ɹ� false=��¼ʧ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
	 bool logion();
	 
public:
	//�����׽���
	SOCKET m_kSocket;
	//�����¼���û���������
	std::string m_strUsrName;
	std::string m_strUsrPassword;
	 /*******************************************************
    Function : ЭͬͨѶ�๹�캯��
	Parameter: 
             1   In    string   p_strIp   ����IP��ַ
	Return   : ��
	Used Var : ��
    Author   : �ŷ� Eli zhang
	*******************************************************/
	OCollaborationCommunication(std::string p_strIp);

	
	/*******************************************************
	Function : �������������Ϣ
	Parameter: 
             1   In    string   p_strSendMessage   Ҫ���͵��ַ���
	Return   : bool true=���ͳɹ� false=����ʧ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
	 bool sendMessage(std::string p_strSendMessage);
	~OCollaborationCommunication(void);
};

