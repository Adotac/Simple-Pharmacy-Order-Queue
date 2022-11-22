#include "header.h"
int dwSel = NULL;
int queCtr = 0, stackCtr = 0, fullFlag = FALSE, saveFlag = FALSE;

char* selecWord = (char*)malloc(sizeof(char) * CHARSIZE);
char orderObjects[MAX_ORDERS][CHARSIZE] = { "________________________", "Betadine", "Hydrite",
											"Kremil-S", "Diatabs", "Biogesic (Adults)",
											"Biogesic (Kids)", "Solmux", "Neozep",
											"Allerta", "Comprilex", "Fenoflex",
											"Mupicin", "Relestal", "Skelan"};

HBITMAP vacant, full, clear;
HWND winVacant, winFull, winNum;

HWND windowClient, firstChildHandle, pharmacyHandle, editorHandle;
HWND inPharmaList, outPharmaList, outEditBox, histCountBox;

HFONT TextBoxFont = CreateFontA(20, 0, 0, 0, 5,
	FALSE, FALSE, FALSE,
	ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Times New Roman");

HFONT numFont = CreateFontA(70, 0, 0, 0, 5,
	FALSE, FALSE, FALSE,
	ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Cambria");

/////////////////////---------------------------------------------------------------////////////////////////

HWND CreateNewMDIChild(HWND hMDIClient, const char* className, int width, int height, int w_offset, int h_offset) {
	MDICREATESTRUCT mcs;
	HWND hChild;
	mcs.szTitle = NULL;
	mcs.szClass = className;
	mcs.hOwner = gh_Inst;
	mcs.x = w_offset;
	mcs.cx = width;
	mcs.y = h_offset;
	mcs.cy = height;
	mcs.style = WS_EX_NOPARENTNOTIFY | WS_BORDER;
	hChild = (HWND)SendMessage(hMDIClient, WM_MDICREATE, 0, (LONG)&mcs);
	if (!hChild)
	{
		MessageBox(hMDIClient, "MDI Child creation failed.", "Oh Oh...",
			MB_ICONEXCLAMATION | MB_OK);
	}
	return hChild;
}

WNDCLASSEX wcInitialize(HINSTANCE gInst, const char* wCN) {
	WNDCLASSEX temp;
	temp.cbSize = sizeof(WNDCLASSEX);
	temp.style = CS_HREDRAW | CS_VREDRAW;
	temp.lpfnWndProc = mainWinProcedure;
	temp.cbClsExtra = 0;
	temp.cbWndExtra = 0;
	temp.hInstance = gInst;
	temp.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	temp.hCursor = LoadCursor(NULL, IDC_ARROW);
	temp.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	temp.lpszMenuName = "MAIN";
	temp.lpszClassName = wCN;
	temp.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	return temp;
}
LRESULT CALLBACK mainWinProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg)
	{
	case WM_COMMAND:
		switch (wp) {
		case PHARMA_DLG:
			pharmacyHandle = CreateNewMDIChild(windowClient, pharmaChildClassName, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH_OFFSET, WINDOW_HEIGHT_OFFSET);
			ShowWindow(editorHandle, SW_HIDE);
			
			SetWindowLong(pharmacyHandle, GWL_STYLE,
				GetWindowLong(pharmacyHandle, GWL_STYLE) & ~WS_SIZEBOX);

			//Reset All Variables//
			front = NULL; rear = NULL; queCtr = 0; fullFlag = FALSE;
			top = NULL; undo = NULL; stackCtr = 0;
			numProcess();
			break;
		case EDIT_DLG:
			editorHandle = CreateNewMDIChild(windowClient, editorChildClassName, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH_OFFSET, WINDOW_HEIGHT_OFFSET);
			ShowWindow(pharmacyHandle, SW_HIDE);

			SetWindowLong(editorHandle, GWL_STYLE,
				GetWindowLong(editorHandle, GWL_STYLE) & ~WS_SIZEBOX);
			//Reset All Variables//
			top = NULL; undo = NULL; stackCtr = 0;
			front = NULL; rear = NULL; queCtr = 0; fullFlag = FALSE;
			break;
		}
		break;
	case WM_CREATE: {
		
		CLIENTCREATESTRUCT ccs;
		ccs.idFirstChild = IDM_FIRSTCHILD;
		windowClient = CreateWindowEx(WS_EX_CLIENTEDGE, "mdiclient", NULL,
			WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hWnd, (HMENU)MAIN_MDI, GetModuleHandle(NULL), (LPVOID)&ccs);;

		mainAddMenus(hWnd);
		ShowWindow(windowClient, globalCmd);

		LoadImageBuffer(hWnd);

		pharmacyHandle = CreateNewMDIChild(windowClient, pharmaChildClassName, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH_OFFSET, WINDOW_HEIGHT_OFFSET);
		ShowWindow(editorHandle, SW_HIDE);

		SetWindowLong(pharmacyHandle, GWL_STYLE,
			GetWindowLong(pharmacyHandle, GWL_STYLE) & ~WS_SIZEBOX);

		//Reset All Variables//
		front = NULL; rear = NULL; queCtr = 0; fullFlag = FALSE;
		top = NULL; undo = NULL; stackCtr = 0;
		numProcess();

		break;
	}
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefFrameProcA(hWnd, windowClient, msg, wp, lp);
	}
	return 0;

}

