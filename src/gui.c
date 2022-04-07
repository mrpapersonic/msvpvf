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
#define OPEN_FILE_BUTTON 0
#define COMBOBOX         1
#define LISTBOX          2
#define SAVE_FILE_BUTTON 3

HWND hWndListBox, hWndComboBox;
int16_t version = 11;
enum type {
	vf,
	veg
};
char* file_name = " ";

void set_data(unsigned char magic[], uint16_t version, FILE* target) {
	int i;
	fseek(target, 0x46, SEEK_SET);
	fputc(version, target);
	for (i=0; i<=sizeof(*magic); ++i) {
		fseek(target, 0x18+i, SEEK_SET);
		fputc(magic[i], target);
	}
}

int copy_file(char* source_file, char* target_file) {
	/* Copy a file */
	FILE *source, *target;

	source = fopen(source_file, "rb");
	if (source == NULL) return 1;
	target = fopen(target_file, "wb");
	if (target == NULL) {
		fclose(source);
		return 1;
	}

	size_t n, m;
	unsigned char buff[8192];
	do {
		n = fread(buff, 1, sizeof(buff), source);
		if (n) m = fwrite(buff, 1, n, target);
		else m = 0;
	} while ((n > 0) && (n == m));

	fclose(target);
	fclose(source);
	return 0;
}

void display_file(char* path) {
	/* Read the file to memory */
	FILE* file;
	file = fopen(path, "rb");
	fseek(file, 0, SEEK_END);
	int _size = ftell(file);
	rewind(file);
	char* data = calloc(_size+1, sizeof(char*));
	fread(data, _size, 1, file);
	data[_size] = '\0';

	free(data);
	fclose(file);
}

char* open_file(HWND hWnd) {
	OPENFILENAME ofn;
	char filename[256];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = filename;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = "Project files\0*.veg;*.vf\0All files\0*.*\0";
	ofn.nFilterIndex = 1;

	GetOpenFileName(&ofn);

	display_file(filename);

	return _strdup(filename);
}

void save_file(HWND hWnd, char* input_file) {
	if (strcmp(input_file, " ") == 0) {
		MessageBoxW(hWnd, 
					L"Please open a file first!", 
					L"Invalid input file!", 
					MB_ICONEXCLAMATION); 
		return;
	}
	OPENFILENAME ofn;
	char output_file[256];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = output_file;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = "Movie Studio project files\0*.vf\0VEGAS Pro project files\0*.veg\0All files\0*.*\0";
	ofn.nFilterIndex = type+1;

	GetSaveFileName(&ofn);

	copy_file(input_file, output_file);
	FILE* output = fopen(output_file, "r+b");
	if (output == NULL) {
		MessageBoxW(hWnd, L"Failed to save project file!", L"Saving project failed!", MB_ICONEXCLAMATION); 
		return;
	}

	switch (type) {
		case vf:
			unsigned char magic[] = {0xEF, 0x29, 0xC4, 0x46, 0x4A, 0x90, 0xD2, 0x11, 
									 0x87, 0x22, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A};
			set_data(magic, version, output);
			break;
		default:
			unsigned char magic[] = {0xF6, 0x1B, 0x3C, 0x53, 0x35, 0xD6, 0xF3, 0x43, 
									 0x8A, 0x90, 0x64, 0xB8, 0x87, 0x23, 0x1F, 0x7F};
			set_data(magic, version, output);
			break;
	}

	fclose(output);
}

void AddControls(HWND hWnd) {
	/* Versions */
	hWndComboBox = CreateWindowW(L"ComboBox", NULL,
								 CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_VSCROLL,
								 (int)((225 - 50)/2), 30, 50, 200, 
								 hWnd, (HMENU)COMBOBOX, NULL, NULL); /**
																	  * I don't understand what half of 
																	  * these arguments are for, so chances
																	  * are that you don't either. 
																	 **/
	TCHAR versions[][10] = {TEXT("8"), TEXT("9"), TEXT("10"), 
							TEXT("11"), TEXT("12"), TEXT("13"), 
							TEXT("14"), TEXT("15"), TEXT("16"), 
							TEXT("17"), TEXT("18"), TEXT("19")};

	TCHAR A[16];

	/**
	 * Here we can't just use a for loop 
	 * and cast all of those to `TEXT()`.
	 * Why? I don't know. My brain is too
	 * small to figure it out.
	**/
	memset(&A,0,sizeof(A));
	int i = 0;
	for (i = 0; i <= 11; i++) {
		wcscpy_s((WCHAR*)A, sizeof(A)/sizeof(TCHAR), (WCHAR*)versions[i]);
		SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
	}
	SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)3, (LPARAM)0);
	/* Open File */
	HWND open_button = CreateWindowW(L"Button", L"Open", WS_VISIBLE | WS_CHILD, (int)((225 - 50)/2), 5, 50, 20, hWnd, (HMENU)OPEN_FILE_BUTTON, NULL, NULL);
	/* Type */
	TCHAR listbox_items[][13] = {TEXT("VEGAS Pro"), TEXT("Movie Studio")};
	hWndListBox = CreateWindowW(L"Listbox", NULL, WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY, (int)((225 - 100)/2), 55, 100, 40, hWnd, (HMENU)LISTBOX, NULL, NULL);
	for (i = 0; i < ARRAYSIZE(listbox_items); i++) {
		int pos = (int)SendMessage(hWndListBox, LB_ADDSTRING, i, (LPARAM) listbox_items[i]);
		SendMessage(hWndListBox, LB_SETITEMDATA, pos, (LPARAM) i);
	}
	SendMessage(hWndListBox, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	/* Save File */
	HWND save_button = CreateWindowW(L"Button", L"Save", WS_VISIBLE | WS_CHILD, (int)((225 - 50)/2), 90, 50, 20, hWnd, (HMENU)SAVE_FILE_BUTTON, NULL, NULL);
	if (open_button == NULL || save_button == NULL || hWndListBox == NULL || hWndComboBox == NULL)
		MessageBoxW(hWnd, L"how did you even trigger this", L"GUI could not be initialized!", MB_ICONEXCLAMATION); 
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
					break;
				case COMBOBOX:
					break; /* NOTE: remove these 4 lines if we don't end up doing anything with them */
				case LISTBOX:
					break;
				case SAVE_FILE_BUTTON:
					save_file(hWnd, file_name);
					break;
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
			return DefWindowProcW(hWnd, msg, wParam, lParam);
	}
	return false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int ncmdshow) {
	WNDCLASSW wc = {0};

	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInstance;
	wc.lpszClassName = L"msvpvf";
	wc.lpfnWndProc = WindowProcedure;

	if (!RegisterClassW(&wc)) return -1;

	CreateWindowW(L"msvpvf", L"Movie Studio / Vegas Pro version spoofer", WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU, 100, 100, 225, 200, NULL, NULL, hInstance, NULL);

	MSG msg = {0};

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
