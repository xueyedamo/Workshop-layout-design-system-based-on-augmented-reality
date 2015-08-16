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

	bind(Listening,(SOCKADDR *)&ServerAddr,sizeof(ServerAddr));/*绑定一个IP地址和一个特定的端口号*/       
	listen(Listening,5);/*使流套接字处于监听状态*/
}

void MobileTextComment::communitionBegin()
{
	while(true)
	{
		/*从处于监听状态的流套接字的客户连接请求队列中取出排在最前的一个客户请求，
		并且创建一个新的套接字来与客户套接字创建连接通道*/
		//创建一个能够进行网络通信的套接字
		printf( "等待手机接入.....\n" ); 
		NewConnection = accept(Listening,(SOCKADDR *)&ClientAddr,&ClientAddrLen);      

		/*从TCP连接的另一端接收数据*/
		if((m_nRecvBytes=recv(NewConnection,buf,MSG_LENGTH,0)) ==  SOCKET_ERROR )      
		{      
		    printf( "\nErrorcode=%d,   Recv   from   client\n", WSAGetLastError() );   
		}
		//服务器收到的字符数组
		buf[m_nRecvBytes]='\0';
		//用于分割字符数组的字符
		const char *d = "*";
		char *p;
		p = strtok(buf,d);
		//存放分割后的字符串的容器
		vector<string> vectorStr;
		//字符数组的分割
		while(p)
		{
			string strRec(p);
			//printf("%s\n",p);
			vectorStr.push_back(strRec);
			p=strtok(NULL,d);
		}
		//判断处于登录状态(用数据库判断登录状态)
		//if(vectorStr.size()==3&&vectorStr[0]=="logion")
		//{
		//	mySqlHelper.dbOpen();//打开数据库连接
		//	CString strSqlSelect;//具体执行的SQL语句
		//	CString name=vectorStr[1].c_str();
		//	CString password=vectorStr[2].c_str();
		//	strSqlSelect.Format("select * from usrInfo where usrName='"+name+"'and usrPassword='"+password+"'" );
		//	_RecordsetPtr   m_pRecordset=mySqlHelper.executeQuery(strSqlSelect);

		//	//输出登录者信息并返回成功信息
		//	if(!m_pRecordset->adoEOF)//遍历并读取name列的记录并输出  
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

		//		//向登录者回复登录成功
		//		char* resp="y\n";
		//		///*向TCP连接的另一端发送数据。*/
		//		if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
		//		{   
		//			printf("发送信息失败!");  
		//		}  
		//		cout<<vectorStr[1]<<" success logioning! \n";
		//	}
		//	else
		//	{
		//		//回复登录失败信息
		//		char* resp="fail\n";
		//		///*向TCP连接的另一端发送数据。*/
		//		if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
		//		{   
		//			printf("发送信息失败!");  
		//		}  
		//		cout<<vectorStr[1]<<" failed logioning! \n";
		//	}

		//	mySqlHelper.dbClose();//关闭数据库连接(注意关闭连接m_pRecordset不可用)
		//}
		
		//判断登录状态
		if(vectorStr.size()==3&&vectorStr[0]=="logion")
		{
			std::string name=vectorStr[1].c_str();
			std::string password=vectorStr[2].c_str();

			if(name!="")
			{
				cout<<name<<":登陆成功！\n";
				/*向TCP连接的另一端发送数据。*/
				char* resp="y\n";
				if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
				{   
					printf("发送信息失败!");  
				}  
			}
			else
			{
				//回复登录失败信息
				char* resp="fail\n";
				///*向TCP连接的另一端发送数据。*/
				if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
				{   
					printf("发送信息失败!");  
				}  
				cout<<name<<":登录失败! \n";
			}
		}
		//已成功登录处于通信状态
		else
		{
			//得到发送者的用户名
			string userName=vectorStr[0];
			//把字符串还原为接受的信息
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
			//判断是否选中模型，若选中向数据库提交备注
			if(selectID!=-1)
			{
				//将评论信息加入模型ID，评论键值对
				mapIDMessage[selectID]+=userName+":"+message+";";

				char* resp="success\n";
				///*向TCP连接的另一端发送数据。*/
				if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
				{   
					printf("发送失败!");
				}  
			}
			else
			{
				char* resp="failed\n";
				///*向TCP连接的另一端发送数据。*/
				if((m_nSendBytes = send(NewConnection,resp, static_cast<int>(strlen(resp)),0)) == SOCKET_ERROR)   
				{   
					printf("发送失败!");
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
	/* 应用程序在完成对请求的Socket库的使用后，要调用WSACleanup函数来解除与
Socket库的绑定并且释放Socket库所占用的系统资源。*/
	WSACleanup(); 
}