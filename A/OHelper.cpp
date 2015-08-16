#include "StdAfx.h"
#include "Helper.h"

sqlHelper::sqlHelper(void)
{
	m_pConnection = NULL;  //connection   object's   pointer
	CoInitialize(NULL);//COMע�᲻����
}

bool sqlHelper::dbOpen()
{
	//��������SQLServer���ݿ⣬�������£�
	//���ӵ�MS SQL Server  
	//��ʼ��ָ��  
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));  

	//��ʼ�����Ӳ���  
	_bstr_t strConnect = "Provider=SQLOLEDB;Server=XL-20120116DGDY;Database=layout;uid=sa; pwd=sasa;"; //Databaseָ��ϵͳ�е����ݿ��� 
	 	
	//ִ������  
	try  
		{  
			// Open���������ִ�������BSTR����_bstr_t����  
			m_pConnection->Open(strConnect, "", "", NULL);  
			return true;
		}  
	catch(_com_error &e)  
		{  
			//MessageBox(e.Description(),"����", MB_OK|MB_ICONINFORMATION);
			cout<<e.Description()<<endl;
			return false;
		}//�������Ӵ��� 
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