/*     Pharmacy Implementations       */
WNDCLASSEX pharmChildInitialize(HINSTANCE gInst, const char* wCN) {
	WNDCLASSEX temp;
	temp.cbSize = sizeof(WNDCLASSEX);
	temp.style = NULL;
	temp.lpfnWndProc = pharmChildWinProc;
	temp.cbClsExtra = 0;
	temp.cbWndExtra = 0;
	temp.hInstance = gInst;
	temp.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	temp.hCursor = LoadCursor(NULL, IDC_ARROW);
	temp.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	temp.lpszMenuName = NULL;
	temp.lpszClassName = wCN;
	temp.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	return temp;
}
LRESULT CALLBACK pharmChildWinProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	char* selecTemp = (char*)malloc(sizeof(char) * CHARSIZE);

	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case HLISTBOX:
			switch (HIWORD(wp)) {
			case LBN_SELCHANGE:
				{  //Read the selected Medicine
				dwSel = SendMessage(inPharmaList, LB_GETCURSEL, 0, 0);

				if (dwSel > 0){
					SendMessageA(inPharmaList, LB_GETTEXT, (DWORD)dwSel, (LPARAM)selecWord);
					}			
				break;
				}
			}
			break;
		}
		
		switch (wp) {
		case IN_BUY_BTN:
			if (fullFlag == TRUE) {
				MessageBox(NULL, "Current batch is FULL! Please wait...", "FULL!",
					MB_ICONEXCLAMATION | MB_OK);
				break;
			}

			if (isFull() == TRUE) {
				SendMessageA(winFull, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)full);
				SendMessageA(winVacant, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)clear);
				fullFlag = TRUE;
				if (queCtr > MAX_CUSTOMERS-1) {
					MessageBox(NULL, "Current batch is FULL! Please wait...", "FULL!",
						MB_ICONEXCLAMATION | MB_OK);
					break;
				}

			}
			else{
				SendMessageA(winVacant, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)vacant);
			}
			//-------Get the selected product in the list------//
			if (dwSel != NULL) {
				enqueue(queCtr + 1, selecWord);
				dwSel = NULL;
				qTemp = front;
				displayOut();
				numProcess();
			}
			else
				MessageBoxA(hWnd, "Select a Product first!", "Warning!", MB_OK);

			break;
		case OUT_NEXT_BTN:
			if (fullFlag == TRUE) {
				dequeue();
				qTemp = front;
				displayOut();

				if (queCtr == 0) {
					SendMessageA(winVacant, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)vacant); //set vacancy
					SendMessageA(winFull, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)clear);
					numProcess();
					fullFlag = FALSE;
				}
			}
			else
				MessageBox(NULL, "Wait for the batch to be full!", "Warning!",
					MB_ICONEXCLAMATION | MB_OK);
			break;
		}
		
		break;
	case WM_CREATE: {
		CREATESTRUCT* pCreateStruct;
		MDICREATESTRUCT* pMDICreateStruct;
		pCreateStruct = (CREATESTRUCT*)lp;
		pMDICreateStruct = (MDICREATESTRUCT*)pCreateStruct->lpCreateParams;

		if (ShowWindow(pharmacyHandle, SW_HIDE) == TRUE) {
			ShowWindow(pharmacyHandle, SW_SHOW);
		}

		LoadImageBuffer(hWnd);
		createWindows(hWnd);

		SendMessageA(inPharmaList, WM_SETFONT, WPARAM(TextBoxFont), TRUE);
		SendMessageA(outPharmaList, WM_SETFONT, WPARAM(TextBoxFont), TRUE);

		for (int i = 0; i < MAX_ORDERS; i++) {
			SendMessageA(inPharmaList, LB_ADDSTRING, 0, (LPARAM)orderObjects[i]);
		}
		
		displayOut();
		SendMessageA(winVacant, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)vacant); //set vacancy
		SendMessageA(winFull, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)clear);
		numProcess();


		break;
	}
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefMDIChildProc(hWnd, msg, wp, lp);
	}
	return 0;

}

