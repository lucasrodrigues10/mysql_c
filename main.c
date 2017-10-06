#include <stdio.h>
#include <mysql.h> // Para usar o mysql
#include <stdlib.h> // Para usar a funcao exit
#include <string.h> // Para usar strtok e outros
#define MAX 1000 //Tamanho maximo do registro

int main()
{

    //Cria um ponteiro para o arquivo
    FILE *ptr_file;

    //Cria uma variavel para localizar o arquivo
    char* filename = "C:\\Users\\notebook\\Documents\\T_20\\filmes.txt";

    //Variavel para armazenar os dados do arquivo texto
    char buff[MAX];

    //Abre o arquivo filmes.txt em modo leitura (r)
    ptr_file = fopen(filename,"r");

    //Checa se abriu o arquivo
    if(!(ptr_file == NULL))
    {
        printf("Abriu o arquivo!\n");
    }
    else
    {
        printf("Nao abriu o arquivo (\n");
        exit(1);
    }

    //Objeto para conexao mysql
    MYSQL conexao;
    mysql_init(&conexao);
    if ( mysql_real_connect(&conexao, "localhost", "root", "123",NULL, 0, NULL, 0) )
    {
        printf("Conexao ao banco realizada com sucesso!\n");
    }
    else
    {
        printf("Falha de conexao\n");
        printf("Erro %d : %s\n", mysql_errno(&conexao), mysql_error(&conexao));
        exit(1);
    }

    //Deleta database se ja existir
    if (mysql_query(&conexao, "DROP DATABASE db") == 0)
    {
        printf("Database destruida\n");
    }
    else
    {
        printf("Database nao foi destruida");
        printf("MySQL error: %s\n", mysql_error(&conexao));
        exit(1);
    }

    //Cria database
    if (mysql_query(&conexao, "CREATE DATABASE db") == 0)
    {
        printf("Database criada\n");
    }
    else
    {
        printf("Database nao foi criada");
        printf("MySQL error: %s\n", mysql_error(&conexao));
        exit(1);
    }

    //Usar a database criada
    mysql_query(&conexao, "USE db");

    if (mysql_query(&conexao, "create table tabfilmes(codfilme integer not null primary key auto_increment, nomefilme varchar(100), generofilme varchar(45), anolancamento year);") == 0)
    {
        printf("Tabela criada\n");
    }
    else
    {
        printf("Table nao foi criada ");
        printf("MySQL error: %s\n", mysql_error(&conexao));
        exit(1);
    }

    //Variaveis para guardar os valores que irao para a base de dados
    char nomefilme[MAX];
    char generofilme[MAX];
    char anolancamento[MAX];

    // Enquanto tiver dados na linha do text
    while(fgets(buff,MAX,ptr_file) != NULL)
    {
        //Coloca os dados na variavel
        char * pch;
        pch = strtok (buff," ,.-");

        //nomefilme
        strcpy(nomefilme, pch);
        pch = strtok (NULL, " ,.-");

        //generofilme
        strcpy(generofilme, pch);
        pch = strtok (NULL, " ,.-");

        //anolancamento
        strcpy(anolancamento, pch);

        //Cria a query de inserir
        char query_inserir[MAX];
        snprintf( query_inserir, sizeof( query_inserir ), "%s%s%s%s%s%s%s%s%s%s",
                  "insert into tabfilmes (nomefilme,generofilme,anolancamento) values (","\"", nomefilme, "\",","\"", generofilme, "\",","\"", anolancamento, "\");");
        //Coloca a tupla na tabela
        if (mysql_query(&conexao, query_inserir) == 0)
        {
            printf("Registro inserido\n");
        }
        else
        {
            printf("Registro nao foi inserido\n");
            printf("MySQL error: %s\n", mysql_error(&conexao));
            exit(1);
        }
    }

    mysql_query(&conexao, "SELECT * FROM tabfilmes");
    MYSQL_RES *resultado = mysql_store_result(&conexao);
    int totalrows = mysql_num_rows(resultado);
    int numfields = mysql_num_fields(resultado);
    MYSQL_FIELD *mfield;
    int i,j;

    MYSQL_ROW row = mysql_fetch_row(resultado);
    printf("  ID                             Nome Filme           Ano Lancamento\n");
    for(j = 0; j < totalrows; j++)
    {
        for(i = 0; i < numfields; i++)
        {
            char *val = row[i];
            switch(i)
            {
            case 0:
                printf("%4s\t", val);

                break;
            case 1:
                printf("%35s\t", val);

                break;
            case 2:
                printf("%20s", val);
                break;
            }
        }
        row = mysql_fetch_row(resultado);
        printf("\n");
    }



    //Libera result para nao ter leak de memoria
    mysql_free_result(resultado);

    //Fecha conexao mysql
    mysql_close(&conexao);

    //Fecha o arquivo
    fclose(ptr_file);
    system("PAUSE");
    return(0);
}
