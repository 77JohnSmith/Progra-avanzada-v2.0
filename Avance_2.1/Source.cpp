#include <windows.h>
#include "resource.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <Commctrl.h>
#include <CommDlg.h>
using namespace std;

struct usuario {
	usuario* sig;
	usuario* ant;
	char nomUsuario[25];
	char contraUsuario[20];
	char nomVendedor[50];
	char nomEmpresa[20];
	//foto
};

struct usuarioLogin {
	usuarioLogin* sig;
	char nomUsuario[25];
	char contraUsuario[20];
};

struct infoVendedor {
	char nomVendedor[50];
	char nomEmpresa[20];
	//foto
};

struct producto {
	producto* sig;
	producto* ant;
	char usVendedor[25];
	char nomProducto[50];
	int cantInventario;
	int codigoProd; 
	char marcaProducto[50];
	char descripcionProducto[100];
	int precioProducto;
	//imagen
};

struct fecha {
	char dia[3];
	char mes[3];
	char year[5];
};

struct envio {
	envio* ant;
	envio* sig;
	fecha diaEnvio;
	char usVendedor[25];
    char calleUs[50];
	char colUs[50];
	char ciudadUs[50];
	char estadoUs[50];
	char nomProducto[50];
	char mensaje[50];
	int cantComprado;
	char estatus[20];
	int precioFinal;
};

usuario* iniSes, * auxSes, * userSes = nullptr;
producto* iniProd, * auxProd = nullptr;
envio* iniEnvio, * auxEnvio, * finEnvio = nullptr;
usuarioLogin* iniLogin, * auxLogin = nullptr;

HINSTANCE hinstGlobal;
HWND hVInicio;
HWND hProductos;
HWND hEnvios;
SYSTEMTIME sysTm;
HBITMAP bmp;
WNDPROC lpEditWndProc;

BOOL CALLBACK vInicioSesion(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
BOOL CALLBACK vRegistro(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
BOOL CALLBACK vInicio(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
BOOL CALLBACK vInfoVend(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
BOOL CALLBACK vAgregar(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
BOOL CALLBACK vEliminar(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
BOOL CALLBACK vEditarProd(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
BOOL CALLBACK vMisProd(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
BOOL CALLBACK vCompra(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
BOOL CALLBACK vCancelacion(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
BOOL CALLBACK vEditarEnvio(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);

void registroUs(char nomUs[25],char contraUs[20]);
void infoVend(char nomVendedor[50], char nomEmpresa[50]);
void escribirSes();
void leerSes();

void listaUsLogin(char usLogin[25]);

void agregarProd(producto* newProd);
void eliminarProd(producto* auxProd);
void editarProd(producto* editProd, char nomProd[50]);
void escribirProd();
void leerProd();

void nuevoEnvio(envio* newEnvio);
void eliminarEnvio(envio* auxEnvio);
void modificarEnvio(envio* editEnvio, char nomProd[50]);

void escribirEnvio();
void leerEnvio();

void quickSort(envio* iniL, envio* finL);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
	
	hinstGlobal = hInstance;
	leerSes();
	leerProd();
	leerEnvio();
	auxSes = iniSes;

	DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_InicioSesion), 0, vInicioSesion);

	MSG Mensaje;

	ZeroMemory(&Mensaje, sizeof(Mensaje));

	while (GetMessage(&Mensaje, 0, 0, 0)) {
		TranslateMessage(&Mensaje);
		DispatchMessage(&Mensaje);
	}

	return Mensaje.wParam;
}

BOOL CALLBACK vInicioSesion(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) {

	switch (msg) {

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDC_BUT1_Regis: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Registro), 0, vRegistro);
		}
		case IDOK1_InSes: {

			char nomUs[25];
			char contraUs[20];
			GetDlgItemText(hwnd, IDC_EDIT1_Us, nomUs, 25);
			GetDlgItemText(hwnd, IDC_EDIT1_Cont, contraUs, 20);

			auxSes = iniSes;
			if (iniSes == nullptr) {
				MessageBox(hwnd, "No hay usuarios registrados. Cree una cuenta primero.", "AVISO", MB_OK | MB_ICONWARNING);
			}
			else {
				while (auxSes != nullptr && strcmp(nomUs, auxSes->nomUsuario)) {
					auxSes = auxSes->sig;
				}

				if (auxSes == nullptr) {
					MessageBox(hwnd, "Usuario no encontrado.", "AVISO", MB_OK | MB_ICONWARNING);
				}
				else {
					userSes = auxSes;
					listaUsLogin(userSes->nomUsuario);
					hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
					ShowWindow(hVInicio, SW_SHOW);
					EndDialog(hwnd, 0);
				}
			}
		}break;
		}break;
	}break;

	case WM_CLOSE: {
		int opc = MessageBox(hwnd, "¿Seguro que desea salir?", "AVISO", MB_YESNO | MB_ICONQUESTION);
		switch (opc) {
		case IDYES:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
	}break;
	}
	return 0;
}

BOOL CALLBACK vRegistro(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) {

	switch (msg) {

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDOK2_Registrarse: {

			char nomUs[25];
			char contraUs[20];
			GetDlgItemText(hwnd, IDC_EDIT2_Us, nomUs, 25);
			GetDlgItemText(hwnd, IDC_EDIT2_Cont, contraUs, 20);

			registroUs(nomUs,contraUs);
			userSes = auxSes;
			escribirSes();
			MessageBox(hwnd, "Registrado con exito", "AVISO", MB_OK | MB_ICONINFORMATION);

			// buscar que no se repita con algun nombre en Usuarios.bin;
			// 
			// if (se repite) {
			// cout << Nombre de usuario existente. Intente uno nuevo; 
			// EndDialog(hwnd, 0);
			// DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Registro), 0, vRegistro);}
			// 
			// else {
			// verficar que sean solo letras;
			//		if (no son letras){
			//		cout << ingrese letras unicamente;}
			//		else {Guardar binario;}}
			// 


			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_InfoVendedor), 0, vInfoVend);
		}break;
		case IDCANCEL_2: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_InicioSesion), 0, vInicioSesion);
		}break;
		}
	}break;

	case WM_CLOSE: {
		int opc = MessageBox(0, "¿Seguro que desea salir?", "AVISO", MB_YESNO | MB_ICONQUESTION);
		switch (opc) {
		case IDYES:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
	}break;
	}
	return 0;
}

