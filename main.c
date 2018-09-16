#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> /*Library Function For Open()*/
#include <string.h>
#include <sys/mman.h>/*Library Function For mmap()*/
#include <unistd.h>/*Library Function For Close()*/

#define Pagemask 0xffff
#define Offsetmask 0xff
/*********************************************************************************/
/*Global Variables Declaritions To Be Seen Among All Functions Including Main*/
int TLB[16][2];/*2D Array For TLB First Coloum For Page Number Second For Frame Number*/
int Page_Table[256];
int TLB_Upd = -1;/*Variable For Update_TLB Function*/
int Fault_Count =0;
int TLB_Counter = 0;
/********************************************************************************/
void Intialize_PageTable_Tlb(int n)
{
 int i;
 for(i=0;i<16;i++)
  {
   TLB[i][0]=n;
   TLB[i][1]=n;
  }
 for(i=0;i<256;i++)
  {
   Page_Table[i]=n;
  }
}

int Generate_PageNumber_Offset(int logical,int choice)
{
 switch(choice)
 {
  case 1:
  return (logical&Pagemask) >>8;
  break;
  case 2:
  return logical & Offsetmask;
  break;
  default:
  return 0;
 }
}

int TLB_Search(int Page_Number)
{
 int i;
 for(i=0;i<16;i++)
 {
   if(TLB[i][0]==Page_Number)
   {
    TLB_Counter++;
    return TLB[i][1];
   }
 }
 return -1;/*Indicates Search Failed*/
}

int Page_Table_Search(int Page_Number)
{
 if(Page_Table[Page_Number]==-1)
  {
    Fault_Count++; /*Fault Counter*/
  }

  return Page_Table[Page_Number];
}

void Update_TLB(int Page_Number,int Frame_Number)
{
 /*Using FIFO Replacement Algorithm*/
 if(TLB_Upd==-1)/*Indicates Empty TLB*/
 {
  TLB_Upd=0;
  TLB[TLB_Upd][0]=Page_Number;
  TLB[TLB_Upd][1]=Frame_Number;
  TLB_Upd++;
 }
  else
   {
    TLB[TLB_Upd][0]=Page_Number;
    TLB[TLB_Upd][1]=Frame_Number;
    TLB_Upd=(TLB_Upd+1)%16;
   }
}

