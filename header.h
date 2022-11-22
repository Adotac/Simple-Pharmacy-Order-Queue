#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "framework.h"
#pragma comment(lib,"comctl32.lib") //adding/linking the library

#ifndef header
#define header

#define TRUE 1
#define FALSE 0
#define CHARSIZE 64
#define MAX_CUSTOMERS 10
#define MAX_ORDERS 15

//define buttons
#define IN_BUY_BTN 50
#define OUT_NEXT_BTN 51
#define UNDO_BTN 52
#define SAVE_BTN 53

#define PHARMA_DLG 201
#define EDIT_DLG 202

//defined call messages
#define MAIN_MDI 101
#define CHILD_EDIT 105
#define IDM_FIRSTCHILD 50000
#define IDM_CODE_SAMPLES 10001
#define IC_STATISTICS 30 

#define HLISTBOX 40
#define OUTLISTBOX 41

//defualt dimensions
#define IMAGE_SIZE 60
#define WINDOW_WIDTH 850
#define WINDOW_HEIGHT 520
#define WINDOW_WIDTH_OFFSET -10
#define WINDOW_HEIGHT_OFFSET -55

extern HBITMAP vacant, full, clear;
extern HWND winVacant, winFull, winNum;

extern HWND windowClient, firstChildHandle;
extern HWND pharmacyHandle, editorHandle;
extern HWND inPharmaList, outPharmaList, outEditBox, histCountBox;

extern HINSTANCE gh_Inst;
extern int globalCmd;

const char windowClassName[] = "mainWindowClass";
const char pharmaChildClassName[] = "pChildClass";
const char editorChildClassName[] = "eChildClass";

extern HFONT TextBoxFont, numFont;
extern char orderObjects[MAX_ORDERS][CHARSIZE];
extern char* selecWord;

extern int queCtr, stackCtr;
typedef struct queue QUEUE;
struct queue {
	int id_number;
	char order[CHARSIZE];
	QUEUE* next;
};
extern QUEUE* front, * rear, *qTemp;

typedef struct stack STACK;
struct stack {
	char* text;
	STACK* next;
};
extern STACK* top, * undo;

HWND CreateNewMDIChild(HWND hMDIClient, const char* className, int width, int height, int w_offset, int h_offset);
WNDCLASSEX wcInitialize(HINSTANCE gInst, const char* wCN);
LRESULT CALLBACK mainWinProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASSEX pharmChildInitialize(HINSTANCE gInst, const char* wCN);
LRESULT CALLBACK pharmChildWinProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASSEX editChildInitialize(HINSTANCE gInst, const char* wCN);
LRESULT CALLBACK editWinProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

void mainAddMenus(HWND hWnd);
void LoadImageBuffer(HWND hWnd);
void displayOut();
void createWindows(HWND hWnd);
void numProcess();

void editWindows(HWND hWnd);
void saveProcess();
void undoProcess();

void enqueue(int id_num, char ord[]);
void dequeue();
int isFull();

void push(STACK** stk, char* data);
void pop(STACK** stk);


#endif 