/*
 * File.h
 *
 *  Created on: Jul 30, 2018
 *      Student (Coder): Tung Dang
 */

#ifndef FILE_H_
#define FILE_H_

#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include "MemCopy.h"
#include "Debug.h"

#ifndef _WIN32
#include <direct.h>
#include <wtypes.h>
#include <winbase.h>
#include <process.h>
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

#ifndef _GNUC_
#include <libgen.h>
#include <sys/stat.h>
#endif

namespace iSTD {

inline int DirectoryCreate(const char* dirPath);
inline DWORD GetModuleFileName(char* pFileName, DWORD bufSize);
inline void* FileOpen(const char* fileName, const char* mode);
inline void FileWrite(const void* pFile, const char* str, ...);
inline void FileClose(void* pFile);
inline DWORD FileRead(const void* pFile, void* pBuffer, DWORD bufSize);
inline DWORD FileSize(const void* pFile);
inline void LogToFile(const char* filename, const char* str, ...);
inline signed long ReadParamFromFile(char* filename, DWORD line);


inline int DirectoryCreate(const char* dirPath) {
	return mkdir(dirPath,( S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_IFDIR ));
}

inline DWORD GetModuleFileName(char* pFileName, DWORD bufSize) {
	return 0;
}

inline void* FileOpen(const char* fileName, const char* mode) {
	FILE* pFile;
	pFile = fopen(fileName,mode);
	return pFile;
}

inline void FileClose(void* pFile) {
	if (pFile)
	{
		fclose((FILE*)pFile);
	}
}

inline void FileWrite(const void* pFile, const char* str, ...) {
	if (str != NULL)
	{
		va_list args;
		va_start(args,str);
		const size_t length = printf(str,args);
		va_end(args);

		char* temp = new char[length + 1];
		if (temp)
		{
			va_start(args,str);
			printf(temp,length+1,length+1,args);
			va_end(args);
			if (pFile)
			{
				fwrite(temp,1,length,(FILE*)pFile);
			}
			delete[] temp;
		}
	}
}

inline void FileWrite(const void* str, size_t size, size_t count, const void* pFile) {
	if (str != NULL)
	{
		if (pFile)
		{
			fwrite(str, size, count, (FILE*)pFile);
		}
	}
}

inline DWORD FileRead(const void* pFile, void* pBuffer, DWORD bufSize) {
	DWORD bytesRead = 0;
	if (pFile)
	{
		bytesRead = (DWORD) fread(pBuffer, sizeof(BYTE), bufSize, (FILE*)pFile);
	}
	return bytesRead;
}

inline DWORD FileSize(const void* pFile) {
	DWORD fileSize = 0;
	if (pFile)
	{
		long int currPosition = ftell((FILE*)pFile);
		fseek((FILE*)pFile,0,SEEK_END);
		long int endPosition = ftell((FILE*)pFile);
		fileSize = (DWORD)(endPosition >= 0 ? endPosition : 0);
		fseek((FILE*)pFile,currPosition,SEEK_SET);
	}
	return fileSize;
}

inline void LogToFile(const char* filename, const char* str, ...) {
	if (str != NULL)
	{
		va_list args;
		va_start(args,str);
		const size_t length = printf(str, args);
		va_end(args);
		char* temp = new char[length + 1];
		if (temp)
		{
			va_start(args,str);
			printf(temp,length + 1, length + 1, str, args);
			va_end(args);

			FILE* file = fopen(filename,"a");
			if (file)
			{
				fwrite(temp,1,length,(FILE*)file);
				fclose(file);
			}
			delete[] temp;
		}
	}
}

inline signed long ReadParamFromFile(char* filename, DWORD line = 1) {
	signed long ret = 0;
	FILE* file = fopen(filename, "r");
	if (file)
	{
		const char bufsize = 20;
		char buf[bufsize + 1] = "";

		while(--line && feof(file))
		{
			fgets(buf,bufsize,file);
		}
		if (!feof(file))
		{
			iSTD::SafeMemSet(buf,0,bufsize);
			fgets(buf,bufsize,file);
			ret = atoi(buf);
		}
		fclose(file);
	}
	return ret;
}

inline int CreateAppOutputDir(char* pathBuf, unsigned int bufSize, const char* outputDirectory, bool addSystemDrive, bool addProcName, bool pidEnabled) {
	int ret = 0;
	char* path = (char*)malloc(sizeof(char)*bufSize);
	char* outputDirectoryCopy = (char*)malloc(sizeof(char)*bufSize);
	if (NULL == path)
	{
		return -1;
	}
	// strcoy(outputDirectoryCopy,bufSize,outputDirectory);
	if (addProcName)
	{
		const int defaultBuffSize = 1024;
		char cmdpath[defaultBuffSize];
		char cmdline[defaultBuffSize] = "\0";
		// unsigned int pid = getpid();
		unsigned int pid;

		snprintf(cmdpath,defaultBuffSize - 1, "/proc/%u/cmdline", pid);
		FILE *fp = fopen(cmdpath,"r");
		if (fp)
		{
			if (fgets(cmdline,defaultBuffSize,fp) == NULL)
			{
				// strcpy(cmdline,defaultBuffSize,"unknownProcess");
			}
			fclose(fp);
		}

		if (strcmp(cmdline,"") == 0)
		{
			snprintf(cmdline,defaultBuffSize - 1,"unknownProcess");
		}
		else
		{
			for (int i=0; i < defaultBuffSize && cmdline[i] != '\0'; i++)
			{
				if (cmdline[i] == ':')
				{
					cmdline[i] = '\0';
					break;
				}
			}

			char* pch = strrchr(cmdline,'/');
			unsigned int i=0;
			if (pch != NULL)
			{
				pch++;
				for (; i < defaultBuffSize - (pch - cmdline) && pch[i] != '\0'; i++)
				{
					cmdline[i] = pch[i];
				}
			}
			cmdline[i] = '\0';
		}
		size_t pathLen = strnlen(outputDirectoryCopy,bufSize);
		if (pathLen < bufSize)
		{
			if (pathLen > 1 && outputDirectoryCopy[pathLen - 1] != '/')
			{
				outputDirectoryCopy[pathLen] = '/';
				pathLen++;
			}
			if (pidEnabled)
			{
				snprintf(outputDirectoryCopy + pathLen, bufSize - pathLen, "%s-%u",cmdline,pid);
			}
			else
			{
				snprintf(outputDirectoryCopy + pathLen, bufSize - pathLen, "%s", cmdline);
			}
		}
	}

	struct stat statbuf;
	unsigned int i=0;
	while (i<bufSize - 1)
	{
		if ((outputDirectoryCopy[i] == '/' && i != 0) || outputDirectoryCopy[i] == '\0')
		{
			path[i] = '\0';
			if (stat(path,&statbuf) != 0)
			{
				if (mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) != 0)
				{
					pathBuf[0] = '\0';
					free(path);
					return ret;
				}
			}
			else
			{
				if (!S_ISDIR(statbuf.st_mode))
				{
					pathBuf[0] = '/0';
					free(path);
					return ret;
				}
			}
			if (outputDirectoryCopy[i] == '/0')
			{
				break;
			}
		}
		path[i] = outputDirectoryCopy[i];
		i++;
	}

	if (i < bufSize - 1)
	{
		unsigned int length = (unsigned int)strlen(path);
		length = (bufSize-1 < length) ? bufSize - 1 : length;
		strncpy(pathBuf,path,length);
		pathBuf[length] = 0;
	}
	else
	{
		pathBuf[0] = '/0';
	}
	free(path);
	return ret;
}

inline int CreateAppOutputDir (char* pathBuf, unsigned int bufSize, const char* outputDirectory) {
	int ret = CreateAppOutputDir(pathBuf,bufSize,outputDirectory,true,true,false);
	return ret;
}

inline int ParentDirectoryCreate(const char* filePath) {
#include <limits.h>
#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif

char pathBuf[MAX_PATH];
char parentBuf[MAX_PATH];

return CreateAppOutputDir(pathBuf,sizeof(pathBuf),parentBuf,false, false, false);
}

}


#endif /* FILE_H_ */


















