int main()
{
int input;/*Users Choice In Begin Of Program*/
int choice;/*Variable For Generating Pagenumber And Offset Function*/
int Counter = 1;/*Address Counter*/
int Page_Number;
int logical;/*Variable To Hold Logical Address And Perform Operation On*/
int Offset;
int Value;/*Variable To Hold The Value Stored At The Corresponding Physical Address*/
float TLB_Rate;
float Fault_Rate;
char Memory[256*256];/*An Array Simulates Physical Memory Of Size 256*256*/
int Page_Address;
int freespace = 0; /*Variable For Physical Memory Fulfilling*/
int Physical;
int Frame_Number;

printf("\t\tWelcome To Virtual Memory Manager\n\n");
printf("Please Select One Of The Following Choices:\n");
printf("1-To Print Contents Here In The Console\n"
       "2-To Print Contents In An External Text File\n");
scanf("%d",&input);
FILE *fp;  /*Read From Address File*/
FILE *fo;  /*Output File*/
char read[8]; /*An Array To Hold Contents Of The Address File Line By Line */
int f_back;/*Variable To Open Backing Store.bin*/
fp= fopen("addresses.txt","r");
fo= fopen("output.txt","w+");
char *store_mmap;/*Variable To Store The Mapping Function*/
/******************************************************************************/
Intialize_PageTable_Tlb(-1);/*Function To Intialize Both TLB And Page Table Elements To -1 */
/*****************************************************************************/
if(fp==NULL)/*Print Error Message If Address File Doesnt Exist Or Not Opened Correctly*/
 {
   printf("Address File Not Opened");
   exit(EXIT_FAILURE);
 }
/* Opening Backing Store.bin And Mapping Addresses To Memory*/
f_back=open("BACKING_STORE.bin",O_RDONLY);
store_mmap = mmap(0, 65536, PROT_READ, MAP_SHARED, f_back, 0);
 if (store_mmap == MAP_FAILED)
 {
    close(f_back);
    printf("Mapping Backing Store Error");
    exit(EXIT_FAILURE);
 }

while(fgets(read,8,fp))/*Loop To Read Address From The File Line By Line*/
{
logical=atoi(read);/*Convert String To Integer*/
Counter++;/*Address Counter To Count Page Fault Rate And TLB Rate*/
/*printf("%d\t",logical);*/
choice=1;
Page_Number=Generate_PageNumber_Offset(logical,choice);
/*printf("%d\n",Page_Number);*/
choice=2;
Offset=Generate_PageNumber_Offset(logical,choice);
/*printf("%d\n",Offset);*/
Frame_Number=TLB_Search(Page_Number);/*Go To TLB To Extract Frame Number If it Exists*/
if(Frame_Number!=-1)
  {
    Physical=Frame_Number+Offset;
    Value=Memory[Physical];
  }
  else /*Frame Not Found In TLB,We Go To Page Table*/
    {
      Frame_Number=Page_Table_Search(Page_Number);
      if(Frame_Number!=-1)/*If Frame Is Found*/
      {
        Physical=Frame_Number+Offset;
        /*If Frame Search Is Successful Here We Need To Update TLB With New Page And Frame Number*/
        Update_TLB(Page_Number,Frame_Number);
        Value=Memory[Physical];
       }

    else /*Frame Not Found Neither In TLB Nor In Page Table ,Page Fault Occurs*/
      {
        Page_Address=Page_Number*256;
     /*Variable Index Here Is Used To Tell The Array Of Memory The Place For Storing The Contents Of The Mapping*/
       if(freespace!=-1)
         {
           memcpy(Memory+freespace,store_mmap+Page_Address,256);
           Frame_Number=freespace;
           Physical=Frame_Number+Offset;
           Value=Memory[Physical];
           Page_Table[Page_Number]=freespace;
           Update_TLB(Page_Number,Frame_Number);
          }
           if(freespace<65536-256)/*Check I f The Index Is Less Than The Size Of The Physical Memory*/
            {
              freespace+=256;
            }
             else
              {
                freespace= -1;/*Memory Is Full*/
              }

       }

     }
switch(input)/*Allow User Whether To Print On Console Or External Text File*/
 {
  case 1:
  printf("Logical Address: %d\t",logical);
  printf("Physical Address: %d\t",Physical);
  printf("Value: %d\t\n",Value);
  break;
  case 2:
  if(fo==NULL)/*Exiting If Output File Corrupted Or Any Error Happens */
 {
   printf("Output File To Be Written In Doesnt Exist Or Corrupted");
   exit(EXIT_FAILURE);
 }
  fprintf(fo,"Logical Address: %d\t",logical);
  fprintf(fo,"Physical Address: %d\t",Physical);
  fprintf(fo,"Value: %d\t\n",Value);
  break;
 }
}//End While Loop
Fault_Rate = (float) Fault_Count / Counter;/*Casting Integer From Float*/
TLB_Rate = (float) TLB_Counter / Counter;
if(input==1)
{
 printf("Number Of Translated Addresses: %d\n",Counter);
 printf("Number Of Page Faults: %d\n",Fault_Count);
 printf("Fault Rate: %f\n",Fault_Rate);
 printf("Number Of TLB Hits: %d\n",TLB_Counter);
 printf("TLB Hit Rate: %f\n",TLB_Rate);
}
else
{
 fprintf(fo,"Number Of Translated Addresses: %d\n",Counter);
 fprintf(fo,"Number Of Page Faults: %d\n",Fault_Count);
 fprintf(fo,"Fault Rate: %f\n",Fault_Rate);
 fprintf(fo,"Number Of TLB Hits: %d\n",TLB_Counter);
 fprintf(fo,"TLB Hit Rate: %f\n",TLB_Rate);
}
}
