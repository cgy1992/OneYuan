#include "StdAfx.h"
#include <string.h>

#include "PrtStation.h"
#include "prtCommon.h"

#include "prtGlobal.h"
#include "serverComm.h"
#include "photoConvert.h"
#include "PrtTaskHdl.h"
#include "printComm.h"
#include "PrtService.h"
#include "etpLib.h"
#include "UserInfoDlg.h"

PrtService::PrtService(void)
{
	m_isRegistering = FALSE;

	m_etpEvtThrdPool = NULL;

	m_ReqCnt = 0;
	m_SevrNoTaskCnt = 0;

	m_stopGetTask = FALSE;
	m_printerStopped = FALSE;

	m_printerSdPercent = 0;
	m_printerPrtedCnt = 0;
}


PrtService::~PrtService(void)
{
}

PrtService* PrtService::instance()
{
	static PrtService *prtService = NULL;

	if(prtService == NULL)
	{
		prtService = new PrtService();
	}
	return prtService;
}

BOOL PrtService::isRegistering()
{
	return m_isRegistering;
}

BOOL PrtService::isTaskHdling()
{
	int size = g_taskHdl->getTaskingCnt();
	return (size>0 ? TRUE : FALSE);
}

void PrtService::_registerTaskEntity(void* param1, void* param2, void *param3)
{
	wsSendRegInfo();
	g_prtService->m_isRegistering = FALSE;
	return;
}

int PrtService::registerTaskHdl()
{
	m_isRegistering = TRUE;

	PRT_LOG_INFO("begin to register.");
	
	if(TRUE == etp_add_evt_job(m_etpEvtThrdPool, PrtService::_registerTaskEntity,
					NULL, NULL, NULL))
	{
		return OK;
	}

	m_isRegistering = FALSE;
	return ET_ERROR;
}


void PrtService::_printTaskEntity(void* param1, void* param2, void *param3)
{
	PRT_TASK_T *taskInfo = (PRT_TASK_T*)param1;

	PRT_TEMPLATE_T *prtFormat = NULL;
	prtFormat = prtTemplateGet();
	assert(NULL != prtFormat);

	PRT_PRINTER_CFG_T *prtParam = printerCfgGet();

	int ret = OK;
	if (taskInfo->operFlag & FLAG_PRINT)
	{
		/*��Ҫ��ӡ,������ж��Ƿ���Ҫд��*/
		ret = XpsPrint(prtParam->printer, prtParam->cardReaderType, taskInfo, prtFormat);
		if (OK != ret)
		{
			taskInfo->endByErrors = TRUE;
			PRT_LOG_ERROR("xps print oper failed, info��%s, return: %s", taskInfo->resultFailStr, taskInfo->resultOkStr);
			//MessageBox(NULL, _T("��ӡ��ʧ�ܣ�"), _T("����"), MB_ICONINFORMATION | MB_TASKMODAL | MB_OK);
			::PostMessage(g_parentWnd, WM_POPUP_WARNING_DLG, WARN_PRINT_FAILED, (LPARAM)0);
		}
		else
		{
			PRT_LOG_INFO("xps print oper succ, return: %s", taskInfo->resultOkStr);
		}
	}
	else
	{
		/*����д��*/
		ret = XpsWriteIc(taskInfo);
		if (OK != ret)
		{
			taskInfo->endByErrors = TRUE;
			PRT_LOG_ERROR("xps write oper failed, info��%s, return: %s", taskInfo->resultFailStr, taskInfo->resultOkStr);
			PRT_LOG_ERROR("input icformat %s.", taskInfo->outIcFmtStr);
			PRT_LOG_ERROR("input icdata %s.", taskInfo->outIcStr);
			//MessageBox(NULL, _T("д��ʧ�ܣ�"), _T("����"), MB_OK);
			/*֪ͨ�����򿨶Ի���*/
			::PostMessage(g_parentWnd, WM_POPUP_WARNING_DLG, WARN_WRITE_FAILED, (LPARAM)0);
		}
		else
		{
			PRT_LOG_INFO("xps write oper succ, return: %s", taskInfo->resultOkStr);
		}
	}

	taskInfo->isOpering = FALSE;
	
	/*��������жϴ���*/
	if (g_taskHdl->isTaskHdlEnd(taskInfo))
	{
		/*����������ͽ��*/
		g_taskHdl->sendTaskResult(taskInfo);		
		g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_END);
	}
	else
	{
		/*֪ͨ�����򿨶Ի���*/
		::SendMessage(g_parentWnd, WM_POPUP_USER_DLG, NULL, (LPARAM)taskInfo);
	}
}