BOOL CALLBACK vInicio(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam){

	SYSTEMTIME fechaHoy;
	ZeroMemory(&fechaHoy, sizeof(fechaHoy));
	GetSystemTime(&fechaHoy);
	
	switch (msg) {

	case WM_INITDIALOG: {
		
		finEnvio = auxEnvio;
		quickSort(iniEnvio, finEnvio);
		auxEnvio = iniEnvio;
		while (auxEnvio != nullptr) {
			if (strcmp(auxEnvio->usVendedor, userSes->nomUsuario) == 0) {
				int dia = atoi(auxEnvio->diaEnvio.dia);
				int mes = atoi(auxEnvio->diaEnvio.mes);
				int anio = atoi(auxEnvio->diaEnvio.year);
				SendMessage(GetDlgItem(hwnd, IDC_LIST11_ME), LB_ADDSTRING, 0, (LPARAM)auxEnvio->nomProducto);
			}
			auxEnvio = auxEnvio->sig;
		}
	}break;

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		
		case ID_PERFIL_INFORMACIONDELVENDEDOR: {
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_InfoVendedor), 0, vInfoVend);
		}break;
		case ID_PRODUCTOS_AGREGARNUEVOPRODUCTO: {
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ProductoNuevo), 0, vAgregar);
		}break;
		case ID_PRODUCTOS_ELIMINARUNPRODUCTO: {
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Eliminar), 0, vEliminar);
		}break;
		case ID_PRODUCTOS_EDITARUNPRODUCTO: {
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditarProd), 0, vEditarProd);
		}break;
		case ID_PRODUCTOS_MISPRODUCTOS: {
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_MisProductos), 0, vMisProd);
		}break;
		case ID_ENVIOS_COMPRARPRODUCTOS: {
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ComProd), 0, vCompra);
		}break;
		case ID_ENVIOS_CANCELARUNENVIO: {
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Cancelacion), 0, vCancelacion);
		}break;
		case ID_ENVIOS_EDITARUNENVIO: {
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditEnv), 0, vEditarEnvio);
		}break;
		case IDC_LIST11_ME: {
			switch (HIWORD(wParam)) {
			case  LBN_DBLCLK: {
				char list[40] = { 0 };
				int indice = 0;
				char escFecha[15] = { 0 };

				indice = SendDlgItemMessage(hwnd, IDC_LIST11_ME, LB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hwnd, IDC_LIST11_ME, LB_GETTEXT, indice, (LPARAM)list);

				auxEnvio = iniEnvio;
				while (auxEnvio != nullptr && strcmp(list, auxEnvio->nomProducto) != 0) {
					auxEnvio = auxEnvio->sig;
				}
				SetDlgItemText(hwnd, 1200, auxEnvio->nomProducto);
				SetDlgItemInt(hwnd, 1201, (UINT)auxEnvio->cantComprado, FALSE);
				SetDlgItemText(hwnd, IDC_EDIT11_Calle, auxEnvio->calleUs);
				SetDlgItemText(hwnd, IDC_EDIT11_Col, auxEnvio->colUs);
				SetDlgItemText(hwnd, IDC_EDIT11_Ciud, auxEnvio->ciudadUs);
				SetDlgItemText(hwnd, IDC_EDIT11_ESt, auxEnvio->estadoUs);
				SetDlgItemText(hwnd, IDC_EDIT11_Status, auxEnvio->estatus);

				strcat_s(escFecha, auxEnvio->diaEnvio.dia);
				strcat_s(escFecha, "/");
				strcat_s(escFecha, auxEnvio->diaEnvio.mes);
				strcat_s(escFecha, "/");
				strcat_s(escFecha, auxEnvio->diaEnvio.year);
				SetDlgItemText(hwnd, IDC_EDIT11_FEcha, escFecha);

				int dia = atoi(auxEnvio->diaEnvio.dia);
				int mes = atoi(auxEnvio->diaEnvio.mes);
				int year = atoi(auxEnvio->diaEnvio.year);

				if (fechaHoy.wDay <= dia) {
					if (fechaHoy.wMonth >= mes) {
						if (fechaHoy.wYear <= year) {
							SetDlgItemText(hwnd, IDC_EDIT11_Status, "ENVIO PENDIENTE");
						}
						else {
							SetDlgItemText(hwnd, IDC_EDIT11_Status, "ENTREGADO");
						}
					}
					else{
						SetDlgItemText(hwnd, IDC_EDIT11_Status, "ENVIO PENDIENTE");
					}
					
				}
				else {
					if (fechaHoy.wMonth <= mes) {
						if (fechaHoy.wYear > year) {
							SetDlgItemText(hwnd, IDC_EDIT11_Status, "ENTREGADO");
						}
						else {
							SetDlgItemText(hwnd, IDC_EDIT11_Status, "ENVIO PENDIENTE");
						}
					}
					else {
						if (fechaHoy.wYear < year) {
							SetDlgItemText(hwnd, IDC_EDIT11_Status, "ENVIO PENDIENTE");
						}
						else {
							SetDlgItemText(hwnd, IDC_EDIT11_Status, "ENTREGADO");
						}
					}
				}
			}break;
			}break;
		}break;

		case ID_SALIR_SALIRDELPROGRAMA: {
			DestroyWindow(hwnd);
			PostQuitMessage(0);
		}break;
		}
		break;
	}

	case WM_CLOSE: {
		int opc = MessageBox(0, "¿Seguro que desea salir?", "AVISO", MB_YESNO | MB_ICONQUESTION);
		switch (opc) {
		case IDYES:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
	}break;
	}
	return 0;
}

BOOL CALLBACK vInfoVend(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG: {
		DestroyWindow(hVInicio);

		auxSes = iniSes;
			if (strcmp(auxSes->nomUsuario, userSes->nomUsuario) == 0) {
				SetDlgItemText(hwnd, IDC_EDIT3_Nombre, auxSes->nomVendedor);
				SetDlgItemText(hwnd, IDC_EDIT3_Alias, auxSes->nomEmpresa);
			}
	}break;

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_PERFIL_INFORMACIONDELVENDEDOR: {
			SetDlgItemText(hwnd, IDC_EDIT3_Nombre, auxSes->nomVendedor);
			SetDlgItemText(hwnd, IDC_EDIT3_Alias, auxSes->nomEmpresa);
		}break;
		case ID_PRODUCTOS_AGREGARNUEVOPRODUCTO: {
			//if (infoUs es =! null) {
			//
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ProductoNuevo), 0, vAgregar);
		}break;
		case ID_PRODUCTOS_ELIMINARUNPRODUCTO: {
			//if (infoUs es =! null) {
			//
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Eliminar), 0, vEliminar);
		}break;
		case ID_PRODUCTOS_EDITARUNPRODUCTO: {
			//if (infoUs es =! null) {
			//
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditarProd), 0, vEditarProd);
		}break;
		case ID_PRODUCTOS_MISPRODUCTOS: {
			//if (infoUs es =! null) {
			//
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_MisProductos), 0, vMisProd);
		}break;
		case ID_ENVIOS_COMPRARPRODUCTOS: {
			//if (infoUs es =! null) {
			//
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ComProd), 0, vCompra);
		}break;
		case ID_ENVIOS_CANCELARUNENVIO: {
			//if (infoUs es =! null) {
			//
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Cancelacion), 0, vCancelacion);
		}break;
		case ID_ENVIOS_EDITARUNENVIO: {
			//if (infoUs es =! null) {
			//
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditEnv), 0, vEditarEnvio);
		}break;
		case ID_ENVIOS_MISENVIOS: {
			//if (infoUs es =! null) {
			//
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		case IDOK3_Actualizar: {
			
			GetDlgItemText(hwnd, IDC_EDIT3_Nombre, auxSes->nomVendedor, 50);
			GetDlgItemText(hwnd, IDC_EDIT3_Alias, auxSes->nomEmpresa, 20);

			MessageBox(hProductos, "Información del usuario actualizado", "AVISO", MB_OK | MB_ICONINFORMATION);

			escribirSes();
		
			SetDlgItemText(hwnd, IDC_EDIT3_Nombre, auxSes->nomVendedor);
			SetDlgItemText(hwnd, IDC_EDIT3_Alias, auxSes->nomEmpresa);
		}break;

		case IDCANCEL_3: {
			//if (infoUs es =! null) {
			//
			EndDialog(hwnd, 0);
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
		}break;
		}
	}break;

	case WM_CLOSE: {
		int opc = MessageBox(0, "¿Seguro que desea salir?", "AVISO", MB_YESNO | MB_ICONQUESTION);
		switch (opc) {
		case IDYES:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
	}break;
	}

	return 0;
}

