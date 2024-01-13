#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>


FILE *archivo;
char caracter;

int fila=0;
int columna=0;


char *listaPalRes[] = {"inicio", "cadena", "entero", "decimal", "booleano", "constante", "hacer", "mostrar", "leer", "si", "mientras", "sino", "sisino", "para"};


void Sentencia();
void Match(char* expectedToken);
void Sent_if();
void Sent_for();
void Sent_leer();
void Sent_asig();
void Sent_mostrar();
void Sent_while();
void Exp();
void Exp_s();
void Exp_simple();
void Term();
void Factor();

enum TipoToken
{
    PalRes,
    Id,
    Num,
    NumDec,
    Sim,
    OpeLog,
    OpeRel,
    OpeAri,
    OpeAsg,
    CadText,
    Comenta,
    Error
};

struct Token
{
    enum TipoToken Tipo;
    char Lexema[100];
    int Valor;
    float ValorDec;
    int Fila;
};

struct Nodo
{
    struct Token info;
    struct Nodo *izq;
    struct Nodo *der;
};

struct Nodo *raiz;
struct Nodo *actual;
struct Nodo *aux;

void insertar(struct Token token)
{
    if(token.Tipo == 11)
    {
        printf("ERROR DE CO: valor: ");
        printf("%s", token.Lexema);
        printf(" incorrecto");
        printf(" Linea: ");
        printf("%i", token.Fila);
        exit(1);
    }

    struct Nodo *nuevo;
    nuevo = malloc(sizeof(struct Nodo));

    nuevo->info = token;
    nuevo->izq = NULL;
    nuevo->der = NULL;

    if(raiz == NULL)
    {
        raiz = nuevo;
        actual = nuevo;
    }else
    {
        nuevo->izq = actual;
        actual->der = nuevo;
        actual = nuevo;
    }
}

void imprimirLista(struct Nodo *reco)
{
    if(reco != NULL)
    {
        printf("%d\t\t", reco->info.Tipo);
        printf("%s\t\t\t", reco->info.Lexema);
        printf("%i\t\t\t", reco->info.Valor);
        printf("%f\n", reco->info.ValorDec);
        printf("------------------------------------------------------------------------------------\n");
        imprimirLista(reco->der);
    }
}

//DEFINICION DE AUTOMATAS
void AutomataID()
{
    int estado = 1;
    char buffer[100] = "";

    buffer[strlen(buffer)] = caracter;
    estado = 2;
    caracter = fgetc(archivo);
    while(estado == 2){
        if(isalnum(caracter))
        {
            buffer[strlen(buffer)] = caracter;
            caracter = fgetc(archivo);
        }else
        {
            estado = 3;
        }
    }

    struct Token token;
    token.Tipo = Id;
    int i;
    for( i=0; i<14; i++)
    {
        if(strcmp(buffer, listaPalRes[i]) == 0)
        {
            token.Tipo = PalRes;
            break;
        }
    }
    strcpy(token.Lexema, buffer);
    token.Valor = 0;
    token.Fila = fila+1;
    token.ValorDec = 0;
    insertar(token);
}