int PrtService::printTaskHdl(PRT_TASK_T *taskInfo)
{	
	if (taskInfo->isOpering)
	{
		MessageBox(NULL, _T("��������ִ��,��ȴ���"), _T("��ʾ"), MB_OK);
		return ET_ERROR;
	}

	PRT_PRINTER_CFG_T *prtParam = printerCfgGet();
	m_printerPrtedCnt = XpsGetPrtedCardCnt(prtParam->printer);
	m_printerSdPercent = XpsGetSdPercent(prtParam->printer);
	if (m_printerPrtedCnt >= prtParam->tishiThrd)
	{
		PRT_LOG_WARN("Already print %d card, need clear printer", m_printerPrtedCnt);
		::PostMessage(g_parentWnd, WM_POPUP_WARNING_DLG, WARN_NEED_CLEAR_PRINTER, (LPARAM)0);
	}
	else if (m_printerPrtedCnt >= prtParam->stopThrd)
	{
		::PostMessage(g_parentWnd, WM_POPUP_WARNING_DLG, WARN_NEED_CLEAR_PRINTER_END, (LPARAM)0);

		taskInfo->endByErrors = TRUE;
		SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN - 1,
								"%s|printer need clean.",
								getErrCodeDesc(ERR_TASK_FULL));
		PRT_LOG_WARN("Already print %d card, can't printer anymore", m_printerPrtedCnt);
		/*����������ͽ��*/
		g_taskHdl->sendTaskResult(taskInfo);	
		g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_END);
		return ET_ERROR;
	}

	taskInfo->isOpering = TRUE;
	if(TRUE == etp_add_evt_job(m_etpEvtThrdPool, PrtService::_printTaskEntity,
						(void*)taskInfo, NULL, NULL))
	{
		return OK;
	}
	taskInfo->isOpering = FALSE;

	taskInfo->endByErrors = TRUE;
	SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN - 1,
							"%s|add task failed.",
							getErrCodeDesc(ERR_ADD_TASK_ERR));
	PRT_LOG_ERROR("%s", taskInfo->resultFailStr);

	/*����������ͽ��*/
	g_taskHdl->sendTaskResult(taskInfo);	
	g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_END);
	return ET_ERROR;
}

void PrtService::_taskHandle(int sessionId, char *taskData, int taskDataLen, char *taskSerialNo)
{
	PRT_TASK_T *taskInfo = (PRT_TASK_T*)malloc(sizeof(PRT_TASK_T));
	if (NULL == taskInfo)
	{
		PRT_LOG_ERROR("�����ڴ�ʧ��");
		return;
	}
	memset(taskInfo, 0, sizeof(PRT_TASK_T));

	taskInfo->taskSessionId = sessionId;

	time_t nowTime = time(NULL); //��ȡĿǰ��ʱ��
	tm* local; //����ʱ��
	local = localtime(&nowTime); //תΪ����ʱ��
	strftime(taskInfo->timeBuf, sizeof(taskInfo->timeBuf) - 1, "%Y-%m-%d %H:%M:%S", local);

	//PRT_LOG_INFO("parse taskData %s.", taskData);

	int outIcStrLen = 0;
	int outFmtStrLen = 0;
	if (OK != g_taskHdl->dataParse(taskData, taskDataLen,
				&taskInfo->outPrtInfo,
				taskInfo->outIcStr, &outIcStrLen,
				taskInfo->outIcFmtStr, &outFmtStrLen,
				taskSerialNo,
				taskInfo->resultFailStr))
	{
		prtSetNetStatus(2, "��������ʧ��");

		taskInfo->endByErrors = TRUE;
		PRT_LOG_ERROR("parse data failed, send result");
		//MessageBox(NULL, _T("�������ݽ���ʧ�ܣ�"), _T("����"), MB_OK);
		::PostMessage(g_parentWnd, WM_POPUP_WARNING_DLG, WARN_PARSE_FAILED, (LPARAM)0);

		/*����������ͽ��*/
		taskInfo->flag = FLAG_SEND_RET;
		g_taskHdl->sendTaskResult(taskInfo);

		/*���������û�����,��Ҫֱ���ͷ�*/
		free(taskInfo);
		return;
	}

	g_taskHdl->dataExpand(taskInfo->outIcStr, &outIcStrLen,
			taskInfo->outIcFmtStr, &outFmtStrLen);
	PRT_LOG_INFO("parse task data succ.");

	PRT_PRINTER_CFG_T *prtParam = printerCfgGet();

	taskInfo->flag = FLAG_PRINT | FLAG_SEND_RET;
	if (FALSE == prtParam->isNotWrIc)
	{
		//DEBUGģʽ�²�д��
		taskInfo->flag |= FLAG_WRITE_IC;
	}
	
	/*�����ʼ״̬*/
	g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_INIT);
	if(OK != printTaskHdl(taskInfo))
	{
		return;
	}

	return;
}

