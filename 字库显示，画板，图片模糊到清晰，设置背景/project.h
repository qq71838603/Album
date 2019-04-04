
#ifndef __PROJECT_H__
#define __PROJECT_H__

//声明性的语句（不需分配数据存储空间），可存放于头文件

#define  FB_SIZE  800*480*4


 typedef struct User_Info{   //用户信息结构
	
	char Tel[20];
	char Name[30]; 
	char Passwd[6];
	unsigned char Gender;
	unsigned char Age;
	unsigned short Id;
	int Money;
	//unsigned char pic[0];
	//unsigned char *pic;	 	 
 }UInfo,*pUInfo;
 
 typedef struct User_Data{   //用户请求服务数据
	 
	 int Value;
	 
 }UData,*pUData;
 
typedef int  (*WinP_fn)(pUInfo,pUData);  //定义窗口服务函数指针
 
typedef struct Windows_Info{

	unsigned int PCount;  //窗口的服务次数
	unsigned int TCount;  //窗口的服务时长
	WinP_fn Process_fn;   //窗口的服务函数
	unsigned short UsrId[100];       //该窗口的服务用户Id
	unsigned char Total;  //总服务人数
	unsigned short CurIndex; //当前服务的用户序号
	 
 }WInfo,*pWInfo;
 
 
//BMP文件信息头  14 BYTE
typedef struct BMPfileHeader { 
unsigned short bfType; 
unsigned int bfSize; 
unsigned short bfReserved1; 
unsigned short bfReserved2; 
unsigned int bfOffBits; 
}__attribute__ ((packed)) BMPFH, *pBMPFH;


//BMP位图信息头  40 BYTE
typedef struct BMPInfoHeader{ 
unsigned int biSize; 
unsigned long  biWidth; 
unsigned long  biHeight; 
unsigned short biPlanes; 
unsigned short biBitCount; 
unsigned int biCompression; 
unsigned int biSizeImage; 
unsigned long  biXPelsPerMeter; 
unsigned long  biYPelsPerMeter; 
unsigned int biClrUsed; 
unsigned int biClrImportant; 
}__attribute__ ((packed)) BMPIH, *pBMPIH;

 
 
typedef struct Pos {
	
	int x;
	int y;
	
}POS,*pPOS; 
 
 
typedef struct Key {

		unsigned char sn;
		char cn;
		int x;
		int y;
		char pic_name[30]; 
}KEY,*pKEY; 
 
 
extern int Win0_Process_fn(pUInfo Usr,pUData Data);
extern int Win1_Process_fn(pUInfo Usr,pUData Data);
extern int Win2_Process_fn(pUInfo Usr,pUData Data);
extern int Win3_Process_fn(pUInfo Usr,pUData Data);
extern int Win4_Process_fn(pUInfo Usr,pUData Data);
extern int Win5_Process_fn(pUInfo Usr,pUData Data);
extern int Win6_Process_fn(pUInfo Usr,pUData Data);
extern int Win7_Process_fn(pUInfo Usr,pUData Data);


#endif  //__PROJECT_H__