void mainAddMenus(HWND hWnd) {
	HMENU hMenu = CreateMenu(), hSubMenu = CreatePopupMenu();

	AppendMenu(hSubMenu, MF_STRING, PHARMA_DLG, "&Pharmacy Queue order");
	AppendMenu(hSubMenu, MF_STRING, EDIT_DLG, "&Text Saving-History simulation");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Programs");

	SetMenu(hWnd, hMenu);
}

void numProcess() {
	char currNum[CHARSIZE];
	memset(currNum, '\0', CHARSIZE);

	_itoa(queCtr+1, currNum, 10);

	if (queCtr > MAX_CUSTOMERS-1) {
		memset(currNum, '\0', CHARSIZE);
		currNum[0] = '?';
	}
		

	SetWindowTextA(winNum, currNum);

}

void LoadImageBuffer(HWND hWnd) {
	vacant = (HBITMAP)LoadImageA(NULL, "green.bmp", IMAGE_BITMAP, IMAGE_SIZE, IMAGE_SIZE, LR_LOADFROMFILE);
	full = (HBITMAP)LoadImageA(NULL, "red.bmp", IMAGE_BITMAP, IMAGE_SIZE, IMAGE_SIZE, LR_LOADFROMFILE);
	clear = (HBITMAP)LoadImageA(NULL, "tranp.bmp", IMAGE_BITMAP, IMAGE_SIZE, IMAGE_SIZE, LR_LOADFROMFILE);
}

