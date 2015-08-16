#include "StdAfx.h"
#include "Helper.h"

sqlHelper::sqlHelper(void)
{
	m_pConnection = NULL;  //connection   object's   pointer
	CoInitialize(NULL);//COM注册不可少
}

bool sqlHelper::dbOpen()
{
	//例如连接SQLServer数据库，代码如下：
	//连接到MS SQL Server  
	//初始化指针  
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));  

	//初始化链接参数  
	_bstr_t strConnect = "Provider=SQLOLEDB;Server=XL-20120116DGDY;Database=layout;uid=sa; pwd=sasa;"; //Database指你系统中的数据库名 
	 	
	//执行连接  
	try  
		{  
			// Open方法连接字串必须四BSTR或者_bstr_t类型  
			m_pConnection->Open(strConnect, "", "", NULL);  
			return true;
		}  
	catch(_com_error &e)  
		{  
			//MessageBox(e.Description(),"警告", MB_OK|MB_ICONINFORMATION);
			cout<<e.Description()<<endl;
			return false;
		}//发生链接错误 
}

void sqlHelper::dbClose()
{
	m_pConnection->Close();
	m_pConnection = NULL ;
}

_RecordsetPtr sqlHelper::executeQuery(CString sqlStr)
{
	m_pRecordset=m_pConnection->Execute(_bstr_t(sqlStr),NULL,adCmdText);
	cout<<"Query success!"<<endl;
	return m_pRecordset;
}

_RecordsetPtr sqlHelper::executeInsert(CString sqlStr)
{
	m_pRecordset=m_pConnection->Execute(_bstr_t(sqlStr),NULL,adCmdText);
	cout<<"Insert success!"<<endl;
	return m_pRecordset;
}

_RecordsetPtr sqlHelper::executeDelete(CString sqlStr)
{
	m_pRecordset=m_pConnection->Execute(_bstr_t(sqlStr),NULL,adCmdText);
	cout<<"Delete success!"<<endl;
	return m_pRecordset;
}

_RecordsetPtr sqlHelper::executeUpdate(CString sqlStr)
{
	m_pRecordset=m_pConnection->Execute(_bstr_t(sqlStr),NULL,adCmdText);
	cout<<"Update success!"<<endl;
	return m_pRecordset;
}

sqlHelper::~sqlHelper(void)
{
}