BOOL CALLBACK vAgregar(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG: {
		DestroyWindow(hVInicio);
	}break;

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_PERFIL_INFORMACIONDELVENDEDOR: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_InfoVendedor), 0, vInfoVend);
		}break;		
		case ID_PRODUCTOS_ELIMINARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Eliminar), 0, vEliminar);
		}break;
		case ID_PRODUCTOS_EDITARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditarProd), 0, vEditarProd);
		}break;
		case ID_PRODUCTOS_MISPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_MisProductos), 0, vMisProd);
		}break;
		case ID_ENVIOS_COMPRARPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ComProd), 0, vCompra);
		}break;
		case ID_ENVIOS_CANCELARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Cancelacion), 0, vCancelacion);
		}break;
		case ID_ENVIOS_EDITARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditEnv), 0, vEditarEnvio);
		}break;
		case ID_ENVIOS_MISENVIOS: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;

		case IDOK4_Agregar: {
			producto* newProd = new producto;
			GetDlgItemText(hwnd, IDC_EDIT4_Nombre, newProd->nomProducto, 50);
			GetDlgItemText(hwnd, IDC_EDIT4_Marca, newProd->marcaProducto, 50);
			GetDlgItemText(hwnd, IDC_EDIT4_Dscrp, newProd->descripcionProducto, 50);
			newProd->codigoProd = GetDlgItemInt(hwnd, IDC_EDIT4_Cod, NULL, FALSE);
			newProd->cantInventario = GetDlgItemInt(hwnd, IDC_EDIT4_Cant, NULL, FALSE);
			newProd->precioProducto = GetDlgItemInt(hwnd, IDC_EDIT4_Precio, NULL, FALSE);
			strcpy_s(newProd->usVendedor, userSes->nomUsuario);

			agregarProd(newProd);
			escribirProd();
			MessageBox(hwnd, "Producto agregado a la tienda.", "AVISO", MB_OK | MB_ICONINFORMATION);

			SetWindowText(GetDlgItem(hwnd, IDC_EDIT4_Nombre), "");
			SetWindowText(GetDlgItem(hwnd, IDC_EDIT4_Marca), "");
			SetWindowText(GetDlgItem(hwnd, IDC_EDIT4_Cod), "");
			SetWindowText(GetDlgItem(hwnd, IDC_EDIT4_Dscrp), "");
			SetWindowText(GetDlgItem(hwnd, IDC_EDIT4_Cant), "");
			SetWindowText(GetDlgItem(hwnd, IDC_EDIT4_Precio), "");

			delete newProd;
		}break;

		case IDCANCEL_4: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		}
	}break;

	case WM_CLOSE: {
		int opc = MessageBox(0, "¿Seguro que desea salir?", "AVISO", MB_YESNO | MB_ICONQUESTION);
		switch (opc) {
		case IDYES:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
	}break;
	}
	return 0;
}

BOOL CALLBACK vEliminar(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG: {
		DestroyWindow(hVInicio);
	}break;

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
			
		case ID_PERFIL_INFORMACIONDELVENDEDOR: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_InfoVendedor), 0, vInfoVend);
		}break;
		case ID_PRODUCTOS_AGREGARNUEVOPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ProductoNuevo), 0, vAgregar);
		}break;
		case ID_PRODUCTOS_EDITARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditarProd), 0, vEditarProd);
		}break;
		case ID_PRODUCTOS_MISPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_MisProductos), 0, vMisProd);
		}break;
		case ID_ENVIOS_COMPRARPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ComProd), 0, vCompra);
		}break;
		case ID_ENVIOS_CANCELARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Cancelacion), 0, vCancelacion);
		}break;
		case ID_ENVIOS_EDITARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditEnv), 0, vEditarEnvio);
		}break;
		case ID_ENVIOS_MISENVIOS: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		case IDC_BUT5_Buscar: {
			char buscarProd[50];
			GetDlgItemText(hwnd, IDC_EDIT5_Buscador, buscarProd, 50);

			auxProd = iniProd;
			while (auxProd != nullptr && strcmp(buscarProd, auxProd->nomProducto) != 0) {
				auxProd = auxProd->sig;
			}
			if (auxProd == nullptr) {
				MessageBox(0, "No hay productos en su inventario.", "AVISO", MB_OK | MB_ICONERROR);
			}
			else {
				if (strcmp(userSes->nomUsuario, auxProd->usVendedor) == 0) {
					SetDlgItemText(hwnd, IDC_EDIT5_Nom, auxProd->nomProducto);
					SetDlgItemText(hwnd, IDC_EDIT5_Marca, auxProd->marcaProducto);
					SetDlgItemText(hwnd, IDC_EDIT5_Desc, auxProd->descripcionProducto);
					SetDlgItemInt(hwnd, IDC_EDIT5_Cod, auxProd->codigoProd, FALSE);
					SetDlgItemInt(hwnd, IDC_EDIT5_Cant, auxProd->cantInventario, FALSE);
					SetDlgItemInt(hwnd, IDC_EDIT5_Precio, auxProd->precioProducto, FALSE);
					EnableWindow(GetDlgItem(hwnd, IDOK5_ConfElim), TRUE);
				}
				else {
					MessageBox(0, "Producto no encontrado", "AVISO", MB_OK | MB_ICONERROR);
				}
			}
		} break;
		case IDOK5_ConfElim: {
			int opc = MessageBox(0, "Se eliminará el producto de su inventario. ¿Desea continuar?", "AVISO", MB_YESNO | MB_ICONQUESTION);
			if (opc == IDYES) {
				eliminarProd(auxProd);
				escribirProd();

				SetWindowText(GetDlgItem(hwnd, IDC_EDIT5_Buscador), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT5_Nom), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT5_Marca), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT5_Cod), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT5_Cant), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT5_Desc), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT5_Precio), "");
				EnableWindow(GetDlgItem(hwnd, IDOK5_ConfElim), FALSE);
			}
		}break;
		case IDCANCEL_5: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		}
	}break;

	case WM_CLOSE: {
		int opc = MessageBox(0, "¿Seguro que desea salir?", "AVISO", MB_YESNO | MB_ICONQUESTION);
		switch (opc) {
		case IDYES:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
	}break;
	}

	return 0;
}

