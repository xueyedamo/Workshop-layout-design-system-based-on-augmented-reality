/*******************************************************
  Function : // 动态手势识别的类（应用动态时间规整算法DTW进行动态手势识别）
  CopyRight: // 版权公开(Public)
  Author   : // 张帆 Eli zhang
  ================= Modified list ====================  
  Version    : // v1.2.0
  Modify Date: // 2015.02.20
  Mender     : // zhangxuanyu0904@163.com
  Why        : // 动态手势识别
  How        : // 添加DTW算法
  Attention  : // 注意事项
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
#define G 15//特征矢量的维
#define THRESHOLD 50.0//用于训练的阈值

class ODynamicGestureDistinguish
{
public:
	ODynamicGestureDistinguish(void);
	~ODynamicGestureDistinguish(void);

	/*******************************************************
	Function : 求两个数组之间的匹配距离
	Parameter: 
             1   In    double[][]   p_a2dFirst     第一个二维数组
			 2   In    int          p_iFirstCount  第一个数组长度
			 3   In    double[][]   p_a2dSecond    第二个二维数组
			 4   In    int          p_iSecondCount 第二个数组长度
			 5   In    int          P_iTemp        匹配窗口的大小
			 /* P_iTemp的大小一般取为数组长度的1/10到1/30
	Return   : double 两个数组之间的匹配距离，如果返回－1.0，表明数组长度太大了
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
	double DTWDistanceFun(double p_a2dFirst[][G],int p_iFirstCount,double p_a2dSecond[][G],int p_iSecondCount,int P_iTemp);

	/*******************************************************
	Function : 将数据手套采集数据识别为动态手势
	Parameter: 
             1   In    double[][]   p_a2dGestureData     采集的手势数据数组
			 2   In    int          P_iDataCount         手势数据数组长度
	Return   : int 识别出的动态手势序号
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
	int gestureDistinguish(double p_a2dGestureData[][G],int P_iDataCount);

	/*******************************************************
	Function : 训练手势模板数据及保存手势模板
	Parameter: 
             1   In    double[][]   p_a2dRawTemplateData     采集的手势原始数据
			 2   In    int          p_iRawFrameCount         手势数据数组长度
	Return   : bool true=训练手势模板成功 false=训练手势模板失败
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
	bool trainTemplateData(double p_a2dRawTemplateData[][G],int p_iRawFrameCount);

	/*******************************************************
	Function : 建立手势模板
	Parameter: 
             1   In    double[][]   p_a2dTemplateData     已经建立好的模板
			 2   In    int          p_iTemplateDataCount  已建立模板的长度
			 3   In    double[][]   p_a2dRawData          新加入的训练样本
			 4   In    int          p_iRawDataCount       新加入的训练样本长度
			 5   out   double[][]   temp                  保存匹配最新训练后的模板，建议temp[DTWMAXNUM]，函数返回最新训练后模板的长度
			 6   in    int          turn                  turn为训练的次数,在第一次用两个数组建立模板时，r为1，这是出于权值的考虑
			 7   in    double       tt                    tt为样本之间距离的阀值，自行定义
			 8   in    double       rltdistance           保存距离，第一次两个数组建立模板时可以随意赋予一个值后面用前一次返回的值赋予该参数
	Return   : int 如果函数返回-1，表明训练样本之间距离过大，需要重新选择训练样本，
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
	int DTWTemplate(double p_a2dTemplateData[][G],int p_iTemplateDataCount,double p_a2dRawData[][G],int p_iRawDataCount,double temp[][G],int turn=5,double tt=50,double rltdistance=50);
	//保存模板数据
	void saveTemplateData();

private:
	//属性
	/*保存距离*/
	double distance[DTWMAXNUM][DTWMAXNUM];
	/*保存路径*/
    double dtwpath[DTWMAXNUM][DTWMAXNUM]; 
	//训练好的模板数据
	double m_a2dTemplateData[DTWMAXNUM][G];
	//训练好的模板帧总数
	int frameCount;

	/*******************************************************
	Function : 求向量的欧氏距离
	Parameter: 
             1   In    double[][]   p_a2First     第一个向量
			 2   In    double[][]   p_a2Second    第二个向量
	Return   : double 两个向量的距离
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
	double Euclidean (double p_a2First[],double p_a2Second[]);
	//读入动态手势模板数据
	void readTemplateData();

};

