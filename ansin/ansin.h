/*** Librerias utilizadas ***/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>


/*** Definiciones ***/

#define L_CORCHETE 	      256
#define R_CORCHETE	      257
#define L_LLAVE 	      258
#define R_LLAVE 	      259
#define COMA		      260
#define DOS_PUNTOS	      261
#define STRING            262
#define NUMBER  	      263
#define PR_TRUE		      264
#define PR_FALSE	      265
#define PR_NULL           266
#define TAMAUX            5
#define TAMLEX            50
#define TAMCONJ           10
#define VACIO             1
#define TAMARRAY          500

/*** Estructuras ***/

typedef struct {
	int compLex;
    char lexema[TAMLEX];
    int linea;
} token;

void imprimirArchivo();
void recorrerCompLex();
void getToken();
void error(const char* mensaje);
void scan(int listConj1[], int listConj2[]);
void validarInput(int conPri[], int conSig[]);
void errorSintactico(const char* mensaje);
void parser();
void json(int listConj[]);
void element();
void object(int listConj[]);
void array(int listConj[]);
void objectFin(int listConj[]);
void arrayFin(int listConj[]);
void elementList();
void elementListFin(int listConj[]);
void atribute();
void atributeList();
void atributeListFin(int listConj[]);
void atributeName();
void atributeValue(int setSync[]);