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
#include "StdAfx.h"
#include "ODynamicGestureDistinguish.h"
//#include<fstream>

ODynamicGestureDistinguish::ODynamicGestureDistinguish(void)
{
	//matlab��ʼ��
	MyDTWInitializeWithHandlers(NULL,0);
	MyDTWInitialize();
	//��ȡģ������
	readTemplateData();
}

ODynamicGestureDistinguish::~ODynamicGestureDistinguish(void)
{
}

/*******************************************************
	Function : ����������֮���DTWƥ�����
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
double ODynamicGestureDistinguish::DTWDistanceFun(double p_a2dFirst[][G],int p_iFirstCount,double p_a2dSecond[][G],int p_iSecondCount,int P_iTemp);
{
	int i,j;
	double dist;
	int istart,imax;
	int r2=P_iTemp+ABS(p_iFirstCount-p_iSecondCount);/*ƥ�����*/
	double g1,g2,g3;
	int pathsig=1;/*·���ı�־*/

	/*����������Ч��*/
	if(p_iFirstCount>DTWMAXNUM||p_iSecondCount>DTWMAXNUM){
		//printf("Too big number\n");
		return -1.0;
	}
	
	/*����һЩ��Ҫ�ĳ�ʼ��*/
	for(i=0;i<p_iFirstCount;i++){
		for(j=0;j<p_iSecondCount;j++){
			dtwpath[i][j]=0;
			distance[i][j]=DTWVERYBIG;
		}
	}
	
	/*��̬�滮����С����*/
	/*�����Ҳ��õ�·���� -------
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
	/*Ѱ��·��,�����Ҳ���������������*/
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
	
	/*����ģ��*/
	for(i=0;i<p_iTemplateDataCount;i++)
	{
		double ftemp[DTWMAXNUM];
		//��ʼ��
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
		   temp[i][g]=(p_a2dTemplateData[i][g]*turn+ftemp[g])/((double)(turn+1));/*ע�������Ȩֵ*/
		}
	}
	
	return p_iTemplateDataCount;/*����ģ��ĳ���*/
}

	/*******************************************************
	Function : ��������ŷ�Ͼ���
	Parameter: 
             1   In    double[][]   p_a2First     ��һ������
			 2   In    double[][]   p_a2Second    �ڶ�������
	Return   : double ���������ľ���
	Used Var : ��
	Author   : �ŷ� Eli zhang
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


//��ȡģ������
void ODynamicGestureDistinguish::readTemplateData()
{
	//����ѵ���õ�ģ��
	std::fstream f("d://dynamicGesture.txt",std::ios::in);
	int rowRolum;
	//�����ݵ�����������
	f>>rowRolum;
	frameCount=rowRolum;
	//����ģ������
	for(int i=0;i<rowRolum;i++)
	{
		for(int j=0;j<15;j++)
		{
		   f>>m_a2dTemplateData[i][j];//��ȡ����
		}
	}
	f.close();
	std::cout<<"haved successed loading dynamicGestureTemplateData\n";
}

    /*******************************************************
	Function : ѵ������ģ�����ݼ���������ģ��
	Parameter: 
             1   In    double[][]   p_a2dRawTemplateData     �ɼ�������ԭʼ����
			 2   In    int          p_iRawFrameCount         �����������鳤��
	Return   : bool true=ѵ������ģ��ɹ� false=ѵ������ģ��ʧ��
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
bool ODynamicGestureDistinguish::trainTemplateData(double p_a2dRawTemplateData[][G],int p_iRawFrameCount)
{
	//p_a2dRawTemplateData[][G]����ѵ��������
	//p_iRawFrameCount����ѵ��������֡����
	//ѵ���õ�ģ������(��ʱ����)
	double trainedTemplateData[DTWMAXNUM][G];
	//ѵ���õ�ģ��֡����(��ʱ����)
	int trainedFrameCount;
	//ѵ���õ�ģ�����ݱ�����trainedTemplateData
    trainedFrameCount=DTWTemplate(m_a2dTemplateData,frameCount,p_a2dRawTemplateData,p_iRawFrameCount,trainedTemplateData);
	//��trainedTemplateData�����ݸ��Ƶ�m_a2dTemplateData��ȥ
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
//����ѵ���õ�ģ������
void ODynamicGestureDistinguish::saveTemplateData()
{
	//����ѵ���õ�ģ��
	std::fstream f("d://dynamicGesture.txt",std::ios::out);
	//д��ģ�������ܵ�����
	f<<frameCount<<' ';
	//д��ģ������
	for(int i=0;i<frameCount;i++)
	{
		for(int j=0;j<15;j++)
		{

			f<<templateData[i][j]<<' '; //д������

		}
	}
	f.close();
}

	/*******************************************************
	Function : ���������ײɼ�����ʶ��Ϊ��̬����
	Parameter: 
             1   In    double[][]   p_a2dGestureData     �ɼ���������������
			 2   In    int          P_iDataCount         �����������鳤��
	Return   : int ʶ����Ķ�̬�������
	Used Var : ��
	Author   : �ŷ� Eli zhang
	*******************************************************/
int ODynamicGestureDistinguish::gestureDistinguish(double p_a2dGestureData[][G],int P_iDataCount)
{
	int gestureNum=-1;
	//ʹ�ñ�д��DTW�㷨��������ʶ��
	double dist=DTWDistanceFun(m_a2dTemplateData,frameCount,p_a2dGestureData,P_iDataCount,2);

	//ʹ��matlab��DTW�㷨��������ʶ��
	// 1��1��ʾ����Ĵ�С������maltabֻ��һ�ֱ��������Ǿ���Ϊ�˺�Cpp�����ӹ죬���ó�1*1�ľ���mxDOUBLE_CLASS��ʾ�����ľ��ȣ�
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