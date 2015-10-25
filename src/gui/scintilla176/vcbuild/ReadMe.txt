

PlatWin.cxx
    初始化

ScintillaWin.cxx：
    系统初始化 DllMain() --> Scintilla_RegisterClasses -->ScintillaWin::SWndProc 
    ScintillaWin::WndProc中处理WM_XXX消息

Editor.cxx 
    响应 SCI_XXX消息 


-->SetScrollInfo(SB_HORZ, ..)