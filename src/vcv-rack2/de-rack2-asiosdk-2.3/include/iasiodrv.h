#pragma once

#include <asio.h>

/* Forward Declarations */

//#ifndef interface
//#define interface class
//#endif

//#ifndef __ASIODRIVER_FWD_DEFINED__
//#define __ASIODRIVER_FWD_DEFINED__
////typedef interface IASIO IASIO;
//class IASIO;

//#endif 	/* __ASIODRIVER_FWD_DEFINED__ */

class IASIO : public IUnknown
{
public:
	virtual ASIOBool init(void *sysHandle) = 0;
	virtual void getDriverName(char *name) = 0;
	virtual long getDriverVersion() = 0;
	virtual void getErrorMessage(char *string) = 0;
	virtual ASIOError start() = 0;
	virtual ASIOError stop() = 0;
	virtual ASIOError getChannels(long *numInputChannels, long *numOutputChannels) = 0;
	virtual ASIOError getLatencies(long *inputLatency, long *outputLatency) = 0;
	virtual ASIOError getBufferSize(long *minSize, long *maxSize,
		long *preferredSize, long *granularity) = 0;
	virtual ASIOError canSampleRate(ASIOSampleRate sampleRate) = 0;
	virtual ASIOError getSampleRate(ASIOSampleRate *sampleRate) = 0;
	virtual ASIOError setSampleRate(ASIOSampleRate sampleRate) = 0;
	virtual ASIOError getClockSources(ASIOClockSource *clocks, long *numSources) = 0;
	virtual ASIOError setClockSource(long reference) = 0;
	virtual ASIOError getSamplePosition(ASIOSamples *sPos, ASIOTimeStamp *tStamp) = 0;
	virtual ASIOError getChannelInfo(ASIOChannelInfo *info) = 0;
	virtual ASIOError createBuffers(ASIOBufferInfo *bufferInfos, long numChannels,
		long bufferSize, ASIOCallbacks *callbacks) = 0;
	virtual ASIOError disposeBuffers() = 0;
	virtual ASIOError controlPanel() = 0;
	virtual ASIOError future(long selector,void *opt) = 0;
	virtual ASIOError outputReady() = 0;
};

#define DRVERR			-5000
#define DRVERR_INVALID_PARAM		DRVERR-1
#define DRVERR_DEVICE_ALREADY_OPEN	DRVERR-2
#define DRVERR_DEVICE_NOT_FOUND		DRVERR-3

#define MAXPATHLEN			512
#define MAXDRVNAMELEN		128

struct asiodrvstruct
{
	int						drvID;
	CLSID					clsid;
	char					dllpath[MAXPATHLEN];
	char					drvname[MAXDRVNAMELEN];
	LPVOID					asiodrv;
	struct asiodrvstruct	*next;
};

typedef struct asiodrvstruct ASIODRVSTRUCT;
typedef ASIODRVSTRUCT	*LPASIODRVSTRUCT;

class AsioDriverList
{
public:
	AsioDriverList();
	~AsioDriverList();

	///@todo Benni
	IASIO* getIASIO (int drvID);

	LONG asioOpenDriver (int,VOID **);
	LONG asioCloseDriver (int);
	LONG asioGetNumDev (VOID);
	LONG asioGetDriverName (int,char *,int);
	LONG asioGetDriverPath (int,char *,int);
	LONG asioGetDriverCLSID (int,CLSID *);

	// or use directly access
	LPASIODRVSTRUCT	lpdrvlist;
	int				numdrv;
};

typedef class AsioDriverList *LPASIODRIVERLIST;


#if MAC
   #include "CodeFragments.hpp"
   class AsioDrivers : public CodeFragments
#elif WINDOWS
   // #include <windows.h>
   class AsioDrivers : public AsioDriverList
#elif SGI || BEOS
   class AsioDrivers : public AsioDriverList
#else
   #error implement me
#endif

{
public:
    AsioDrivers();
    ~AsioDrivers();

    bool getCurrentDriverName(char *name);
    long getDriverNames(char **names, long maxDrivers);
    bool loadDriver(char *name);
    void removeCurrentDriver();
    long getCurrentDriverIndex() {return curIndex;}
protected:
    unsigned long connID;
    long curIndex;
};
