/* ****************************************************************************
 * L2C V2.00 Application
 *
 *
 *
 * ****************************************************************************/


/* ***************************************************************************
*  INCLUDE FILES
*/

//#include "LinkL3.h"
//#include "Network.h"
//#include "Event.h"
#include "BootLoaderApp.h"

#include "Application.h"

/*!
 * \mainpage
\brief L2C Architecture
\dot
digraph L2C {

subgraph system {

	subgraph L1 {
		Timer -> Poll -> L1Comm
		L3Comm -> L3Host
		L3Host -> L3Comm
		}

	subgraph L3 {
		L2C_HW -> Events -> L3Comm
		L1Comm -> L1Slaves
		L1Slaves -> L1Comm
		L1Comm -> Poll
		}


	}

Poll -> Events
L3Comm -> Poll
L3Comm -> L1Comm
L1Comm -> L3Comm

{ rank = same; "L2C_HW"; "Timer"; }
{ rank = same; "Events"; "Poll"; }
{ rank = same; "L1Comm"; "L3Comm"; }
{ rank = same; "L3Host"; "L1Slaves";}

}
\enddot

There are three for data-flows in this diagram.
1. L3 request commands to L1 elements. L1 responds.
2. L3 request polling of L1 elements. Poll requests L1 elements and if there is an event a notification goes to L3.
3. L2C hardware generates an event that is forwarded to L3.
4. L3 request a command to L2C. L2C responds.


*/

/*!
 * Initialize all application modules
 */
void f_ApplicationInit(void)
{
//	f_LinkL3Init();
//	f_NetworkInit();
//	f_EventInit();
	f_Init_BootLoaderApp();
}

/*!
 *  Main Application Root Function
 */
void f_ApplicationControl(void)
{
	/* f_SysTickTimer is running on main (caller) above */
//	f_LinkL3Task();
//	f_EventTask();
//	f_NetWorkDispatchL2();
	f_BootLoaderApp();
}