void AutomataDigit()
{
    int estado = 1;
    char buffer[100] = "";
    buffer[strlen(buffer)] = caracter;
    estado = 2;
    caracter = fgetc(archivo);
    struct Token token;

    if(buffer[0] == 45)
    {
        if(!isdigit(caracter) && caracter != 46)
        {
            token.Tipo = OpeAri;
            strcpy(token.Lexema, buffer);
            token.Valor = 0;
            token.ValorDec = 0;
            insertar(token);
            return;

        }
    }

    while(estado == 2){
        if(isdigit(caracter))
        {
            buffer[strlen(buffer)] = caracter;
            caracter = fgetc(archivo);
        }else if(caracter == 46) //Es igual al punto "."
        {
            buffer[strlen(buffer)] = caracter;
            caracter = fgetc(archivo);
            estado = 3;
        }else
        {
            estado = 5;
        }
    }

    if(estado == 3)
    {
        if(isdigit(caracter))
        {
            buffer[strlen(buffer)] = caracter;
            caracter = fgetc(archivo);
            estado = 4;

            while(estado == 4)
            {
                if(isdigit(caracter))
                {
                    buffer[strlen(buffer)] = caracter;
                    caracter = fgetc(archivo);
                }else
                {
                    estado = 6;
                }
            }
        }else
        {
            buffer[strlen(buffer)] = caracter;
            caracter = fgetc(archivo);
            estado = 7;
        }
    }

    if(estado == 5)
    {
        token.Tipo = Num;
        strcpy(token.Lexema, buffer);
        token.Valor = strtol(buffer, NULL, 10);

        //token.Valor = atoi(buffer);
        token.Fila = fila+1;
        token.ValorDec = 0;
    }else if(estado == 6)
    {
        token.Tipo = NumDec;
        strcpy(token.Lexema, buffer);
        token.Valor = 0;
        token.Fila = fila+1;
        token.ValorDec = atof(buffer);
    }else if(estado == 7)
    {
        token.Tipo = Error;
        strcpy(token.Lexema, buffer);
        token.Valor = 0;
        token.Fila = fila+1;
        token.ValorDec = 0;
    }
    insertar(token);
}
//-----------------------------------------------
void AutomataSimIg()
{
    char buffer[100] = "";

    buffer[strlen(buffer)] = caracter;
    caracter = fgetc(archivo);

    struct Token token;
    if(caracter == 61)
    {
        caracter = fgetc(archivo);
        token.Tipo = OpeRel;
        strcpy(token.Lexema, "==");
    }else
    {
        token.Tipo = OpeAsg;
        strcpy(token.Lexema, "=");
    }
    token.Valor = 0;
    token.Fila = fila+1;
    token.ValorDec = 0;
    insertar(token);
}

void AutomataOpeRel()
{
    char buffer[100] = "";

    buffer[strlen(buffer)] = caracter;
    caracter = fgetc(archivo);

    struct Token token;
    if(caracter == 61)
    {
        buffer[strlen(buffer)] = caracter;
        caracter = fgetc(archivo);
        token.Tipo = OpeRel;
        strcpy(token.Lexema, buffer);
    }else
    {
        token.Tipo = OpeRel;
        strcpy(token.Lexema, buffer);
    }
    token.Valor = 0;
    token.Fila = fila+1;
    token.ValorDec = 0;
    insertar(token);
}

void AutomataOpeLog()
{
    char buffer[100] = "";
    char aux = caracter;
    buffer[strlen(buffer)] = caracter;
    struct Token token;
    if(caracter == 33) //Si es el simbolo de negacion
    {
        caracter = fgetc(archivo);
        if(caracter == 61) //Si es el simbolo de asignacion
        {
            buffer[strlen(buffer)] = caracter;
            caracter = fgetc(archivo);
            token.Tipo = OpeRel;
            strcpy(token.Lexema, buffer);
        }else
        {
            token.Tipo = OpeLog;
            strcpy(token.Lexema, buffer);
        }
    }else
    {
        caracter = fgetc(archivo);
        if(caracter == aux)
        {
            buffer[strlen(buffer)] = caracter;
            caracter = fgetc(archivo);
            token.Tipo = OpeLog;
            strcpy(token.Lexema, buffer);
        }
    }
    token.Valor = 0;
    token.Fila = fila+1;
    token.ValorDec = 0;
    insertar(token);
}

void AutomataOpeAri()
{
    char buffer[100] = "";
    buffer[strlen(buffer)] = caracter;
    struct Token token;
    token.Tipo = OpeAri;
    strcpy(token.Lexema, buffer);
    caracter = fgetc(archivo);
    token.Valor = 0;
    token.Fila = fila+1;
    token.ValorDec = 0;
    insertar(token);
}

void AutomataSim()
{
    char buffer[100] = "";
    buffer[strlen(buffer)] = caracter;
    int esCaracterEsp = 0;
	
	int i;
    for( i=0;i<33;i++)
    {
        if(caracter == i)
        {
            esCaracterEsp = 1;
            break;
        }
    }
    if(esCaracterEsp == 0)
    {
        struct Token token;
        token.Tipo = Sim;
        strcpy(token.Lexema, buffer);
        token.Valor = 0;
        token.Fila = fila+1;
        token.ValorDec = 0;
        insertar(token);
    }
    caracter = fgetc(archivo);
}

