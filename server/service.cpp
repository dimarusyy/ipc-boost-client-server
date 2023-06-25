#include <Windows.h>
#include <tchar.h>
#include <string>
#include <cstdio>


SERVICE_STATUS        g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE                g_ServiceStopEvent = INVALID_HANDLE_VALUE;

VOID WINAPI ServiceMain( DWORD argc, LPTSTR* argv );
VOID WINAPI ServiceCtrlHandler( DWORD );
DWORD WINAPI ServiceWorkerThread( LPVOID lpParam );

#define SERVICE_NAME _T("IPC Boost Service")

int _tmain( int argc, TCHAR* argv[] )
{
	OutputDebugString( _T( "IPC Boost Service: Main: Entry" ) );

	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{ const_cast< LPSTR >( SERVICE_NAME ), ( LPSERVICE_MAIN_FUNCTION )ServiceMain },
		{ NULL, NULL }
	};

	if ( StartServiceCtrlDispatcher( ServiceTable ) == FALSE )
	{
		OutputDebugString( _T( "IPC Boost Service: Main: StartServiceCtrlDispatcher returned error" ) );
		return GetLastError();
	}

	OutputDebugString( _T( "IPC Boost Service: Main: Exit" ) );
	return 0;
}


VOID WINAPI ServiceMain( DWORD argc, LPTSTR* argv )
{
	DWORD Status = E_FAIL;

	OutputDebugString( _T( "IPC Boost Service: ServiceMain: Entry" ) );

	g_StatusHandle = RegisterServiceCtrlHandler( SERVICE_NAME, ServiceCtrlHandler );

	if ( g_StatusHandle == NULL )
	{
		OutputDebugString( _T( "IPC Boost Service: ServiceMain: RegisterServiceCtrlHandler returned error" ) );
		return;
	}

	// Tell the service controller we are starting
	ZeroMemory( &g_ServiceStatus, sizeof( g_ServiceStatus ) );
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	if ( SetServiceStatus( g_StatusHandle, &g_ServiceStatus ) == FALSE )
	{
		OutputDebugString( _T( "IPC Boost Service: ServiceMain: SetServiceStatus returned error" ) );
	}

	/*
	 * Perform tasks necessary to start the service here
	 */
	OutputDebugString( _T( "IPC Boost Service: ServiceMain: Performing Service Start Operations" ) );

	// Create stop event to wait on later.
	g_ServiceStopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	if ( g_ServiceStopEvent == NULL )
	{
		OutputDebugString( _T( "IPC Boost Service: ServiceMain: CreateEvent(g_ServiceStopEvent) returned error" ) );

		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = GetLastError();
		g_ServiceStatus.dwCheckPoint = 1;

		if ( SetServiceStatus( g_StatusHandle, &g_ServiceStatus ) == FALSE )
		{
			OutputDebugString( _T( "IPC Boost Service: ServiceMain: SetServiceStatus returned error" ) );
		}
		return;
	}

	// Tell the service controller we are started
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	if ( SetServiceStatus( g_StatusHandle, &g_ServiceStatus ) == FALSE )
	{
		OutputDebugString( _T( "IPC Boost Service: ServiceMain: SetServiceStatus returned error" ) );
	}

	// Start the thread that will perform the main task of the service
	HANDLE hThread = CreateThread( NULL, 0, ServiceWorkerThread, NULL, 0, NULL );

	OutputDebugString( _T( "IPC Boost Service: ServiceMain: Waiting for Worker Thread to complete" ) );

	// Wait until our worker thread exits effectively signaling that the service needs to stop
	WaitForSingleObject( hThread, INFINITE );

	OutputDebugString( _T( "IPC Boost Service: ServiceMain: Worker Thread Stop Event signaled" ) );


	/*
	 * Perform any cleanup tasks
	 */
	OutputDebugString( _T( "IPC Boost Service: ServiceMain: Performing Cleanup Operations" ) );

	CloseHandle( g_ServiceStopEvent );

	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 3;

	if ( SetServiceStatus( g_StatusHandle, &g_ServiceStatus ) == FALSE )
	{
		OutputDebugString( _T( "IPC Boost Service: ServiceMain: SetServiceStatus returned error" ) );
	}

	return;
}


VOID WINAPI ServiceCtrlHandler( DWORD CtrlCode )
{
	OutputDebugString( _T( "IPC Boost Service: ServiceCtrlHandler: Entry" ) );

	switch ( CtrlCode )
	{
	case SERVICE_CONTROL_STOP:

		OutputDebugString( _T( "IPC Boost Service: ServiceCtrlHandler: SERVICE_CONTROL_STOP Request" ) );

		if ( g_ServiceStatus.dwCurrentState != SERVICE_RUNNING )
			break;

		/*
		 * Perform tasks necessary to stop the service here
		 */

		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 4;

		if ( SetServiceStatus( g_StatusHandle, &g_ServiceStatus ) == FALSE )
		{
			OutputDebugString( _T( "IPC Boost Service: ServiceCtrlHandler: SetServiceStatus returned error" ) );
		}

		// This will signal the worker thread to start shutting down
		SetEvent( g_ServiceStopEvent );

		break;

	default:
		break;
	}

	OutputDebugString( _T( "IPC Boost Service: ServiceCtrlHandler: Exit" ) );
}


#include "ipc-boost-server.h"
#include <chrono>
#include <thread>

DWORD WINAPI ServiceWorkerThread( LPVOID lpParam )
{
	OutputDebugString( _T( "IPC Boost Service: ServiceWorkerThread: Entry" ) );
	int i = 0;
	//  Periodically check if the service has been requested to stop
	while ( WaitForSingleObject( g_ServiceStopEvent, 0 ) != WAIT_OBJECT_0 )
	{
		ipc_boost_service::run();
		std::this_thread::sleep_for( std::chrono::seconds{1} );
	}

	OutputDebugString( _T( "IPC Boost Service: ServiceWorkerThread: Exit" ) );

	return ERROR_SUCCESS;
}