BOOL CALLBACK vEditarProd(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG: {
		DestroyWindow(hVInicio);
	}break;

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_PERFIL_INFORMACIONDELVENDEDOR: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_InfoVendedor), 0, vInfoVend);
		}break;
		case ID_PRODUCTOS_AGREGARNUEVOPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ProductoNuevo), 0, vAgregar);
		}break;
		case ID_PRODUCTOS_ELIMINARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Eliminar), 0, vEliminar);
		}break;
		case ID_PRODUCTOS_MISPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_MisProductos), 0, vMisProd);
		}break;
		case ID_ENVIOS_COMPRARPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ComProd), 0, vCompra);
		}break;
		case ID_ENVIOS_CANCELARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Cancelacion), 0, vCancelacion);
		}break;
		case ID_ENVIOS_EDITARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditEnv), 0, vEditarEnvio);
		}break;
		case ID_ENVIOS_MISENVIOS: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		case IDC_BUT6_Buscar: {
			char buscarProd[50];
			GetDlgItemText(hwnd, IDC_EDIT6_Buscador, buscarProd, 50);
			auxProd = iniProd;

			while (auxProd != nullptr && strcmp(buscarProd, auxProd->nomProducto) != 0) {
				auxProd = auxProd->sig;
			}
			if (auxProd == nullptr) {
				MessageBox(0, "Producto no encontrado.", "AVISO", MB_OK | MB_ICONERROR);
			}
			else {
				if (strcmp(userSes->nomUsuario, auxProd->usVendedor) == 0) {
					
					SetDlgItemText(hwnd, IDC_EDIT6_Marca, auxProd->marcaProducto);
					SetDlgItemInt(hwnd, IDC_EDIT6_Cod, (UINT)auxProd->codigoProd, FALSE);
					SetDlgItemText(hwnd, IDC_EDIT6_Nom, auxProd->nomProducto);
					SetDlgItemText(hwnd, IDC_EDIT6_Desc, auxProd->descripcionProducto);
					SetDlgItemInt(hwnd, IDC_EDIT6_Cant, (UINT)auxProd->cantInventario, FALSE);
					SetDlgItemInt(hwnd, IDC_EDIT6_Precio, (UINT)auxProd->precioProducto, FALSE);
					EnableWindow(GetDlgItem(hwnd, IDOK6_ConfEd), TRUE);
				}
				else {
					MessageBox(0, "Producto no encontrado", "AVISO", MB_OK | MB_ICONERROR);
				}
			}
		}break;
		case IDOK6_ConfEd: {
			int opc = MessageBox(0, "La información de este producto se actualizará. ¿Desea continuar?", "AVISO", MB_YESNO | MB_ICONQUESTION);
			if (opc == IDYES) {

				GetDlgItemText(hwnd, IDC_EDIT6_Nom, auxProd->nomProducto, 50);
				GetDlgItemText(hwnd, IDC_EDIT6_Marca, auxProd->marcaProducto, 50);
				auxProd->codigoProd = GetDlgItemInt(hwnd, IDC_EDIT6_Cod, NULL, FALSE);

				MessageBox(hProductos, "Información del producto actualizado con exito.", "AVISO", MB_OK | MB_ICONINFORMATION);

				escribirProd();
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT6_Buscador), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT6_Nom), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT6_Marca), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT6_Cod), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT6_Desc), "");
				SetDlgItemInt(hwnd, IDC_EDIT6_Cant, (UINT)auxProd->cantInventario, FALSE);
				SetDlgItemInt(hwnd, IDC_EDIT6_Precio, (UINT)auxProd->precioProducto, FALSE);
				EnableWindow(GetDlgItem(hwnd, IDOK6_ConfEd), FALSE);
			}
		}break;
		case IDCANCEL_6: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		}
	}break;

	case WM_CLOSE: {
		int opc = MessageBox(0, "¿Seguro que desea salir?", "AVISO", MB_YESNO | MB_ICONQUESTION);
		switch (opc) {
		case IDYES:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
	}break;
	}

	return 0;
}

BOOL CALLBACK vMisProd(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) {

	hProductos = hwnd;
	switch (msg) {

	case WM_INITDIALOG: {
		DestroyWindow(hVInicio);

		auxProd = iniProd;
		while (auxProd != nullptr) {
			if (strcmp(auxProd->usVendedor, userSes->nomUsuario) == 0) {
				SendMessage(GetDlgItem(hwnd, IDC_LIST7_MisProd), LB_ADDSTRING, 0, (LPARAM)auxProd->nomProducto);
			}
			auxProd = auxProd->sig;
		}
	}break;
	
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_PERFIL_INFORMACIONDELVENDEDOR: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_InfoVendedor), 0, vInfoVend);
		}break;
		case ID_PRODUCTOS_AGREGARNUEVOPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ProductoNuevo), 0, vAgregar);
		}break;
		case ID_PRODUCTOS_ELIMINARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Eliminar), 0, vEliminar);
		}break;
		case ID_PRODUCTOS_EDITARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditarProd), 0, vEditarProd);
		}break;
		case ID_ENVIOS_COMPRARPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ComProd), 0, vCompra);
		}break;
		case ID_ENVIOS_CANCELARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Cancelacion), 0, vCancelacion);
		}break;
		case ID_ENVIOS_EDITARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditEnv), 0, vEditarEnvio);
		}break;
		case ID_ENVIOS_MISENVIOS: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		case IDC_LIST7_MisProd: {
			switch (HIWORD(wParam)) {
			case  LBN_DBLCLK: {
				char list[40] = { 0 };
				int indice = 0;
				indice = SendDlgItemMessage(hwnd, IDC_LIST7_MisProd, LB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hwnd, IDC_LIST7_MisProd, LB_GETTEXT, indice, (LPARAM)list);

				auxProd = iniProd;
				while (auxProd != nullptr && strcmp(list, auxProd->nomProducto) != 0) {
					auxProd = auxProd->sig;
				}
				SetDlgItemText(hwnd, IDC_EDIT7_Marca, auxProd->marcaProducto);
				SetDlgItemInt(hwnd, IDC_EDIT7_Cod, (UINT)auxProd->codigoProd, FALSE);
				SetDlgItemText(hwnd, IDC_EDIT7_Nom, auxProd->nomProducto);
				SetDlgItemText(hwnd, IDC_EDIT7_Desc, auxProd->descripcionProducto);
				SetDlgItemInt(hwnd, IDC_EDIT7_Cant, (UINT)auxProd->cantInventario, FALSE);
				SetDlgItemInt(hwnd, IDC_EDIT7_Precio, (UINT)auxProd->precioProducto, FALSE);
			}break;
			}
		}break;

		case IDCANCEL_7: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		}
	}break;

	case WM_CLOSE: {
		int opc = MessageBox(0, "¿Seguro que desea salir?", "AVISO", MB_YESNO | MB_ICONQUESTION);
		switch (opc) {
		case IDYES:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
	}break;
	}

	return 0;
}

