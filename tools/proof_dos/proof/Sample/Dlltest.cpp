//------------------------------------------------------
//      dlltest.cpp - Example of ProofDLL Usage
//------------------------------------------------------

#define WIN32_LEAN_AND_MEAN     // Faster compilation

#ifdef __WATCOMC__

        #define CDECL   __cdecl
        #define EXPORT  __declspec(dllexport)
#else
        #define CDECL
        #define EXPORT  _declspec(dllexport)
#endif

#define FOREVER while(1)

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#include "proofdll.h"   // Wolverine-provided header file

//--------------------------------------
//      Local Function Prototypes
//--------------------------------------

int     GetEntryPoint           (char *name);
int     SendCommand             (char *command);

//--------------------------------------
//      Local Statics
//--------------------------------------

int             (CDECL *ProofDLL)               (int on_off);
int             (CDECL *ProofOpenLayout)        (char *filename);
int             (CDECL *ProofSendTraceLine)     (char *image);
void            (CDECL *ProofSuspend)           (void);
void            (CDECL *ProofResume)            (void);
int             (CDECL *ProofStatus)            (void);

static HINSTANCE        hProofLib;      // Proof DLL instance

//--------------------------------------
//      main()
//--------------------------------------

main()
        {
        int             iterations, rc;

        char            command[256];   // command buffer

//--->  if ((hProofLib = LoadLibrary("p4.dll")) == NULL)        // Commercial DLL
        if ((hProofLib = LoadLibrary("sp4.dll")) == NULL)       // Student DLL
                {
                printf("DLL load failed!\n");
                exit(0);
                }

        printf("ProofDLL loaded\n");

        *((int *) &ProofDLL)            = GetEntryPoint("ProofDLL_");
        *((int *) &ProofOpenLayout)     = GetEntryPoint("ProofOpenLayout_");
        *((int *) &ProofResume)         = GetEntryPoint("ProofResume_");
        *((int *) &ProofSendTraceLine)  = GetEntryPoint("ProofSendTraceLine_");
        *((int *) &ProofSuspend)        = GetEntryPoint("ProofSuspend_");
        *((int *) &ProofStatus)         = GetEntryPoint("ProofStatus_");

        rc = (*ProofDLL) (999);       // argument currently unused

        if (rc == 0)
                {
                printf("Unable to initiate Proof");
                exit(0);
                }

        printf("Opening blttestx.lay\n");

        rc = (*ProofOpenLayout) ("blttestx.lay");

        if (rc == DLLRC_OK)
                printf("blttestx.lay loaded OK\n");
        else
                {
                printf("Unable to load blttestx.lay\n");
                exit(0);
                }

        for (iterations = 1; iterations <= 40; iterations++)
                {
                if (iterations % 10 == 0)
                        {
                        (*ProofSuspend) ();             // from our side

                        printf("We have issued a ProofSuspend() call\n");

                        rc = MessageBox(
                                GetDesktopWindow(),
                                "Suspend/Resume Test",
                                "Press OK to resume executing.",
                                MB_OKCANCEL | MB_SETFOREGROUND);

                        if (rc != IDOK)
                                exit(0);

                        printf("We'll resume in one second\n");
                        Sleep(1000);

                        (*ProofResume) ();
                        }

                sprintf(command, "Create xact %i", iterations);

                if (SendCommand(command) < 0)
                        break;

                sprintf(command, "Place %i on loop", iterations);

                if (SendCommand(command) < 0)
                        break;

                sprintf(command, "Write count %i", iterations);

                if (SendCommand(command) < 0)
                        break;

                sprintf(command, "Time %i", iterations * 3);

                if (SendCommand(command) < 0)
                        break;
                }

        SendCommand("End");

        printf("Last trace command sent\n");

        FOREVER
                {
                rc = (*ProofStatus) ();         // current status

                if (rc & PSF_END)               // end command processed
                        break;

                printf("State = %2X\n", rc);

                Sleep(1000);                    // wait one second
                }

        Sleep(1000);

        printf("Shutting down the Proof DLL\n");

        (*ProofDLL) (FALSE);

        Sleep(1000);
        printf("Restarting the Proof DLL\n");
        Sleep(1000);

        (*ProofDLL) (999);

        Sleep(1000);
        printf("Shutting down the Proof DLL again\n");
        Sleep(1000);

        (*ProofDLL) (FALSE);

        printf("Trace streaming test complete!\n");

        Sleep(2000);
        exit(0);

        }       // end of main()

//--------------------------------------
//      GetEntryPoint
//--------------------------------------

int GetEntryPoint(char *name)
        {
        void    *address;

        address = GetProcAddress(hProofLib, name);

        if (address == NULL)
                fprintf(stderr, "Unable to find \"%s\" in the Proof Library (proofdll.dll)\n", name);

        return (int) address;
        }

//--------------------------------------
//      SendCommand
//--------------------------------------

int SendCommand(char *command)
        {
        int     rc,             // DLL return code
                status1,        // test ProofStatus()
                status2;

        FOREVER
                {
                status1 = (*ProofStatus) ();            // current status

                rc = (*ProofSendTraceLine) (command);   // trace stream command

                switch (rc)
                        {
case DLLRC_YIELDED:     status2 = (*ProofStatus) ();    // current status

                        printf("Proof has yielded back to us (%2X/%2X)\n", status1, status2);

                        Sleep(1000);

                        rc = MessageBox(
                                GetDesktopWindow(),
                                "Yield/Continue Test",
                                "Press OK to resume executing.",
                                MB_OKCANCEL | MB_SETFOREGROUND);

                        if (rc != IDOK)
                                exit(0);

                        printf("We'll continue in one second\n");
                        Sleep(1000);

                        (*ProofResume) ();      // re-enable Proof
                        continue;               // retry

case DLLRC_OK:          return 1;               // "OK" return

default:                printf("Trace stream error\n");

                        return -1;              // error return
                        }
                }

        }       // end of SendCommand()