void AutomataCadText()
{
    char buffer[100] = "";
    buffer[strlen(buffer)] = caracter;
    int estado = 2;
    caracter = fgetc(archivo);

    while(estado == 2)
    {
        if(caracter == 34)
        {
            estado = 3;
        }else if(caracter == '\n' || caracter =='\r')
        {
            estado = 4;
        }
        buffer[strlen(buffer)] = caracter;
        caracter = fgetc(archivo);
    }
    if(estado == 3)
    {
        struct Token token;
        token.Tipo = CadText;
        strcpy(token.Lexema, buffer);
        token.Valor = 0;
        token.Fila = fila+1;
        token.ValorDec = 0;
        insertar(token);
    }
    if(estado == 4)
    {
        struct Token token;
        token.Tipo = Error;
        strcpy(token.Lexema, buffer);
        token.Valor = 0;
        token.Fila = fila+1;
        token.ValorDec = 0;
        insertar(token);
    }
}

void AutomataComentarios()
{
    char buffer[100] = "";
    buffer[strlen(buffer)] = caracter;
    int estado = 2;
    caracter = fgetc(archivo);

    if(caracter != 42)
    {
        struct Token token;
        token.Tipo = OpeAri;
        strcpy(token.Lexema, buffer);
        token.Valor = 0;
        token.ValorDec = 0;
        insertar(token);
        return;
    }

    while(estado == 2)
    {
        if(caracter == 42)
        {
            buffer[strlen(buffer)] = caracter;
            caracter = fgetc(archivo);
            if (caracter == 47)
            {
                buffer[strlen(buffer)] = caracter;
                caracter = fgetc(archivo);
                estado = 3;
            }
        }
        buffer[strlen(buffer)] = caracter;
        caracter = fgetc(archivo);
    }
    if(estado == 3)
    {
        struct Token token;
        token.Tipo = CadText;
        strcpy(token.Lexema, buffer);
        token.Valor = 0;
        token.ValorDec = 0;
        insertar(token);
    }
}

//ANALIZADOR SINTACTICO
void SyntaxError(char* error)
{
    printf("SYNTAX ERROR: Se esperaba un: ");
    printf("%s", error);
    if(aux == NULL)
    {
        printf(" en la ultima fila");
    }else{
        printf(" en la fila: ");
        printf("%i", aux->info.Fila);
    }

    exit(1);
}
void SyntaxTypeError(int expectedInt)
{
    printf("SYNTAX ERROR: Se esperaba un tipo: ");
    printf("%i", expectedInt);
    if(aux == NULL)
    {
        printf(" en la ultima fila");
    }else
    {
        printf(" en la fila: ");
        printf("%i", aux->info.Fila);
    }
    exit(1);
}

void AvanzarToken()
{
    if(aux != NULL)
    {
        if(aux->der != NULL)
        {
            aux = aux->der;
        }else
        {
            aux = NULL;
        }
    }

}

void Match(char* expectedToken)
{
    if(aux == NULL){
        SyntaxError(expectedToken);
    }
    else if(strcmp(aux->info.Lexema, expectedToken) != 0)
    {
        SyntaxError(expectedToken);
    }
    AvanzarToken();
}

void MatchTipo(int expectedType)
{
    if(aux == NULL){
        SyntaxTypeError(expectedType);
    }
    else if(aux->info.Tipo != expectedType)
    {
        SyntaxTypeError(expectedType);
    }
    AvanzarToken();
}

void MatchTipoDato()
{
    if(strcmp(aux->info.Lexema, listaPalRes[1]) == 0) return;
    else if(strcmp(aux->info.Lexema, listaPalRes[2]) == 0) return;
    else if(strcmp(aux->info.Lexema, listaPalRes[3]) == 0) return;
    else if(strcmp(aux->info.Lexema, listaPalRes[4]) == 0) return;
    else SyntaxError("TipoDato");
}

