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

//��ʼ������
void OStaticGestureDistinguish::initialize()
{
	//����ѵ���õ�ģ��
	fstream f("d://staticGesture.txt",ios::in);
	for(int i=0;i<gestureNum;i++)
	{
		for(int j=0;j<templateNum;j++)
		{
			for(int k=0;k<15;k++)
			{
				f>>m_a2dTemplateData[i][j][k];//��ȡ����
			}
		}
	}
	cout<<"haved loaded template\n";

	//������������
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
			//����ѵ���õ�ģ��
			fstream f("d://staticGesture.txt",ios::out);
			for(int i=0;i<gestureNum;i++)
			{
				for(int j=0;j<templateNum;j++)
				{
					for(int k=0;k<15;k++)
					{
						f<<templateData[i][j][k]<<' '; //д������
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
		//GetKeyState(VK_F9)//�ж��Ƿ��а�������
		double preGloveData[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//�洢ǰһ֡����
        double prePreGloveData[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//�洢��ǰһ֡����
		int start=0;
		while(true)
		{
			double curGloveData[15];//�洢��ǰ�Ĳ�������
			int j=0;
			for(int i=0;i<21;i++)//ѭ��������������ԭʼֵ
			{
				if(i!=3&&i!=6&&i!=7&&i!=10&&i!=14&&i!=18)//�������ö�ȡ�Ĵ�����
				{
					curGloveData[j]=glove->getRawData(i);
					j++;
				}
			}
		    _sleep(20);//�ӳ�50����
			//����һ֡�������б��
			  bool prePositiveSlope=false;//��һ֡�������б���Ƿ�Ϊ��
			  int preNegativeCount=0,prePositiveCount=0;//�����԰˸����ݵı仯��Ϊ��������
			  //����˸����ݱ仯�ʷֱ�Ϊ����������
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
			  //���8���仯������8��Ϊ�����ܵı仯��Ϊ��
			  if(prePositiveCount>=7)
			  {
				  prePositiveSlope=true;
			  }
			  if(preNegativeCount>=7)
			  {
				  prePositiveSlope=false;
			  }
			  //��ǰ֡�������б��
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
			  //ѭ�����¼�¼������
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

		//����ѵ���õ�ģ��
		fstream f("d://dynamicGesture.txt",ios::out);
		f<<dataCount<<' ';
		for(int i=0;i<dataCount;i++)
		{
			for(int k=0;k<15;k++)
			{
				f<<dynamicGestureTemplate[i][k]<<' '; //д������
			}
		}
		f.close();
	}
}
//ʶ�����ƺ���
int OStaticGestureDistinguish::gestureDistinguish() 
{
	float gloveData[15];
	glove->update();
	int j=0;
	int data;
	for(int i=0;i<21;i++)//ѭ��������������ԭʼֵ
	{
		if(i!=3&&i!=6&&i!=7&&i!=10&&i!=14&&i!=18)//�������ö�ȡ�Ĵ�����
		{
			gloveData[j]=glove->getRawData(i);
			j++;
		}
	}
	double minDistance=1000;int testGestureNum=-1;//������С��ŷʽ���룬���������
	for(int i=0;i<gestureNum;i++)//��ÿ������ģ��ƥ��
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
//ѵ���µ�ģ�庯��
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
				for(int i=0;i<21;i++)//ѭ��������������ԭʼֵ
				{
					if(i!=3&&i!=6&&i!=7&&i!=10&&i!=14&&i!=18)//�������ö�ȡ�Ĵ�����
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
//�õ�������������
void OStaticGestureDistinguish::getGloveData(double gloveData[15])
{
	glove->update();
	int j=0;
	int data;
	//ѭ��������������ԭʼֵ
	for(int i=0;i<21;i++)
	{
		//�������ö�ȡ�Ĵ�����
		if(i!=3&&i!=6&&i!=7&&i!=10&&i!=14&&i!=18)
		{
			gloveData[j]=glove->getRawData(i);
			j++;
		}
	}
}
//��������ŷ�Ͼ���
double OStaticGestureDistinguish::Euclidean (float A[],float B[])
{
	double ret = 0.0;
    for(int i=0;i<15;++i)
	{
		ret+= pow(abs(A[i] - B[i]), 2);
	}
	return pow(ret, 1.0/2.0);
}
