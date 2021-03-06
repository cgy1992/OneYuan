
#ifndef _XPS_PRINT_H
#define _XPS_PRINT_H

typedef  int (CALLBACK *  Gy_pCleanPrinter)(char*printName);
typedef  int (CALLBACK *  Gy_pfGetPrinterCardCount)(char *printName,int *cardcount);//获取计数
typedef  int (CALLBACK *  Gy_pfGetPrinterRibbonPercentRemaining)(char *printName,int *percent);

typedef  int (CALLBACK *  Gy_pfInitialDriver)(CString printerName);
typedef  int (CALLBACK *  Gy_pfStartJob)(char* printerName, int ReaderType);
typedef  int (CALLBACK *  Gy_pfEncodeMag)(char*  track1,char*  track2,char*  track3);
typedef  int (CALLBACK *  Gy_pfReadMag)(char* track1,char* track2,char* track3);
typedef  int (CALLBACK *  Gy_pfEndJob)(int timeout);

typedef  int (CALLBACK *  Gy_pfParkSmartCard)(int iStation);
typedef  int (CALLBACK *  Gy_pfUnParkSmartCard)();
typedef  int (CALLBACK *  Gy_pPrePrint)();
typedef  int (CALLBACK *  Gy_pPostPrint)(BOOL bHaveNextPage);

typedef  int (CALLBACK * Gy_pPrintText)(
							float X,							//打印位置的横坐标
							float Y,							//打印位置的纵坐标
							char* Font,					//使用的字体名称
							float Size,						//字号大小
							unsigned char Attribute,		//文字属性
							int Color,					//文字颜色（使用标准的Web颜色代码）
							char* Text 	,				//打印的文本
							BOOL  TransBkColor,
							int  Space
							);
typedef  int (CALLBACK * Gy_pPrintImage)(
											float X,		//打印位置的横坐标
											float Y,		//打印位置的纵坐标
											float Weight,	//打印图片的宽度
											float Height,	//打印图片的高度
											char* Path,	//图片的全路径
											byte* picData,
											int Len);

typedef  int (CALLBACK *  Gy_pPrintPhoto)(
											float X,		//打印位置的横坐标
											float Y,		//打印位置的纵坐标
											float Height,	//打印图片的高度
											float Weight,	//打印图片的宽度											
											char* Path,	//图片的全路径
											DWORD m_TransColour,
											char optype
											);

typedef  int (CALLBACK * Gy_pGetLastErrOperateMsg) (char *msg1,char *msg2,int &len1,int &len2);


typedef  int (CALLBACK * Gy_pGetPrinterSerialNum)(char*printName,char*serialnum,unsigned short &rlen);


typedef  int (CALLBACK * Gy_pSetPrinterOrientation) (short Orientation);
/////////////////////////////////////////////////////////////////////////////

typedef unsigned short  (CALLBACK *   Gy_pfScGetDLLVer)(char* ProductionModle,char*DllVer,short* DllType) ;
typedef unsigned short  (CALLBACK *   Gy_pfScOpenReader)(char* Sn,HANDLE*devNo) ;
typedef unsigned short  (CALLBACK *   Gy_pfScCloseReader)(HANDLE devNo) ;
typedef unsigned short  (CALLBACK *   Gy_pfScGetReaderSN)(HANDLE devNo,char*ReaderSN)  ;
typedef unsigned short  (CALLBACK *   Gy_pfScBeep)(HANDLE devNo,unsigned short ms) ;
typedef char*			(CALLBACK *   Gy_pfScErrorCode)(int ErrorCode) ;
typedef unsigned short  (CALLBACK *   Gy_pfScResetCard)(HANDLE devNo,byte ivCardSeat,byte* ivCardType,char*srABCardPhySn,char*srAtr,char*AtrLen) ;
typedef unsigned short  (CALLBACK *   Gy_pfScSendApduHex)(HANDLE devNo,byte ivCardSeat,char* apduData,int apduLen,char* respData,short * respLen) ;
typedef unsigned short  (CALLBACK *   Gy_pfScSendApdu)(HANDLE devNo,byte ivCardSeat,char* apduData,char* respData) ;
typedef unsigned short  (CALLBACK *   Gy_pfScSendCommand)(HANDLE devNo,byte ivCardSeat,char* commandData,char* respData)  ;
typedef unsigned short  (CALLBACK *   Gy_pfScSendCommandHex)(HANDLE devNo,byte ivCardSeat,char* apduData,int apduLen,char* respData,int* respLen)  ;
typedef unsigned short  (CALLBACK *   Gy_pfScGetAllReaderSn)(char* Sn)  ;


typedef  HANDLE  (CALLBACK *  pf_ic_dc_init)(char* pszName);
typedef unsigned short  (CALLBACK *   pf_ic_dc_exit)(HANDLE icdev);
typedef unsigned short  (CALLBACK *   pf_ic_dc_setcpu)(HANDLE icdev, unsigned char _Byte);
typedef unsigned short  (CALLBACK *   pf_ic_dc_cpureset)(HANDLE icdev, unsigned char *rlen, unsigned char *databuffer);
typedef unsigned short  (CALLBACK *   pf_ic_dc_cpuapdu)(HANDLE icdev, short slen, unsigned char *sendbuffer, short *rlen, unsigned char *databuffer);
typedef unsigned short  (CALLBACK *   pf_ic_dc_cpuapdu_hex)(HANDLE icdev, unsigned char slen, unsigned char *sendbuffer, short *rlen, unsigned char *databuffer);


typedef BOOL (CALLBACK* Gy_pWCard_ICC_DATA)(char* InfoData,char* FlagData,char* RetInfo,char* ErrData);
//extern "C" _declspec(dllimport)	BOOL Get_Jami_YinZi(int iReadType,int iPrinterPsamPos,char*Jamiyinzi,char*ErrData);
typedef BOOL (WINAPIV* Gy_pGet_Jami_YinZi)(int iReadType,int iPrinterPsamPos,char*Jamiyinzi,char*ErrData);

#define  _XPS_PRINT_TEXT(text, attr)\
		 Gy_PrintText((attr)->X, (attr)->Y, (attr)->Font, (attr)->Size, \
				 (attr)->Attribute, (attr)->Color, text, (attr)->TransBkColor, (attr)->Space);

#define  _XPS_PRINT_IMAGE(imageFile, attr)\
		 Gy_PrintImage((attr)->X, (attr)->Y, (attr)->Weight, (attr)->Height, (imageFile));


enum
{
	XPS_LOAD_INIT = 0,
	XPS_LOAD_SUCC,
	XPS_LOAD_FAIL,
};

extern int XpsErrConvert(int ret);

extern int XpsGetJamiYinzi(int iReadType,int iPrinterPsamPos,char*Jamiyinzi,char*ErrData);


extern int XpsWriteIc(PRT_TASK_T *taskInfo);
extern int XpsPrint(char *prtName, int readType, PRT_TASK_T *taskInfo, PRT_TEMPLATE_T *prtFormat);

extern int XpsDllLoad();
extern void XpsDllUnLoad();

extern int XpsWaitLoaded();

extern int XpsCleanPrinter(char *prtName);
extern int XpsGetPrtedCardCnt(char *prtName);
extern int XpsGetSdPercent(char *prtName);
extern int XpsCardGoOut(char *prtName, int readType, char *errData);
extern void XpsCardGoBack();
#endif
