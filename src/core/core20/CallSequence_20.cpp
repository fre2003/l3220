#include "stdafx.h"
#include "windows.h"

 
#include <fstream> 

class ITSAPI CTraceCallSequence
{
public:
	static void Start(const char * szFileName);	  // Set the file name and start
	CTraceCallSequence(const char * szText);			// The only constructor
	virtual ~CTraceCallSequence();

protected:
	static int Stack[512]; 
	static int Level; 
	static std::ofstream * OutStream;
};

#ifdef _DEBUG
#define TRSEQ_START(szFileName)  CTraceCallSequence::Start(szFileName)
#define TRSEQ(szText)            CTraceCallSequence m_UniqueVariableName (szText)
#else
#define TRSEQ_START(szFileName)    
#define TRSEQ (szText)             
#endif

int CTraceCallSequence::Level;
std::ofstream * CTraceCallSequence::OutStream;
int CTraceCallSequence::Stack[512];

void CTraceCallSequence::Start(const char *fileName)
{
	OutStream = new std::ofstream (fileName);
}

CTraceCallSequence::CTraceCallSequence(const char * text)
{
	if (!OutStream)				// Outstream should be initialized
		return;

	for (int i = 0; i < Level; i++)
		*OutStream << '\t';		// Make indentation

	Stack[Level]++;				// Increment the sequence number

	for (i = 0; i < Level, Stack[i] > 0; i++)
		*OutStream << Stack[i] << '.';	// Print the sequence number

	*OutStream << " ";			// Add a space
	*OutStream << text << "\n";	// Write the text followed by newline
	Level++;					// Increase the level
}

CTraceCallSequence::~CTraceCallSequence()
{
	Stack[Level] = 0;			// Reset the sequence number
	Level--;					// Decrease the level
	if (Level < 0)				// If we are finished
	{
		delete OutStream;		// Write to the file and delete stream
		OutStream = 0;			// Reset the stream pointer
	}
}

