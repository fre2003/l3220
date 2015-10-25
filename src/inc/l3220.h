//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a file of error code definitoin for L3220 Library.
// Copyright (C) 2002-2008 RISS Ltd.
// All rights reserved.
// 
// This source code or binary libary can be used, distributed or modified
// with or without modification, are permitted including commercial applications.
// -------------- 
// l3220.h:  
// Error No. Definiton
//*******************************************************************************


#ifndef __L3220_H__
#define __L3220_H__

START_HEADER_I


#define TRIM_CSTRING_I(x)       { x.TrimLeft(); x.TrimRight(); }
#define TRIM_CSTRING_LOWER_I(x) { x.TrimLeft(); x.TrimRight(); x.MakeLower();}
#define TRIM_CSTRING_UPPER_I(x) { x.TrimLeft(); x.TrimRight(); x.MakeUpper();}
 

END_HEADER_I
 
#endif //__L3220_H__




