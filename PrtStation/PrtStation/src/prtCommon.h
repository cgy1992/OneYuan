/*
 * prtCommon.h
 *
 *  Created on: 2015��7��4��
 *      Author: abcht
 */

#ifndef PRTCOMMON_H_
#define PRTCOMMON_H_

#include <afx.h>
#include "etpCommontype.h"


#define DEV_IP_MAX_LEN  48



#define  OWNER_TEXT_MAX_LEN  256
#define  OWNER_PHOTO_MAX_LEN   0x200000

typedef struct
{
	char ownerName[OWNER_TEXT_MAX_LEN + 1];
	char ownerCardNum[OWNER_TEXT_MAX_LEN + 1];

	char ownerSex[OWNER_TEXT_MAX_LEN + 1];
	char cardLaunchTime[OWNER_TEXT_MAX_LEN + 1];
	char cardLaunchPlace[OWNER_TEXT_MAX_LEN + 1];

	char ownerIdentityNum[OWNER_TEXT_MAX_LEN + 1];
	char ownerPhoto[OWNER_PHOTO_MAX_LEN + 1];
	unsigned int  ownerPhotoLen;
}PRT_INFO_T;



#define  OUTPUT_ICSTR_MAX_LEN  8096
#define  OUTPUT_FORMAT_MAX_LEN  8096

#define  RESULT_OK_STR_LEN  512
#define  RESULT_FAIL_STR_LEN   512
#define  SEND_FAIL_STR_LEN  256


/*
 *
0000000100  ������Դ������
0000000200  PIN�룬 ���þʹ���û���ò���
0000000300  �籣sam����λ�� ����Ǽ��ܻ�ģʽ�� �Ͳ���
0000000400  ��ӡ��sam����λ
0000000500  �籣����������   1��PSAM�����ͣ� 2�����ܻ�
0000000600  ���ܻ�IP�˿ڣ����ܻ�ģʽ�Ŵ�����ʽΪ1.1.1.1@8080
0000000700  ���ܻ��û�������, ���ܻ�ģʽ�Ŵ��� ��ʽΪuser@password
0000000800  ��д�����ͣ�1 ��HD100��2����������д����3�������ܶ�д����4����ACR��д��������
0000000900  �û������ͣ� 1 ���ƣ�2 ����; 3 ����
0000000A00  ���п���
0000000B00  Ϊ1ʱ������������п��ţ����򲻽��з������������޴˽�ʱ���������ơ�
0000000C00  Ϊ1ʱ��ATR�г��д������������Դһ�����������޴˽�ʱ����������
0000000D00  Ϊ1ʱ�����ۿ���������Կ��ǿ��д��Կ���������޴˽�ʱ���Զ���⿨��������Կ��������Կ��д��Կ��ֻд���ݡ�
0000000E00  �������ӿ�����, 1���汾1(��)�� 2���汾2��
*/

#define ATTR_DATA_SOURCE_DESC  		"0000000100"   /*���ڱ����а���*/
#define ATTR_PIN_CODE_DESC  		"0000000200"   /*16λ*/
#define ATTR_SHEBAO_SAM_SLOT_DESC  	"0000000300"   /*1-4*/
#define ATTR_PRINTER_SAM_SLOT_DESC  "0000000400"  /*1-4*/
#define ATTR_SHEBAO_JIAMI_TYPE_DESC  	"0000000500"  /*�籣����������   1��PSAM�����ͣ� 2�����ܻ�*/
#define ATTR_JIAMIJI_IPADDR_DESC  	"0000000600"  /*���ܻ�IP�˿ڣ����ܻ�ģʽ�Ŵ�����ʽΪ1.1.1.1@8080*/
#define ATTR_JIAMIJI_USER_DESC  	"0000000700"  /*���ܻ��û�������, ���ܻ�ģʽ�Ŵ��� ��ʽΪuser@password*/
#define ATTR_CARD_READER_DESC  		"0000000800"  /*��д�����ͣ�1 ��HD100��2����������д����3�������ܶ�д����4����ACR��д��*/
#define ATTR_USER_CARD_TYPE_DESC  			"0000000900"  /*�û������ͣ� 1 ���ƣ�2 ����; 3 ����*/

#define ATTR_BANK_CODE_DESC  			"0000000A00"  /*��ֵ��ʾ����ģʽ�� ����Ϊ��ʽдIC��ģʽ*/

#define ATTR_WR_MATCH_BANK_DESC  			"0000000B00"  /*��ֵ��ʾ����ģʽ�� ����Ϊ��ʽдIC��ģʽ*/
#define ATTR_WR_MATCH_ATR_DESC  			"0000000C00"  /*��ֵ��ʾ����ģʽ�� ����Ϊ��ʽдIC��ģʽ*/
#define ATTR_WR_FORCE_MIYAO_DESC  			"0000000D00"  /*��ֵ��ʾ����ģʽ�� ����Ϊ��ʽдIC��ģʽ*/

#define ATTR_PRINTER_SAM_TYPE_DESC  			"0000000E00"  /*�������ӿ�����, 1���汾1(��)�� 2���汾2��*/


enum
{
	MSG_PRINT_TASK = 1,
};

typedef struct
{
	int type;
	int len;
}MSG_HDR_T;



enum
{
	ERR_DATA_INVALID = 0,
	ERR_TASK_FULL,
	ERR_ADD_TASK_ERR,

	ERR_PRINTER_CARD_READER,
	ERR_PRINTER_NOT_READY,

	ERR_PRINTER_PHOTO_PARSE_FAIL,
	ERR_PRINTER_PHOTO_PRINT_FAIL,
	ERR_PRINTER_LOAD_DLL_FAIL,
	ERR_PRINTER_CANT_GET_INFO,
	ERR_PRINTER_IN_ABNORMAL,
	ERR_PRINTER_LAST_TASK_ABNORMAL,

	ERR_PRINTER_WR_IC_ERROR,
	ERR_PRINTER_RET_DATA_INVALID,

	ERR_TYPE_MAX
};

extern const char* g_errCodeDesc[];


static const char* getErrCodeDesc(int errCode)
{
	return g_errCodeDesc[errCode];
}

#endif /* PRTCOMMON_H_ */
