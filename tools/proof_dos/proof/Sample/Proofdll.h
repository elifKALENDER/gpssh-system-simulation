//*****************************************************************************
//      proofdll.h - include file for C, C++ use of Proof DLL
//*****************************************************************************

#ifndef PROOF_DLL_INCLUDED
#define PROOF_DLL_INCLUDED      ONCE_ONLY

//*****************************************************************************
//      DLL request Return Codes
//*****************************************************************************

#define DLLRC_OK                1       // normal return
#define DLLRC_FAILURE           2       // error return
#define DLLRC_TERMINATED        3       // currently unused (cannot terminate DLL)
#define DLLRC_YIELDED           4       // the user clicked the yield sign

//*****************************************************************************
//      DLL Status Flags (Returned by ProofStatus)
//*****************************************************************************

#define PSF_IDLE                0x01    // 1 => Proof is idle
#define PSF_INPUT_NEEDED        0x02    // 1 => Proof is waiting for a trace line
#define PSF_INPUT_AVAILABLE     0x04    // 1 => An unprocessed trace line is ready
#define PSF_END                 0x08    // 1 => End command has been processed
#define PSF_RUNNING             0x10    // 1 => Proof is executing trace commands
#define PSF_SUSPENDED           0x20    // 1 => Proof is suspended
#define PSF_MINIMIZED           0x40    // 1 => Proof Window is minimized
#define PSF_LAYOUT_CHANGED      0x80    // 1 => Proof layout has been changed

//*****************************************************************************
//      DLL Entry Points
//*****************************************************************************

/*

Entry Point         Argument            Purpose                 Return Values
-----------         --------            -------                 -------------

ProofDLL            int on_off          non-zero => startup     non-zero => success
                                        zero => shutdown

ProofOpenLayout     char *filename      open layout             DLLRC_OK      => success
                                                                DLLRC_FAILURE => error, e.g., file not found

ProofSendTraceLine  char *command       transmit a trace line   DLLRC_OK      => normal
                                                                DLLRC_YIELDED => yield sign clicked; must retransmit
                                                                DLLRC_FAILURE => error

ProofSuspend        void                suspends Proof          none
ProofResume         void                resumes Proof           none
ProofStatus         void                get Proof status        PSF_... flags (See above)

Notes:  

(1) Calling ProofDLL is not really necessary.  ProofOpenLayout will initialize 
the DLL if ProofDLL has not yet been called. 

(2) ProofDLL can be called with a zero argument to terminate Proof.

(3) If ProofSendTraceLine returns DLLRC_YIELDED, the trace command your 
application sent remains unprocessed; you must retransmit it upon resuming. 
        
(4) If ProofSendTraceLine returns DLLRC_YIELDED, suspension of Proof is implied; 
it is not necessary to call ProofSuspend. 
        
(5) If ProofSendTraceLine is called when Proof is suspended, Proof is NOT
auomatically resumed; an explicit call of ProofResume is necessary. 
        
(6) You should not write to the screen while Proof is running.  Doing so will 
cause DirectDraw to jerk the rug out from under Proof, yielding results that are 
at best unpredictable.  If you need to write to the screen, you should bracket 
screen updates with ProofSuspend and ProofResume calls. 
        
(7) If your application stops while a trace stream is being played, Proof's menu 
bar will flash once / second.  If your application needs to stop for input, it 
should call ProofSuspend.  Otherwise, the user will have to minimize Proof in 
response to the blinking menu bar, and the NEXT trace line it sends to 
ProofSendTraceLine will elicit a DLLRC_YIELDED return code. 
        
*/

/*box
        Extended DLL Declarations
*/

struct DLLSetup
        {
        int     mask;

#define DLL_OEM                 1
#define DLL_REALTIME            2
#define DLL_NONFULLSCREEN       4

        int     screen_width,
                screen_height;

        RECT    window_rect;
        };

#endif