void Sentencia()
{

    if(strcmp(aux->info.Lexema, listaPalRes[9]) == 0) //Si el Token es un "si"
    {
        Sent_if();
    }else if(strcmp(aux->info.Lexema, listaPalRes[13]) == 0) //Si el Token es un "para"
    {
        Sent_for();
    }else if(strcmp(aux->info.Lexema, listaPalRes[8]) == 0) //Si el Token es un "leer"
    {
        Sent_leer();
    }else if(strcmp(aux->info.Lexema, listaPalRes[7]) == 0) //Si el Token es un "mostrar"
    {
        Sent_mostrar();
    }else if(strcmp(aux->info.Lexema, listaPalRes[10]) == 0) //Si Token es un "mientras"
    {
        Sent_while();
    }else if((strcmp(aux->info.Lexema, listaPalRes[5]) == 0) ||
             (strcmp(aux->info.Lexema, listaPalRes[1]) == 0) ||
             (strcmp(aux->info.Lexema, listaPalRes[2]) == 0) ||
             (strcmp(aux->info.Lexema, listaPalRes[3]) == 0) ||
             (strcmp(aux->info.Lexema, listaPalRes[4]) == 0) ||
             (aux->info.Tipo == 1))
    {
        Sent_asig();
    }else{
        printf("SYNTAX ERROR: No se reconoce la sentencia: ");
        printf("%s", aux->info.Lexema);
        printf(" en la linea: ");
        printf("%i", aux->info.Fila);
        exit(1);
    }


}

void Sent_while()
{
    Match("mientras");
    Match("(");
    Exp();
    Match(")");
    Match("{");
    while((aux != NULL) && (strcmp(aux->info.Lexema, "}") != 0))
    {
        Sentencia();
    }
    Match("}");
}

void Sent_asig()
{
    if(strcmp(aux->info.Lexema, listaPalRes[5]) == 0)
    {
        Match("constante");
        MatchTipoDato();
        AvanzarToken();
        MatchTipo(1);
        Match("=");
        Exp();
        Match(";");
    }else if(aux->info.Tipo == 1)
    {
        MatchTipo(1);
        Match("=");
        Exp();
        Match(";");
    }else
    {
        MatchTipoDato();
        AvanzarToken();
        MatchTipo(1);
        Match("=");
        Exp();
        Match(";");
    }
}

void Sent_if()
{
    Match("si");
    Match("(");
    Exp();
    Match(")");
    Match("{");
    while((aux != NULL) && (strcmp(aux->info.Lexema, "}") != 0))
    {
        Sentencia();
    }
    Match("}");

    while((aux != NULL) && (strcmp(aux->info.Lexema, listaPalRes[12]) == 0))
    {
        Match("sisino");
        Match("(");
        Exp();
        Match(")");
        Match("{");
        while((aux != NULL) && (strcmp(aux->info.Lexema, "}") != 0))
        {
            Sentencia();
        }
        Match("}");
    }
    if((aux != NULL) && (strcmp(aux->info.Lexema, listaPalRes[11]) == 0))
    {
        Match("sino");
        Match("{");
        while((aux != NULL) && (strcmp(aux->info.Lexema, "}") != 0))
        {
            Sentencia();
        }
        Match("}");
    }
}

