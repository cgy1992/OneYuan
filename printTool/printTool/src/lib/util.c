/*
 * util.c
 *
 *  Created on: 2015��6��27��
 *      Author: abcht
 */



#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <stdarg.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#define STRDUP(a) _strdup(a)
#else
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#define STRDUP(a) strdup(a)
#endif

#include "commontype.h"

int CreatDir(char *pDir)
{
	int i = 0;
	int iRet;
	int iLen;
	char* pszDir;

	if(NULL == pDir)
	{
		return 0;
	}

	pszDir = STRDUP(pDir);
	iLen = strlen(pszDir);

	// �����м�Ŀ¼
	for (i = 0;i < iLen;i ++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{
			pszDir[i] = '\0';

			//���������,����
			iRet = ACCESS(pszDir,0);
			if (iRet != 0)
			{
				iRet = MKDIR(pszDir);
				if (iRet != 0)
				{
					return -1;
				}
			}
			//֧��linux,������\����/
			pszDir[i] = '/';
		}
	}

	iRet = MKDIR(pszDir);
	free(pszDir);
	return iRet;
}