BOOL CALLBACK vCompra(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG: {
		DestroyWindow(hVInicio);

		auxProd = iniProd;
		while (auxProd != nullptr) {
			SendMessage(GetDlgItem(hwnd, IDC_LIST8_Prod), LB_ADDSTRING, 0, (LPARAM)auxProd->nomProducto);
			auxProd = auxProd->sig;
		}
	}break;

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_PERFIL_INFORMACIONDELVENDEDOR: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_InfoVendedor), 0, vInfoVend);
		}break;
		case ID_PRODUCTOS_AGREGARNUEVOPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ProductoNuevo), 0, vAgregar);
		}break;
		case ID_PRODUCTOS_ELIMINARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Eliminar), 0, vEliminar);
		}break;
		case ID_PRODUCTOS_EDITARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditarProd), 0, vEditarProd);
		}break;
		case ID_PRODUCTOS_MISPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_MisProductos), 0, vMisProd);
		}break;
		case ID_ENVIOS_CANCELARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Cancelacion), 0, vCancelacion);
		}break;
		case ID_ENVIOS_EDITARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditEnv), 0, vEditarEnvio);
		}break;
		case ID_ENVIOS_MISENVIOS: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		case IDOK8_Cont: {
			envio* nuevoEnv = new envio;
			SYSTEMTIME diaEnvio;

			int indice;
			char nomProd[50];
			char cantidad [3];
			int precio;
			int precioF;

			GetDlgItemText(hwnd, IDC_EDIT8_Calle, nuevoEnv->calleUs, 50);
			GetDlgItemText(hwnd, IDC_EDIT8_Dire, nuevoEnv->colUs, 50);
			GetDlgItemText(hwnd, IDC_EDIT8_Ciud, nuevoEnv->ciudadUs, 50);
			GetDlgItemText(hwnd, IDC_EDIT8_Est, nuevoEnv->estadoUs, 50);
			GetDlgItemText(hwnd, IDC_EDIT8_Mens, nuevoEnv->mensaje, 50);
			nuevoEnv->cantComprado = GetDlgItemInt(hwnd, IDC_EDIT8_Cant, NULL, FALSE);

			//nuevoEnv->precioFinal = GetDlgItemInt(hwnd, IDC_EDIT8_Precio, NULL, FALSE);
			//GetDlgItemInt(hwnd, IDC_EDIT8_Cant, cantidad, NULL);
			//nuevoEnv->cantComprado* nuevoEnv->precioFinal = nuevoEnv->precioFinal;
			
			ZeroMemory(&diaEnvio, sizeof(diaEnvio));

			SendDlgItemMessage(hwnd, IDC_DATETIMEPICKER8, DTM_GETSYSTEMTIME, 0, (LPARAM)&diaEnvio);
			sprintf_s(nuevoEnv->diaEnvio.dia, "%d", diaEnvio.wDay);
			sprintf_s(nuevoEnv->diaEnvio.mes, "%d", diaEnvio.wMonth);
			sprintf_s(nuevoEnv->diaEnvio.year, "%d", diaEnvio.wYear);

			indice = SendDlgItemMessage(hwnd, IDC_LIST8_Prod, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hwnd, IDC_LIST8_Prod, LB_GETTEXT, indice, (LPARAM)nomProd);
			strcpy_s(nuevoEnv->nomProducto, nomProd);
			strcpy_s(nuevoEnv->usVendedor, userSes->nomUsuario);

			nuevoEnvio(nuevoEnv);
			escribirEnvio();

			MessageBox(hwnd, "Envio procesado.", "AVISO", MB_OK | MB_ICONINFORMATION);
			SetWindowText(GetDlgItem(hwnd, IDC_EDIT8_Ciud), "");
			delete nuevoEnv;
		}
		case IDCANCEL_8: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		}
	}break;

	case WM_CLOSE: {
		int opc = MessageBox(0, "¿Seguro que desea salir?", "AVISO", MB_YESNO | MB_ICONQUESTION);
		switch (opc) {
		case IDYES:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
	}break;
	}
	return 0;
}

BOOL CALLBACK vCancelacion(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) {

	SYSTEMTIME fechaHoy;
	ZeroMemory(&fechaHoy, sizeof(fechaHoy));
	GetSystemTime(&fechaHoy);

	switch (msg) {

	case WM_INITDIALOG: {
		DestroyWindow(hVInicio);
	}break;

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_PERFIL_INFORMACIONDELVENDEDOR: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_InfoVendedor), 0, vInfoVend);
		}break;
		case ID_PRODUCTOS_AGREGARNUEVOPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ProductoNuevo), 0, vAgregar);
		}break;
		case ID_PRODUCTOS_ELIMINARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Eliminar), 0, vEliminar);
		}break;
		case ID_PRODUCTOS_EDITARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditarProd), 0, vEditarProd);
		}break;
		case ID_PRODUCTOS_MISPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_MisProductos), 0, vMisProd);
		}break;
		case ID_ENVIOS_COMPRARPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ComProd), 0, vCompra);
		}break;
		case ID_ENVIOS_EDITARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditEnv), 0, vEditarEnvio);
		}break;
		case ID_ENVIOS_MISENVIOS: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		case IDC_BUT9_Buscar: {
			char buscarEnv[50];
			char escFecha[15] = { 0 };
			int status=0;

			GetDlgItemText(hwnd, IDC_EDIT9_Buscador, buscarEnv, 50);

			auxEnvio = iniEnvio;
			while (auxEnvio != nullptr && strcmp(buscarEnv, auxEnvio->nomProducto) != 0) {
				auxEnvio = auxEnvio->sig;
			}

			if (auxEnvio == nullptr) {
				MessageBox(0, "No se han hecho envios", "AVISO", MB_OK | MB_ICONERROR);
			}
			else {
				if (strcmp(userSes->nomUsuario, auxEnvio->usVendedor) == 0) {

					int dia = atoi(auxEnvio->diaEnvio.dia);
					int mes = atoi(auxEnvio->diaEnvio.mes);
					int year = atoi(auxEnvio->diaEnvio.year);

					//1 = pendiente, 2 = entregado
					if (fechaHoy.wDay <= dia) {
						if (fechaHoy.wMonth >= mes) {
							if (fechaHoy.wYear <= year) {
								status = 1;
							}
							else {
								status = 2;
							}
						}
						else {
							status = 1;
						}
					}
					else {
						if (fechaHoy.wMonth <= mes) {
							if (fechaHoy.wYear > year) {
								status = 2;
							}
							else {
								status = 1;
							}
						}
						else {
							if (fechaHoy.wYear < year) {
								status = 1;
							}
							else {
								status = 2;
							}
						}
					}

					if (status == 2) {
						MessageBox(0, "No se puede cancelar un envio que ya fue entregado.", "AVISO", MB_OK | MB_ICONERROR);
					}
					else {
						SetDlgItemText(hwnd, IDC_EDIT9_nom, auxEnvio->nomProducto);
						SetDlgItemText(hwnd, IDC_EDIT9_Calle, auxEnvio->calleUs);
						SetDlgItemText(hwnd, IDC_EDIT9_Col, auxEnvio->colUs);
						SetDlgItemText(hwnd, IDC_EDIT9_Ciud, auxEnvio->ciudadUs);
						SetDlgItemText(hwnd, IDC_EDIT9_Est, auxEnvio->estadoUs);
						SetDlgItemText(hwnd, IDC_EDIT9_Status, "ENVIO PENDIENTE");
						strcat_s(escFecha, auxEnvio->diaEnvio.dia);
						strcat_s(escFecha, "/");
						strcat_s(escFecha, auxEnvio->diaEnvio.mes);
						strcat_s(escFecha, "/");
						strcat_s(escFecha, auxEnvio->diaEnvio.year);
						SetDlgItemText(hwnd, IDC_EDIT9_Fecha, escFecha);
						SetDlgItemInt(hwnd, IDC_EDIT9_Prec, auxEnvio->precioFinal, FALSE);
						SetDlgItemInt(hwnd, IDC_EDIT9_cant, auxEnvio->cantComprado, FALSE);
						EnableWindow(GetDlgItem(hwnd, IDOK9_Cont), TRUE);
					}		
				}
				else {
					MessageBox(0, "No se encontró el envio.", "AVISO", MB_OK | MB_ICONERROR);
				}
			}
		} break;
		case IDOK9_Cont: {
			int opc = MessageBox(0, "Se eliminará el producto de su inventario. ¿Desea continuar?", "AVISO", MB_YESNO | MB_ICONQUESTION);
			if (opc == IDYES) {
				eliminarEnvio(auxEnvio);
				escribirEnvio();

				SetWindowText(GetDlgItem(hwnd, IDC_EDIT9_Buscador), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT9_nom), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT9_Calle), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT9_Col), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT9_Ciud), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT9_Est), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT9_Prec), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT9_cant), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT9_Fecha), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT9_Status), "");
				EnableWindow(GetDlgItem(hwnd, IDOK9_Cont), FALSE);
			}
		}break;

		case IDCANCEL_9: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		}
	}break;
	case WM_CLOSE: {
		int opc = MessageBox(0, "¿Seguro que desea salir?", "AVISO", MB_YESNO | MB_ICONQUESTION);
		switch (opc) {
		case IDYES:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
	}break;
	}

	return 0;
}

