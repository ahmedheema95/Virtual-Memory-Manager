# Virtual-Memory-Manager
### Project Overview:
> This project consists of writing a program that translates logical to
physical addresses. for a virtual address space of size 2^16 =
65,536 bytes. We read an address file that contains random 32-bit
Integers that represent logical addresses, we are only concerned
with first 16-bit only that are divided into two parts 8-bit page
number and 8-bit page offset.
### How To Run:
Running the code is very simple just use any ‘ide’ program under linux
environment or open the terminal and type the following command (gcc
main.c) then (./a.out).
### Program Interface:
When the code is successfully compiled and opened then a menu appears to the
user asking to choose one of the methods to show the output content either on
the console window or on an external output file.
### Explanation Of The Code Functions:
* void Intialize_PageTable_Tlb(int n)
Receives (-1) from the main() and set all the Pagetable and TLB elements to -1 .
* int Generate_PageNumber_Offset(int logical,int choice)
Receives the logical address from the main() and generates the page number by shifting
each address by masking then 8 and generates the offset by masking with 255..
* int TLB_Search(int Page_Number)
Receives the page number from the main() and loops through the entries of the tlb to return
the corresponding frame number of the page number and if the page number is found then
the tlb counter is incremented by 1 to count the tlb hit rate at the end of the program , if page
number not found in tlb then it return -1 indicating search failed .
* int Page_Table_Search(int Page_Number)
Receives the page number from the main() and loops through the page table to search for
the equivalent page number to extract the frame number .
* void Update_TLB(int Page_Number,int Frame_Number)
Receives the page number and the frame number from the main to update the tlb using FIFO
replacement algorithm .
### Methodology Of The Program :
* The program open two files ‘addresses.txt’ and ‘BACKING_STORE.bin’
* The first text file is opened for the addresses to be read line by line and send to
the functions to obtain page number and offset ,the second binary file is
mapped to the memory.
* After page number and offset are obtained the page number is send to
the function (TLB_Search) to search for the equivalent page number and
returning the frame number.
* If the frame number is found in the tlb then physical address is
calculated by adding frame number and offset , and value is calculated
by obtaining the value stored at array of characters of memory at index
physical.
* If the frame number is not found in the tlb then ‘tlb miss’ happens and
function returns -1 indicating search failed .
* Then we go to page table to search for the page number if it is found,
then physical address and value are calculated and we need to update the
tlb by passing page number and frame number to the function
(Update_TLB) which updates the tlb by FIFO replacement algorithm .
* The (Update_TLB) works as follows : the variable ‘TLB_Upd’ which is
intialized to -1 at beginning of the program tells us that the tlb is empty
then for the first time the tlb is updated ‘TLB_Upd’ is set to 0 to fulfill
the first place in the page number and the frame number and then
incremented by 1.
* If the frame is not found in the page table then page fault occurs and we
need to retrieve it from ‘BACKING_STORE.bin’ and replace it in the
physical memory and update the tlb.
* We count page address by multiplying page number by (frame size =
256).
* ‘Index’ variable is initialized to 0 at beginning execution of the program
to tell memory array the place for storing contents of mapping.
* Tlb and page table are updated afterwards.
* Index needs to be less than (256*256) the size of the memory , if else it
is set to -1 indicating memory is full.
