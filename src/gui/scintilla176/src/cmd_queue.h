////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 1998-2009, RISS Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of RISS. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmission of any content of this file is not allowed except with the 
// written permission of RISS Technologies, Inc.
//
// FileName: cmd_queue.h
// 
// Description:
// TODO: ...
// 
// Revision History:
//    NAME              DATE                REMAKS
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CMD_QUEUE_H__
#define __CMD_QUEUE_H__

#define MAX_INPUT_SIZE  512
typedef struct
{
  char cmdLine[MAX_INPUT_SIZE];
  unsigned long status;
} INPUT_KEY;

class CKeyQueue
{
public:
  CKeyQueue();
  ~CKeyQueue();

  typedef struct tagQUEUE_MSG_INTERNAL
  {
     unsigned long  current_idx; 
     unsigned long  msg_num;
     unsigned long  in_index;
     unsigned long  out_index;
     unsigned long  queue_max;
     unsigned char* pMsgBuffer;
  }QUEUE_MSG_INTERNAL;

  typedef struct _QUEUE_INSTANCE
  {
    unsigned char _QUEUE_DATA[32];
  }QUEUE_INSTANCE;

public:
  long InitKeyQueue();
  long PeekKeyQueue(INPUT_KEY* msg, bool bUp);
  long PostKeyQueue(INPUT_KEY* msg);

public:
  long SaveQueue();
  long LoadQueue(const char* pFileName);
  long TrimString(char* pBuffer);
  bool IsCmdHistory();
  char m_lastCmd[MAX_INPUT_SIZE];
  
protected:

private:
  #define MAX_KEY_MSG_NUM   500
  char message_key_buffer[sizeof(INPUT_KEY) * MAX_KEY_MSG_NUM];
  QUEUE_INSTANCE g_hKeyQueue;
  char m_szCommandHistoryFile[256];
  bool m_bEnableHistoryCmd;
};

#endif //__CMD_QUEUE_H__


