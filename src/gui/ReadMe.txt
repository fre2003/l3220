

ScintillaWin::SWndProc 消息入口：

AddChar：处理字符按键 

int Editor::KeyDown : 处理非字符按键 

//按键处理的实际入口 ：
int Editor::KeyCommand(unsigned int iMessage) {
	switch (iMessage) {
	case SCI_LINEDOWN:
		CursorUpOrDown(1);

回车后：
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