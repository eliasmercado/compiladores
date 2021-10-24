/*
 *	Analizador Sintactico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	
 *	Descripcion:
 *	Implementacion de un analizador sintactico para el lenguaje JSON - simplificado
 *	
 */

/*********** Inclusion de cabecera **************/

#include "ansin.h"
#define TRUE 1
#define FALSE 0

/************* Variables globales **************/
token t;				    // token global para recibir componentes del Analizador Lexico


/***** Variables para el analizador lexico *****/

FILE *archivo, *output;               // Fuente JSON
char id[TAMLEX];		    // Utilizado por el analizador lexico
int numLinea=1;			    // Numero de Linea
char espacios[100];     
int cantidad_espacios=-1;                 
token tokenActual;
int posicion=-1;
char message[41];
int errors = FALSE;
token listTokens[TAMARRAY];
int indexActual = 0;
int listConj[TAMCONJ];
/**************** Funciones **********************/

void error(const char* mensaje)
{
	fprintf(output,"Linea %d: Error Lexico. %s",numLinea,mensaje);	
}

void obtenerSiguienteToken()
{
    int i=0;
    char caracter=0;
    int acepto=0;
	int estado=0;
    short bandera = 0;
	char msg[41];
	char aux[TAMAUX] = " ";
	cantidad_espacios = -1;
	token e;
     
   	while((caracter=fgetc(archivo))!=EOF)
	{
        if(caracter=='\n')
	    {
		    numLinea++;
            fprintf(output,"\n");
		    continue;
	    }
	    else if (caracter==' ')
	    {
            do
            {
                cantidad_espacios++;
                espacios[cantidad_espacios]=' ';
                caracter=fgetc(archivo); 
            }while(caracter ==' ');
            caracter=ungetc(caracter, archivo);
	    }
        else if (caracter=='\t')
        { 
            while(caracter=='\t')
            { 
                fprintf(output,"%c", caracter);
	        	caracter=fgetc(archivo);
            }
        }
    	else if (isdigit(caracter))
	    {
            i=0;
            estado=0;
            acepto=0;
            id[i]=caracter;
			while(!acepto)
			{
				switch(estado)
				{
				    case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						caracter=fgetc(archivo);
						if (isdigit(caracter))
						{
							id[++i]=caracter;
							estado=0;
						}
						else if(caracter=='.')
						{
							id[++i]=caracter;
							estado=1;
						}
						else if(tolower(caracter)=='e')
						{
							id[++i]=caracter;
							estado=3;
						}
						else
							estado=6;
						break;		
					case 1://un punto, debe seguir un digito 
						caracter=fgetc(archivo);						
						if (isdigit(caracter))
						{
							id[++i]=caracter;
							estado=2;
						}
						else{
							fprintf(output,"%s %s %c", msg,"No se esperaba '%c'",caracter);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						caracter=fgetc(archivo);
						if (isdigit(caracter))
						{
							id[++i]=caracter;
							estado=2;
						}
						else if(tolower(caracter)=='e')
						{
							id[++i]=caracter;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						caracter=fgetc(archivo);
						if (caracter=='+' || caracter=='-')
						{
							id[++i]=caracter;
							estado=4;
						}
						else if(isdigit(caracter))
						{
							id[++i]=caracter;
							estado=5;
						}
						else
						{
							fprintf(output,"%s %s %c", msg,"No se esperaba '%c'",caracter);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						caracter=fgetc(archivo);
						if (isdigit(caracter))
						{
							id[++i]=caracter;
							estado=5;
						}
						else
						{
							fprintf(output,"%s %s %c",msg,"No se esperaba '%c'",caracter);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						caracter=fgetc(archivo);
						if (isdigit(caracter))
						{
							id[++i]=caracter;
							estado=5;
						}
						else
							estado=6;
						break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (caracter!=EOF)
							ungetc(caracter,archivo);
						else
                            caracter=0;
						id[++i]='\0';
						acepto=1;
                        t.compLex=NUMBER;
						t.linea=numLinea;
						strcpy(t.lexema,id);
						listTokens[indexActual] = t;
						indexActual++;
						break;
					case -1:
						if (caracter==EOF)
                            error("No se esperaba el fin de archivo\n");
						else
                            error(msg);
                        acepto=1;
                    t.compLex=VACIO;
					t.linea=numLinea;
                    while(caracter!='\n')
                        caracter=fgetc(archivo);
                    ungetc(caracter,archivo);
					break;
				}
			}
			break;
		}
        else if (caracter=='\"')
		{
			i=0;
			id[i]=caracter;
			i++;
			do
			{
				caracter=fgetc(archivo);
				if (caracter=='\"')
				{
                    id[i]=caracter;
                    i++;
                    bandera=1;
                    break;
				}
                else if(caracter==EOF || caracter==',' || caracter=='\n' || caracter==':')
				{
                    fprintf(output,"%s %s",msg,"Se esperaba que finalize el literal");
					error(msg);
                    
                    while(caracter!='\n')
                        caracter=fgetc(archivo);

                    ungetc(caracter,archivo);
                    cantidad_espacios=-1;
                    break;                       
				}
				else
				{
					id[i]=caracter;
					i++;
				}
			}while(isascii(caracter)  || bandera == 0);
			    id[i]='\0';
            strcpy(t.lexema,id);
			t.compLex = STRING;
			t.linea=numLinea;
			listTokens[indexActual] = t;
            indexActual++;
			break;
		}
		else if (caracter==':')
		{
            t.compLex=DOS_PUNTOS;
			t.linea=numLinea;
            strcpy(t.lexema,":");
			listTokens[indexActual] = t;
            indexActual++;
            break;
		}
		else if (caracter==',')
		{
			t.compLex=COMA;
			t.linea=numLinea;
			strcpy(t.lexema,",");
			listTokens[indexActual] = t;
            indexActual++;
			break;
		}
		else if (caracter=='[')
		{
			t.compLex=L_CORCHETE;
			t.linea=numLinea;
			strcpy(t.lexema,"[");
			listTokens[indexActual] = t;
            indexActual++;
			break;
		}
		else if (caracter==']')
		{
			t.compLex=R_CORCHETE;
			t.linea=numLinea;
			strcpy(t.lexema,"]");
			listTokens[indexActual] = t;
            indexActual++;
			break;
		}
		else if (caracter=='{')
		{
			t.compLex=L_LLAVE;
			t.linea=numLinea;
			strcpy(t.lexema,"{");
			listTokens[indexActual] = t;
            indexActual++;
			break;		
        }
        else if (caracter=='}')
		{
			t.compLex=R_LLAVE;
			t.linea=numLinea;
			strcpy(t.lexema,"}");
			listTokens[indexActual] = t;
            indexActual++;                        
			break;		
        }
		else if (caracter=='n' || caracter=='N')
        {
            ungetc(caracter,archivo);
            fgets(aux,5,archivo);//ver si es null
            if (strcmp(aux, "null")==0 || strcmp(aux, "NULL")==0)
            {
                t.compLex = PR_NULL;
				t.linea=numLinea;
                strcpy(t.lexema,aux);
				listTokens[indexActual] = t;
            	indexActual++;
            }
            else
            {
                fprintf(output,"%s %s %c",msg,"%c no se esperaba",caracter);
			    error(msg);

                while(caracter!='\n')
                    caracter=fgetc(archivo);

                t.compLex = VACIO;
				t.linea=numLinea;
                ungetc(caracter,archivo);
            }
            break;
        }   
        else if (caracter=='f' || caracter=='F')
        {
            ungetc(caracter,archivo);
            fgets(aux,6,archivo);//verificar null
            if (strcmp(aux, "false")==0 || strcmp(aux, "FALSE")==0)
            {
                t.compLex = PR_FALSE;
             	t.linea=numLinea;
			    strcpy(t.lexema,aux);
				listTokens[indexActual] = t;
            	indexActual++;
            }
            else{
                fprintf(output,"%s %s %c", msg,"%c no se esperaba",caracter);
			    error(msg);

                while(caracter!='\n')
                    caracter=fgetc(archivo);    

                t.compLex = VACIO;
				t.linea=numLinea;
                ungetc(caracter,archivo);
            }
            break;
        }   
        else if (caracter=='t' || caracter=='T')
        {
            ungetc(caracter,archivo);
            fgets(aux,5,archivo);//verificar null
            if (strcmp(aux, "true")==0 || strcmp(aux, "TRUE")==0)
            {
                t.compLex = PR_TRUE;
				t.linea=numLinea;
                strcpy(t.lexema,aux);
				listTokens[indexActual] = t;
            	indexActual++;
            }
            else
            {
                fprintf(output,"%s %s %c",msg,"%c no esperado",caracter);
			    error(msg);

                while(caracter!='\n')
                    caracter=fgetc(archivo);

                t.compLex = VACIO;
				t.linea=numLinea;
                ungetc(caracter,archivo);
            }
            break;
        }
        else if (caracter!=EOF)
		{
			fprintf(output,"%s %s %c", msg,"%c no se esperaba",caracter);
			error(msg);
            while(caracter!='\n')
                caracter=fgetc(archivo);
            strcpy(espacios, " ");
            cantidad_espacios = -1; //variable que controla los espacios que se imprimen
            ungetc(caracter,archivo);
		}
	}
	if (caracter==EOF)
	{
		t.compLex=EOF;
		t.linea=numLinea;
		strcpy(t.lexema,"EOF");
		listTokens[indexActual] = t;
        indexActual++;
	}
}

void recorrerCompLex() {
    while (t.compLex!=EOF)
	{
		obtenerSiguienteToken();
        if(cantidad_espacios > -1)
        {
            int j = 0;
            for(j=0; j<=cantidad_espacios;j++)
                fprintf(output,"%c", espacios[j]);
        }
        switch(t.compLex)
		{
            case L_CORCHETE:
                imprimirArchivo(" L_CORCHETE");
                break;
            case R_CORCHETE:
                imprimirArchivo(" R_CORCHETE");
                break;
            case L_LLAVE:
                imprimirArchivo(" L_LLAVE");
                break;
            case R_LLAVE:
                imprimirArchivo(" R_LLAVE");
                break;
            case COMA:
                imprimirArchivo(" COMA");
                break;
            case DOS_PUNTOS:
                imprimirArchivo(" DOS_PUNTOS");
                break;
            case STRING:
                imprimirArchivo(" STRING");
                break;
            case NUMBER:
                imprimirArchivo(" NUMBER");
                break;
            case PR_TRUE:
                imprimirArchivo(" PR_TRUE");
                break;
            case PR_FALSE:
                imprimirArchivo(" PR_FALSE");
                break;
            case PR_NULL:
                imprimirArchivo(" PR_NULL");
                break;
            case EOF:
                break;
        }
		
	}
}

void imprimirArchivo(const char *lex) {
    fprintf(output,"%s",lex);
}

void getToken()
{
    posicion++;
    tokenActual = listTokens[posicion];
}

void match(int t)
{
    if(t == tokenActual.compLex)
    {
        getToken();
    }else
    {
        sprintf(message, "En la linea %d No se esperaba  %s\n", tokenActual.linea, tokenActual.lexema );
        errorSintactico(message);
    }
}

void scan(int listConj1[], int listConj2[])
{
    listConj[16];
    int i=0;
    int bandera=0;
    int tam= 8;
    int tam1 = 16;
    int j;   

    for(j=0; j<8;j++)
    {
        listConj[j] = listConj1[j];
    }
    int k;
    int y=0;
    for(k=8; k<16;k++){
        listConj[k] = listConj2[y];
        y++;
    }
    do{
        while(i < tam1)
        { 
            if(tokenActual.compLex != listConj[i] && tokenActual.compLex != EOF)
            {
                bandera = 1;
                i++;
            }else{
                bandera=0;
                i=16;
                break;
            }
       }
       if(bandera==1)
       {
           getToken();
           i=0;
       }
    }while(i < tam1);
}

void validarInput(int conPri[], int conSig[])
{
    int tam= 8;
    int i;
    int bandera= 0; // Si es cero si esta en el conjunto primero.    
    for (i = 0; i < tam; i++) 
    {
        if(tokenActual.compLex == conPri[i])
        {
            bandera=0;
            break;
        }else{
            bandera = 1; // Si se prende es porque el token no esta en el conjuntoprimero y debe lanzar un error.
        }
    }    
    if(bandera==1)
    {
        sprintf(message, "En la linea %d No se esperaba  %s\n", tokenActual.linea, tokenActual.lexema);
        errorSintactico(message);
        scan(conPri, conSig);
    }
}
 

void errorSintactico(const char* mensaje) 
{
   errors = TRUE;
   printf(" Error sintactico. %s",  mensaje);

}

void parser()
{
    getToken();
    int conSig []={EOF,0,0,0,0,0,0,0};
    json(conSig);
}

void json(int listConj[])
{
    if(tokenActual.compLex == L_LLAVE || tokenActual.compLex == L_CORCHETE){
        element();
    }else
    {
        sprintf(message, "En la linea %d Se esperaba L_LLAVE o L_CORCHETE\n" , tokenActual.linea);
        errorSintactico(message);
    }
}

void element()
{ 
    int conPri[] = {L_LLAVE, L_CORCHETE,0,0,0,0,0,0};
    int conSig []={EOF,COMA, R_CORCHETE, R_LLAVE,0,0,0,0};
    validarInput(conPri, conSig);
    switch(tokenActual.compLex)
    {
        case L_LLAVE:
            object(conSig);
            break;
        case L_CORCHETE:
            array(conSig);
            break;
        default:
            sprintf(message, "En la linea %d Se esperaba L_LLAVE o L_CORCHETE\n" , tokenActual.linea);
            errorSintactico(message);
            break;
    }
    validarInput(conSig,conPri);
}


void object(int listConj[])
{
    int conPri[]={L_LLAVE,0,0,0,0,0,0,0};
    int conSig[]={EOF,COMA, R_CORCHETE, R_LLAVE,0,0,0,0};
    validarInput(conPri,conSig);
    switch (tokenActual.compLex)
    {
        case L_LLAVE:
            match(L_LLAVE);        
            objectFin(listConj);
            break;
        default:
            sprintf(message, "En la linea %d Se esperaba L_LLAVE\n" , tokenActual.linea);
            errorSintactico(message);
            break;
    }   
    validarInput(conSig,conPri);
}

void array(int listConj[])
{
    int conPri[]={L_CORCHETE,0,0,0,0,0,0,0};
    int conSig[] = {EOF,COMA, R_CORCHETE, R_LLAVE,0,0,0,0};
    validarInput(conPri,conSig);
    switch (tokenActual.compLex)
    {
        case L_CORCHETE:
            match(L_CORCHETE);        
            arrayFin(listConj);
            break;
        default:
            sprintf(message, "En la linea %d Se esperaba L_CORCHETE\n" , tokenActual.linea);
            errorSintactico(message);
            break;
    }
    validarInput(conSig,conPri);
}


void objectFin(int listConj[])
{
    int conPri[]={STRING,R_LLAVE,0,0,0,0,0,0};
    int conSig[] = {EOF,COMA, R_CORCHETE, R_LLAVE,0,0,0,0};
    validarInput(conPri,conSig);
    switch (tokenActual.compLex)
    {
        case STRING:
            atributeList();
            match(R_LLAVE);        
            break;
        case R_LLAVE:
            match(R_LLAVE);
            break;
        default:
            sprintf(message, "En la linea %d Se esperaba STRING o R_LLAVE\n" , tokenActual.linea);
            errorSintactico(message);

            break;
    }
    validarInput(conSig,conPri);
}

void arrayFin(int listConj[])
{
    int conPri[]={L_LLAVE, L_CORCHETE, R_CORCHETE,0,0,0,0,0};
    int conSig[] = {EOF,COMA, R_CORCHETE, R_LLAVE,0,0,0,0};
    validarInput(conPri,conSig);
    switch (tokenActual.compLex)
    {
        case L_LLAVE:
            elementList();
            match(R_CORCHETE);        
            break;
        case L_CORCHETE:
            elementList();
            match(R_CORCHETE);
            break;
        case R_CORCHETE:
            match(R_CORCHETE);
            break;
        default:
            sprintf(message, "En la linea %d Se esperaba L_LLAVE, L_CORCHETE o R_CORCHETE\n" , tokenActual.linea);
            errorSintactico(message);
            break;
    }
    validarInput(conSig,conPri);
}

void elementList()
{
    int conPri[]={L_LLAVE,L_CORCHETE,0,0,0,0,0,0};
    int conSig[]={R_CORCHETE,0,0,0,0,0,0,0};
    validarInput(conPri,conSig);
    if(tokenActual.compLex ==L_LLAVE || tokenActual.compLex == L_CORCHETE){
        element();
        elementListFin(conSig);
    }else{
        sprintf(message, "En la linea %d Se esperaba L_LLAVE, L_CORCHETE\n" , tokenActual.linea);
        errorSintactico(message);
        validarInput(conSig,conPri);
    }
}


void elementListFin(int listConj[])
{
    int conPri[]={COMA,0,0,0,0,0,0,0};
    int conSig[]={R_CORCHETE};
    if(tokenActual.compLex==R_CORCHETE){
        validarInput(conSig,conPri);
    }else{
        validarInput(conPri,conSig);
    }
    switch(tokenActual.compLex)
    {
        case COMA:
            match(COMA);
            element();
            elementListFin(conSig);
            break;
    }
   validarInput(conSig,conPri);
}


void atribute()
{
    int conPri[]={STRING,0,0,0,0,0,0,0};
    int conSig[]={COMA,R_LLAVE,0,0,0,0,0,0};
    validarInput(conPri,conSig);
    switch(tokenActual.compLex)
    {
        case STRING:
            atributeName();
            match(DOS_PUNTOS);
            atributeValue(conSig);
            break;
        default:
            sprintf(message, "En la linea %d Se esperaba STRING\n" , tokenActual.linea);
            errorSintactico(message);
            break;
    }
}

void atributeList()
{
    int conPri[]={STRING,0,0,0,0,0,0,0};
    int conSig[]={R_LLAVE,0,0,0,0,0,0,0};
    validarInput(conPri,conSig);
    if(tokenActual.compLex==STRING){
        atribute();
        atributeListFin(conSig);
    }else{
        sprintf(message, "En la linea %d Se esperaba STRING\n" , tokenActual.linea);
        errorSintactico(message);
    }
    validarInput(conSig,conPri);
}

void atributeListFin(int listConj[])
{
    int conPri[]={COMA,0,0,0,0,0,0,0};
    int conSig[]={R_LLAVE,0,0,0,0,0,0,0};
    
    if(tokenActual.compLex == R_LLAVE){
        validarInput(conSig,conPri);
    }else{        
        validarInput(conPri,conSig);
    }
    switch(tokenActual.compLex)
    {
        case COMA:
            match(COMA);
            atribute();
            atributeListFin(conSig);
            break;
    }
    validarInput(conSig,conPri);
}

void atributeName()
{
    int conPri[]={STRING,0,0,0,0,0,0,0};
    int conSig[]={DOS_PUNTOS,0,0,0,0,0,0,0};
    validarInput(conPri,conSig);
    switch(tokenActual.compLex)
    {
        case STRING:
            match(STRING);
            break;
        default:
            sprintf(message, "En la linea %d Se esperaba STRING\n" , tokenActual.linea);
            errorSintactico(message);
            break;
    }
}

void atributeValue(int setSync[])
{
    int conPri[]={L_LLAVE, L_CORCHETE, STRING, NUMBER, PR_TRUE, PR_FALSE,PR_NULL,0};
    int conSig[]={COMA,R_LLAVE,0,0,0,0,0,0};
    validarInput(conPri,conSig);
    switch(tokenActual.compLex)
    {
        case L_LLAVE:
            element();
            break;
        case L_CORCHETE:
            element();
            break;
        case STRING:
            match(STRING);
            break;
        case NUMBER:
            match(NUMBER);
            break;
        case PR_TRUE:
            match(PR_TRUE);
            break;
        case PR_FALSE:
            match(PR_FALSE);
            break;
        case PR_NULL:
            match(PR_NULL);
            break;
        default:
            errorSintactico("Se esperaba L_LLAVE, L_CORCHETE, STRING, NUMBER, TRUE,FALSE O NULL\n");
            break;
    }
}

int main(int argc,char* args[])
{
	output = fopen ("output.txt", "w");

	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
        recorrerCompLex();

		fclose(archivo);
        fclose(output);
        parser();
        if(errors == FALSE)
        {
            printf("Es Correctamente Sintactico\n");
        }
        else
        {
            printf("No es correctamente sintactico\n");
        }
	}
	else
	{
		printf("Se debe pasar por parametro el archivo fuente.\n");
		exit(1);
	}
	return 0;
}