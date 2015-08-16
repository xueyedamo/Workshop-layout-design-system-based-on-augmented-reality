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
#include "StdAfx.h"
#include "ODynamicGestureDistinguish.h"
//#include<fstream>

ODynamicGestureDistinguish::ODynamicGestureDistinguish(void)
{
	//matlab初始化
	MyDTWInitializeWithHandlers(NULL,0);
	MyDTWInitialize();
	//读取模板数据
	readTemplateData();
}

ODynamicGestureDistinguish::~ODynamicGestureDistinguish(void)
{
}

/*******************************************************
	Function : 求两个数组之间的DTW匹配距离
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
double ODynamicGestureDistinguish::DTWDistanceFun(double p_a2dFirst[][G],int p_iFirstCount,double p_a2dSecond[][G],int p_iSecondCount,int P_iTemp);
{
	int i,j;
	double dist;
	int istart,imax;
	int r2=P_iTemp+ABS(p_iFirstCount-p_iSecondCount);/*匹配距离*/
	double g1,g2,g3;
	int pathsig=1;/*路径的标志*/

	/*检查参数的有效性*/
	if(p_iFirstCount>DTWMAXNUM||p_iSecondCount>DTWMAXNUM){
		//printf("Too big number\n");
		return -1.0;
	}
	
	/*进行一些必要的初始化*/
	for(i=0;i<p_iFirstCount;i++){
		for(j=0;j<p_iSecondCount;j++){
			dtwpath[i][j]=0;
			distance[i][j]=DTWVERYBIG;
		}
	}
	
	/*动态规划求最小距离*/
	/*这里我采用的路径是 -------
	                          . |
	                        .   |
	                      .     |
	                    .       |
	 */
	distance[0][0]=(double)2*Euclidean(p_a2dFirst[0],p_a2dSecond[0]);
	for(i=1;i<=r2;i++){
		distance[i][0]=distance[i-1][0]+Euclidean(p_a2dFirst[i],p_a2dSecond[0]);
	}
	for(j=1;j<=r2;j++){
		distance[0][j]=distance[0][j-1]+Euclidean(p_a2dFirst[0],p_a2dSecond[j]);
	}
	
	for(j=1;j<p_iSecondCount;j++){
		istart=j-r2;
		if(j<=r2)
			istart=1;
		imax=j+r2;
		if(imax>=p_iFirstCount)
			imax=p_iFirstCount-1;
		
		for(i=istart;i<=imax;i++){
			g1=distance[i-1][j]+Euclidean(p_a2dFirst[i],p_a2dSecond[j]);
			g2=distance[i-1][j-1]+2*Euclidean(p_a2dFirst[i],p_a2dSecond[j]);
			g3=distance[i][j-1]+Euclidean(p_a2dFirst[i],p_a2dSecond[j]);
			g2=MIN(g1,g2);
			g3=MIN(g2,g3);
			distance[i][j]=g3;
		}
	}
		
	dist=distance[p_iFirstCount-1][p_iSecondCount-1];//((double)(p_iFirstCount+p_iSecondCount));
	return dist;
}

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
int ODynamicGestureDistinguish::DTWTemplate(double p_a2dTemplateData[][G],int p_iTemplateDataCount,double p_a2dRawData[][G],int p_iRawDataCount,double temp[][G],int turn,double tt,double rltdistance)
{
	double dist;
	int i,j;
	int pathsig=1;
	dist=DTWDistanceFun(p_a2dTemplateData,p_iTemplateDataCount,p_a2dRawData,p_iRawDataCount,(int)(p_iTemplateDataCount/30));
	if(dist>tt){
		printf("\nSample doesn't match!\n");
		return -1;
	}
		
	if(turn==1)
		rltdistance=dist;
	else{
		rltdistance=((rltdistance)*(turn-1)+dist)/turn;
	}
	/*寻找路径,这里我采用了逆向搜索法*/
	i=p_iTemplateDataCount-1;
	j=p_iRawDataCount-1;
	while(j>=1||i>=1){
		double m;
		if(i>0&&j>0){
			m=MIN(MIN(distance[i-1][j],distance[i-1][j-1]),distance[i][j-1]);
			if(m==distance[i-1][j]){
				dtwpath[i-1][j]=pathsig;
				i--;
			}
			else if(m==distance[i-1][j-1]){
				dtwpath[i-1][j-1]=pathsig;
				i--;
				j--;
			}
			else{
				dtwpath[i][j-1]=pathsig;
				j--;
			}
		}
		else if(i==0){
			dtwpath[0][j-1]=pathsig;
			j--;
		}
		else{/*j==0*/
			dtwpath[i-1][0]=pathsig;
			i--;
		}
	}
	dtwpath[0][0]=pathsig;
	dtwpath[p_iTemplateDataCount-1][p_iRawDataCount-1]=pathsig;
	
	/*建立模板*/
	for(i=0;i<p_iTemplateDataCount;i++)
	{
		double ftemp[DTWMAXNUM];
		//初始化
		for(int g=0;g<G;++g)
		{
			ftemp[g]=0.0;
		}

		int ntemp=0;
		for(j=0;j<p_iRawDataCount;j++)
		{
			if(dtwpath[i][j]==pathsig)
			{
				for(int g=0;g<G;++g)
		         {
				    ftemp[g]+=p_a2dRawData[j][g];
				 }
				ntemp++;
			}
		}
		for(int g=0;g<G;++g)
	    {
		   ftemp[g]/=((double)ntemp);
		   temp[i][g]=(p_a2dTemplateData[i][g]*turn+ftemp[g])/((double)(turn+1));/*注意这里的权值*/
		}
	}
	
	return p_iTemplateDataCount;/*返回模板的长度*/
}

	/*******************************************************
	Function : 求向量的欧氏距离
	Parameter: 
             1   In    double[][]   p_a2First     第一个向量
			 2   In    double[][]   p_a2Second    第二个向量
	Return   : double 两个向量的距离
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
double ODynamicGestureDistinguish::Euclidean (double p_a2First[],double p_a2Second[])
{
	double ret = 0.0;
    for(int i=0;i<G;++i)
	{
		ret+= pow(abs(p_a2First[i] - p_a2Second[i]), 2);
	}
	return pow(ret, 1.0/2.0);
}


//读取模板数据
void ODynamicGestureDistinguish::readTemplateData()
{
	//载入训练好的模板
	std::fstream f("d://dynamicGesture.txt",std::ios::in);
	int rowRolum;
	//把数据的行数读出来
	f>>rowRolum;
	frameCount=rowRolum;
	//读入模板数据
	for(int i=0;i<rowRolum;i++)
	{
		for(int j=0;j<15;j++)
		{
		   f>>m_a2dTemplateData[i][j];//读取数据
		}
	}
	f.close();
	std::cout<<"haved successed loading dynamicGestureTemplateData\n";
}

    /*******************************************************
	Function : 训练手势模板数据及保存手势模板
	Parameter: 
             1   In    double[][]   p_a2dRawTemplateData     采集的手势原始数据
			 2   In    int          p_iRawFrameCount         手势数据数组长度
	Return   : bool true=训练手势模板成功 false=训练手势模板失败
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
bool ODynamicGestureDistinguish::trainTemplateData(double p_a2dRawTemplateData[][G],int p_iRawFrameCount)
{
	//p_a2dRawTemplateData[][G]用于训练的数据
	//p_iRawFrameCount用于训练的数据帧总数
	//训练好的模板数据(临时数据)
	double trainedTemplateData[DTWMAXNUM][G];
	//训练好的模板帧总数(临时数据)
	int trainedFrameCount;
	//训练好的模板数据保存在trainedTemplateData
    trainedFrameCount=DTWTemplate(m_a2dTemplateData,frameCount,p_a2dRawTemplateData,p_iRawFrameCount,trainedTemplateData);
	//把trainedTemplateData的数据复制到m_a2dTemplateData中去
	frameCount=trainedFrameCount;
	for(int i=0;i<trainedFrameCount;++i)
	{
		for(int j=0;j<15;++j)
		{
			m_a2dTemplateData[i][j]=trainedTemplateData[i][j];
		}
	}
	return true;
}
//保存训练好的模板数据
void ODynamicGestureDistinguish::saveTemplateData()
{
	//保存训练好的模板
	std::fstream f("d://dynamicGesture.txt",std::ios::out);
	//写入模板数据总的行数
	f<<frameCount<<' ';
	//写入模板数据
	for(int i=0;i<frameCount;i++)
	{
		for(int j=0;j<15;j++)
		{

			f<<templateData[i][j]<<' '; //写入数据

		}
	}
	f.close();
}

	/*******************************************************
	Function : 将数据手套采集数据识别为动态手势
	Parameter: 
             1   In    double[][]   p_a2dGestureData     采集的手势数据数组
			 2   In    int          P_iDataCount         手势数据数组长度
	Return   : int 识别出的动态手势序号
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
int ODynamicGestureDistinguish::gestureDistinguish(double p_a2dGestureData[][G],int P_iDataCount)
{
	int gestureNum=-1;
	//使用编写的DTW算法进行手势识别
	double dist=DTWDistanceFun(m_a2dTemplateData,frameCount,p_a2dGestureData,P_iDataCount,2);

	//使用matlab的DTW算法进行手势识别
	// 1，1表示矩阵的大小（所有maltab只有一种变量，就是矩阵，为了和Cpp变量接轨，设置成1*1的矩阵，mxDOUBLE_CLASS表示变量的精度）
	mwArray mwA(P_iDataCount, G, mxDOUBLE_CLASS); 
	mwArray mwB(frameCount, G, mxDOUBLE_CLASS);
	mwArray mwC(1, 1, mxDOUBLE_CLASS);
	double *ptest=new double[G*P_iDataCount];
	double *ptemplateData=new double[G*frameCount];
	int k=0;
	for(int i=0;i<G;++i)
	{
		for(int j=0;j<frameCount;++j)
		{
			ptemplateData[k]=m_a2dTemplateData[j][i];
			++k;
		}
	}
	k=0;
	for(int i=0;i<G;++i)
	{
		for(int j=0;j<P_iDataCount;++j)
		{
			ptest[k]=p_a2dGestureData[j][i];
			++k;
		}
	}
	mwA.SetData(ptest, G*P_iDataCount);
	mwB.SetData(ptemplateData, G*frameCount);
	
	dtw2(1, mwC, mwB, mwA);
	//double dist = mwC.Get(1, 1);
	if(dist<1000)
	{
		gestureNum=1;
	}
	return gestureNum;
}