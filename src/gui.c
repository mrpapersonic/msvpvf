/**
 * msvpvf GUI for Windows 
 * Copyright (c) Paper 2022
 *
 * View this file with 4-tab spacing; if you don't it will be a formatting nightmare.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
**/
#include <stdio.h>
#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include <commdlg.h>
#include "../include/common.h"
#define _WIN32_WINNT 0x0400
#define ARRAYSIZE(a) \
	sizeof(a)/sizeof(a[0])
#define OPEN_FILE_BUTTON	0
#define COMBOBOX		1
#define LISTBOX			2
#define SAVE_FILE_BUTTON	3
#define VERSION			4
#ifdef _MSC_VER
#define strdup(p) _strdup(p)
#endif

HWND hWndListBox, hWndComboBox, hWndVersion;
int16_t version = 13;
enum types {
	vf,
	veg
} type;
char* file_name = " ";

void display_file(char* path) {
	/* Read the file to memory */
	FILE* file;
	file = fopen(path, "rb");
	fseek(file, 0x46, SEEK_SET);
	int f_version = fgetc(file);
	fseek(file, 0x18, SEEK_SET);
	TCHAR p[32];
	switch (fgetc(file)) {
		case 0xEF:
			snprintf(p, 32, "File version: %s %d", "VEGAS Pro", f_version);
			break;
		case 0xF6:
			snprintf(p, 32, "File version: %s %d", "Movie Studio", f_version);
			break;
		default:
			snprintf(p, 32, "File version: %s %d", "Unknown", f_version);
			break;
	}
	printf("%s", p);
	SendMessage(hWndVersion, WM_SETTEXT, (WPARAM)0, (LPARAM)p);
	fclose(file);
}

char* open_file(HWND hWnd) {
	OPENFILENAME ofn;
	char filename[256];

	ZeroMemory(&ofn, OPENFILENAME_SIZE_VERSION_400);
	ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = filename;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = "Project files\0*.veg;*.vf\0All files\0*.*\0";
	ofn.nFilterIndex = 1;

	if (GetOpenFileName(&ofn) == 0) {
		return " ";
	}

	display_file(filename);

	return strdup(filename);
}

void save_file(HWND hWnd, char* input_file) {
	if (strcmp(input_file, " ") == 0) {
		MessageBox(hWnd,
					TEXT("Please open a file first!"),
					TEXT("Invalid input file!"),
					MB_ICONEXCLAMATION);
		return;
	}
	OPENFILENAME ofn;
	char output_file[256];

	ZeroMemory(&ofn, OPENFILENAME_SIZE_VERSION_400);
	ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = output_file;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = "Movie Studio project files\0*.vf\0VEGAS Pro project files\0*.veg\0All files\0*.*\0";
	ofn.nFilterIndex = (int)type+1;

	if (GetSaveFileName(&ofn) == 0) {
		return;
	}

	if (CopyFile((TCHAR*)input_file, (TCHAR*)output_file, 0) == 0) {
		MessageBox(hWnd, TEXT("Failed to copy original project file! Does the destination file already exist?"), TEXT("Saving project failed!"), MB_ICONEXCLAMATION);
                return;
	}
	FILE* output = fopen(output_file, "r+b");
	if (output == NULL) {
		MessageBox(hWnd, TEXT("Failed to save project file!"), TEXT("Saving project failed!"), MB_ICONEXCLAMATION); 
		return;
	}
	unsigned char magic_veg[] = {0xEF, 0x29, 0xC4, 0x46, 0x4A, 0x90, 0xD2, 0x11, 0x87, 0x22, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A};
	unsigned char magic_vf[] = {0xF6, 0x1B, 0x3C, 0x53, 0x35, 0xD6, 0xF3, 0x43, 0x8A, 0x90, 0x64, 0xB8, 0x87, 0x23, 0x1F, 0x7F};

	set_data(type == veg ? magic_veg : magic_vf, version, output);

	fclose(output);
}

