#include <stdio.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include<pthread.h>
#include <stdlib.h>
#include<sys/time.h>

// basic global data of an array
int arr1[20][20];
int arr2[20][20];
int res[20][20];
int row1,col1,row2,col2,count=0;

// this struct i used in main function to store row and colun number and paa them to ThreadPerElem function
typedef struct Indeces{
  int Row,Col;
}Indeces;


void* ThreadPerElement(void* arg)
{
  int k,sum=0;
  Indeces ind=*(Indeces*)arg;
  for(k=0;k<row2;k++)
  {
   sum+=arr1[ind.Row][k]*arr2[k][ind.Col];
  }
  res[ind.Row][ind.Col]=sum;
  free(arg);// here i free the dynamic memory preserved in main function for struct with row and column numbers
  return NULL;
}

void* ThreadPerRow(void* arg)
{
  int j,k;
  int rowIndex=*(int*)arg;
    for(j=0;j<col2;j++)
    {
      int sum=0;
      for(k=0;k<row2;k++)
      {
         sum+=arr1[rowIndex][k]*arr2[k][j];
      }
      res[rowIndex][j]=sum;
    }
    free(arg);  // here i free the dynamic memory preserved in main function for row numb
    return NULL;
}

void* ThreadPerMatrix()
{
  int i,j,k;
 for(i=0;i<row1;i++){
    for(j=0;j<col2;j++)
    {
      int sum=0;
      for(k=0;k<row2;k++)
      {
         sum+=arr1[i][k]*arr2[k][j];
      }
      res[i][j]=sum;
    }
   }
     return NULL;
}



