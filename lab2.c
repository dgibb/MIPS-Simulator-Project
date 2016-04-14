 
//David Gibb, Daniel Mathieu 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include<ctype.h>
//feel free to add here any additional library names you may need 
#define SINGLE 1
#define BATCH 0
#define REG_NUM 32
typedef struct inst { // struct to serve as the broken down instruction
	char *op;
	int destReg;
    int sourceReg1;
    int sourceReg2;
    int immediate;
}inst;
typedef struct latch{ // a struct serve as the latches between stages 
	char* op;
	int DR;
    int R1;
    int R2;
    int flag;
    int EX_Value;
    struct inst in;
} latch;
int error=0;
int IFKill,IDKill,EXKill,MEMKill,WBKill;
char **progScanner( char* line);
void EX(int n, int a);
void MEM(int n);
void WB();
void exDelay(int a, int m);
void memDelay(int a);
void parser(char* c, int i);
char **regNumberConverter(char **n);
void IF( int c, int delay);
void ID();
int exCount,memCount,IFCounter;
int dataMemory[512]; // the data memory also holds 512 words 
int IFU,IDU,EXU,MEMU,WBU; // the utilizer counter
struct latch IF_ID ={"nop",0,0,0,0,0,NULL}; // initialize all of the latches 
struct latch ID_EX={"nop",0,0,0,0,0,NULL};
struct latch EX_MEM={"nop",0,0,0,0,0,NULL};
struct latch MEM_WB={"nop",0,0,0,0,0,NULL};
int branchFlag=0;
int pgm_c;//program counter
int RVB[REG_NUM];// register file valid bit
int exhold,memhold; 
struct inst IM[512]; // the instruction memory holds 2k bytes or 512 words 
int mips_reg[REG_NUM];
int main (int argc, char *argv[]){

	int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
	int c,m,n;
	int i;//for loop counter
	
	long sim_cycle=0;//simulation cycle counter

	int counter=0;
	pgm_c=0;
	error=0;
	exCount=0;
	memCount=0;
	exhold=0;
	memhold=0;
	IFKill=0;
	IDKill=0;
	EXKill=0;
	MEMKill=0;
	WBKill=0;
	char line[100];
	int test_counter=0;
	FILE *input=NULL;
	FILE *output=NULL;
	printf("The arguments are:");
	
	for(i=1;i<argc;i++){
		printf("%s ",argv[i]);
	}
	
	printf("\n");
	
	if(argc==7){
	
		if(strcmp("-s",argv[1])==0){
			sim_mode=SINGLE;
		}
		
		else if(strcmp("-b",argv[1])==0){
			sim_mode=BATCH;
		}
		
		else{
			printf("Wrong sim mode chosen\n");
			exit(0);
		}
		
		m=atoi(argv[2]);
		n=atoi(argv[3]);
		c=atoi(argv[4]);
		input=fopen(argv[5],"r");
		output=fopen(argv[6],"w");
		
	}
	
	else{
		printf("Usage: ./sim-mips -s m n c input_name output_name (single-sysle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
		printf("m,n,c stand for number of cycles needed by multiplication, other operation, and memory access, respectively\n");
		exit(0);
	}
	
	if(input==NULL){
		printf("Unable to open input or output file\n");
		exit(0);
	}
	
	if(output==NULL){
		printf("Cannot create output file\n");
		exit(0);
	}
	
	for(i=0; i<520; i++){// allocate memory for the op code 
		IM[i].op=(char *)malloc(60*sizeof(char));
	}
	
	for (i=0;i<REG_NUM;i++){
		mips_reg[i]=0;
	}
	
	//for (i=1;i<=REG_NUM;i++){
	//	printf("%d  ",mips_reg[i]);
	//}
	
	int j=0;
	i =0;
	IFCounter=c;
	while (fgets(line,100, input)!=NULL &&error==0&&counter<512) {// loads the IM with the instructions; stops if there is an error
		parser(line,i);
		i++;
		counter++;
	}
			
	printf("prerun EXcounter %d\n",exCount);
	i=0;
	
	while((IFKill+IFKill+EXKill+MEMKill+WBKill)!=5&&error==0){ // run while loop off of all the kills sum to 5 and the error detection 
	
		printf("CLOCK CYCLE: %d\n",sim_cycle); // insert this in loop for the single pass mode
		printf("\n");
	
		WB();
		
		printf("%s  ","MEM_WB Latch: ");
		printf("%s  ",MEM_WB.op);
		printf("%d  ",MEM_WB.DR);
		printf("%d  ",MEM_WB.R1);
		printf("%d  ",MEM_WB.R2);
		printf("%d  ",MEM_WB.EX_Value);
		printf("\n");
		printf("\n");
		
		MEM(c);
		
		printf("%s  ","EX_MEM Latch: ");
		printf("%s  ",EX_MEM.op);
		printf("%d  ",EX_MEM.DR);
		printf("%d  ",EX_MEM.R1);
		printf("%d  ",EX_MEM.R2);
		printf("%d  ",EX_MEM.EX_Value);
		printf("\n");
		printf("\n");
		
		EX(n,m);
		
		printf("%s  ","ID_EX Latch: ");
		printf("%s  ",ID_EX.op);
		printf("%d  ",ID_EX.DR);
		printf("%d  ",ID_EX.R1);
		printf("%d  ",ID_EX.R2);
		printf("%d  ",ID_EX.EX_Value);
		printf("\n");
		printf("\n");
		
		ID();
		
		printf("%s  ","IF_ID Latch: ");
		printf("%s  ",IF_ID.op);
		printf("%d  ",IF_ID.DR);
		printf("%d  ",IF_ID.R1);
		printf("%d  ",IF_ID.R2);
		printf("%d  ",IF_ID.EX_Value);
		printf("%s  ",IF_ID.in.op);
		printf("%d  ",IF_ID.in.destReg);
		printf("%d  ",IF_ID.in.sourceReg1);
		printf("%d  ",IF_ID.in.sourceReg2);
		printf("%d  ",IF_ID.in.immediate);
		printf("\n");
		printf("\n");
		
		IF(sim_cycle,c);
		
		i++;
		printf("Counter %d\n", IFCounter);
		
		printf("\n");
		printf("\n");
		printf("%s\n", "Instruction Memory");
		
		for (i=1;i<=10;i++){
			printf("%s  ",IM[i].op);
			printf("%d  ",IM[i].destReg);
			printf("%d  ",IM[i].sourceReg1);
			printf("%d  ",IM[i].sourceReg2);
			printf("%d  ",IM[i].immediate);
			printf("\n");
			
		}
			
		printf("\n");
		printf("%s\n", "Register Values");
		
		for (i=1;i<=REG_NUM;i++){
			printf("%d  ",mips_reg[i]);
		}
		
		printf("\n");
		printf("\n");
		printf("%s\n", "Data Memory");
		
		for (i=1;i<=REG_NUM;i++){
			printf("%d  ",dataMemory[i]);
		}
			
		printf("%d\n",pgm_c);
		sim_cycle+=1;
		test_counter++;
		printf("press ENTER to continue\n");
		while(getchar() != '\n');
		
		printf("%s\n", "-----------------------------------------");
		printf("%s\n", "End of Cycle");
		printf("%s\n", "-----------------------------------------");
		printf("\n");
	}
	
	if(sim_mode==0){
		fprintf(output,"program name: %s\n",argv[5]);
		fprintf(output,"stage utilization: %f  %f  %f  %f  %f \n", IFU, IDU, EXU, MEMU, WBU);
	   // add the (double) stage_counter/sim_cycle for each 
	   // stage following sequence IF ID EX MEM WB
	   	fprintf(output,"register values ");
	   	for (i=1;i<REG_NUM;i++){
	   		fprintf(output,"%d  ",mips_reg[i]);
		}
	   	fprintf(output,"%d\n",pgm_c);
	}
	
	fclose(input);
	fclose(output);
	return 0;
}
	
void ID(){

	if(ID_EX.flag!=1){//if the ID_EX latch is not full
		if(IF_ID.flag ==1){ // there is an instruction in the IF ID latch
			IF_ID.flag =0; // the latch is loaded
			if( (strcmp(IF_ID.in.op,"add")==0)|| (strcmp(IF_ID.in.op,"sub")==0)||(strcmp(IF_ID.in.op,"mult")==0)){ // R type instructions
				if((RVB[IF_ID.in.sourceReg1]!=1)&&(RVB[IF_ID.in.sourceReg2]!=1)){ // checks that both registers are valid
					ID_EX.op=IF_ID.in.op;
					ID_EX.R1= mips_reg[IF_ID.in.sourceReg1];
					ID_EX.R2= mips_reg[IF_ID.in.sourceReg2];
					ID_EX.DR= IF_ID.in.destReg;
					RVB[IF_ID.in.destReg]==1;// changes the status of the destination register to avoid data hazards undone at WB stage
					ID_EX.flag=1;// there is now a value in the ID_EX latch
					IDU++;// actually did work
					printf("R ID EXECUTED\n");
				}
				else {
					IF_ID.flag==1; // data hazard exits so the inst in the IF_ID register is left in the latch for later
				}
			}	
			else if(strcmp( IF_ID.in.op,"beq")==0){ // branch instructions only use two registers and we ignore labels
				if((RVB[IF_ID.in.sourceReg1]!=1)&&(RVB[IF_ID.in.sourceReg2]!=1)){
					ID_EX.op=IF_ID.in.op;
					ID_EX.R1= mips_reg[IF_ID.in.sourceReg1];
					ID_EX.R2= mips_reg[IF_ID.in.sourceReg2];
					ID_EX.flag=1;
					branchFlag=1; // branch instruction 
					IDU++;// actually did work
					printf("BRANCH ID EXECUTED\n");
				} // the latch is already cleared when this function is called 
				else {
					IF_ID.flag==1; // data hazard
				}
			}
			else if(strcmp(IF_ID.in.op,"haltSimulation")==0){
				IDKill=1;
				ID_EX.op=IF_ID.in.op;
				ID_EX.flag=1;
				printf(" ID EXECUTED haltSimulation\n");
			}
			else if( (strcmp(IF_ID.in.op,"addi")==0)||(strcmp(IF_ID.in.op,"lw")==0)||(strcmp(IF_ID.in.op,"sw"))){ // I type instructions use one source reg and an immediate that goes into the second source reg since there is no mux
				if(RVB[IF_ID.in.sourceReg1]!=1){ // checks that both registers are valid
					ID_EX.op=IF_ID.in.op;
					ID_EX.R1= mips_reg[IF_ID.in.sourceReg1];
					ID_EX.R2= IF_ID.in.immediate;
					ID_EX.DR= IF_ID.in.destReg;
					RVB[IF_ID.in.destReg]==1;
					ID_EX.flag=1;
					IDU++;// actually did work
					printf("I ID EXECUTED\n");
				}
				else {
					IF_ID.flag==1; // data hazard
				}	
			}
		}
	}
	printf("ID_EX flag %d\n",ID_EX.flag);
	printf("If_ID flag %d\n", IF_ID.flag);
	printf("Branch Flag %d\n", branchFlag);
}


void IF(int c, int delay){

//	if(IFKill ==0){															// the turn off is false 
		//if(IFCounter==0){													// the amount of time it takes for the memory to be accessed
	//		if(IF_ID.flag!=1){												// checks to see if the IF_ID latch is already full
	//			if(branchFlag!=1){											// checks to see if there is a branch being worked out 
					IF_ID.in = IM[pgm_c];									// grabs the next instruction from the IM		
					IFU++;													// work actually done
					printf("PC %d\n",pgm_c);
					
					if(strcmp(IM[pgm_c].op,"haltSimulation")==0){			//turns the IF stage off and stores the cycle it did 
						IFKill=1;
					}
					
					pgm_c++;												// increment pgm_c
					IFCounter=delay;										// reset the delay
					printf("IF excecuted %s\n",IF_ID.in.op);
					IF_ID.flag=1;											// the flag is reset
//				}
	//		}
	//	}
	//	else IFCounter--;													// if counter != 0 increment
//	}
}
																				// warning scanner cant handle "enter" 
void EX(int n, int a){	
														// needs to check if there is a value in the ID_EX Register 
	if(exhold==0&&strcmp(ID_EX.op,"nop")!=0&&ID_EX.flag==1&&EX_MEM.flag==0){	// sets the inital condition 
	exDelay(n,a);
	exhold=1;
	}
	
	if(EXKill==0){																// if the EX stage hasnt been turned off 
		if (exCount==0){														//if the EX stage isnt currently still delayed by operation
			if(EX_MEM.flag!=1){													// if there is no instruciton in the register 
				if(ID_EX.flag==1){												// if there is a instruction to excute 
	
	
					if(strcmp(ID_EX.op,"add")==0){
																				// if the EX cycle has waited the n cycles for the operation
				EX_MEM.EX_Value=ID_EX.R1+ID_EX.R2;								// preform the operations
				EX_MEM.DR=ID_EX.DR; 
				ID_EX.flag=0;
				EX_MEM.flag=1;
				EX_MEM.op=ID_EX.op;
				EXU++;
				exhold=0;
				printf("EX EXCUTED add\n");				
		}
			
			
		else if (strcmp(ID_EX.op,"sub")==0){
		
			EX_MEM.EX_Value=ID_EX.R1-ID_EX.R2;
			EX_MEM.DR=ID_EX.DR;
			ID_EX.flag=0;
			EX_MEM.flag=1;
			EX_MEM.op=ID_EX.op;
			EXU++;
			exhold=0;
			printf("EX EXCUTED sub\n");
		}
		
				
		else if (strcmp(ID_EX.op,"mult")==0){
				 // waits to see if a cycles has passed for multiplication 
			EX_MEM.EX_Value=ID_EX.R1*ID_EX.R2;
			EX_MEM.DR=ID_EX.DR;
			ID_EX.flag=0;
			EX_MEM.flag=1;
			EX_MEM.op=ID_EX.op;
			exhold=0;
			EXU++; 
			printf("EX EXCUTED mult\n");
		}
				
		else if (strcmp(ID_EX.op,"addi")==0){
			
			EX_MEM.EX_Value=ID_EX.R1+ID_EX.R2;
			EX_MEM.DR=ID_EX.DR;
			ID_EX.flag=0;
			EX_MEM.flag=1;
			EX_MEM.op=ID_EX.op;
			EXU++; 
			exhold=0;
			printf("EX EXCUTED addi\n");
		}
		
				
		else if (strcmp(ID_EX.op,"lw")==0){  // needs to check if its okay
	
			EX_MEM.EX_Value=ID_EX.R1+ID_EX.R2;
			EX_MEM.DR=ID_EX.DR;
			ID_EX.flag=0;
			EX_MEM.flag=1;;
			EX_MEM.op=ID_EX.op;
			exhold=0;
			EXU++;
			printf("EX EXCUTED lw\n");
		}
				
				
		else if (strcmp(ID_EX.op,"sw")==0){
	
			EX_MEM.EX_Value=ID_EX.R1+ID_EX.R2;
			EX_MEM.DR=ID_EX.R2;
			ID_EX.flag=0;
			EX_MEM.flag=1;
			EX_MEM.op=ID_EX.op;
			EXU++; 
			exhold=0;
			printf("EX EXCUTED sw\n");
		}
			
				
		else if (strcmp(ID_EX.op,"beq")==0){
			
			if(ID_EX.R1-ID_EX.R2==0){
			
				EX_MEM.DR=ID_EX.R2+pgm_c; 
				branchFlag=0;
				ID_EX.flag=0;
				RVB[MEM_WB.DR]=0; // return the status flag to 0 
				EX_MEM.op=ID_EX.op;
				EXU++; 
				exhold=0;
				EX_MEM.flag=1;
				printf("EX EXCUTED beq\n");
			}
		}
		
		else if (strcmp(ID_EX.op,"haltSimulation")==0){
			EXKill=1;
			ID_EX.flag=0;
			EX_MEM.flag=1;
			EX_MEM.op=ID_EX.op;
			printf("EX Executed haltSimulation \n");}
		
		else{
		
			printf( "instruction not recognised: %s\n ", ID_EX.op);
			error=1;
		}
	} 
}
}
	else exCount--;
	}
	printf("EX Counter %d \n ", exCount);
	printf("EX_MEM flag %d\n", EX_MEM.flag);
}


void MEM(int n){																// needs to check if there actually is an instruction in the EX_MEM register before starting the loop 
	if(memhold==0&&strcmp(EX_MEM.op,"nop")!=0&&MEM_WB.flag!=1&&EX_MEM.flag==1){ // sets the inital condition 
	memDelay(n);
	memhold=1;
	}
	if(MEMKill==0){ // if the EX stage hasnt been turned off 
		if (memCount==0){  //if the EX stage isnt currently still delayed by operation
			if(MEM_WB.flag!=1){ // if there is no instruciton in the register 
				if(EX_MEM.flag==1){ // if there is a instruction to excute 
	
					if((strcmp(EX_MEM.op,"add")==0)||(strcmp(EX_MEM.op,"sub")==0)||(strcmp(EX_MEM.op,"mult")==0)||(strcmp(EX_MEM.op,"addi")==0)||(strcmp(EX_MEM.op,"beq")==0)){
						MEM_WB.DR=EX_MEM.DR; // these instructions do not need to use the Data memory
						MEM_WB.EX_Value=EX_MEM.EX_Value;
						MEM_WB.op=EX_MEM.op;
						EX_MEM.flag=0;
						MEM_WB.flag=1;
						memhold=0;
						printf("MEM Executed R Type\n");
						MEMU++;
					}
			
				
					else if (strcmp(EX_MEM.op,"lw")==0){
						if(EX_MEM.EX_Value%4==0){
							MEM_WB.EX_Value=dataMemory[EX_MEM.EX_Value]; // gets the value from data memory
							MEM_WB.DR=EX_MEM.DR;
							MEM_WB.op=EX_MEM.op;
							memhold=0;
							EX_MEM.flag=0;
							MEMU++;
							printf("MEM Executed lw\n");
						}
						else {error=1; // the memory offset is not a factor of 4
							printf("Invalid Memory Offset");
						}
					}
			
				 
					else if (strcmp(EX_MEM.op,"sw")==0){
						if(EX_MEM.EX_Value%4==0){
							dataMemory[EX_MEM.EX_Value] = mips_reg[EX_MEM.DR];// this is wrong 
							memhold=0;
							EX_MEM.flag=0;
							RVB[EX_MEM.DR]=0; // return the status flag to 0 
							MEMU++;
							printf("MEM Executed sw\n");}
							else {error=1; // the memory offset is not a factor of 4
								printf("Invalid Memory Offset");
							}
						}
			
					else if (strcmp(EX_MEM.op,"haltSimulation")==0){
						MEMKill=1;
					MEM_WB.flag=1;
					EX_MEM.flag=0;
					MEM_WB.op=EX_MEM.op;
					printf("MEM Executed haltSimulation \n");
					}
				}
			}
		} else {
			memCount--;	
		}
	}
	printf("memCount %d\n",memCount);
}
	
void WB(){ 
	if(MEM_WB.flag ==1){	
		if((strcmp(MEM_WB.op,"add")==0)||(strcmp(MEM_WB.op,"sub")==0)||(strcmp(MEM_WB.op,"mult")==0)||(strcmp(MEM_WB.op,"addi")==0)||(strcmp(MEM_WB.op,"lw")==0)){
			mips_reg[MEM_WB.DR]=MEM_WB.EX_Value; // if it is an R type or addi or lw instruction store the calculated value in the register file
			WBU++;
			RVB[MEM_WB.DR]=0; // return the status flag to 0 
			MEM_WB.flag=0;
			printf("WB Executed R type");
		}	
		
		else if((strcmp(MEM_WB.op,"sw")==0)||(strcmp(MEM_WB.op,"beq")==0)){
			WBU++;
			MEM_WB.flag=0; 
			printf("WB executed sw or beq");
		}
			
		else if (strcmp(MEM_WB.op,"haltSimulation")==0){
		WBKill=1;
		printf("WB executed haltSimulation\n");}	
	}	
}
	
void parser(char* c, int i){
	 char **n =regNumberConverter(progScanner(c));
	// struct inst in;
   char *ptr;
	if( (strcmp("add",n[0])==0)|| (strcmp("sub",n[0])==0)||(strcmp("mult",n[0])==0)){ // R type instructions
	strcpy(IM[i].op,n[0]);
	IM[i].sourceReg1=(int)strtol(n[2],&ptr,10);
	IM[i].sourceReg2=(int)strtol(n[3],&ptr,10);
	IM[i].destReg=(int)strtol(n[1],&ptr,10);
	}
	else if((strcmp("beq",n[0])==0)||(strcmp("addi",n[0])==0)){ // I type instructions
	strcpy(IM[i].op,n[0]);
	 IM[i].sourceReg1=(int)strtol(n[2],&ptr,10);
	 IM[i].immediate=(int)strtol(n[3],&ptr,10);
	IM[i].destReg=(int)strtol(n[1],&ptr,10);
	if(IM[i].immediate>65535){// the immediate cannot be stored in the 16 bits 
		error=1;
		printf("Immediate Field Can Not Be Stored in 16 Bits");
	}
	}
	else if((strcmp("sw",n[0])==0)||(strcmp("lw",n[0])==0)){
	strcpy(IM[i].op,n[0]);
	 IM[i].sourceReg1=(int)strtol(n[3],&ptr,10);
	 IM[i].immediate=(int)strtol(n[2],&ptr,10);
	 IM[i].destReg=(int)strtol(n[1],&ptr,10);
	if(IM[i].immediate>65535){// the immediate cannot be stored in the 16 bits 
		error=1;
		printf("Immediate Field Can Not Be Stored in 16 Bits");
	}	
	}
	else if((strcmp("haltSimulation",n[0])==0)){
	strcpy(IM[i].op,n[0]);

	}
	//return in;
}


int opValid( char** n){ 
	int flag =0;
	int i;
	 char ** op;
	 op= (char **)malloc(100*sizeof(char *)); //allocate memory for the tokens to be broken up and stored
		for (i=0; i<8; i++)
       *(op+i) = (char *) malloc(20*sizeof(char *)); 
	 op[0]= "add";
	 op[1]= "addi";
	 op[2]= "sub";
	 op[3]= "mult";
	 op[4]= "beq";
	 op[5]= "lw";
	 op[6]= "sw";
	 op[7]="haltSimulation";

	 
	for (  i= 0; i <8; ++i){
		if(strcmp(op[i],n[0])==0){
			flag=1;
		}
	}
  
	
	
	return flag;
}
int regValid( char* n){ 
	int flag =0;
	int i;
	 char ** reg;
	 reg = (char **)malloc(100*sizeof(char *)); //allocate memory for the tokens to be broken up and stored
		for (i=0; i<70; i++)
       *(reg+i) = (char *) malloc(20*sizeof(char *));
	 reg[0]= "$0";
	 reg[1]= "$1";
	 reg[2]= "$1";
	 reg[3]= "$3";
	 reg[4]= "$4";
	 reg[5]= "$5";
	 reg[6]= "$6";
	 reg[7]= "$7";
	 reg[8]= "$8";
	 reg[9]= "$9";
	 reg[10]= "$10";
	 reg[11]= "$11";
	 reg[12]= "$12";
	 reg[13]= "$13";
	 reg[14]= "$14";
	 reg[15]= "$15";
	 reg[16]= "$16";
	 reg[17]= "$17";
	 reg[18]= "$18";
	 reg[19]= "$19";
	 reg[20]= "$20";
     reg[21]= "$21";
	 reg[22]= "$22";
	 reg[23]= "$23";
	 reg[25]= "$24";
	 reg[26]= "$25";
	 reg[27]= "$26";
	 reg[28]= "$27";
	 reg[29]= "$28";
	 reg[30]= "$29";
	 reg[31]= "$30";
	 reg[32]= "$31";
	 reg[33]= "$32";
	 reg[34]= "$zero";
	 reg[35]= "$at";
	 reg[36]= "$v0";
	 reg[37]="$v1";
	 reg[38]= "$a0";
	 reg[39]= "$a1";
	 reg[40]= "$a2";
	 reg[41]= "$a3";
	 reg[42]= "$t0";
	 reg[43]="$t1";
	 reg[44]="$t2";
	 reg[45]="$t3";
	 reg[46]="$t4";
	 reg[47]="$t5";
	 reg[48]="$t6";
	 reg[49]="$t7";
	 reg[50]="$s0";
	 reg[51]="$s1";
	 reg[52]="$s2";
	 reg[53]="$s2";
	 reg[54]="$s3";
	 reg[55]="$s4";
	 reg[56]="$s5";
	 reg[57]="$s6";
	 reg[58]="$s7";
	 reg[59]="$t8";
	 reg[61]="$t9";
	 reg[62]="$t9";
	 reg[63]="$k0";
	 reg[64]="$k1";
	 reg[65]="$gp";
	 reg[66]="$gp";
	 reg[67]="$sp";
	 reg[68]="$fp";
	 reg[69]="$ra";
	 
	for (  i= 0; i <70; ++i){
		if(strcmp(reg[i],n)==0){
			flag=1;
		}
	}
  
	
	
	return flag;
}

char *converter(char* n){
		char *out =" ";
	int i;
	char ** v;
	 v = (char **)malloc(100*sizeof(char *)); //allocate memory for the tokens to be broken up and stored
		for (i=0; i<70; i++)
       *(v+i) = (char *) malloc(20*sizeof(char *));
	 char ** reg;
	 reg = (char **)malloc(100*sizeof(char *)); //allocate memory for the tokens to be broken up and stored
		for (i=0; i<70; i++)
       *(reg+i) = (char *) malloc(20*sizeof(char *));
	 reg[0]= "$0";
	 reg[1]= "$1";
	 reg[2]= "$1";
	 reg[3]= "$3";
	 reg[4]= "$4";
	 reg[5]= "$5";
	 reg[6]= "$6";
	 reg[7]= "$7";
	 reg[8]= "$8";
	 reg[9]= "$9";
	 reg[10]= "$10";
	 reg[11]= "$11";
	 reg[12]= "$12";
	 reg[13]= "$13";
	 reg[14]= "$14";
	 reg[15]= "$15";
	 reg[16]= "$16";
	 reg[17]= "$17";
	 reg[18]= "$18";
	 reg[19]= "$19";
	 reg[20]= "$20";
     reg[21]= "$21";
	 reg[22]= "$22";
	 reg[23]= "$23";
	 reg[25]= "$24";
	 reg[26]= "$25";
	 reg[27]= "$26";
	 reg[28]= "$27";
	 reg[29]= "$28";
	 reg[30]= "$29";
	 reg[31]= "$30";
	 reg[32]= "$31";
	 reg[33]= "$32";
	 reg[34]= "$zero";
	 reg[35]= "$at";
	 reg[36]= "$v0";
	 reg[37]="$v1";
	 reg[38]= "$a0";
	 reg[39]= "$a1";
	 reg[40]= "$a2";
	 reg[41]= "$a3";
	 reg[42]= "$t0";
	 reg[43]="$t1";
	 reg[44]="$t2";
	 reg[45]="$t3";
	 reg[46]="$t4";
	 reg[47]="$t5";
	 reg[48]="$t6";
	 reg[49]="$t7";
	 reg[50]="$s0";
	 reg[51]="$s1";
	 reg[52]="$s2";
	 reg[53]="$s2";
	 reg[54]="$s3";
	 reg[55]="$s4";
	 reg[56]="$s5";
	 reg[57]="$s6";
	 reg[58]="$s7";
	 reg[59]="$t8";
	 reg[61]="$t9";
	 reg[62]="$t9";
	 reg[63]="$k0";
	 reg[64]="$k1";
	 reg[65]="$gp";
	 reg[66]="$gp";
	 reg[67]="$sp";
	 reg[68]="$fp";
	 reg[69]="$ra";
	 
	 v[0]= "0";
	 v[1]= "1";
	 v[2]= "1";
	 v[3]= "3";
	 v[4]= "4";
	 v[5]= "5";
	 v[6]= "6";
	 v[7]= "7";
	 v[8]= "8";
	 v[9]= "9";
	 v[10]= "10";
	 v[11]= "11";
	 v[12]= "12";
	 v[13]= "13";
	 v[14]= "14";
	 v[15]= "15";
	 v[16]= "16";
	 v[17]= "17";
	 v[18]= "18";
	 v[19]= "19";
	 v[20]= "20";
     v[21]= "21";
	 v[22]= "22";
	 v[23]= "23";
	 v[25]= "24";
	 v[26]= "25";
	 v[27]= "26";
	 v[28]= "27";
	 v[29]= "28";
	 v[30]= "29";
	 v[31]= "30";
	 v[32]= "31";
	 v[33]= "32";
	 
	for (  i= 0; i <70; ++i){
		if(strcmp(reg[i],n)==0){
			out = v[i%34];
			return out;
			
		}
	}
  
	
	
	return out;
}
char **regNumberConverter(char **n){
	int i=0;
	char *x;
	while(n[i]!=NULL){
		if(n[i][0]=='$'){ // if  the token is a register
			if(regValid(n[i])){  // checks if the register is valid
			x= converter(n[i]);
			n[i]=x;}
			else {
				printf("Illegal Register %s\n", n[i]);
			error =1;}
		}
		i++;
	}
	return n;
}
	char **progScanner( char* line){
	
		char delimiter[]="	 ,\n";
		char lw[]="() ";
		char *x; 
		 char ** ar;
		 int j;
		int l =0;
		char **reg;
		char c =')';
		char d='(';
		int i=0;
		
		int p=0;
		ar = (char **)malloc(100*sizeof(char *)); //allocate memory for the tokens to be broken up and stored
		for (j=0; j<3; j++)
       *(ar+j) = (char *) malloc(20*sizeof(char *));
		reg = (char **)malloc(100*sizeof(char *)); //allocate memory for the tokens to be broken up and stored
		for (j=0; j<6; j++)
       *(reg+j) = (char *) malloc(20*sizeof(char *));
   
		 x= strtok(line, delimiter);
		 ar[i++]=x;
		 while(x!= NULL) // delimites the instruction into an array
		 {
		 x=strtok(NULL,delimiter);
		 ar[i++]=x;
		 }
		 i=0;
		if(opValid(ar)){ // if it is a valid opcode
		
			if((strcmp(ar[0],"lw")==0)||(strcmp(ar[0],"sw")==0)){ // if it is sw or lw check
			
		
		if( isdigit(ar[2][0]) ){// the offset is a number 
			while ( isdigit(ar[2][i])){// iterates through the number 
				i++;
									  }
		
			if(ar[2][i]=='('){// the char is ")""
				i++;
				if(ar[2][i]=='$'){// if it is a register 
				reg[0][0]=ar[2][i];
				i++;
					while(isalnum(ar[2][i])){ // scans the register into a array to be tested 
						reg[0][++p]=ar[2][i];
						i++; 
						
						}
						
						if(regValid(reg[0])){ // if register is valid
					
							if(ar[2][i]==')'){// next space is a close )
								i++;
								if(!isalnum(ar[2][i])&&!ispunct(ar[2][i])){
								
								x= strtok(ar[2], lw);
								i=2;
								ar[++i]=x;
								while(x!= NULL) // delimites the instruction into an array
								{
									x=strtok(NULL,lw);// remove the  ()
									ar[i++]=x;
									}
									}
								else{ // extra character
									printf("Syntax Error %c\n", ar[2][i]); // sytax error
									error =1;}
							}
							else{ // missing ) 
								printf("Syntax Error %c\n", ar[2][i]); // sytax error
								error =1;}
						}
						
						else{ // invalid register
							printf("Invalid Register %s\n", reg[0]); // sytax error
							error =1;
							}		
								}
				else{ // missing $
						printf("Syntax Error %c\n", ar[2][i]); // sytax error
						error =1;}
				
						}
			else{ // wrong char 
				printf("Syntax Error %s\n", ar[2][i]); // sytax error
				error =1;
				}
	
								}
		else{
			printf("Syntax Error %s\n", ar[1]);
			error =1;
			}
		
		}
		}
else{
			printf("Invalid OP Code %s\n", ar[0]);
			error =1;
}

return ar;}

void memDelay(int a){
	if ((strcmp(EX_MEM.op,"mult")==0)||(strcmp(EX_MEM.op,"addi")==0)||(strcmp(EX_MEM.op,"beq")==0)||(strcmp(EX_MEM.op,"add")==0)||(strcmp(EX_MEM.op,"sub")==0)){
		memCount=0;
	}
	else if((strcmp(EX_MEM.op,"lw")==0)||(strcmp(EX_MEM.op,"sw")==0)){
		memCount=a;
	}
}
void exDelay( int a, int m){
	if ((strcmp(ID_EX.op,"sw")==0)||(strcmp(ID_EX.op,"lw")==0)||(strcmp(ID_EX.op,"addi")==0)||(strcmp(ID_EX.op,"beq")==0)||(strcmp(ID_EX.op,"add")==0)||(strcmp(ID_EX.op,"sub")==0)){
		exCount=a;
	}
	else if(strcmp(ID_EX.op,"mult")==0){
		exCount=m;
	}
	printf("EXDelay excucute: counter is %d\n", exCount);
}