
#include "prtGlobal.h"
#include "socketwrap.h"
#include "jiamijiProxy.h"

int g_jiamijiProxyAddr = DEF_JIAMIJI_PROXY_ADDR;
int g_jiamijiProxyPort = DEF_JIAMIJI_PROXY_PORT;

static int g_jiamijiProxyStop = 0;
static HANDLE g_thrd_handle = NULL;

static unsigned int WINAPI threadFunc(void * arg)
{
    PRT_LOG_INFO("start jiamiji local proxy thread.");

	int listen_sock = sock_create_server(g_jiamijiProxyPort);
    if (-1 == listen_sock)
    {
    	PRT_LOG_ERROR("create proxy server failed.");
    	return 0;
    }

    fd_set rset;
	struct timeval tv;
	int maxFd = 0;

	int client_sock = -1;
	int proxy_sock = -1;

    while(g_jiamijiProxyStop == 0)
    {
		FD_ZERO(&rset);

		FD_SET(listen_sock, &rset);
		maxFd = listen_sock;

		if (client_sock != -1)
		{
			FD_SET(client_sock, &rset);
			if (client_sock > maxFd)
			{
				maxFd = client_sock;
			}
		}

		if (proxy_sock != -1)
		{
			FD_SET(proxy_sock, &rset);
			if (proxy_sock > maxFd)
			{
				maxFd = proxy_sock;
			}
		}

		tv.tv_sec = 1;
		tv.tv_usec = 0;
		int fd_num = select(maxFd + 1, &rset, NULL, NULL, &tv);
		if (fd_num > 0)
		{
			if(FD_ISSET(listen_sock, &rset))
            {
                /*有新的连接*/
                unsigned int client_ip = 0;
				unsigned short client_port = 0;
			    int tmp_client_sock = sock_accept(listen_sock, &client_ip, &client_port);
			    if (-1 != tmp_client_sock)
			    {
			    	PRT_LOG_INFO("accept a new jiamiji client, fd %d", tmp_client_sock);
			    	if (client_sock != -1)
			    	{
			    		PRT_LOG_ERROR("already has jiamiji session, fd %d, close it", client_sock);
			    		sock_close(client_sock);
			    		client_sock = -1;
			    	}

		    		client_sock = tmp_client_sock;
		    		sock_set_unblock(client_sock);

		    		/*连接加密机服务器*/
				    PRT_PRINTER_CFG_T* prtcfg = printerCfgGet();
				    int tmp_proxy_sock = sock_connect(prtcfg->jimijiAddrDesc, prtcfg->jimijiPort);
				    if (-1 == tmp_proxy_sock)
				    {
				    	PRT_LOG_ERROR("connect to jiamiji %s failed", prtcfg->jimijiAddrDesc);
						sock_close(client_sock);
						client_sock = -1;
				    }
				    else
				    {
				    	proxy_sock = tmp_proxy_sock;
				    	sock_set_unblock(proxy_sock);
				    }
			    }
			    else
			    {
			    	PRT_LOG_ERROR("accept failed");
			    }	    
            }

            if (-1 != client_sock)
            {
	            if (FD_ISSET(client_sock, &rset))
	            {
	            	char buffer[256] = {0};
	            	int recvLen = recv(client_sock, buffer, 256, 0);
					if (recvLen < 0)
					{
						DWORD dwError = WSAGetLastError();
						if (dwError != WSAEINTR && dwError != WSAEWOULDBLOCK)
						{
							char err_buf[64] = {0};
							PRT_LOG_ERROR("jiamiji client read failed [%d]", dwError);
							sock_close(client_sock);
							client_sock = -1;
							sock_close(proxy_sock);
							proxy_sock = -1;
						}
					}
					else if (0 == recvLen)
					{
						PRT_LOG_INFO("jiamiji client closed");
						sock_close(client_sock);
						client_sock = -1;
						sock_close(proxy_sock);
						proxy_sock = -1;
					}
					else
					{
						/*发给jiamiji*/
						int sendLen = sock_write_timeout(proxy_sock, buffer, recvLen, 6);
						if (sendLen != recvLen)
						{
							PRT_LOG_ERROR("send to jiamiji server failed");
							sock_close(client_sock);
							client_sock = -1;
							sock_close(proxy_sock);
							proxy_sock = -1;
						}
						else
						{
							PRT_LOG_INFO("recv from jiamiji client, send to server, len %d", sendLen);
						}
					}
	            }
        	}

        	if (-1 != proxy_sock)
            {
	            if (FD_ISSET(proxy_sock, &rset))
	            {
	            	char buffer[256] = {0};
	            	int recvLen = recv(proxy_sock, buffer, 256, 0);
					if (recvLen < 0)
					{
						DWORD dwError = WSAGetLastError();
						if (dwError != WSAEINTR && dwError != WSAEWOULDBLOCK)
						{
							char err_buf[64] = {0};
							PRT_LOG_ERROR("jiamiji server read failed [%d]", dwError);
							sock_close(client_sock);
							client_sock = -1;
							sock_close(proxy_sock);
							proxy_sock = -1;
						}
					}
					else if (0 == recvLen)
					{
						PRT_LOG_INFO("jiamiji server closed");
						sock_close(client_sock);
						client_sock = -1;
						sock_close(proxy_sock);
						proxy_sock = -1;
					}
					else
					{
						/*发给jiamiji*/
						int sendLen = sock_write_timeout(client_sock, buffer, recvLen, 6);
						if (sendLen != recvLen)
						{
							PRT_LOG_ERROR("send to jiamiji client failed");
							sock_close(client_sock);
							client_sock = -1;
							sock_close(proxy_sock);
							proxy_sock = -1;
						}
						else
						{
							PRT_LOG_INFO("recv from jiamiji server, send to client, len %d", sendLen);
						}
					}
	            }
        	}
		}
	}

	sock_close(listen_sock);
	PRT_LOG_INFO("jiamiji local proxy thread stopped.");
	return 1;
}

int jiamijiProxyInit()
{
	g_jiamijiProxyStop = 0;

	/*start thread to get config*/
    g_thrd_handle = (HANDLE)_beginthreadex(NULL, 0, threadFunc, NULL, 0, NULL);
    return 0;
}

void jiamijiProxyFree()
{
	if (NULL == g_thrd_handle)
	{
		return;
	}
	
	g_jiamijiProxyStop = 1;

	::WaitForSingleObject(g_thrd_handle, INFINITE);
	/*CloseHandle 只是减少了一个索引，线程真正退出时会释放资源*/
	::CloseHandle(g_thrd_handle);
}
