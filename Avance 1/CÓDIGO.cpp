#include "resource.h"
#include "framework.h"
#include <fstream>
#include <CommDlg.h>
#include <WinUser.h>
#include <iostream>
#include <tchar.h>
#include<conio.h>
#include <windows.h>

using namespace std;
#define MAX_LOADSTRING 100
 
HINSTANCE hInstGlob = 0;

//Estructuras
struct nodo_usuarios;
struct nodo_citas;

struct nodo_citas {
    int numc=0; //numero de cita (dado por el programa)
    char fecha[100];
    char hora[100];
    char nombrec[50]; //nombre del cliente
    char especie[20];
    char tel[10];
    char nombrem[15]; //nombre de la mascota
    char motivo[300]; //motivo de la consulta
    char costo[50];
    char estatus[15];
    nodo_citas* ant;
    nodo_citas* sig;
};

nodo_citas* inicioC, * auxC, * finC = NULL;
char Fecha[100], Hora[100], Cliente[50], Especie[20], Tel[10],Mascota[15],Motivo[300],Costo[50],estatus[5];
int numc=0;
nodo_citas* citas = new nodo_citas;


struct nodo_usuarios {
    int numu = 0;
    char nombreU[30];
    char cedula[10];
    char clave[10]; 
    char contra[10];
    char telU[10];
    char foto[MAX_PATH];
    nodo_usuarios* ant;
    nodo_usuarios* sig;
};
char Nombre[30], Ced[10], Clave[10], Contra[10], TelU[10];
int numu = 0;
nodo_usuarios* usuarios = new nodo_usuarios;
nodo_usuarios* inicioU = nullptr;
nodo_usuarios* auxU = nullptr;
nodo_usuarios* finU = nullptr;
HWND hEdit;


//Funciones
bool validcontraseña(const char* cadena);
bool validarNombre(const char* cadena);
void leer_archivo(char* archivo);
void escribir_archivo(char* archivo);
void eliminar_cita(nodo_citas* aux);
void agregar_cita(nodo_citas* nuevo);
void agregar_cuenta(nodo_usuarios* nuevo); //agregar nuevo login, nuevo doctor

nodo_citas* buscar_cita(int numc);
/*void Examinar(HWND hDlg, short tipo);
void IMAGEN(HWND hwnd, char file[], int Pic);
void agregar_doctor(nodo_citas* aux);*/

LRESULT CALLBACK resgistro(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK infodoctor(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Agenda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Login(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    HWND hwnd;
    MSG mensaje;
    //hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(ID_LOGIN), NULL, Login);
    hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(ID_REGISTRO), NULL, resgistro);
    hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(INFO_USUARIO), NULL, infodoctor);
    hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(ID_AGENDA), NULL, Agenda);
    hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(ID_LOGIN), NULL, Login);

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    while (TRUE == GetMessage(&mensaje, 0, 0, 0))
    {
        TranslateMessage(&mensaje);
        DispatchMessage(&mensaje);
    }
    return mensaje.wParam;
}
nodo_citas* temp = 0;
int idx = 0;
char s_num[10] = { 0 };

char contraseñaU;
char usuario;
char ContraU2[10], UsuarioLog[10];


LRESULT CALLBACK Login(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND: {
        if (LOWORD(wParam) == BTN_INICIO) {

            HWND HWuser = GetDlgItem(hwnd, CBB_USER);
            HWND HWpswrd = GetDlgItem(hwnd, CBB_PSWRD);

            GetDlgItemText(hwnd, CBB_USER, UsuarioLog, sizeof(Clave));
            strcpy_s(usuarios->clave, Clave);
            GetDlgItemText(hwnd, CBB_PSWRD, ContraU2, sizeof(Contra));
            strcpy_s(usuarios->contra, Contra);

            SetWindowText(GetDlgItem(hwnd, ST_CLAVE), Clave);
            SetWindowText(GetDlgItem(hwnd, ST_CONTRA), Contra);

            SetWindowText(HWuser, "");
            SetWindowText(HWpswrd, "");


            // Validar nombre de usuario y contraseña
            if (validarNombre(UsuarioLog) && validcontraseña(ContraU2)) {
                MessageBox(hwnd, "Usuario y contraseña correctos", "Aviso", MB_OK | MB_ICONINFORMATION);
                EndDialog(hwnd, 0);
                DialogBox(hInstGlob, MAKEINTRESOURCE(ID_AGENDA), NULL, Agenda);
            }
            else {
                MessageBox(hwnd, "Usuario o contraseña incorrectos", "ERROR", MB_OK | MB_ICONINFORMATION);
            }
        }
    } break;
    case WM_CLOSE: {
        DestroyWindow(hwnd);
        PostQuitMessage(0);
    } break;
    case WM_DESTROY:
        break;
    }
    return 0;
}