void createWindows(HWND hWnd) {
	inPharmaList = CreateWindowExA(WS_EX_CLIENTEDGE, "LISTBOX", NULL,
		WS_CHILD | WS_VISIBLE | LBS_STANDARD | LBS_HASSTRINGS,
		80, 80, 240, 200,
		hWnd, (HMENU)HLISTBOX, gh_Inst, NULL);

	outPharmaList = CreateWindowExA(WS_EX_CLIENTEDGE, "LISTBOX", NULL,
		WS_CHILD | WS_VISIBLE | LBS_HASSTRINGS | WS_BORDER | WS_VSCROLL,
		(WINDOW_WIDTH / 2) + 80, 80, 240, 200,
		hWnd, (HMENU)OUTLISTBOX, gh_Inst, NULL);
	////////////////-------------------------------------//////////////////
	CreateWindowExA(WS_EX_CLIENTEDGE,
		"Static", "Medicine List:",
		WS_VISIBLE | WS_CHILD | ES_CENTER,
		WINDOW_WIDTH_OFFSET + 150, WINDOW_HEIGHT_OFFSET + 100,
		120, 25,
		hWnd, NULL, NULL, NULL);

	CreateWindowExA(WS_EX_CLIENTEDGE,
		"Static", "Queue List:",
		WS_VISIBLE | WS_CHILD | ES_CENTER,
		(WINDOW_WIDTH/2) + WINDOW_WIDTH_OFFSET + 150, WINDOW_HEIGHT_OFFSET + 100,
		120, 25,
		hWnd, NULL, NULL, NULL);

	CreateWindowExA(WS_EX_CLIENTEDGE,
		"Button", "BUY",
		WS_VISIBLE | WS_CHILD,
		150, 280,
		100, 45,
		hWnd, (HMENU)IN_BUY_BTN, NULL, NULL);
	///////////////////////////////////////////////
	winVacant = CreateWindowExA(WS_EX_CLIENTEDGE,
		"Static", NULL,
		WS_VISIBLE | WS_CHILD | SS_BITMAP,
		WINDOW_WIDTH_OFFSET + 220, WINDOW_HEIGHT_OFFSET + 390,
		60, 60,
		hWnd, NULL, NULL, NULL);
	//SendMessageA(winVacant, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)vacant);

	winFull = CreateWindowExA(WS_EX_CLIENTEDGE,
		"Static", NULL,
		WS_VISIBLE | WS_CHILD | SS_BITMAP,
		WINDOW_WIDTH_OFFSET + 220, WINDOW_HEIGHT_OFFSET + 460,
		60, 60,
		hWnd, NULL, NULL, NULL);
	//SendMessageA(winFull, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)full);
	////////////////////////////////////////
	 CreateWindowExA(WS_EX_CLIENTEDGE,
		"Static", "Your Number:",
		WS_VISIBLE | WS_CHILD | ES_CENTER,
		WINDOW_WIDTH_OFFSET + 90, WINDOW_HEIGHT_OFFSET + 400,
		100, 25,
		hWnd, NULL, NULL, NULL);

	 winNum = CreateWindowExA(WS_EX_CLIENTEDGE,
		"Static", NULL,
		WS_VISIBLE | WS_CHILD | ES_CENTER,
		WINDOW_WIDTH_OFFSET + 100, WINDOW_HEIGHT_OFFSET + 440,
		80, 80,
		hWnd, NULL, NULL, NULL);
	 SendMessageA(winNum, WM_SETFONT, WPARAM(numFont), TRUE);

	 CreateWindowExA(WS_EX_CLIENTEDGE,
		 "Static", "Vacant",
		 WS_VISIBLE | WS_CHILD | ES_CENTER,
		 WINDOW_WIDTH_OFFSET + 300, WINDOW_HEIGHT_OFFSET + 410,
		 80, 25,
		 hWnd, NULL, NULL, NULL);

	 CreateWindowExA(WS_EX_CLIENTEDGE,
		 "Static", "Full",
		 WS_VISIBLE | WS_CHILD | ES_CENTER,
		 WINDOW_WIDTH_OFFSET + 300, WINDOW_HEIGHT_OFFSET + 480,
		 80, 25,
		 hWnd, NULL, NULL, NULL);

	//Dividing Line
	CreateWindowExA(WS_EX_CLIENTEDGE,
		"Static", NULL,
		WS_VISIBLE | WS_CHILD,
		(WINDOW_WIDTH / 2) + WINDOW_WIDTH_OFFSET, 0,
		10, WINDOW_HEIGHT,
		hWnd, NULL, NULL, NULL);

	CreateWindowExA(WS_EX_CLIENTEDGE,
		"Button", "NEXT",
		WS_VISIBLE | WS_CHILD,
		(WINDOW_WIDTH/2) + WINDOW_WIDTH_OFFSET + 160, 300,
		100, 45,
		hWnd, (HMENU)OUT_NEXT_BTN, NULL, NULL);
}

