#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <atlstr.h>//����CString

using namespace std;

#pragma warning(disable:4146)
#pragma warning(disable:4786)
#import "c:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")
class sqlHelper
{
public:
	sqlHelper(void);
	~sqlHelper(void);
	bool dbOpen();//�����ݿ������
	void dbClose();//�ر����ݿ������
	_RecordsetPtr executeQuery(CString sqlStr);//��ѯ
	_RecordsetPtr executeInsert(CString sqlStr);//����
	_RecordsetPtr executeDelete(CString sqlStr);//ɾ��
	_RecordsetPtr executeUpdate(CString sqlStr);//����

private:
	_ConnectionPtr  m_pConnection;  //connection   object's   pointer     
	_CommandPtr     m_pCommand;     //command   object's   pointer   
	_ParameterPtr   m_pParameter;   //Parameter   object's   pointer   
	_RecordsetPtr   m_pRecordset;  

	
	//�������ж�Ӧ������������ɾ���Ȳ���ʱʹ��
	int m_intNo;
	char m_strName[10];
	int m_intPrice;
};

