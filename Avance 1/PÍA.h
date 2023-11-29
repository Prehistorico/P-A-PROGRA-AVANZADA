#include "resource.h"
#include "framework.h"
#include <fstream>
#include <CommDlg.h>
#include <WinUser.h >
#include <WinUser.h>
#include <iostream>

using namespace std;

LRESULT CALLBACK fVentanaIN(HWND, UINT, WPARAM, LPARAM); //FUNCION



int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, PSTR cmdLine, int cShow) {

	HWND hVentanaIN = CreateDialog(hInst, MAKEINTRESOURCE(ID_INGRESAR), NULL, fVentanaIN);
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	ShowWindow(hVentanaIN, cShow);

	while (GetMessage(&msg, hVentanaIN,NULL, NULL)) { //True de todos los mensajes
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK fVentanaIN(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

		/*
		    USAR EN LA VENTANA PRINCIPAL
			case WM_DESTROY: {
			cout << "Gracias por usar el programa" << endl;
			PostQuitMessage(100);
		}break;*/


	}
	return FALSE;
}