BOOL CALLBACK vEditarEnvio(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam) {
	
	SYSTEMTIME fechaHoy;
	ZeroMemory(&fechaHoy, sizeof(fechaHoy));
	GetSystemTime(&fechaHoy);

	switch (msg) {

	case WM_INITDIALOG: {
		DestroyWindow(hVInicio);
	}break;

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_PERFIL_INFORMACIONDELVENDEDOR: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_InfoVendedor), 0, vInfoVend);
		}break;
		case ID_PRODUCTOS_AGREGARNUEVOPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ProductoNuevo), 0, vAgregar);
		}break;
		case ID_PRODUCTOS_ELIMINARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Eliminar), 0, vEliminar);
		}break;
		case ID_PRODUCTOS_EDITARUNPRODUCTO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_EditarProd), 0, vEditarProd);
		}break;
		case ID_PRODUCTOS_MISPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_MisProductos), 0, vMisProd);
		}break;
		case ID_ENVIOS_COMPRARPRODUCTOS: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_ComProd), 0, vCompra);
		}break;
		case ID_ENVIOS_CANCELARUNENVIO: {
			EndDialog(hwnd, 0);
			DialogBox(hinstGlobal, MAKEINTRESOURCE(IDD_Cancelacion), 0, vCancelacion);
		}break;
		case ID_ENVIOS_MISENVIOS: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
			}break;
		case IDC_BUT10_Buscar: {
			char buscarEnv[50];
			char escFecha[15] = { 0 };
			int status = 0;
			GetDlgItemText(hwnd, IDC_EDIT10_Buscador, buscarEnv, 50);

			auxEnvio = iniEnvio;
			while (auxEnvio != nullptr && strcmp(buscarEnv, auxEnvio->nomProducto) != 0) {
				auxEnvio = auxEnvio->sig;
			}
			if (auxEnvio == nullptr) {
				MessageBox(0, "No se han hecho envios", "AVISO", MB_OK | MB_ICONERROR);
			}
			else {
				if (strcmp(userSes->nomUsuario, auxEnvio->usVendedor) == 0) {
					int dia = atoi(auxEnvio->diaEnvio.dia);
					int mes = atoi(auxEnvio->diaEnvio.mes);
					int year = atoi(auxEnvio->diaEnvio.year);

					//1 = pendiente, 2 = entregado
					if (fechaHoy.wDay <= dia) {
						if (fechaHoy.wMonth >= mes) {
							if (fechaHoy.wYear <= year) {
								status = 1;
							}
							else {
								status = 2;
							}
						}
						else {
							status = 1;
						}
					}
					else {
						if (fechaHoy.wMonth <= mes) {
							if (fechaHoy.wYear > year) {
								status = 2;
							}
							else {
								status = 1;
							}
						}
						else {
							if (fechaHoy.wYear < year) {
								status = 1;
							}
							else {
								status = 2;
							}
						}
					}

					if (status == 2) {
						MessageBox(0, "No se puede cancelar un envio que ya fue entregado.", "AVISO", MB_OK | MB_ICONERROR);
					}
					else {
						SetDlgItemText(hwnd, IDC_EDIT10_Nom, auxEnvio->nomProducto);
						SetDlgItemText(hwnd, IDC_EDIT10_Calle, auxEnvio->calleUs);
						SetDlgItemText(hwnd, IDC_EDIT10_Col, auxEnvio->colUs);
						SetDlgItemText(hwnd, IDC_EDIT10_Ciud, auxEnvio->ciudadUs);
						SetDlgItemText(hwnd, IDC_EDIT10_Est, auxEnvio->estadoUs);
						SetDlgItemText(hwnd, IDC_EDIT10_Status, "ENVIO PENDIENTE");
						strcat_s(escFecha, auxEnvio->diaEnvio.dia);
						strcat_s(escFecha, "/");
						strcat_s(escFecha, auxEnvio->diaEnvio.mes);
						strcat_s(escFecha, "/");
						strcat_s(escFecha, auxEnvio->diaEnvio.year);
						SetDlgItemText(hwnd, IDC_EDIT10_Fecha, escFecha);
						SetDlgItemInt(hwnd, IDC_EDIT10_Precio, auxEnvio->precioFinal, FALSE);
						SetDlgItemInt(hwnd, IDC_EDIT10_cant, auxEnvio->cantComprado, FALSE);
						EnableWindow(GetDlgItem(hwnd, IDOK10_Cont), TRUE);
					}

				}
				else {
					MessageBox(0, "No se encontró el envio.", "AVISO", MB_OK | MB_ICONERROR);
				}
			}
		} break;
		case IDOK10_Cont: {
			int opc = MessageBox(0, "La información de este producto se actualizará. ¿Desea continuar?", "AVISO", MB_YESNO | MB_ICONQUESTION);
			if (opc == IDYES) {

				GetDlgItemText(hwnd, IDC_EDIT10_Calle, auxEnvio->calleUs, 50);
				GetDlgItemText(hwnd, IDC_EDIT10_Col, auxEnvio->colUs, 50);
				GetDlgItemText(hwnd, IDC_EDIT10_Ciud, auxEnvio->ciudadUs, 50);
				GetDlgItemText(hwnd, IDC_EDIT10_Est, auxEnvio->estadoUs, 50);

				MessageBox(hProductos, "Información del producto actualizado con exito.", "AVISO", MB_OK | MB_ICONINFORMATION);

				escribirEnvio();
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT10_Buscador), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT10_Nom), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT10_Calle), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT10_Col), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT10_Ciud), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT10_Est), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT10_Precio), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT10_cant), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT10_Mens), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT10_Fecha), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT10_Status), "");
				EnableWindow(GetDlgItem(hwnd, IDOK10_Cont), FALSE);
			}
		}break;
		case IDCANCEL_10: {
			hVInicio = CreateDialog(hinstGlobal, MAKEINTRESOURCE(IDD_MisEnvios), NULL, vInicio);
			ShowWindow(hVInicio, SW_SHOW);
			EndDialog(hwnd, 0);
		}break;
		}
	}break;
	case WM_CLOSE: {
		int opc = MessageBox(0, "¿Seguro que desea salir?", "AVISO", MB_YESNO | MB_ICONQUESTION);
		switch (opc) {
		case IDYES:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
	}break;
	}

	return 0;
}

void leerSes() {
	ifstream leerSes;
	leerSes.open("Usuarios.bin", ios::in | ios::binary);

	auxSes = iniSes;
	if (leerSes.is_open()) {

		usuario* sesLeida = new usuario;

		while (!leerSes.read((char*)sesLeida, sizeof(usuario)).eof()) {

			while (auxSes != nullptr && auxSes->sig != nullptr) {
				auxSes = auxSes->sig;
			}

			if (auxSes == nullptr) {
				iniSes = sesLeida;
				iniSes->sig = nullptr;
				iniSes->ant = nullptr;
				auxSes = iniSes;
			}
			else {
				auxSes->sig = sesLeida;
				auxSes->sig->ant = auxSes;
				auxSes = auxSes->sig;
				auxSes->sig = nullptr;
			}
			sesLeida = new usuario;
		}
		leerSes.close();
	}
}