int main(int argc,char* argv[])
{
   char x[20]=".txt";
   char x2[3][10]={"a","b","c"};
   // if user do not enter parameters for file name i assume them a,b,c
   if(argc==4){
   int i;
   for( i=1;i<argc-1;i++)
    {
     strcpy(x2[i-1],argv[i]);
     strcat(x2[i-1],x);
    }
    strcpy(x2[i-1],argv[i]);
    }else{
        strcat(x2[0],x);
        strcat(x2[1],x);
    }
    // in this while loop i through first to file a and then to file b to store their information in arr1 and arr2
  while(count<2){
  FILE *fp;
   if(count==0)
   {

      fp=fopen(x2[0], "r");
   }else{
      fp=fopen(x2[1], "r");

   }
    if (fp == NULL)
    {
        printf("Error: could not open file %s", x2[count]);
        return 1;
    }
    char firstCol[30];
    char numColRow[2][20]; //this array to store first line arguements after first split
    fgets(firstCol, 30, fp);
    int i=0;
    char * token = strtok(firstCol, " ");
     while(token !=NULL)
     {
         strcpy(numColRow[i],token);
         token=strtok(NULL ,"\n");
        i++;
     }
     char SrowCol[4][20]; //this array for storing first line parameters which are row num co num
     int row,col;
     i=0;
     int j=0;
     while(i<2)
     {
       char * token = strtok(numColRow[i], "=");
        while(token !=NULL)
         {
           strcpy(SrowCol[j],token);
           j++;
         if(i==0){
           token=strtok(NULL ,"");
           }else{
           token=strtok(NULL ,"\n");
           }
         }
      i++;
     }
     row=(int)strtol(SrowCol[1], NULL, 10);
     col=(int)strtol(SrowCol[3], NULL, 10);

     char buffer[row][500];
     i=0;
     //In the while i get lines fron second line in file line by line and then split these lines by tokens to get numbers
     while(fgets(buffer[i], 500, fp))
     {

         int j=0;
        char * token = strtok(buffer[i], "\t");
        while(token !=NULL)
         {

           char element[20]={'\0'};
           strcpy(element,token);

           if(count==0){
               arr1[i][j]=(int)strtol(element, NULL, 10);

            }else{
              arr2[i][j]=(int)strtol(element, NULL, 10);
            }
         if(j==col-1){
           token=strtok(NULL ,"\n");

           }else{
           token=strtok(NULL ,"\t");
           }
           j++;
         }
        i++;
     }
    fclose(fp);
  if(count==0)
  {
     row1=row;
     col1=col;

  }else{
    row2=row;
    col2=col;

  }
  count++;

}

//Here i check for correct matrices maltiplication
 if(col1 ==row2)
  {

   int i,j;

    char partFile[3][20]={"_per_matrix.txt","_per_row.txt","_per_element.txt"};
    char finalFile[20]={'\0'};
    strcpy(finalFile,x2[2]);
    strcat(finalFile,partFile[0]);
    struct timeval stop, start;
    /////////////////////////////thread per matrix////////////////

    gettimeofday(&start, NULL); //start checking time
    pthread_t th;
    if(pthread_create(&th,NULL,&ThreadPerMatrix,NULL)!=0)
    {
      return 1;
    }
    if(pthread_join(th,NULL)!=0)
    {
       return 2;
    }
    //////////////////////////////////////// printing part in a file ////////////////

    FILE *fp = fopen(finalFile, "w");
    if (fp == NULL)
    {
        printf("Error opening the file %s", finalFile);
        return -1;
    }
    fprintf(fp, "Method: A thread per Matrix\n");
    fprintf(fp, "row=%d   col=%d\n",row1,col2);

    for(i=0;i<row1;i++)
    {
      for(j=0;j<col2;j++)
      {
       fprintf(fp, "%d ",res[i][j]);
       res[i][j]=0;
      }
      fprintf(fp,"\n");
    }

    gettimeofday(&stop, NULL); //end checking time
    printf("Microseconds taken for thread per Matrix : %lu  and #threads is:%d\n", stop.tv_usec - start.tv_usec,1);
    ///////////////////////////////threadperRow/////////////////////////////////////////

     gettimeofday(&start, NULL); //start checking time
     pthread_t thArrRow[row1];
     int m;
     for(m=0;m<row1;m++)
     {
      int* index=malloc(sizeof(int));
      *index=m;
      if(pthread_create(&thArrRow[m],NULL,&ThreadPerRow,index) !=0)
      {
          return 3;
      }
     }
     for(m=0;m<row1;m++)
     {
       if(pthread_join(thArrRow[m],NULL)!=0)
       {
         printf("Failed to join thread...");
         return 4;
       }
     }
     //////////////////////////////////////// printing part in a file ////////////////

    strcpy(finalFile,x2[2]);
    strcat(finalFile,partFile[1]);
    FILE *fpR = fopen(finalFile, "w");
    if (fpR == NULL)
    {
        printf("Error opening the file %s", finalFile);
        return -1;
    }
    fprintf(fpR, "Method: A thread per row\n");
    fprintf(fpR, "row=%d   col=%d\n",row1,col2);
    for(i=0;i<row1;i++)
    {
      for(j=0;j<col2;j++)
      {
       fprintf(fpR, "%d ",res[i][j]);
       res[i][j]=0;
      }
      fprintf(fpR,"\n");
    }
    gettimeofday(&stop, NULL); //end checking time
    printf("Microseconds taken for thread per Row : %lu  and #threads is:%d\n", stop.tv_usec - start.tv_usec,row1);

   ///////////////////////////////threadPerElement//////////////////////////////////////////

   gettimeofday(&start, NULL); //start checking time
   pthread_t thArrElem[row1*col2];
     int r,c,arrInd=0;
     for(r=0;r<row1;r++)
     {
       for(c=0;c<col2;c++){
         Indeces ind={
           .Row=r,
           .Col=c
         };
         Indeces* index=malloc(sizeof(Indeces));
         *index=ind;
         if(pthread_create(&thArrElem[arrInd],NULL,&ThreadPerElement,index) !=0)
          {
            return 3;
          }
          arrInd++; //this is an index for threads
      }
     }

    for(c=0;c<col2*row1;c++){

       if(pthread_join(thArrElem[c],NULL)!=0)
       {
         printf("Failed to join thread...");
         return 4;
       }
     }

     //////////////////////////////////////// printing part in a file ////////////////
    strcpy(finalFile,x2[2]);
    strcat(finalFile,partFile[2]);
     FILE *fpE = fopen(finalFile, "w");
    if (fpE == NULL)
    {
        printf("Error opening the file %s", finalFile);
        return -1;
    }
    fprintf(fpE, "Method: A thread per element\n");
    fprintf(fpE, "row=%d   col=%d\n",row1,col2);
    for(i=0;i<row1;i++)
    {
      for(j=0;j<col2;j++)
      {
       fprintf(fpE, "%d ",res[i][j]);
       res[i][j]=0;
      }
      fprintf(fpE,"\n");
    }
     gettimeofday(&stop, NULL); //end checking time
    printf("Microseconds taken for thread per Element : %lu  and #threads is:%d\n", stop.tv_usec - start.tv_usec,row1*col2);


    }
    else{
      printf("Incorrect Maltiplication matrices...\n");
      return 3;
    }

    return 0;
}
