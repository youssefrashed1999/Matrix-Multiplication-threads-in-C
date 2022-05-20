#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//struct that will be passed as argment to the thread function
typedef struct{
    int i,j;
}args;

int matrix1[10][10];
int matrix2[10][10];
int result1[10][10];
int result2[10][10];
int row1,row2,column1,column2;

void readfile()
{
    FILE *f;
    char path[30];
    //get the file name
    printf("Enter file name: ");
    fgets(path,30,stdin);
    //to make fgets work \n should be replaced with \0 as fgets reads the enter as \n i guess
    for(int i=0;i<sizeof(path);i++)
    {
        if(path[i]=='\n')
        {
            path[i]='\0';
            break;
        }
    }
    //open the file
    f=fopen(path,"r");
    //if the file name entered doesn't exist the program terminates
    if(f == NULL)
    {
        printf("File name entered doesn't exist in this directory!\n");
        exit(0);
    }

    //getting the first matrix's size
    fscanf(f,"%d",&row1);
    fscanf(f,"%d",&column1);
    //initialize the first matrix
    //read the matrix values
    for(int i=0;i<row1;i++)
    {
        for(int j=0;j<column1;j++)
        {
            fscanf(f,"%d",&matrix1[i][j]);
        }
    }
    //getting the second matrix's size
    fscanf(f,"%d",&row2);
    fscanf(f,"%d",&column2);
    //initialize the second matrix
    //read the matrix values
    for(int i=0;i<row2;i++)
    {
        for(int j=0;j<column2;j++)
        {
            fscanf(f,"%d",&matrix2[i][j]);
        }
    }
    fclose(f);
}

void writefile(int result[10][10],double time,char * identifier)
{
    FILE *f;
    f=fopen("output.txt","a");
    fprintf(f,"%s",identifier);
    for(int i=0;i<row1;i++)
    {
        for(int j=0;j<column2;j++)
        {
            fprintf(f,"%d ",result[i][j]);
        }
        fprintf(f,"\n");
    }
    fprintf(f,"time taken : %lf \n",time);
    fclose(f);
}

void * matrixMultiplyByElement(void * arg)
{
    args *resultArg = arg;

    for (int k = 0; k < row2; k++)
    {
    result1[resultArg->i][resultArg->j] += matrix1[resultArg->i][k] * matrix2[k][resultArg->j];
    }
}

void * matrixMultiplyByRow(void * arg)
{
    args *resultArg = arg;
    for(int j = 0; j < column2; j++)
    {
            result2[resultArg->i][j]=0;
            for (int k = 0; k < row2; k++)
            {
                result2[resultArg->i][j] += matrix1[resultArg->i][k] * matrix2[k][j];
            }
    }

}

int main()
{
    //read the matrices
    readfile();
    //checks that the two matrices entered can be multiplicated
    if(column1 != row2)
    {
        printf("The two matrices entered can't be multiplied!\n");
        return 0;
    }

    //initiate an array of arguments
    args arg[100];
    int l=0;
    //initiate an array of threads
    pthread_t thread[row1*column2];
    clock_t start,end;
    double executetime;
    //multipy matrices by thread for each output element
    start=clock();
    for (int i = 0; i < row1; i++)
    {
        for (int j = 0; j < column2; j++)
        {
            result1[i][j] = 0;
            arg[l].i=i;
            arg[l].j=j;
            pthread_create(&thread[l],NULL,&matrixMultiplyByElement,&arg[l]);
            l++;

            //printf("%d\t", rslt[i][j]);
        }

        //printf("\n");
    }
    //joining the threads
    for(int i=0;i<row1*column2;i++)
    {
        pthread_join(thread[i],NULL);
    }
    end=clock();
    executetime=((double) (end - start)) / CLOCKS_PER_SEC;
    writefile(result1,executetime,"by element\n");
    //multiply matrices by thread for each output row
    start=clock();
    for (int i = 0; i < row1; i++)
    {
        arg[i].i=i;
        pthread_create(&thread[i],NULL,&matrixMultiplyByRow,&arg[i]);
    }
    //joining the threads
    for(int i=0;i<row1;i++)
    {
        pthread_join(thread[i],NULL);
    }

    end=clock();
    executetime=((double) (end - start)) / CLOCKS_PER_SEC;
    writefile(result2,executetime,"by row\n");

    printf("The program terminated successfully\n");
    printf("Open output.txt to see the results :)\n");
    return 0;
}