void registroUs(char nomUs[25],char contraUs[20]) {
	if (iniSes == nullptr) {
		iniSes = new usuario;
		auxSes = iniSes;

		auxSes->sig = nullptr;
		auxSes->ant = nullptr;

		strcpy_s(auxSes->nomUsuario, nomUs);
		strcpy_s(auxSes->contraUsuario, contraUs);

	}
	else {
		auxSes = iniSes;

		while (auxSes->sig != nullptr) {
			auxSes = auxSes->sig;
		}

		auxSes->sig = new usuario;
		auxSes->sig->sig = nullptr;
		auxSes->sig->ant = auxSes;
		auxSes = auxSes->sig;

		strcpy_s(auxSes->nomUsuario, nomUs);
		strcpy_s(auxSes->contraUsuario, contraUs);
	}
}

void leerProd() {
	ifstream leerProd;
	leerProd.open("Productos.bin", ios::in | ios::binary);

	auxProd = iniProd;
	if (leerProd.is_open()) {

		producto* prodLeido = new producto;

		while (!leerProd.read((char*)prodLeido, sizeof(producto)).eof()) {

			while (auxProd != nullptr && auxProd->sig != nullptr) {
				auxProd = auxProd->sig;
			}

			if (auxProd == nullptr) {
				iniProd = prodLeido;
				iniProd->sig = nullptr;
				iniProd->ant = nullptr;
				auxProd = iniProd;
			}
			else {
				auxProd->sig = prodLeido;
				auxProd->sig->ant = auxProd;
				auxProd = auxProd->sig;
				auxProd->sig = nullptr;
			}
			prodLeido = new producto;
		}
		leerProd.close();
	}
}

void leerEnvio() {
	ifstream leerEnvio;
	leerEnvio.open("Envios.bin", ios::in | ios::binary);
	auxEnvio = iniEnvio;

	if (leerEnvio.is_open()) {

		envio* envioLeido = new envio;


		while (!leerEnvio.read((char*)envioLeido, sizeof(envio)).eof()) {

			while (auxEnvio != nullptr && auxEnvio->sig != nullptr) {
				auxEnvio = auxEnvio->sig;
			}

			if (auxEnvio == nullptr) {
				iniEnvio = envioLeido;
				iniEnvio->sig = nullptr;
				iniEnvio->ant = nullptr;
				auxEnvio = iniEnvio;
			}
			else {
				auxEnvio->sig = envioLeido;
				auxEnvio->sig->ant = auxEnvio;
				auxEnvio = auxEnvio->sig;
				auxEnvio->sig = nullptr;
			}
			envioLeido = new envio;
		}
		leerEnvio.close();
	}
}

void listaUsLogin(char usLogin[25]) {

	if (iniLogin == nullptr) {
		iniLogin = new usuarioLogin;
		auxLogin = iniLogin;

		auxLogin->sig = nullptr;
		
		strcpy_s(auxLogin->nomUsuario, usLogin);
		strcpy_s(auxLogin->contraUsuario, usLogin);
	}
	else {
		auxLogin = iniLogin;

		while (auxLogin->sig != nullptr) {
			auxLogin = auxLogin->sig;
		}

		auxLogin->sig = new usuarioLogin;
		auxLogin = auxLogin->sig;
		auxLogin->sig = nullptr;

		strcpy_s(auxLogin->nomUsuario, usLogin);
		strcpy_s(auxLogin->contraUsuario, usLogin);
	}
}
void escribirSes() {
	auxSes = iniSes;

	ofstream escribirSes;
	escribirSes.open("Usuarios.bin", ios::out | ios::binary | ios::trunc);

	if (escribirSes.is_open()) {
		while (auxSes != nullptr)
		{
			escribirSes.write((char*)auxSes, sizeof(usuario));
			auxSes = auxSes->sig;
		}

		escribirSes.close();
	}
}

void agregarProd(producto* newProd) {
	if (iniProd == nullptr) {
		iniProd = new producto;
		auxProd = iniProd;

		auxProd->sig = nullptr;
		auxProd->ant = nullptr;

		strcpy_s(auxProd->nomProducto, newProd->nomProducto);
		strcpy_s(auxProd->marcaProducto, newProd->marcaProducto);
		auxProd->codigoProd = newProd->codigoProd;
		strcpy_s(auxProd->descripcionProducto, newProd->descripcionProducto);
		auxProd->cantInventario = newProd->cantInventario;
		auxProd->precioProducto = newProd->precioProducto;
		strcpy_s(auxProd->usVendedor, newProd->usVendedor);
	}
	else {
		auxProd = iniProd;

		while (auxProd->sig != nullptr){
			auxProd = auxProd->sig;
		}

		auxProd->sig = new producto;
		auxProd->sig->sig = nullptr;
		auxProd->sig->ant = auxProd;
		auxProd = auxProd->sig;

		strcpy_s(auxProd->nomProducto, newProd->nomProducto);
		strcpy_s(auxProd->marcaProducto, newProd->marcaProducto);
		auxProd->codigoProd = newProd->codigoProd;
		strcpy_s(auxProd->descripcionProducto, newProd->descripcionProducto);
		auxProd->cantInventario = newProd->cantInventario;
		auxProd->precioProducto = newProd->precioProducto;
		strcpy_s(auxProd->usVendedor, newProd->usVendedor);
	}
}
void escribirProd() {

	auxProd = iniProd;
	ofstream escribirProducto;
	escribirProducto.open("Productos.bin", ios::out | ios::binary | ios::trunc);

	if (escribirProducto.is_open()) {
		while (auxProd != nullptr)
		{
			escribirProducto.write((char*)auxProd, sizeof(producto));
			auxProd = auxProd->sig;
		}

		escribirProducto.close();
	}

}
void eliminarProd(producto* auxProd) {

	if (auxProd == iniProd) {
		if (auxProd->sig == nullptr) {
			delete auxProd;
			iniProd = nullptr;
			auxProd = iniProd;
		}
		else {

			iniProd = auxProd->sig;
			delete auxProd;
			auxProd = iniProd;
			auxProd->ant = nullptr;
		}
		MessageBox(hProductos, "El producto ha sido eliminado", "AVISO", MB_OK | MB_ICONINFORMATION);
}
	else {
		if (auxProd->sig == nullptr) { 
			auxProd->ant->sig = nullptr;
		}
		else {
			auxProd->sig->ant = auxProd->ant;
			auxProd->ant->sig = auxProd->sig;
		}
		delete auxProd;
		auxProd = iniProd;
		MessageBox(hProductos, "El producto ha sido eliminado", "AVISO", MB_OK | MB_ICONINFORMATION);
	}
}
void editarProd(producto* editProd, char nomProd[50]) {
	auxProd = iniProd;
	char newNom[50];

	if (auxProd == nullptr){
		MessageBox(hProductos, "No hay productos en su inventario.", "AVISO", MB_OK | MB_ICONINFORMATION);
	}
	else {
		while (auxProd != nullptr && strcmp(auxProd->nomProducto, nomProd) != 0) {
			auxProd = auxProd->sig;
		}
		if (auxProd == nullptr) {
			MessageBox(hProductos, "Producto no encontrado.", "AVISO", MB_OK | MB_ICONINFORMATION);
		}
		else {

			strcpy_s(auxProd->nomProducto, editProd->nomProducto);
			strcpy_s(auxProd->marcaProducto, editProd->marcaProducto);
			auxProd->codigoProd = editProd->codigoProd;
			MessageBox(hProductos, "El producto ha sido actualizado.", "AVISO", MB_OK | MB_ICONINFORMATION);
		}
	}
}

