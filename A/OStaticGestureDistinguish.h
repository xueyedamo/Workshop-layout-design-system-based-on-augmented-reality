#pragma once
#include "stdafx.h"
#include <vhandtk/vhtBase.h>
#include <iostream>
#include<fstream>
using namespace std;

class OStaticGestureDistinguish
{
public:
	OStaticGestureDistinguish(int templateAllowance);
	//��������
	//ʶ�����ƺ���
	int gestureDistinguish();
	//ѵ������ģ��
	void trainTemplate();
	//�õ�������������
	void getGloveData(double gloveData[15]);
private:
	//ȫ�ֱ�������
	vhtIOConn *gloveDict;
	vhtCyberGlove *glove;
	//���У���õ�����ģ������
	float templateData[5][3][15];
	int gestureNum;//���Ƶ�����
	int templateNum;//ÿ������ģ����е���������
	int templateAllowance;//ģ����ݲ��
	//��������
	void initialize();
	//��������ŷ�Ͼ���
	double Euclidean (float A[],float B[]);
};