void AddControls(HWND hWnd) {
	/* Versions */
	hWndComboBox = CreateWindow("ComboBox", NULL,
								 CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_VSCROLL,
								 (int)((225 - 50)/2), 30, 50, 200, 
								 hWnd, (HMENU)COMBOBOX, NULL, NULL); /**
																	  * I don't understand what half of 
																	  * these arguments are for, so chances
																	  * are that you don't either. 
																	 **/
	TCHAR versions[][10] = {TEXT("8"),  TEXT("9"),  TEXT("10"), 
							TEXT("11"), TEXT("12"), TEXT("13"), 
							TEXT("14"), TEXT("15"), TEXT("16"), 
							TEXT("17"), TEXT("18"), TEXT("19")};

	int i = 0;
	for (i = 0; i < ARRAYSIZE(versions); i++) {
		SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)versions[i]);
	}
	SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)3, (LPARAM)0);
	/* Open File */
	HWND open_button = CreateWindow("Button", "Open", WS_VISIBLE | WS_CHILD, (int)((225 - 50)/2), 5, 50, 20, hWnd, (HMENU)OPEN_FILE_BUTTON, NULL, NULL);
	/* Type */
	TCHAR listbox_items[][13] = {TEXT("VEGAS Pro"), TEXT("Movie Studio")};
	hWndListBox = CreateWindow("Listbox", NULL, WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY, (int)((225 - 100)/2), 55, 100, 40, hWnd, (HMENU)LISTBOX, NULL, NULL);
	for (i = 0; i < ARRAYSIZE(listbox_items); i++) {
		int pos = (int)SendMessage(hWndListBox, LB_ADDSTRING, i, (LPARAM) listbox_items[i]);
		SendMessage(hWndListBox, LB_SETITEMDATA, pos, (LPARAM) i);
	}
	SendMessage(hWndListBox, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	/* Save File */
	HWND save_button = CreateWindow("Button", "Save", WS_VISIBLE | WS_CHILD, (int)((225 - 50)/2), 90, 50, 20, hWnd, (HMENU)SAVE_FILE_BUTTON, NULL, NULL);
	/* Version and Type display */
	hWndVersion = CreateWindow("Edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_CENTER | ES_MULTILINE | SS_CENTER, (int)((225 - 150)/2), 120, 150, 40, hWnd, (HMENU)VERSION, NULL, NULL);
	if (open_button == NULL || save_button == NULL || hWndListBox == NULL || hWndComboBox == NULL)
		MessageBox(hWnd, TEXT("how did you even trigger this"), TEXT("GUI could not be initialized!"), MB_ICONEXCLAMATION); 
}

bool CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, font, true);
	return true;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch(msg) {
		case WM_COMMAND:
			if(HIWORD(wParam) == CBN_SELCHANGE) {
				if (LOWORD(wParam) == COMBOBOX)
					version = (int16_t)(8+SendMessage((HWND) lParam, (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0));
				if (LOWORD(wParam) == LISTBOX)
					type = SendMessage((HWND) lParam, (UINT) LB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);
			}
			switch(wParam) {
				case OPEN_FILE_BUTTON:
					file_name = open_file(hWnd);
				case COMBOBOX:
				case LISTBOX:
				case VERSION:
					break;
				case SAVE_FILE_BUTTON:
					save_file(hWnd, file_name);
			}
			break;
		case WM_CREATE: {
			AddControls(hWnd);
			EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int ncmdshow) {
	WNDCLASS wc = {0};

	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInstance;
	wc.lpszClassName = "msvpvf";
	wc.lpfnWndProc = WindowProcedure;

	if (!RegisterClass(&wc)) return -1;

	CreateWindow(TEXT("msvpvf"), TEXT("Movie Studio / Vegas Pro version spoofer"), WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU, 100, 100, 225, 200, NULL, NULL, hInstance, NULL);

	MSG msg = {0};

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
