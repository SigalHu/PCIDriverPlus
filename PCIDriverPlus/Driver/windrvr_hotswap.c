/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#include "windrvr_hotswap.h"

#if defined(GO_HOTSWAP) && defined(WIN32) && !defined(WINCE) 
#define RESCAN_BUS
#endif

#if defined(RESCAN_BUS)
extern BOOL RescanBus(const char *bus);
#endif

WD_HS_WATCH * DLLCALLCONV HSWatchAlloc(DWORD dwNumMatchTables)
{
    DWORD dwSize;
    DWORD dwNeedAllocate;
    WD_HS_WATCH *pwNew;

    dwNeedAllocate = dwNumMatchTables > 0 ? dwNumMatchTables-1 : 0;
    dwSize = sizeof(WD_HS_WATCH) + sizeof(WD_HS_MATCH_TABLE) * dwNeedAllocate;
    pwNew = (WD_HS_WATCH *)malloc(dwSize);
    if (!pwNew)
        return NULL;

    memset(pwNew, 0, dwSize);
    pwNew->dwNumMatchTables = dwNumMatchTables;
    return pwNew;
}

void DLLCALLCONV HSWatchFree(WD_HS_WATCH *pWatch)
{
    free(pWatch);    
}

BOOL DLLCALLCONV HSConfigurePci(WD_EVENT *pEvent)
{
    pEvent = pEvent; // reduce compilation warnings
#if defined(RESCAN_BUS)
    return RescanBus("PCI");
#endif
    return TRUE;
}
