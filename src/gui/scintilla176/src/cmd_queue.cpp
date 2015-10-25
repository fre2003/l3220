#include "windows.h"
#include "shlwapi.h"
#include "stdio.h"
#include "cmd_queue.h"

#pragma comment(lib, "shlwapi.lib")

CKeyQueue::CKeyQueue()
{
 	ZeroMemory(m_szCommandHistoryFile, sizeof(m_szCommandHistoryFile));
  m_bEnableHistoryCmd = FALSE;
}

CKeyQueue::~CKeyQueue()
{
  
}

long CKeyQueue::InitKeyQueue()
{
  return true;
}

bool CKeyQueue::IsCmdHistory()
{
  return m_bEnableHistoryCmd;
}

long CKeyQueue::TrimString(char* pBuffer)
{
  StrTrimA(pBuffer, " \t\r\n");
  
  return 0;
}

long CKeyQueue::LoadQueue(const char* pFileName)
{
  if(pFileName == NULL)
  {
    GetModuleFileNameA(NULL, m_szCommandHistoryFile, sizeof(m_szCommandHistoryFile) - 1);
    CHAR* lastperiod = strrchr(m_szCommandHistoryFile, ('\\'));
    if (lastperiod)
    {
      lastperiod[0] = 0;
    }
    if(pFileName == NULL)
    {
      strcat(m_szCommandHistoryFile, "\\gui3220_command_line_history.txt");
    }
    else 
    {
      strcat(m_szCommandHistoryFile, "\\");
      strcat(m_szCommandHistoryFile, pFileName);
    }
  }
  else 
  {
     lstrcpynA(m_szCommandHistoryFile, pFileName, sizeof(m_szCommandHistoryFile));
  }
  
  QUEUE_MSG_INTERNAL* pQ = (QUEUE_MSG_INTERNAL*)&g_hKeyQueue;
  
  memset(message_key_buffer, 0, sizeof(INPUT_KEY) * MAX_KEY_MSG_NUM);
  memset(pQ, 0, sizeof(QUEUE_MSG_INTERNAL));
  
  pQ->queue_max  = MAX_KEY_MSG_NUM;
  pQ->pMsgBuffer = (unsigned char*)message_key_buffer;
  memset(m_lastCmd, 0, sizeof(m_lastCmd));
  
  INPUT_KEY queue = { 0, };
  
  FILE* f = fopen(m_szCommandHistoryFile, "r");
  if (f)
  {
    char bufline[512] = { 0, };
    
    while (fgets(bufline, sizeof(bufline) - 1, f) != NULL)
    {
      StrTrimA(bufline, " \t\n");
      if (strlen(bufline) > 0)
      {
        strcpy((char*)queue.cmdLine, bufline);
        queue.status = 0;
        PostKeyQueue(&queue);
      }
    }
    
    fclose(f);
    m_bEnableHistoryCmd = TRUE;
  }
  
  return true;
}

long CKeyQueue::SaveQueue()
{
  QUEUE_MSG_INTERNAL* pQ = (QUEUE_MSG_INTERNAL*)&g_hKeyQueue;
  INPUT_KEY* queue = NULL;

  if(pQ->msg_num > MAX_KEY_MSG_NUM)
  {
    return false;
  }

  SetFileAttributesA(m_szCommandHistoryFile, FILE_ATTRIBUTE_NORMAL);
  FILE* f = fopen(m_szCommandHistoryFile, "w");
  if (f)
  {
    for (unsigned long i = 0; i < pQ->msg_num; i++)
    {
      queue = (INPUT_KEY*)&pQ->pMsgBuffer[sizeof(INPUT_KEY) * i];
      fprintf(f,"%s\n", queue->cmdLine);
    }
    fclose(f);
  }
  
  return true;
}


long CKeyQueue::PeekKeyQueue(INPUT_KEY* msg, bool bUp) // Up or Down
{
  QUEUE_MSG_INTERNAL* pQIns = (QUEUE_MSG_INTERNAL*)&g_hKeyQueue;
  INPUT_KEY* queue          = NULL;
  
  // Current Number
  if( pQIns->msg_num == 0 )
  {
    return false;
  }
  
  if( pQIns->out_index >= pQIns->queue_max )
  {
    return false;
  }
  
  if(bUp)
  {
    if(pQIns->current_idx == -1)
      pQIns->current_idx = pQIns->msg_num;
    else 
      pQIns->current_idx --;
  }
  else 
  {
    if(pQIns->current_idx == -1)
      pQIns->current_idx = 0;
    else 
      pQIns->current_idx ++;

  }
  if (pQIns->current_idx >= pQIns->msg_num)
  {
    pQIns->current_idx = 0;
  }
  
  //if(pQIns->current_idx == 0)
   // return FALSE;

  queue = (INPUT_KEY*)&pQIns->pMsgBuffer[sizeof(INPUT_KEY)*pQIns->current_idx];
  
  memcpy(msg, queue, sizeof(INPUT_KEY));
  
  return true;
}

long CKeyQueue::PostKeyQueue(INPUT_KEY* msg)
{
  QUEUE_MSG_INTERNAL* pQIns = (QUEUE_MSG_INTERNAL*)&g_hKeyQueue;
  INPUT_KEY* pKey;
  
  if (pQIns->msg_num >= pQIns->queue_max)
  {
    return false;
  }
  
  if (pQIns->in_index >= pQIns->queue_max)
  {
    return false;
  }
  
  for (unsigned long i = 0; i < pQIns->msg_num; i++)
  {
    pKey = (INPUT_KEY*)&pQIns->pMsgBuffer[sizeof(INPUT_KEY) * i];
    if(lstrcmpiA(pKey->cmdLine, msg->cmdLine) == 0)
    {
      pQIns->current_idx  = i;
      return true;
    }
  }

  pKey = (INPUT_KEY*)&pQIns->pMsgBuffer[sizeof(INPUT_KEY) * pQIns->in_index];
  
  memcpy(pKey, msg, sizeof(INPUT_KEY));
  
  (pQIns->msg_num)++;
  (pQIns->in_index)++;

  pQIns->current_idx = -1;
  
  if (pQIns->in_index >= pQIns->queue_max)    // Wrap
  {
    // pQIns->in_index = 0;
  }
  
  return true;
}

