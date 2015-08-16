#include "StdAfx.h"
#include "OStaticGestureDistinguish.h"
#include<fstream>

OStaticGestureDistinguish::OStaticGestureDistinguish(int templateAllowance)
{
	this->templateAllowance=templateAllowance;
	gestureNum=5;
	templateNum=3;
	initialize();
}

//初始化函数
void OStaticGestureDistinguish::initialize()
{
	//载入训练好的模板
	fstream f("d://staticGesture.txt",ios::in);
	for(int i=0;i<gestureNum;i++)
	{
		for(int j=0;j<templateNum;j++)
		{
			for(int k=0;k<15;k++)
			{
				f>>m_a2dTemplateData[i][j][k];//读取数据
			}
		}
	}
	cout<<"haved loaded template\n";

	//连接数据手套
	try
	{
		// Connect to the glove.
		gloveDict = vhtIOConn::getDefault( vhtIOConn::glove );
		// Expand the CyberGlove connection to the CyberTouch capabilities.
		glove = new vhtCyberGlove(gloveDict);

	}
	catch (vhtBaseException *e)
	{
		printf("Error: %s\nPress <enter> to exit.\n", e->getMessage());
		getchar();
	}

	cout<<"train new template 's'for staticGesture 'd'for dynamicGesture or 'n'\n";
	char templateFlag;
	cin>>templateFlag;
	if(templateFlag=='s')
	{
		trainTemplate();
		cout<<"save the trained template 'y'or 'no'\n";
		char saveFlag;
		cin>>saveFlag;
		if(saveFlag=='y')
		{
			//保存训练好的模板
			fstream f("d://staticGesture.txt",ios::out);
			for(int i=0;i<gestureNum;i++)
			{
				for(int j=0;j<templateNum;j++)
				{
					for(int k=0;k<15;k++)
					{
						f<<templateData[i][j][k]<<' '; //写入数据
					}
				}
			}
			f.close();
		}
	}
	if(templateFlag=='d')
	{
		double dynamicGestureTemplate[500][15];
		int dataCount=0;
		cout<<"start train !"<<endl;
	    //system("pause");
		//GetKeyState(VK_F9)//判断是否有按键按下
		double preGloveData[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//存储前一帧数据
        double prePreGloveData[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//存储再前一帧数据
		int start=0;
		while(true)
		{
			double curGloveData[15];//存储当前的采样数据
			int j=0;
			for(int i=0;i<21;i++)//循环读出传感器的原始值
			{
				if(i!=3&&i!=6&&i!=7&&i!=10&&i!=14&&i!=18)//跳过不用读取的传感器
				{
					curGloveData[j]=glove->getRawData(i);
					j++;
				}
			}
		    _sleep(20);//延迟50毫秒
			//求上一帧数据相对斜率
			  bool prePositiveSlope=false;//上一帧数据相对斜率是否为正
			  int preNegativeCount=0,prePositiveCount=0;//待测试八个数据的变化率为正负总数
			  //求出八个数据变化率分别为正负的总数
			  for(int i=0;i<15;i++)
			  {
				  if(i==3||i==4||i==5||i==6||i==8||i==9||i==11||i==12)
				  {
					  if(prePreGloveData[i]!=0)
					  {
						  if((preGloveData[i]-prePreGloveData[i])>0)
						  {
							  ++prePositiveCount;
						  }
						  if((preGloveData[i]-prePreGloveData[i])<0)
						  {
							  ++preNegativeCount;
						  }
					  }
				  }
			  }
			  //如果8个变化率中有8个为正则总的变化率为正
			  if(prePositiveCount>=7)
			  {
				  prePositiveSlope=true;
			  }
			  if(preNegativeCount>=7)
			  {
				  prePositiveSlope=false;
			  }
			  //求当前帧数据相对斜率
			  bool curPositiveSlope=false;
			  int curNegativeCount=0,curPositiveCount=0; 
			  for(int i=0;i<15;i++)
			  {
				  if(i==3||i==4||i==5||i==6||i==8||i==9||i==11||i==12)
				  {
					  if(preGloveData[i]!=0)
					  {
						  if((curGloveData[i]-preGloveData[i])>0)
						  {
							  ++curPositiveCount;
						  }
						  if((curGloveData[i]-preGloveData[i])<0)
						  {
							  ++curNegativeCount;
						  }
					  }
				  }
			  }
			  if(curPositiveCount>=7)
			  {
				  curPositiveSlope=true;
			  }
			  if(curNegativeCount>=7)
			  {
				  curPositiveSlope=false;
			  }
			  //循环更新记录的数据
			  for(int i=0;i<15;++i)
			  {
				  prePreGloveData[i]=preGloveData[i];
				  preGloveData[i]=curGloveData[i];
			  }

			  if((prePositiveSlope^curPositiveSlope))
			  {
				  ++start;
			  }
			  if(start==1)
			  {
				  for(int i=0;i<15;i++)
				  {
					  dynamicGestureTemplate[dataCount][i]=curGloveData[i];
				  }
				  ++dataCount;
			  }
			  if(start==2)
			  {
				  break;
			  }
		}

		//保存训练好的模板
		fstream f("d://dynamicGesture.txt",ios::out);
		f<<dataCount<<' ';
		for(int i=0;i<dataCount;i++)
		{
			for(int k=0;k<15;k++)
			{
				f<<dynamicGestureTemplate[i][k]<<' '; //写入数据
			}
		}
		f.close();
	}
}
//识别手势函数
int OStaticGestureDistinguish::gestureDistinguish() 
{
	float gloveData[15];
	glove->update();
	int j=0;
	int data;
	for(int i=0;i<21;i++)//循环读出传感器的原始值
	{
		if(i!=3&&i!=6&&i!=7&&i!=10&&i!=14&&i!=18)//跳过不用读取的传感器
		{
			gloveData[j]=glove->getRawData(i);
			j++;
		}
	}
	double minDistance=1000;int testGestureNum=-1;//保存最小的欧式距离，及手势序号
	for(int i=0;i<gestureNum;i++)//和每个手势模板匹配
	{
		double distance;
		for(j=0;j<templateNum;j++)
		{
	       distance=Euclidean(gloveData,templateData[i][j]);
		   if(distance<minDistance)
		   {
			   testGestureNum=i;
			   minDistance=distance;
		   }
		}
	}
	if(minDistance<templateAllowance)
	{
		return testGestureNum;
	}
	else
	{
       	return -1;
	}
}
//训练新的模板函数
void OStaticGestureDistinguish::trainTemplate()
{
	while(true)
	{
		cout<<"input the number of staticGesture trained(01234) or '5' for abondon \n";
		int gesNumber;
		cin>>gesNumber;
		if(gesNumber==5)
		{
			break;
		}
		else
		{
			for(int i=0;i<15;i++)
			{
				for(int j=0;j<templateNum;++j)
				{
				   templateData[gesNumber][j][i]=0;
				}
			}
			for(int num=0;num<templateNum;num++)
			{
				cout<<"train number"<<gesNumber<<" gesture "<<num+1<<"th group data"<<"\n";
				glove->update();
				int j=0;
				int data;
				for(int i=0;i<21;i++)//循环读出传感器的原始值
				{
					if(i!=3&&i!=6&&i!=7&&i!=10&&i!=14&&i!=18)//跳过不用读取的传感器
					{
						data=glove->getRawData(i);
						templateData[gesNumber][num][j]=data;
						j++;
					}
				}
				system("pause");
			}
		}
	}
}
//得到数据手套数据
void OStaticGestureDistinguish::getGloveData(double gloveData[15])
{
	glove->update();
	int j=0;
	int data;
	//循环读出传感器的原始值
	for(int i=0;i<21;i++)
	{
		//跳过不用读取的传感器
		if(i!=3&&i!=6&&i!=7&&i!=10&&i!=14&&i!=18)
		{
			gloveData[j]=glove->getRawData(i);
			j++;
		}
	}
}
//求向量的欧氏距离
double OStaticGestureDistinguish::Euclidean (float A[],float B[])
{
	double ret = 0.0;
    for(int i=0;i<15;++i)
	{
		ret+= pow(abs(A[i] - B[i]), 2);
	}
	return pow(ret, 1.0/2.0);
}