LRESULT CALLBACK Agenda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND HWfecha = GetDlgItem(hwnd, CBB_FECHA_AGENDA);
    HWND hfecha = GetDlgItem(hwnd, CBB_FECHA_REGISTRO);
    HWND hCombohora = GetDlgItem(hwnd, CBB_HORA_REGISTRO);
    HWND hComboespecie = GetDlgItem(hwnd, CBB_ESPECIE_AGENDA);
    HWND hComboestatus = GetDlgItem(hwnd, CBB_ESTATUS_AGENDA);
    nodo_citas cita_temp;
    switch (msg)
    {
    case WM_INITDIALOG: {
        auxC = inicioC;
        while (auxC!=NULL)           
        {
            SendMessage(GetDlgItem(hwnd, BOX_AGENDA), LB_ADDSTRING, 0, (LPARAM)auxC->nombrec);
            SendMessage(GetDlgItem(hwnd, BOX_AGENDA), LB_SETITEMDATA, idx, (LPARAM)auxC->numc);
            idx++;
            auxC = auxC->sig;
        }
    }break;
    case WM_COMMAND: {
        if (LOWORD(wParam) == BTN_VER)
        {
            idx = SendDlgItemMessage(hwnd, BOX_AGENDA, LB_GETCURSEL, 0, 0);
            SendDlgItemMessage(hwnd, BOX_AGENDA, LB_GETTEXT, idx, (LPARAM)Cliente);

            numc = SendDlgItemMessage(hwnd, BOX_AGENDA, LB_GETITEMDATA, idx, 0);
            _itoa(numc, s_num, 10);
            strcat(Cliente, " - ");
            strcat(Cliente, s_num);
            MessageBox(0, Cliente, "LISTA ALUMNOS", MB_ICONINFORMATION + MB_OK);

            strcpy(Cliente, "");
            temp = buscar_cita(numc);
            if (temp != 0)
            {
                SetWindowText(GetDlgItem(hwnd, CBB_NOMCLI_AGENDA), auxC->nombrec);
                SetWindowText(GetDlgItem(hwnd, CBB_CONTACT_AGENDA), auxC->tel);
                SetWindowText(GetDlgItem(hwnd, CBB_HORA_AGENDA), auxC->hora);
                SetWindowText(GetDlgItem(hwnd, CBB_ESPECIE_AGENDA), auxC->especie);
                SetWindowText(GetDlgItem(hwnd, CBB_MASCOT_AGENDA), auxC->nombrem);
                SetWindowText(GetDlgItem(hwnd, CBB_MOTIVO_AGENDA), auxC->motivo);
                SetWindowText(GetDlgItem(hwnd, CBB_COSTO_AGENDA), auxC->costo);
                SetWindowText(GetDlgItem(hwnd, CBB_ESTATUS_AGENDA), auxC->estatus);
                SetWindowText(HWfecha, auxC->fecha);

            }

          
        }
        if (LOWORD(wParam) == BTN_ELIMINAR)
        {
            SendDlgItemMessage(hwnd, CBB_NOMCLI_AGENDA, WM_SETTEXT, NULL, (LPARAM)"");
            SendDlgItemMessage(hwnd, CBB_CONTACT_AGENDA, WM_SETTEXT, NULL, (LPARAM)"");
            SendDlgItemMessage(hwnd, CBB_HORA_AGENDA, WM_SETTEXT, NULL, (LPARAM)"");
            SendDlgItemMessage(hwnd, CBB_ESPECIE_AGENDA, WM_SETTEXT, NULL, (LPARAM)"");
            SendDlgItemMessage(hwnd, CBB_MASCOT_AGENDA, WM_SETTEXT, NULL, (LPARAM)"");
            SendDlgItemMessage(hwnd, CBB_MOTIVO_AGENDA, WM_SETTEXT, NULL, (LPARAM)"");
            SendDlgItemMessage(hwnd, CBB_COSTO_AGENDA, WM_SETTEXT, NULL, (LPARAM)"");
            SendDlgItemMessage(hwnd, CBB_ESTATUS_AGENDA, WM_SETTEXT, NULL, (LPARAM)"");
            SendDlgItemMessage(hwnd, CBB_FECHA_AGENDA, WM_SETTEXT, NULL, (LPARAM)"");

            HWND clie = GetDlgItem(hwnd, BOX_AGENDA);
            char cclie[50];
            int indicec = (int)SendMessage(clie, LB_GETCURSEL, NULL, NULL);
            SendMessage(clie, LB_GETTEXT, (WPARAM)cclie, (LPARAM)indicec);

            if (citas == NULL) {
                MessageBox(NULL, "No hay productos en la lista", "SIN PRODUCTOS", MB_OK);
                break;
            }

            eliminar_cita(auxC);
            SendMessage(clie, LB_DELETESTRING, (WPARAM)cclie, 0);
        }
        if (LOWORD(wParam) == BTN_MOD)
        {
            idx = SendDlgItemMessage(hwnd, BOX_AGENDA, LB_GETCURSEL, 0, 0);
            SendDlgItemMessage(hwnd, BOX_AGENDA, LB_GETTEXT, idx, (LPARAM)Cliente);

            numc = SendDlgItemMessage(hwnd, BOX_AGENDA, LB_GETITEMDATA, idx, 0);
            _itoa(numc, s_num, 10);
            strcat(Cliente, " - ");
            strcat(Cliente, s_num);
            MessageBox(0, Cliente, "LISTA ALUMNOS", MB_ICONINFORMATION + MB_OK);

            strcpy(Cliente, "");
            temp = buscar_cita(numc);
            if (temp != 0)
            {
                SetWindowText(GetDlgItem(hwnd, CBB_NOMCLI_AGENDA), auxC->nombrec);
                SetWindowText(GetDlgItem(hwnd, CBB_CONTACT_AGENDA), auxC->tel);
                SetWindowText(GetDlgItem(hwnd, CBB_HORA_AGENDA), auxC->hora);
                SetWindowText(GetDlgItem(hwnd, CBB_ESPECIE_AGENDA), auxC->especie);
                SetWindowText(GetDlgItem(hwnd, CBB_MASCOT_AGENDA), auxC->nombrem);
                SetWindowText(GetDlgItem(hwnd,CBB_MOTIVO_AGENDA), auxC->motivo);
                SetWindowText(GetDlgItem(hwnd, CBB_COSTO_AGENDA), auxC->costo);
                SetWindowText(GetDlgItem(hwnd, CBB_ESTATUS_AGENDA), auxC->estatus);
                SetWindowText(HWfecha, auxC->fecha);

                EnableWindow(GetDlgItem(hwnd, CBB_NOMCLI_AGENDA), TRUE);
                EnableWindow(GetDlgItem(hwnd, CBB_CONTACT_AGENDA), TRUE);
                EnableWindow(GetDlgItem(hwnd, CBB_HORA_AGENDA), TRUE);
                EnableWindow(GetDlgItem(hwnd, CBB_ESPECIE_AGENDA), TRUE);
                EnableWindow(GetDlgItem(hwnd, CBB_MASCOT_AGENDA), TRUE);
                EnableWindow(GetDlgItem(hwnd, CBB_MOTIVO_AGENDA), TRUE);
                EnableWindow(GetDlgItem(hwnd, CBB_CONTACT_AGENDA), TRUE);
                EnableWindow(GetDlgItem(hwnd, CBB_COSTO_AGENDA), TRUE);
                EnableWindow(GetDlgItem(hwnd, CBB_FECHA_AGENDA), TRUE);
                EnableWindow(GetDlgItem(hwnd, CBB_ESTATUS_AGENDA), TRUE);
             
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"8:00 - 8:30");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"8:30 - 9:00");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"9:00 - 9:30");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"9:30 - 10:00");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"10:00 - 10:30");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"10:30 - 11:00");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"11:00 - 11:30");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"11:30 - 12:00");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"12:00 - 12:30");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"12:30 - 13:00");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"13:00 - 13:30");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"13:30 - 14:00");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"14:00 - 14:30");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"14:30 - 15:00");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"15:00 - 15:30");
                SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"15:30 - 16:00");
               
                SendMessage(hComboespecie, CB_SETCURSEL, (WPARAM)0, NULL);
                SendMessage(hComboespecie, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Perro");
                SendMessage(hComboespecie, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Gato");
                SendMessage(hComboespecie, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Conejo");
                SendMessage(hComboespecie, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Ave");
                SendMessage(hComboespecie, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Ruedor");
              
                SendMessage(hComboestatus, CB_SETCURSEL, (WPARAM)0, NULL);
                SendMessage(hComboestatus, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Activo");
                SendMessage(hComboestatus, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Cancelada");
                SendMessage(hComboestatus, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Completada");
                UpdateWindow(hwnd);

                ShowWindow(GetDlgItem(hwnd, ID_BOTON_GUARDARCambios), SW_SHOW);
            }

        }
        if (LOWORD(wParam) == ID_BOTON_GUARDARCambios) 
        {
            GetDlgItemText(hwnd, CBB_FECHA_AGENDA, Fecha, sizeof(Fecha));
            strcpy_s(citas->fecha, Fecha);
            GetDlgItemText(hwnd, CBB_HORA_AGENDA, Hora, sizeof(Hora));
            strcpy_s(citas->hora, Hora);
            GetDlgItemText(hwnd, CBB_NOMCLI_AGENDA, Cliente, sizeof(Cliente));
            strcpy_s(citas->nombrec, Cliente);
            GetDlgItemText(hwnd,CBB_ESPECIE_AGENDA, Especie, sizeof(Especie));
            strcpy_s(citas->especie, Especie);
            GetDlgItemText(hwnd, CBB_CONTACT_AGENDA, Tel, sizeof(Tel));
            strcpy_s(citas->tel, Tel);
            GetDlgItemText(hwnd, CBB_MASCOT_AGENDA, Mascota, sizeof(Mascota));
            strcpy_s(citas->nombrem, Mascota);
            GetDlgItemText(hwnd, CBB_MOTIVO_AGENDA, Motivo, sizeof(Motivo));
            strcpy_s(citas->motivo, Motivo);
            GetDlgItemText(hwnd, CBB_COSTO_AGENDA, Costo, sizeof(Costo));
            strcpy_s(citas->costo, Costo);
            GetDlgItemText(hwnd, CBB_ESTATUS_AGENDA, estatus, sizeof(estatus));
            strcpy_s(citas->estatus, estatus);
            agregar_cita(citas);
           
            cita_temp.numc = numc;
            citas->numc = cita_temp.numc;
            SetDlgItemInt(hwnd, ID_CONTADOR_AGENDA, numc, TRUE);

            MessageBox(hwnd, "Registro exitoso", "Aviso", MB_OK | MB_ICONINFORMATION);
            ShowWindow(GetDlgItem(hwnd, ID_BOTON_GUARDARCambios), SW_HIDE);

            EnableWindow(GetDlgItem(hwnd, CBB_NOMCLI_AGENDA), FALSE);
            EnableWindow(GetDlgItem(hwnd, CBB_CONTACT_AGENDA), FALSE);
            EnableWindow(GetDlgItem(hwnd, CBB_HORA_AGENDA), FALSE);
            EnableWindow(GetDlgItem(hwnd, CBB_ESPECIE_AGENDA), FALSE);
            EnableWindow(GetDlgItem(hwnd, CBB_MASCOT_AGENDA), FALSE);
            EnableWindow(GetDlgItem(hwnd, CBB_MOTIVO_AGENDA), FALSE);
            EnableWindow(GetDlgItem(hwnd, CBB_CONTACT_AGENDA), FALSE);
            EnableWindow(GetDlgItem(hwnd, CBB_COSTO_AGENDA), FALSE);
            EnableWindow(GetDlgItem(hwnd, CBB_FECHA_AGENDA), FALSE);
            EnableWindow(GetDlgItem(hwnd, CBB_ESTATUS_AGENDA), FALSE);
        }

        if (LOWORD(wParam) == ID_REGISTRO1)
        {
            EndDialog(hwnd, 0);
            DialogBox(hInstGlob, MAKEINTRESOURCE(ID_REGISTRO), NULL, resgistro);
        }
        if (LOWORD(wParam) == ID_CUENTA)
        {
            EndDialog(hwnd, 0);
            DialogBox(hInstGlob, MAKEINTRESOURCE(INFO_USUARIO), NULL, infodoctor);
        }
        if (LOWORD(wParam) == ID_SALIR)
        {
            int opc = MessageBox(0, "¿Desea salir?", "Advertencia", MB_OKCANCEL | MB_ICONINFORMATION);
            if (opc == IDOK)
            {
                DestroyWindow(hwnd);
                PostQuitMessage(0);
            }
        }
    }break;
    case WM_CLOSE: {
        DestroyWindow(hwnd);
        PostQuitMessage(0);
    }break;
    case WM_DESTROY:
        break;
    }
    return 0;
}

LRESULT CALLBACK resgistro(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND hfecha = GetDlgItem(hwnd, CBB_FECHA_REGISTRO);
    HWND HWnombreC = GetDlgItem(hwnd, CBB_NOMBRE_REGISTRO);
    HWND HWtelefono = GetDlgItem(hwnd, CBB_CONTACT_REGISTRO);
    HWND HWnombreM = GetDlgItem(hwnd, CBB_MASCOT_REGISTRO);
    HWND HWmotivo = GetDlgItem(hwnd, CBB_MOTIVO_REGISTRO);
    HWND HWcosto = GetDlgItem(hwnd, CBB_COSTO_REGISTRO);
    HWND hCombohora = GetDlgItem(hwnd, CBB_HORA_REGISTRO);
    HWND hComboespecie = GetDlgItem(hwnd, CBB_ESPECIE_REGISTRO);
    HWND hComboestatus = GetDlgItem(hwnd, PB_ESTATUS_REGISTRO);
    nodo_citas cita_temp;
    UNREFERENCED_PARAMETER(lParam);
    switch (msg)
    {
    case WM_INITDIALOG: {

    
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"8:00 - 8:30");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"8:30 - 9:00");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"9:00 - 9:30");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"9:30 - 10:00");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"10:00 - 10:30");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"10:30 - 11:00");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"11:00 - 11:30");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"11:30 - 12:00");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"12:00 - 12:30");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"12:30 - 13:00");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"13:00 - 13:30");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"13:30 - 14:00");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"14:00 - 14:30");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"14:30 - 15:00");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"15:00 - 15:30");
        SendMessage(hCombohora, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"15:30 - 16:00");
        
        SendMessage(hComboespecie, CB_SETCURSEL, (WPARAM)0, NULL);
        SendMessage(hComboespecie, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Perro");
        SendMessage(hComboespecie, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Gato");
        SendMessage(hComboespecie, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Conejo");
        SendMessage(hComboespecie, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Ave");
        SendMessage(hComboespecie, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Ruedor");
 
        SendMessage(hComboestatus, CB_SETCURSEL, (WPARAM)0, NULL);
        SendMessage(hComboestatus, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Activo");
        SendMessage(hComboestatus, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Cancelada");
        SendMessage(hComboestatus, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Completada");
        UpdateWindow(hwnd);

    }break;
    case WM_COMMAND: {
        if (LOWORD(wParam) == BTN_GUARDAR_REGISTRO) {
            BOOL nombre = FALSE;
            int j = 0;

            GetWindowText(hfecha, (LPTSTR)Fecha, 100);
            GetDlgItemText(hwnd, CBB_FECHA_REGISTRO, Fecha, sizeof(Fecha));
            strcpy_s(citas->fecha, Fecha);
            GetDlgItemText(hwnd, CBB_HORA_REGISTRO, Hora, sizeof(Hora));
            strcpy_s(citas->hora, Hora);
            GetDlgItemText(hwnd, CBB_NOMBRE_REGISTRO, Cliente, sizeof(Cliente));
            strcpy_s(citas->nombrec, Cliente);
            GetDlgItemText(hwnd, CBB_ESPECIE_REGISTRO, Especie, sizeof(Especie));
            strcpy_s(citas->especie, Especie);
            GetDlgItemText(hwnd, CBB_CONTACT_REGISTRO, Tel, sizeof(Tel));
            strcpy_s(citas->tel, Tel);
            GetDlgItemText(hwnd, CBB_MASCOT_REGISTRO, Mascota, sizeof(Mascota));
            strcpy_s(citas->nombrem, Mascota);
            GetDlgItemText(hwnd, CBB_MOTIVO_REGISTRO, Motivo, sizeof(Motivo));
            strcpy_s(citas->motivo, Motivo);
            GetDlgItemText(hwnd, CBB_COSTO_REGISTRO, Costo, sizeof(Costo));
            strcpy_s(citas->costo, Costo);
            GetDlgItemText(hwnd, PB_ESTATUS_REGISTRO, estatus, sizeof(estatus));
            strcpy_s(citas->estatus, estatus);
            agregar_cita(citas);
            numc = numc + 1;
            cita_temp.numc = numc;
            citas->numc = cita_temp.numc;
            SetDlgItemInt(hwnd, ID_EDIT_NUM_REGISTRO, numc, TRUE);
            MessageBox(hwnd, "Registro exitoso", "Aviso", MB_OK | MB_ICONINFORMATION);

         
            SetWindowText(HWnombreC, "");
            SetWindowText(HWtelefono, "");
            SetWindowText(HWnombreM, "");
            SetWindowText(HWmotivo, "");
            SetWindowText(HWcosto, "");
            SetWindowText(hCombohora, "");
            SetWindowText(hComboespecie, "");
            SetWindowText(hComboestatus, "");
            SetWindowText(GetDlgItem(hwnd, ID_EDIT_NUM_REGISTRO), "");
        }
   
        if (LOWORD(wParam) == ID_AGENDA1)
        {
            EndDialog(hwnd, 0);
            DialogBox(hInstGlob, MAKEINTRESOURCE(ID_AGENDA), NULL, Agenda);
        }
        if (LOWORD(wParam) == ID_CUENTA)
        {
            EndDialog(hwnd, 0);
            DialogBox(hInstGlob, MAKEINTRESOURCE(INFO_USUARIO), NULL, infodoctor);
        }
        if (LOWORD(wParam) == ID_SALIR)
        {
            int opc = MessageBox(0, "¿Desea salir?", "Advertencia", MB_OKCANCEL | MB_ICONINFORMATION);
            if (opc == IDOK)
            {
                DestroyWindow(hwnd);
                PostQuitMessage(0);
            }
        }
    }break;
    case WM_CLOSE: {
        DestroyWindow(hwnd);
        PostQuitMessage(0);
    }break;
    case WM_DESTROY: {
    }break;

    }
    return 0;
}

LRESULT CALLBACK infodoctor(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    
    HWND HWnombreU = GetDlgItem(hwnd, CBB_NOMBRE);
    HWND HWcedula = GetDlgItem(hwnd, CBB_CEDULA);
    HWND HWclave = GetDlgItem(hwnd, CBB_NOMB_US);
    HWND HWcontra = GetDlgItem(hwnd, CBB_CONTRA);
    HWND HWtelU = GetDlgItem(hwnd, CBB_CONTACT);
 
    nodo_citas usuario_temp;
    UNREFERENCED_PARAMETER(lParam);
    switch (msg)
    {
    case WM_INITDIALOG: {

        
        UpdateWindow(hwnd);

    }break;
    case WM_COMMAND: {
        if (LOWORD(wParam) == BTN_GUARDAR) {

            
            GetDlgItemText(hwnd, CBB_NOMBRE, Nombre, sizeof(Nombre));
            strcpy_s(usuarios->nombreU, Nombre);
            GetDlgItemText(hwnd, CBB_CEDULA, Ced, sizeof(Ced));
            strcpy_s(usuarios->cedula, Ced);
            GetDlgItemText(hwnd, CBB_NOMB_US, Clave, sizeof(Clave));
            strcpy_s(usuarios->clave, Clave);
            GetDlgItemText(hwnd, CBB_CONTRA, Contra, sizeof(Contra));
            strcpy_s(usuarios->contra, Contra);
            GetDlgItemText(hwnd, CBB_CONTACT, TelU, sizeof(TelU));
            strcpy_s(usuarios->telU, TelU);

            agregar_cuenta(usuarios); 

            MessageBox(hwnd, "Registro exitoso", "Aviso", MB_OK | MB_ICONINFORMATION);

            
            SetWindowText(GetDlgItem(hwnd, ST_NOMBRE), Nombre);
            SetWindowText(GetDlgItem(hwnd, ST_CEDULA), Ced);
            SetWindowText(GetDlgItem(hwnd, ST_CLAVE), Clave);
            SetWindowText(GetDlgItem(hwnd, ST_CONTRA), Contra);
            SetWindowText(GetDlgItem(hwnd, ST_TELEFONO), TelU);
            
            

            SetWindowText(HWnombreU,"");
            SetWindowText(HWcedula, "");
            SetWindowText(HWclave, "");
            SetWindowText(HWcontra, "");
            SetWindowText(HWtelU, "");
        }
       

        if (LOWORD(wParam) == BTN_EXAMINAR && HIWORD(wParam) == BN_CLICKED) {
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            char foto[MAX_PATH] = "";

            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = foto;
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = "txt";
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
            ofn.lpstrFilter = "Archivos de Imagen\0*.bmp;*.jpg;*.png\0Todos los Archivos\0*.*\0";
            if (GetOpenFileName(&ofn)) {
                HWND hPControl = GetDlgItem(hwnd, ID_FOTO);
                HBITMAP hImage = (HBITMAP)LoadImage(NULL, foto, IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
                SendMessage(hPControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
            }
            break;
        }

        if (LOWORD(wParam) == ID_AGENDA1)
        {
            EndDialog(hwnd, 0);
            DialogBox(hInstGlob, MAKEINTRESOURCE(ID_AGENDA), NULL, Agenda);
        }
        if (LOWORD(wParam) == ID_REGISTRO1)
        {
            EndDialog(hwnd, 0);
            DialogBox(hInstGlob, MAKEINTRESOURCE(ID_REGISTRO), NULL, resgistro);
        }
        if (LOWORD(wParam) == ID_SALIR)
        {
            int opc = MessageBox(0, "¿Desea salir?", "Advertencia", MB_OKCANCEL | MB_ICONINFORMATION);
            if (opc == IDOK)
            {
                DestroyWindow(hwnd);
                PostQuitMessage(0);
            }
        }
    }break;
    case WM_CLOSE: {
        DestroyWindow(hwnd);
        PostQuitMessage(0);
    }break;
    case WM_DESTROY: {

    }break;
    }
    return 0;
}

void  agregar_cita(nodo_citas* nuevo) {
    if (inicioC == NULL) {
        inicioC = new nodo_citas;
        auxC = inicioC;

        auxC->sig = NULL;
        auxC->ant = NULL;

        strcpy_s(auxC->fecha, nuevo->fecha);
        strcpy_s(auxC->hora, nuevo->hora);
        strcpy_s(auxC->nombrec, nuevo->nombrec);
        strcpy_s(auxC->especie, nuevo->especie);
        strcpy_s(auxC->tel, nuevo->tel);
        strcpy_s(auxC->nombrem, nuevo->nombrem);
        strcpy_s(auxC->motivo, nuevo->motivo);
        strcpy_s(auxC->costo, nuevo->costo);
        strcpy_s(auxC->estatus, nuevo->estatus);
 
        auxC->numc = nuevo->numc;

    }
    else {
        auxC = inicioC;
        while (auxC->sig != NULL) {
            auxC = auxC->sig;
        }
        auxC->sig = new nodo_citas;
        auxC->sig->sig = NULL;
        auxC->sig->ant = auxC;
        auxC = auxC->sig;
        finC = auxC;
        strcpy_s(auxC->fecha, nuevo->fecha);
        strcpy_s(auxC->hora, nuevo->hora);
        strcpy_s(auxC->nombrec, nuevo->nombrec);
        strcpy_s(auxC->especie, nuevo->especie);
        strcpy_s(auxC->tel, nuevo->tel);
        strcpy_s(auxC->nombrem, nuevo->nombrem);
        strcpy_s(auxC->motivo, nuevo->motivo);
        strcpy_s(auxC->costo, nuevo->costo);
        strcpy_s(auxC->estatus, nuevo->estatus);

        auxC->numc = nuevo->numc;
    }
}
void  agregar_cuenta(nodo_usuarios* nuevo) {
    if (inicioU == NULL) {
        inicioU = new nodo_usuarios;
        auxU = inicioU;

        auxC->sig = NULL;
        auxC->ant = NULL;

        strcpy_s(auxU->nombreU, nuevo->nombreU);
        strcpy_s(auxU->cedula, nuevo->cedula);
        strcpy_s(auxU->clave, nuevo->clave);
        strcpy_s(auxU->contra, nuevo->contra);
        strcpy_s(auxU->telU, nuevo->telU);
        //numc = 1;
        auxU->numu = nuevo->numu;

    }
    else {
        auxU = inicioU;
        while (auxC->sig != NULL) {
            auxU = auxU->sig;
        }
        auxU->sig = new nodo_usuarios;
        auxU->sig->sig = NULL;
        auxU->sig->ant = auxU;
        auxU = auxU->sig;
        finU = auxU;
        strcpy_s(auxU->nombreU, nuevo->nombreU);
        strcpy_s(auxU->cedula, nuevo->cedula);
        strcpy_s(auxU->clave, nuevo->clave);
        strcpy_s(auxU->contra, nuevo->contra);
        strcpy_s(auxU->telU, nuevo->telU);
        //numc = numc + 1;
        auxU->numu = nuevo->numu;
    }
}
nodo_citas* buscar_cita(int num) {
    nodo_citas* cli = 0;
    auxC = 0;
    auxC = inicioC;
    while (auxC != 0)
    {
        if (auxC->numc == num) {
            cli = auxC;
            break;
        }
        auxC = auxC->sig;
    }
    return cli;
}
void eliminar_cita(nodo_citas* auxC) {
    if (auxC == inicioC) {
        inicioC = auxC->sig;
        inicioC->ant = 0;
        delete auxC;
    }
    else {
        if (auxC == finC)
        {
            finC = auxC->ant;
            finC->sig = 0;
            delete auxC;
        }
        else
        {
            nodo_citas* prev = auxC->ant;
            nodo_citas* next = auxC->sig;
            prev->sig = auxC->sig;
            if (next != 0) {
                next->ant = auxC->ant;
            }
            delete auxC;
        }
    }



}
void leer_archivo(char* archivo) {
    ifstream arch_alumnos;
    arch_alumnos.open(archivo, ios::binary);
    if (arch_alumnos.is_open())
    {
        citas = new nodo_citas;
        arch_alumnos.read((char*)citas, sizeof(citas));
        while (!arch_alumnos.eof())
        {
            auxC = new nodo_citas;
            auxC = citas;

            agregar_cita(auxC);

            citas = new nodo_citas;
            arch_alumnos.read((char*)citas, sizeof(nodo_citas));
        }
        arch_alumnos.close();

    }
}
  
void escribir_archivo(char* archivo) {
    ofstream archi("Agenda_Citas.dat", ios::binary);
    archi.open(archivo, ios::trunc | ios::binary);
    if (!archivo) {

        exit(1);
    }
    if (archi.is_open())
    {
        auxC = inicioC;
        while (auxC != 0)
        {
            archi.write((char*)auxC, sizeof(nodo_citas));
            auxC = auxC->sig;
        }
        archi.close();

    }
}
bool validarNombre(const char* cadena) {
    int longitud = strlen(cadena);

    if (longitud < 5 || longitud > 50) {
        return false;
    }

    for (int i = 0; i < longitud; i++) {
        char caracter = cadena[i];

        if (!isalnum(caracter)) {
            return false;
        }
    }
}

bool validcontraseña(const char* cadena) {
    int longitud = strlen(cadena);


    if (longitud < 1 || longitud > 10) {
        return false;
    }

    bool contieneLetra = false;
    bool contieneNumero = false;
    bool contieneCaracterEspecial = false;

    for (int i = 0; i < longitud; i++) {
        char caracter = cadena[i];

 
        if (isalpha(caracter)) {
            contieneLetra = true;
        }

        else if (isdigit(caracter)) {
            contieneNumero = true;
        }

        else if (!isalnum(caracter)) {
            contieneCaracterEspecial = true;
        }
    }

    if (contieneLetra && contieneNumero && contieneCaracterEspecial) {
        return true;
    }
    else {
        return false;
    }

}
bool validtelefono(const char* cadena) {

    if (strlen(cadena) != 10) {
        return false;
    }


    for (int i = 0; i < 10; i++) {
        if (!isdigit(cadena[i])) {
            return false;
        }
    }

    return true;
}