void Sent_for()
{
    Match("para");
    Match("(");
    Match("entero");
    MatchTipo(1);
    Match("=");
    MatchTipo(2);
    Match(";");
    MatchTipo(1);
    MatchTipo(6);
    MatchTipo(2);
    Match(";");
    MatchTipo(1);
    Match("+");
    MatchTipo(2);
    Match(";");
    Match(")");
    Match("{");
    while((aux != NULL) && (strcmp(aux->info.Lexema, "}") != 0))
    {
        Sentencia();
    }
    Match("}");
}
void Sent_leer()
{
    Match("leer");
    Match("(");
    MatchTipo(1);
    Match(")");
    Match(";");
}
void Sent_mostrar()
{
    Match("mostrar");
    Match("(");
    Exp();
    Match(")");
    Match(";");
}
void Exp()
{
    Exp_s();
    while((aux != NULL) && ((strcmp(aux->info.Lexema, "||") == 0) || (strcmp(aux->info.Lexema, "&&") == 0)))
    {
        AvanzarToken();
        Exp_s();
    }
}
void Exp_s()
{
    Exp_simple();
    while((aux != NULL) && (aux->info.Tipo == 6))
    {
        AvanzarToken();
        Exp_simple();
    }
}
void Exp_simple()
{
    Term();
    while((aux != NULL) && ((strcmp(aux->info.Lexema, "+") == 0) || (strcmp(aux->info.Lexema, "-") == 0)))
    {
        AvanzarToken();
        Term();
    }
}
void Term()
{
    Factor();
    while((aux != NULL) && ((strcmp(aux->info.Lexema, "*") == 0) || (strcmp(aux->info.Lexema, "/") == 0)))
    {
        AvanzarToken();
        Factor();
    }
}
void Factor()
{
    if(aux != NULL){
        if(strcmp(aux->info.Lexema, "(") == 0){
            Match("(");
            Exp();
            Match(")");
        }else if(aux->info.Tipo == 1){
            MatchTipo(1);
        }else if(aux->info.Tipo == 2){
            MatchTipo(2);
        }else if(aux->info.Tipo == 3){
            MatchTipo(3);
        }else if(aux->info.Tipo == 9)
        {
            MatchTipo(9);
        }else{
            SyntaxError("Expresion");
        }
    }
}
int main()
{
    raiz = NULL;
    actual = NULL;

    archivo = fopen("archivoaaa.txt", "r");

    if(archivo == NULL)
    {
        printf("Error al abrir el archivo\n");
    }else
    {
        char carMenor = 60;
        char carAsig = 61;
        char carMayor = 62;

        char carNeg = 33;
        char carAnd = 38;
        char carOr = 124;

        char carSum = 43;
        char carRes = 45;
        char carMul = 42;
        char carDiv = 47;
        char carMod = 37;

        char carDec = 46;

        char carCad = 34; //Las comillas de cadena de texto

        char carDiag = 47; //para los comentarios

        caracter = fgetc(archivo);
        while(feof(archivo) == 0)
        {
            if( caracter == '\n')
            {
                fila++;
                columna=0;
            }
            else
                columna++;

            if(isspace(caracter))
            {
                caracter = fgetc(archivo);
            }

            if(isalpha(caracter))
            {
                AutomataID();
            }else if(caracter == carDec)
            {
                AutomataDigit();
            }else if(isdigit(caracter))
            {
                AutomataDigit();
            }else if(caracter == carRes)// && isdigit(caracter+1)) //-----------------------------------------------------------------------------
            {
                AutomataDigit();    //-----------------------------------------------------------------------------
            }else if(caracter == carDiag)
            {
                AutomataComentarios();
            }else if(caracter == carAsig)
            {
                AutomataSimIg();
            }else if(caracter == carMenor || caracter == carMayor)
            {
                AutomataOpeRel();
            }else if(caracter == carAnd || caracter == carOr || caracter == carNeg)
            {
                AutomataOpeLog();
            }else if(caracter == carSum || caracter == carRes || caracter == carMul || caracter == carDiv || caracter == carMod)
            {
                AutomataOpeAri();
            }else if(caracter == carCad)
            {
                AutomataCadText();
            }else if(isascii(caracter))
            {
                AutomataSim();
            }
        }
        /*
        fclose(archivo);
        printf("TIPO\t\t");
        printf("LEXEMA\t\t\t");
        printf("VALOR\t\t\t");
        printf("VALOR DECIMAL\n");
        printf("------------------------------------------------------------------------------------\n");
        imprimirLista(raiz);
        */
        aux = raiz;

        Match("inicio");
        Match("{");

        while((aux != NULL) && (strcmp(aux->info.Lexema, "}") != 0))
        {
            Sentencia();
        }


        Match("}");
        printf("Compilacion con exito");

    }

    getch();
    return 0;
}
