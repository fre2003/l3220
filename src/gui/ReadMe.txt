

ScintillaWin::SWndProc ��Ϣ��ڣ�

AddChar�������ַ����� 

int Editor::KeyDown : ������ַ����� 

//���������ʵ����� ��
int Editor::KeyCommand(unsigned int iMessage) {
	switch (iMessage) {
	case SCI_LINEDOWN:
		CursorUpOrDown(1);

�س���
SCI_NEWLINE();

SCI_DOCUMENTEND

	case SCI_DELETEBACK:
    {
#ifdef HAVE_CCL
//     int lineStart = pdoc->LineFromPosition(SelectionStart());
//     int lineEnd   = pdoc->LineFromPosition(SelectionEnd());
//     int start     = pdoc->LineStart(lineStart);
//     int end       = pdoc->LineStart(lineEnd + 1);
    
    int line  = pdoc->LineFromPosition(currentPos);
    int start = pdoc->LineStart(line);    
    if(currentPos == start)
    {
      MovePositionTo(pdoc->Length());
      SetLastXChosen();
      return 0;
    }
    //pdoc->DeleteChars(start, currentPos - start);
#endif


// Control charater 
Editor::ControlCharacterString