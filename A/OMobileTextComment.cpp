#include "StdAfx.h"
#include "MobileTextComment.h"


MobileTextComment::MobileTextComment(void)
{
	ClientAddrLen=sizeof(ClientAddr);      
	m_nPort = 5001;
	selectID=-1;

	WSAStartup(MAKEWORD(2,2),&m_WSADATA);      
	Listening = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );      
	ServerAddr.sin_family = AF_INET;      
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);      
	ServerAddr.sin_port = htons(m_nPort);      

	bind(Listening,(SOCKADDR *)&ServerAddr,sizeof(ServerAddr));/*��һ��IP��ַ��һ���ض��Ķ˿ں�*/       
	listen(Listening,5);/*ʹ���׽��ִ��ڼ���״̬*/
}

void MobileTextComment::communitionBegin()
{
	while(true)
	{
		/*�Ӵ��ڼ���״̬�����׽��ֵĿͻ��������������ȡ��������ǰ��һ���ͻ�����
		���Ҵ���һ���µ��׽�������ͻ��׽��ִ�������ͨ��*/
		//����һ���ܹ���������ͨ�ŵ��׽���
		printf( "�ȴ��ֻ�����.....\n" ); 
		NewConnection = accept(Listening,(SOCKADDR *)&ClientAddr,&ClientAddrLen);      

		/*��TCP���ӵ���һ�˽�������*/
		if((m_nRecvBytes=recv(NewConnection,buf,MSG_LENGTH,0)) ==  SOCKET_ERROR )      
		{      
		    printf( "\nErrorcode=%d,   Recv   from   client\n", WSAGetLastError() );   
		}
		//�������յ����ַ�����
		buf[m_nRecvBytes]='\0';
		//���ڷָ��ַ�������ַ�
		const char *d = "*";
		char *p;
		p = strtok(buf,d);
		//��ŷָ����ַ���������
		vector<string> vectorStr;
		//�ַ�����ķָ�
		while(p)
		{
			string strRec(p);
			//printf("%s\n",p);
			vectorStr.push_back(strRec);
			p=strtok(NULL,d);
		}
		//�жϴ��ڵ�¼״̬(�����ݿ��жϵ�¼״̬)
		//if(vectorStr.size()==3&&vectorStr[0]=="logion")
		//{
		//	mySqlHelper.dbOpen();//�����ݿ�����
		//	CString strSqlSelect;//����ִ�е�SQL���
		//	CString name=vectorStr[1].c_str();
		//	CString password=vectorStr[2].c_str();
		//	strSqlSelect.Format("select * from usrInfo where usrName='"+name+"'and usrPassword='"+password+"'" );
		//	_RecordsetPtr   m_pRecordset=mySqlHelper.executeQuery(strSqlSelect);

		//	//�����¼����Ϣ�����سɹ���Ϣ
		//	if(!m_pRecordset->adoEOF)//��������ȡname�еļ�¼�����  
		//	{  
		//		cout<<"name   "<<"password   "<<"usrType"<<endl;
		//		string name    = (char *)(_bstr_t)m_pRecordset->GetFields()->GetItem("usrName")->Value;
		//		string password  = (char *)(_bstr_t)m_pRecordset->GetFields()->GetItem("usrPassword")->Value; 
		//		string usrType = (char *)(_bstr_t)m_pRecordset->GetFields()->GetItem("usrType")->Value; 
		//		//AfxMessageBox(temp);  
		//		cout<<name<<"   ";
		//		cout<<password<<"   ";
		//		cout<<usrType<<"   ";
		//		cout<<endl;
		//		m_pRecordset->MoveNext();

		//		//���¼�߻ظ���¼�ɹ�
		//		char* resp="y\n";
		//		///*��TCP���ӵ���һ�˷������ݡ�*/
		//		if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
		//		{   
		//			printf("������Ϣʧ��!");  
		//		}  
		//		cout<<vectorStr[1]<<" success logioning! \n";
		//	}
		//	else
		//	{
		//		//�ظ���¼ʧ����Ϣ
		//		char* resp="fail\n";
		//		///*��TCP���ӵ���һ�˷������ݡ�*/
		//		if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
		//		{   
		//			printf("������Ϣʧ��!");  
		//		}  
		//		cout<<vectorStr[1]<<" failed logioning! \n";
		//	}

		//	mySqlHelper.dbClose();//�ر����ݿ�����(ע��ر�����m_pRecordset������)
		//}
		
		//�жϵ�¼״̬
		if(vectorStr.size()==3&&vectorStr[0]=="logion")
		{
			std::string name=vectorStr[1].c_str();
			std::string password=vectorStr[2].c_str();

			if(name!="")
			{
				cout<<name<<":��½�ɹ���\n";
				/*��TCP���ӵ���һ�˷������ݡ�*/
				char* resp="y\n";
				if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
				{   
					printf("������Ϣʧ��!");  
				}  
			}
			else
			{
				//�ظ���¼ʧ����Ϣ
				char* resp="fail\n";
				///*��TCP���ӵ���һ�˷������ݡ�*/
				if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
				{   
					printf("������Ϣʧ��!");  
				}  
				cout<<name<<":��¼ʧ��! \n";
			}
		}
		//�ѳɹ���¼����ͨ��״̬
		else
		{
			//�õ������ߵ��û���
			string userName=vectorStr[0];
			//���ַ�����ԭΪ���ܵ���Ϣ
			string message=vectorStr[1];
			if(vectorStr.size()>2)
			{
				for(vector<string>::iterator it  = vectorStr.begin()+2; it != vectorStr.end(); )  
				 {  
					   message+="*"+(*it);
					   it++;
				 }  
			}
			cout<<userName<<":"<<message<<endl;
			//�ж��Ƿ�ѡ��ģ�ͣ���ѡ�������ݿ��ύ��ע
			if(selectID!=-1)
			{
				//��������Ϣ����ģ��ID�����ۼ�ֵ��
				mapIDMessage[selectID]+=userName+":"+message+";";

				char* resp="success\n";
				///*��TCP���ӵ���һ�˷������ݡ�*/
				if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
				{   
					printf("����ʧ��!");
				}  
			}
			else
			{
				char* resp="failed\n";
				///*��TCP���ӵ���һ�˷������ݡ�*/
				if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
				{   
					printf("����ʧ��!");
				}  
			}
		}
	}
}

void MobileTextComment::setModelSelected(int ID)
{
	selectID=ID;
}

void MobileTextComment::communitionEnd()
{
	/* Ӧ�ó�������ɶ������Socket���ʹ�ú�Ҫ����WSACleanup�����������
Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
	WSACleanup(); 
}