void displayOut() {
	char* cTemp = (char*)malloc(sizeof(char) * CHARSIZE); memset(cTemp, '\0', CHARSIZE*sizeof(char));

	SendMessageA(outPharmaList, LB_RESETCONTENT, 0, 0);
	while (qTemp != NULL) {
		snprintf(cTemp, CHARSIZE, "%d | %s", qTemp->id_number, qTemp->order);
		SendMessageA(outPharmaList, LB_ADDSTRING, 0, (LPARAM)cTemp);
		qTemp = qTemp->next;
		memset(cTemp, '\0', CHARSIZE * sizeof(char));
	}
}

/*     Editor Implementations       */
WNDCLASSEX editChildInitialize(HINSTANCE gInst, const char* wCN) {
	WNDCLASSEX temp;
	temp.cbSize = sizeof(WNDCLASSEX);
	temp.style = NULL;
	temp.lpfnWndProc = editWinProcedure;
	temp.cbClsExtra = 0;
	temp.cbWndExtra = 0;
	temp.hInstance = gInst;
	temp.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	temp.hCursor = LoadCursor(NULL, IDC_ARROW);
	temp.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	temp.lpszMenuName = NULL;
	temp.lpszClassName = wCN;
	temp.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	return temp;
}
LRESULT CALLBACK editWinProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	char* histEdit = (char*)malloc(sizeof(char) * CHARSIZE); memset(histEdit, '\0', CHARSIZE * sizeof(char));
	switch (msg)
	{
	case WM_COMMAND:
		switch (wp) {
		case UNDO_BTN:
			if (stackCtr > 0) {
				if (saveFlag == TRUE) {
					undoProcess();
				}
				else {
					saveProcess();
					undoProcess();
					saveFlag = TRUE;
				}

				snprintf(histEdit, CHARSIZE, "History Count:\r\n%d", stackCtr);
				SetWindowTextA(histCountBox, histEdit);
			}
			break;
		case SAVE_BTN:
			saveProcess();
			snprintf(histEdit, CHARSIZE, "History Count:\r\n%d", stackCtr);
			SetWindowTextA(histCountBox, histEdit);
			break;
		}
		break;
	case WM_CREATE: {

		CREATESTRUCT* pCreateStruct;
		MDICREATESTRUCT* pMDICreateStruct;
		pCreateStruct = (CREATESTRUCT*)lp;
		pMDICreateStruct = (MDICREATESTRUCT*)pCreateStruct->lpCreateParams;

		if (ShowWindow(editorHandle, SW_HIDE) == TRUE) {
			ShowWindow(editorHandle, SW_SHOW);
		}

		editWindows(hWnd);

		snprintf(histEdit, CHARSIZE, "History Count:\r\n%d", stackCtr);
		SetWindowTextA(histCountBox, histEdit);

		break;
	}
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefMDIChildProc(hWnd, msg, wp, lp);
	}

	return 0;

}

void saveProcess() {
	char* saveEdit = (char*)malloc(sizeof(char) * CHARSIZE); memset(saveEdit, '\0', CHARSIZE* sizeof(saveEdit));
	

	GetWindowTextA(outEditBox, saveEdit, CHARSIZE*sizeof(saveEdit));

	push(&top, saveEdit);
	saveFlag = FALSE;
	undo = top;
	

}

void undoProcess() {	
	char* undoEdit = (char*)malloc(sizeof(char) * CHARSIZE); memset(undoEdit, '\0', CHARSIZE * sizeof(top->text));
	
	pop(&top);
	snprintf(undoEdit, sizeof(top->text) * CHARSIZE, "%s", top->text);
	SetWindowTextA(outEditBox, undoEdit);
}

