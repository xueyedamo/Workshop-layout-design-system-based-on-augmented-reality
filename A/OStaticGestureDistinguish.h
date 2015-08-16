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
	//函数声明
	//识别手势函数
	int gestureDistinguish();
	//训练更新模板
	void trainTemplate();
	//得到数据手套数据
	void getGloveData(double gloveData[15]);
private:
	//全局变量声明
	vhtIOConn *gloveDict;
	vhtCyberGlove *glove;
	//存放校正好的手势模板数据
	float templateData[5][3][15];
	int gestureNum;//手势的总数
	int templateNum;//每个手势模板具有的数据总数
	int templateAllowance;//模板的容差极限
	//函数声明
	void initialize();
	//求向量的欧氏距离
	double Euclidean (float A[],float B[]);
};