void PrtService::get_task_timer_hdl(void* param1, void* param2, void* param3)
{
	int taskDataLen = 0;
	char *taskData = NULL;

	PrtService *prtSrv = (PrtService*)param1;
	
	/*û�л�ȡ�������ӣ��򲻻�ȡ����*/
	char *jiamiyinzi = jiamiyinziGet();
	if (jiamiyinzi[0] == 0)
	{
		return;
	}

	if (prtSrv->m_stopGetTask || prtSrv->m_printerStopped)
	{
		return;
	}

	if (g_taskHdl->isTaskingFull())
	{
		return;
	}
	
	int ret = OK;
	CString strTmp;

	int taskSessionId = 0;
	char taskSerialNo[9] = {0};
	ret = wsGetTask(&taskSessionId, &taskData, &taskDataLen, taskSerialNo);
	if (SERVER_NO_TASK == ret)
	{
		g_prtService->m_SevrNoTaskCnt++;
		strTmp.Format("������%d��", g_prtService->m_SevrNoTaskCnt);
		prtSetNetStatus(3, strTmp.GetBuffer(0));

		return;
	}
	else if (OK != ret)
	{
		/*��ȡʧ�ܣ�������*/
		return;
	}

	g_prtService->m_ReqCnt++;
	strTmp.Format("��ȡ��%d������, ���к� %s, ID %u",
			g_prtService->m_ReqCnt, taskSerialNo, taskSessionId);
	PRT_LOG_INFO("%s.", strTmp);
	prtSetNetStatus(2, strTmp.GetBuffer(0));

	prtSrv->_taskHandle(taskSessionId, taskData, taskDataLen, taskSerialNo);
	free(taskData);

	return;
}

int PrtService::init()
{
	CString strTmp;

#if 0
	char err_buf[64] = {0};
	
	WSADATA  Ws;
	if (WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
	{
		strTmp.Format("Init Windows Socket Failed, %s!",
					str_error_s(err_buf, sizeof(err_buf), errno));
		MessageBox(NULL, strTmp, "����", MB_OK);
		return ET_ERROR;
	}


	if (FALSE == XpsDllLoad())
	{
		MessageBox(NULL, "����xps��̬��ʧ�ܣ�", "����", MB_OK);
		return ET_ERROR;
	}
#endif

	PRT_PRINTER_CFG_T *prtParam = printerCfgGet();
	m_printerPrtedCnt = XpsGetPrtedCardCnt(prtParam->printer);
	m_printerSdPercent = XpsGetSdPercent(prtParam->printer);

	if (FALSE == etp_init())
	{
		PRT_LOG_ERROR("��ʼ��ETPʧ��.");
		MessageBox(NULL, "��ʼ��ETPʧ�ܣ�", "����", MB_OK);
		return ET_ERROR;
	}

	etp_init_listen_thrds(1, 0, 1);
	m_etpEvtThrdPool = etp_init_evt_thrds(1, 0, 1, XpsDllLoad, XpsDllUnLoad);

	if (ET_ERROR == XpsWaitLoaded())
	{
		PRT_LOG_INFO("printer DLL load failed.");
		return ET_ERROR;
	}

	etp_add_time_job(m_etpEvtThrdPool, PrtService::get_task_timer_hdl,
			(void*)this, NULL, NULL, 5, FALSE);

	etp_start();

	PRT_LOG_INFO("service start success.");
	return OK;
}

void PrtService::cleanup(void)
{
	etp_stop();	

	etp_del_time_job(PrtService::get_task_timer_hdl, (void*)this);
	/*�˳�ʱֱ���˳�,�����������ע��,��Ⱥܾ�*/
	//etp_free_evt_thrds(m_etpEvtThrdPool);
	m_etpEvtThrdPool = NULL;
	
	etp_free();	
}
