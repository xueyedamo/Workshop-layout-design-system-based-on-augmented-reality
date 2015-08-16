/*******************************************************
  Function : // ��̬����ʶ����ࣨӦ�ö�̬ʱ������㷨DTW���ж�̬����ʶ��
  CopyRight: // ��Ȩ����(Public)
  Author   : // �ŷ� Eli zhang
  ================= Modified list ====================  
  Version    : // v1.2.0
  Modify Date: // 2015.02.20
  Mender     : // zhangxuanyu0904@163.com
  Why        : // ��̬����ʶ��
  How        : // ���DTW�㷨
  Attention  : // ע������
*******************************************************/
#pragma once
#include <stdio.h>
#include <iostream>
#include <cmath>
#include "MyDTW.h"
#include "matrix.h"
#include<fstream>
using namespace std;

#define DTWMAXNUM 50
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)>0?(a):(-(a)))
#define DTWVERYBIG 100000000.0
#define G 15//����ʸ����ά
#define THRESHOLD 50.0//����ѵ������ֵ

class ODynamicGestureDistinguish
{
public:
	ODynamicGestureDistinguish(void);
	~ODynamicGestureDistinguish(void);

	/*******************************************************
	Function : ����������֮���ƥ�����
	Parameter: 
             1   In    double[][]   p_a2dFirst     ��һ����ά����
			 2   In    int          p_iFirstCount  ��һ�����鳤��
			 3   In    double[][]   p_a2dSecond    �ڶ�����ά����
			 4   In    int          p_iSecondCount �ڶ������鳤��
			 5   In    int          P_iTemp        ƥ�䴰�ڵĴ�С
			 /* P_iTemp�Ĵ�Сһ��ȡΪ���鳤�ȵ�1/10��1/30
	Return   : double ��������֮���ƥ����룬������أ�1.0���������鳤��̫����
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
	double DTWDistanceFun(double p_a2dFirst[][G],int p_iFirstCount,double p_a2dSecond[][G],int p_iSecondCount,int P_iTemp);

	/*******************************************************
	Function : ���������ײɼ�����ʶ��Ϊ��̬����
	Parameter: 
             1   In    double[][]   p_a2dGestureData     �ɼ���������������
			 2   In    int          P_iDataCount         �����������鳤��
	Return   : int ʶ����Ķ�̬�������
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
	int gestureDistinguish(double p_a2dGestureData[][G],int P_iDataCount);

	/*******************************************************
	Function : ѵ������ģ�����ݼ���������ģ��
	Parameter: 
             1   In    double[][]   p_a2dRawTemplateData     �ɼ�������ԭʼ����
			 2   In    int          p_iRawFrameCount         �����������鳤��
	Return   : bool true=ѵ������ģ��ɹ� false=ѵ������ģ��ʧ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
	bool trainTemplateData(double p_a2dRawTemplateData[][G],int p_iRawFrameCount);

	/*******************************************************
	Function : ��������ģ��
	Parameter: 
             1   In    double[][]   p_a2dTemplateData     �Ѿ������õ�ģ��
			 2   In    int          p_iTemplateDataCount  �ѽ���ģ��ĳ���
			 3   In    double[][]   p_a2dRawData          �¼����ѵ������
			 4   In    int          p_iRawDataCount       �¼����ѵ����������
			 5   out   double[][]   temp                  ����ƥ������ѵ�����ģ�壬����temp[DTWMAXNUM]��������������ѵ����ģ��ĳ���
			 6   in    int          turn                  turnΪѵ���Ĵ���,�ڵ�һ�����������齨��ģ��ʱ��rΪ1�����ǳ���Ȩֵ�Ŀ���
			 7   in    double       tt                    ttΪ����֮�����ķ�ֵ�����ж���
			 8   in    double       rltdistance           ������룬��һ���������齨��ģ��ʱ�������⸳��һ��ֵ������ǰһ�η��ص�ֵ����ò���
	Return   : int �����������-1������ѵ������֮����������Ҫ����ѡ��ѵ��������
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
	int DTWTemplate(double p_a2dTemplateData[][G],int p_iTemplateDataCount,double p_a2dRawData[][G],int p_iRawDataCount,double temp[][G],int turn=5,double tt=50,double rltdistance=50);
	//����ģ������
	void saveTemplateData();

private:
	//����
	/*�������*/
	double distance[DTWMAXNUM][DTWMAXNUM];
	/*����·��*/
    double dtwpath[DTWMAXNUM][DTWMAXNUM]; 
	//ѵ���õ�ģ������
	double m_a2dTemplateData[DTWMAXNUM][G];
	//ѵ���õ�ģ��֡����
	int frameCount;

	/*******************************************************
	Function : ��������ŷ�Ͼ���
	Parameter: 
             1   In    double[][]   p_a2First     ��һ������
			 2   In    double[][]   p_a2Second    �ڶ�������
	Return   : double ���������ľ���
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
	double Euclidean (double p_a2First[],double p_a2Second[]);
	//���붯̬����ģ������
	void readTemplateData();

};