void nuevoEnvio(envio* newEnvio) {
	if (iniEnvio == nullptr) {
		iniEnvio = new envio;
		auxEnvio = iniEnvio;

		auxEnvio->sig = nullptr;
		auxEnvio->ant = nullptr;

		strcpy_s(auxEnvio->ciudadUs, newEnvio->ciudadUs);
		strcpy_s(auxEnvio->diaEnvio.dia, newEnvio->diaEnvio.dia);
		strcpy_s(auxEnvio->diaEnvio.mes, newEnvio->diaEnvio.mes);
		strcpy_s(auxEnvio->diaEnvio.year, newEnvio->diaEnvio.year);
		strcpy_s(auxEnvio->nomProducto, newEnvio->nomProducto);
		strcpy_s(auxEnvio->calleUs, newEnvio->calleUs);
		strcpy_s(auxEnvio->colUs, newEnvio->colUs);
		strcpy_s(auxEnvio->estadoUs, newEnvio->estadoUs);
		strcpy_s(auxEnvio->mensaje, newEnvio->mensaje);
		auxEnvio->cantComprado = newEnvio->cantComprado;
		//auxEnvio->precioFinal = newEnvio->precioFinal;
		strcpy_s(auxEnvio->usVendedor, newEnvio->usVendedor);

		finEnvio = iniEnvio;
	}
	else {
		auxEnvio = iniEnvio;

		while (auxEnvio->sig != nullptr){
			auxEnvio = auxEnvio->sig;
		}

		auxEnvio->sig = new envio;

		auxEnvio->sig->sig = nullptr;
		auxEnvio->sig->ant = auxEnvio;

		auxEnvio = auxEnvio->sig;

		strcpy_s(auxEnvio->ciudadUs, newEnvio->ciudadUs);
		strcpy_s(auxEnvio->diaEnvio.dia, newEnvio->diaEnvio.dia);
		strcpy_s(auxEnvio->diaEnvio.mes, newEnvio->diaEnvio.mes);
		strcpy_s(auxEnvio->diaEnvio.year, newEnvio->diaEnvio.year);
		strcpy_s(auxEnvio->nomProducto, newEnvio->nomProducto);
		strcpy_s(auxEnvio->calleUs, newEnvio->calleUs);
		strcpy_s(auxEnvio->colUs, newEnvio->colUs);
		strcpy_s(auxEnvio->estadoUs, newEnvio->estadoUs);
		strcpy_s(auxEnvio->mensaje, newEnvio->mensaje);
		auxEnvio->cantComprado = newEnvio->cantComprado;
		strcpy_s(auxEnvio->usVendedor, newEnvio->usVendedor);

		finEnvio = auxEnvio;
	}
}
void escribirEnvio() {
	auxEnvio = iniEnvio;
	ofstream escribirEnvio;
	escribirEnvio.open("Envios.bin", ios::out | ios::binary | ios::trunc);

	if (escribirEnvio.is_open()) {
		while (auxEnvio != nullptr)
		{
			escribirEnvio.write((char*)auxEnvio, sizeof(envio));
			auxEnvio = auxEnvio->sig;
		}
	}
	escribirEnvio.close();
}
void eliminarEnvio(envio* auxEnvio) {

	if (auxEnvio == iniEnvio) {	
		if (auxEnvio->sig == nullptr) { 
			delete auxEnvio;

			iniEnvio = nullptr;
			auxEnvio = iniEnvio;
		}
		else { 

			iniEnvio = auxEnvio->sig;
			delete auxEnvio;

			auxEnvio = iniEnvio;	
			auxEnvio->ant = nullptr;
		}
		MessageBox(hEnvios, "Envio cancelado", "AVISO", MB_OK | MB_ICONINFORMATION);
	}
	else {
		if (auxEnvio->sig == nullptr) {
			auxEnvio->ant->sig = nullptr;
		}
		else {
			auxEnvio->sig->ant = auxEnvio->ant;
			auxEnvio->ant->sig = auxEnvio->sig;
		}

		delete auxEnvio;
		auxEnvio = iniEnvio;
		MessageBox(hEnvios, "Envio cancelado", "AVISO", MB_OK | MB_ICONINFORMATION);

	}
}
void modificarEnvio(envio* editEnvio, char nomProd[50]) {
	auxEnvio = iniEnvio;
	char newDir[50];

	if (auxEnvio == nullptr) {
		MessageBox(hEnvios, "No hay envios todavia.", "AVISO", MB_OK | MB_ICONINFORMATION);
	}
	else {
		while (auxEnvio != nullptr && strcmp(auxEnvio->nomProducto, nomProd) != 0) {
			auxEnvio = auxEnvio->sig;
		}
		if (auxEnvio == nullptr) {
			MessageBox(hEnvios, "No se encontro el envio.", "AVISO", MB_OK | MB_ICONINFORMATION);
		}
		else {
			strcpy_s(auxEnvio->ciudadUs, editEnvio->ciudadUs);;
			strcpy_s(auxEnvio->calleUs, editEnvio->calleUs);
			strcpy_s(auxEnvio->colUs, editEnvio->colUs);
			strcpy_s(auxEnvio->estadoUs, editEnvio->estadoUs);

			MessageBox(hProductos, "Se actualizó la dirección de envió", "AVISO", MB_OK | MB_ICONINFORMATION);
		}
	}
}

LRESULT CALLBACK MyEditCallBckProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	if ((msg == WM_CHAR) && (wParam >= '0' && wParam <= '9'))
	{
		MessageBox(hwnd, "No se aceptan numeros.", "AVISO", MB_OK | MB_ICONSTOP);

		return 0;
	}


	return CallWindowProc(lpEditWndProc, hwnd, msg, wParam, lParam);
}

void swap(envio* a, envio* b){
	envio t = *a;
	*a = *b;
	*b = t;

	b->sig = a->sig; b->ant = a->ant;
	a->sig = t.sig; a->ant = t.ant;
}
envio* partition(envio* l, envio* h){

	SYSTEMTIME DateSh;
	ZeroMemory(&DateSh, sizeof(DateSh));

	DateSh.wDay = atoi(h->diaEnvio.dia);
	DateSh.wMonth = atoi(h->diaEnvio.mes);
	DateSh.wYear = atoi(h->diaEnvio.year);
 
	envio* i = l->ant;
	for (envio* j = l; j != h; j = j->sig){

		SYSTEMTIME DateSj;
		ZeroMemory(&DateSj, sizeof(DateSj));
		FILETIME DateFj;
		ZeroMemory(&DateFj, sizeof(DateFj));

		DateSj.wDay = atoi(j->diaEnvio.dia);
		DateSj.wMonth = atoi(j->diaEnvio.mes);
		DateSj.wYear = atoi(j->diaEnvio.year);

		if (DateSj.wYear <= DateSh.wYear && DateSj.wMonth <= DateSh.wMonth && DateSj.wDay < DateSh.wDay){
			if (i == NULL) {
				i = l;
			}
			else {
				i = i->sig;
			}
			swap((i), (j));
		}
	}
	if (i == NULL) {
		i = l;
	}
	else {
		i = i->sig;
	}
	swap((i), (h));
	return i;
}

void quickSort(envio* iniL, envio* finL) {
	if (finL != NULL && iniL != finL && iniL != finL->sig)
	{
		envio* p = partition(iniL, finL);
		quickSort(iniL, p->ant);
		quickSort(p->sig, finL);
	}
}