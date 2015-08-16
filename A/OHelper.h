#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <atlstr.h>//用了CString

using namespace std;

#pragma warning(disable:4146)
#pragma warning(disable:4786)
#import "c:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")
class sqlHelper
{
public:
	sqlHelper(void);
	~sqlHelper(void);
	bool dbOpen();//打开数据库的连接
	void dbClose();//关闭数据库的连接
	_RecordsetPtr executeQuery(CString sqlStr);//查询
	_RecordsetPtr executeInsert(CString sqlStr);//插入
	_RecordsetPtr executeDelete(CString sqlStr);//删除
	_RecordsetPtr executeUpdate(CString sqlStr);//更新

private:
	_ConnectionPtr  m_pConnection;  //connection   object's   pointer     
	_CommandPtr     m_pCommand;     //command   object's   pointer   
	_ParameterPtr   m_pParameter;   //Parameter   object's   pointer   
	_RecordsetPtr   m_pRecordset;  

	
	//表中三列对应变量名，插入删除等操作时使用
	int m_intNo;
	char m_strName[10];
	int m_intPrice;
};

