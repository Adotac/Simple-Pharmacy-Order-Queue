#include "header.h"

QUEUE *front, *rear, *qTemp;
STACK* top, * undo, * redo;
HINSTANCE gh_Inst;


int globalCmd;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc, pharmaChild, editChild;
	front = NULL; rear = NULL, qTemp = NULL; // Node initialize
	top = NULL, undo = NULL;

	gh_Inst = hInst; globalCmd = nCmdShow;
	wc = wcInitialize(gh_Inst, windowClassName);
	pharmaChild = pharmChildInitialize(gh_Inst, pharmaChildClassName);
	editChild = editChildInitialize(gh_Inst, editorChildClassName);


	HWND mainHandle;
	MSG msg;

	if (!RegisterClassEx(&wc)) { //Error check if window was not registered
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}
	if (!RegisterClassEx(&pharmaChild)) { //Error check if window was not registered
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}
	if (!RegisterClassEx(&editChild)) { //Error check if window was not registered
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}


	//Create the main window
	mainHandle = CreateWindowExA(WS_EX_CLIENTEDGE,
		windowClassName, "Simple-Pharmacy-Order-Queue",
		WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInst, NULL);


	//Stops the window resizing
	SetWindowLong(mainHandle, GWL_STYLE,
		GetWindowLong(mainHandle, GWL_STYLE) & ~WS_SIZEBOX);

	if (mainHandle == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(mainHandle, nCmdShow);
	UpdateWindow(mainHandle);

	//Window Run Looping
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateMDISysAccel(windowClient, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}





	return 0;
}