void editWindows(HWND hWnd) {

	CreateWindowExA(WS_EX_CLIENTEDGE,
		"Button", "UNDO",
		WS_VISIBLE | WS_CHILD,
		WINDOW_WIDTH_OFFSET + 130, 80,
		100, 45,
		hWnd, (HMENU)UNDO_BTN, NULL, NULL);

	CreateWindowExA(WS_EX_CLIENTEDGE,
		"Button", "SAVE",
		WS_VISIBLE | WS_CHILD,
		WINDOW_WIDTH_OFFSET + 130, 180,
		100, 45,
		hWnd, (HMENU)SAVE_BTN, NULL, NULL);
	/////////////----------------//////////////
	outEditBox = CreateWindowExA(WS_EX_CLIENTEDGE, "Edit", "",
		WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL,
		WINDOW_WIDTH_OFFSET + 350, WINDOW_HEIGHT_OFFSET + 100, 440, 400,
		hWnd, NULL, gh_Inst, NULL);
	SendMessageA(outEditBox, WM_SETFONT, WPARAM(TextBoxFont), TRUE);

	histCountBox = CreateWindowExA(WS_EX_CLIENTEDGE, "Static", NULL,
		WS_CHILD | WS_VISIBLE | ES_CENTER,
		WINDOW_WIDTH_OFFSET + 115, WINDOW_HEIGHT_OFFSET + 400, 130, 50,
		hWnd, NULL, gh_Inst, NULL);
	SendMessageA(histCountBox, WM_SETFONT, WPARAM(TextBoxFont), TRUE);


}


/////////////////////////////------------QUEUE IMPLEMENTS------------//////////////////////////////////////

void enqueue(int id_num, char ord[]) {
	int ordLen = strlen(ord);
	QUEUE* temp = (QUEUE*)malloc(sizeof(QUEUE));

	if (rear == NULL) {
		rear = (QUEUE*)malloc(sizeof(QUEUE));
		rear->next = NULL;
		rear->id_number = id_num;
		strncpy_s(rear->order, ord, ordLen);
		front = rear;
	}
	else {
		rear->next = temp;
		temp->id_number = id_num;
		strncpy_s(temp->order, ord, ordLen);
		temp->next = NULL;

		rear = temp;
		//free(temp);
	}

	queCtr++;
}

void dequeue() {
	QUEUE* hCopy = front;

	if (hCopy == NULL) {
		MessageBox(NULL, "Element cannot be accessed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	else {
		if (hCopy->next != NULL) {
			hCopy = hCopy->next;
			free(front);
			front = hCopy;
		}
		else {
			free(front);
			front = NULL;
			rear = NULL;
		}
	}

	queCtr--;
}

int isFull() {
	if (queCtr >= MAX_CUSTOMERS-1)
		return TRUE;
	else
		return FALSE;
}

//////////////////////////--------------------STACK IMPLEMENTS--------------------////////////////////////////////////

void push(STACK** stk, char* data)
{
	
	STACK* temp = NULL;
	if (*stk == NULL)
	{
		*stk = (STACK*)malloc(1 * sizeof(STACK));
		(*stk)->text = (char*)malloc(sizeof(char));

		(*stk)->next = NULL;
		snprintf((*stk)->text, sizeof(data)*CHARSIZE, "%s", "");
		push(stk, data);
		stackCtr--;
	}
	else
	{
		temp = (STACK*)malloc(1 * sizeof(STACK));
		temp->text = (char*)malloc(sizeof(data) * CHARSIZE);

		temp->next = *stk;
		snprintf(temp->text, sizeof(data) * CHARSIZE, "%s", data);
		*stk = temp;
		temp = NULL;
	}
	stackCtr++;
}

void pop(STACK** stk){

	undo = *stk; //record latest edit history

	if (undo == NULL)
		return;
	else 
		undo = (*stk)->next;

	*stk = undo;
	stackCtr--;
}

