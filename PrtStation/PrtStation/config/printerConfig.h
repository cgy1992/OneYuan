#ifndef _PRINTER_CFG_H
#define _PRINTER_CFG_H


#define DEFI_STATION_NAME "默认"


#define PRINTER_MAX_LEN  32
#define PRINTER_JIMIKA_INFO_LEN 10

#define PIN_CODE_MAX_LEN 16

#define JIMIJI_USER_MAX_LEN 32
#define JIMIJI_PASSWD_MAX_LEN 64
#define DEF_JIAMIJI_PORT  6666

enum
{
	SHEBAO_JIAMI_TYPE_PSAM = 1,
	SHEBAO_JIAMI_TYPE_JIAMIJI = 2,
};

enum
{
	PRT_JIAMI_TYPE_VER1 = 1, /*德阳,蒋工*/
	PRT_JIAMI_TYPE_VER2 = 2,
};

enum
{
	USERKA_TYPE_DATANG = 1, /*大唐*/
	USERKA_TYPE_HUADA = 2, /*华大*/
	USERKA_TYPE_HUAHONG = 3, /*华宏*/
};

enum
{
	DO_PRT_AND_WR_IC = 1, /*打卡时也写卡*/
	DO_PRT_STEP1 = 2, /*打卡为第一步*/
	DO_WR_IC_STEP1 = 3, /*写卡位第一步*/
};

typedef struct
{
	char printer[PRINTER_MAX_LEN + 1];

	int jimijiAddr;
	char jimijiAddrDesc[DEV_IP_MAX_LEN + 1];
	int jimijiPort;
	char jimijiUserName[JIMIJI_USER_MAX_LEN + 1];
	char jimijiPasswd[JIMIJI_PASSWD_MAX_LEN + 1];

	int shebaoSamSlot;
	int shebaoSamType; /*社保卡加密方式, 加密卡加密 or 加密机加密*/

	int jiamiSamSlot;
	int jiamiSamType; /*加密卡类型PRT_JIAMI_TYPE_VER1 or PRT_JIAMI_TYPE_VER2*/

	int userkaType; /*用户卡类型*/	
	int cardReaderType; /*读写器类型，1 是HD100，2，是明华读写器，3，是龙杰读写器，4，是ACR读写器*/

	bool isUsePin;
	char pinCode[PIN_CODE_MAX_LEN + 1];

	bool isNotWrIc; /*是否不写卡*/

	bool isWrMatchBank;
	bool isWrMatchAtr;
	bool isWrForceMiyao;

	int printDoType; /*打印方式: 先写卡再打卡 等等*/

	int tishiThrd;
	int stopThrd;
}PRT_PRINTER_CFG_T;


int updateJiamiyinzi();
void saveJiamiyinzi(char *jiamiyinzi);

char* jiamiyinziGet();

PRT_PRINTER_CFG_T* printerCfgGet();
int printerCfgSave(PRT_PRINTER_CFG_T* printParam);
int printerCfgInit(char *stationName);

int printerCfgRead(char *stationName, PRT_PRINTER_CFG_T *printerCfg);

#endif
