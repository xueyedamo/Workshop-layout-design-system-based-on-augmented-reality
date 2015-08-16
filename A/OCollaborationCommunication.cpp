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
#include "StdAfx.h"
#include "OCollaborationCommunication.h"

/*******************************************************
    Function : ЭͬͨѶ�๹�캯��
	Parameter: 
             1   In    string   p_strIp   ����IP��ַ
	Return   : ��
	Used Var : ��
    Author   : �ŷ� Eli zhang
*******************************************************/
OCollaborationCommunication::OCollaborationCommunication(std::string p_strIp)
{
	WSAStartup(MAKEWORD(2,2),&wsaData);
	//��ʼ���׽���
	m_kSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_kSocket == INVALID_SOCKET)
     {
         printf(" Failed socket() \n");
     }
	else
	{
		cout<<"�Ѿ��������׽���"<<endl;
	}
	// ��дԶ�̵�ַ��Ϣ
     servAddr.sin_family = AF_INET;
     servAddr.sin_port = htons(4567);
     // ע�⣬����Ҫ��д����������TCPServer�������ڻ�����IP��ַ
     // �����ļ����û��������ֱ��ʹ��127.0.0.1����
     servAddr.sin_addr.S_un.S_addr = inet_addr(p_strIp.c_str());

	 //�����¼����
	 logion();
}

/*******************************************************
	Function : ������������¼
	Parameter: ��
	Return   : bool true=��¼�ɹ� false=��¼ʧ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
*******************************************************/
bool OCollaborationCommunication::logion()
{
	//������������¼ѭ��
	 while(true)
	 {
		 m_kSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		 if(::connect(m_kSocket, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
		 {
			 printf(" Failed connect() \n");
		 }
		 else
		 {
			 cout<<"socket���ӳɹ�"<<endl;
		 }
		cout<<"�������û����� \n";
		cin>>m_strUsrName;
		cout<<"���������룺 \n";
		cin>>m_strUsrPassword;
		string message=m_strUsrName+"*"+m_strUsrPassword;
		const char* szText;
		szText=message.c_str();
		//�����������������û���������
		::send(m_kSocket, szText, strlen(szText), 0) ;
		char recText[256];
		//���շ��������ص�ֵ
		int nRecv = ::recv(m_kSocket, recText, 256, 0);
         if(nRecv > 0)
         {
             recText[nRecv] = '\0';
             string strRec(recText);
			 //��¼�ɹ�
			 if(strRec=="success logion")
			 {
				 cout<<strRec<<endl;
				 break;
			 }
			 //��¼ʧ��
			 else
			 {
				 cout<<strRec<<endl;
				 // �ر��׽���
                 ::closesocket(m_kSocket);
				 cout<<"�����µ�¼��"<<endl;
				 continue;
			 }
         }
	 }
	 return true;
}

/*******************************************************
	Function : �������������Ϣ
	Parameter: 
             1   In    string   p_strSendMessage   Ҫ���͵��ַ���
	Return   : bool true=���ͳɹ� false=����ʧ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
*******************************************************/
bool OCollaborationCommunication::sendMessage(std::string p_strSendMessage)
{
	// ��������˷�������
	const char* szText;
	szText=p_strSendMessage.c_str();
    ::send(m_kSocket, szText, strlen(szText), 0) ;
	cout<<"socket: "<<m_kSocket<<" send message"<<endl;
	return true;
}


OCollaborationCommunication::~OCollaborationCommunication(void)
{
	// �ر��׽���
    ::closesocket(m_kSocket);
}
