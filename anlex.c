/*
 *	Analizador Lexico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	
 *	Descripcion:
 *	Implementacion de un analizador lexico para el lenguaje JSON - simplificado
 *	
 */

/*********** Inclusion de cabecera **************/

#include "anlex.h"


/************* Variables globales **************/
token t;				    // token global para recibir componentes del Analizador Lexico


/***** Variables para el analizador lexico *****/

FILE *archivo, *output;               // Fuente JSON
char id[TAMLEX];		    // Utilizado por el analizador lexico
int numLinea=1;			    // Numero de Linea
char espacios[100];     
int cantidad_espacios=-1;                 

/**************** Funciones **********************/

void error(const char* mensaje)
{
	fprintf(output,"Linea %d: Error Lexico. %s",numLinea,mensaje);	
}

void getToken()
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
						strcpy(t.lexema,id);
						break;
					case -1:
						if (caracter==EOF)
                            error("No se esperaba el fin de archivo\n");
						else
                            error(msg);
                        acepto=1;
                    t.compLex=VACIO;
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
			break;
		}
		else if (caracter==':')
		{
            t.compLex=DOS_PUNTOS;
            strcpy(t.lexema,":");
            break;
		}
		else if (caracter==',')
		{
			t.compLex=COMA;
			strcpy(t.lexema,",");
			break;
		}
		else if (caracter=='[')
		{
			t.compLex=L_CORCHETE;
			strcpy(t.lexema,"[");
			break;
		}
		else if (caracter==']')
		{
			t.compLex=R_CORCHETE;
			strcpy(t.lexema,"]");
			break;
		}
		else if (caracter=='{')
		{
			t.compLex=L_LLAVE;
			strcpy(t.lexema,"{");
			break;		
        }
        else if (caracter=='}')
		{
			t.compLex=R_LLAVE;
			strcpy(t.lexema,"}");                        
			break;		
        }
		else if (caracter=='n' || caracter=='N')
        {
            ungetc(caracter,archivo);
            fgets(aux,5,archivo);//ver si es null
            if (strcmp(aux, "null")==0 || strcmp(aux, "NULL")==0)
            {
                t.compLex = PR_NULL;
                strcpy(t.lexema,aux);
            }
            else
            {
                fprintf(output,"%s %s %c",msg,"%c no se esperaba",caracter);
			    error(msg);

                while(caracter!='\n')
                    caracter=fgetc(archivo);

                t.compLex = VACIO;
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
                strcpy(t.lexema,aux);
            }
            else{
                fprintf(output,"%s %s %c", msg,"%c no se esperaba",caracter);
			    error(msg);

                while(caracter!='\n')
                    caracter=fgetc(archivo);    

                t.compLex = VACIO;
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
                strcpy(t.lexema,aux);
            }
            else
            {
                fprintf(output,"%s %s %c",msg,"%c no esperado",caracter);
			    error(msg);

                while(caracter!='\n')
                    caracter=fgetc(archivo);

                t.compLex = VACIO;
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
		strcpy(t.lexema,"EOF");
	}
}

void recorrerCompLex() {
    while (t.compLex!=EOF)
	{
		getToken();
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
	}
	else
	{
		printf("Se debe pasar por parametro el archivo fuente.\n");
		exit(1);
	}
	fclose(output);
	return 0;
}