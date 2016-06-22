 
//David Gibb, Daniel Mathieu, ECE 353, Lab 2, 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include<ctype.h>

#define single 1
#define batch 0
#define numberOfRegisters 32

typedef struct inst {
    char *opcode;
    int destinationReg;
    int sourceReg1;
    int sourceReg2;
    int immediate;
}inst;

typedef struct latch{ 
    char* opcode;
    int destinationRegister;
    int R1;
    int R2;
    int flag;
    int EX_Value;
    struct inst instruction;
} latch;

struct latch IF_ID ={"nop",0,0,0,0,0,NULL};
struct latch ID_EX={"nop",0,0,0,0,0,NULL};
struct latch EX_MEM={"nop",0,0,0,0,0,NULL};
struct latch MEM_WB={"nop",0,0,0,0,0,NULL};
struct inst instructionMemory[512];

int error; counter
int IFKill,IDKill,EXKill,MEMKill,WBKill;
int exCounter,memCounter,ifCounter;
int dataMemory[512]; 
int IFU,IDU,EXU,MEMU,WBU; 
int branchFlag;
int programCounter;
int RegisterValidBit[numberOfRegisters];
int exhold,memhold; 
int registers[numberOfRegisters];
int cyclesForMultInstructions, cyclesForOtherInstructions, cyclesForMemoryAccess;
int simulationMode;		//mode flag, 1 for single-cycle, 0 for batch
int testCounter;
long simulationCycle;
char **progScanner( char* line);
char **regNumberConverter(char **n);
char line[100];


void IF();
void ID();
void EX();
void MEM();
void WB();
void exDelay();
void memDelay();
void parser(char* c, int i);

int main (int argc, char *argv[]){
	
	simulationMode=0;		//mode flag, 1 for single-cycle, 0 for batch
	simulationCycle=0;
	counter=0;
	programCounter=0;
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
	branchflag=0;
	testCounter=0;
	j=0;
	FILE *input=NULL;
	FILE *output=NULL;
	
	printf("The arguments are:");
	for(int i=1;i<argc;i++){
		printf("%s ",argv[i]);
	}
	printf("\n");
	
	if(argc==7){
	
		if(strcmp("-s",argv[1])==0){
			simulationMode=SINGLE;
		}
		
		else if(strcmp("-b",argv[1])==0){
			simulationMode=BATCH;
		}
		
		else{
			printf("Wrong sim mode chosen\n");
			exit(0);
		}
		
		cyclesForMultInstructions=atoi(argv[2]);
		cyclesForOtherInstructions=atoi(argv[3]);
		cyclesForMemoryAccess=atoi(argv[4]);
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
	
	for(i=0; i<512; i++){// allocate memory for the opcode 
		instructionMemory[i].opcode=(char *)malloc(60*sizeof(char));
	}
	
	for (i=0;i<numberOfRegisters;i++){
		registers[i]=0;
	}
	
	//for (i=1;i<=numberOfRegisters;i++){
	//	printf("%d  ",registers[i]);
	//}
	
	
	i=0;
	IFCounter=cyclesForMemoryAccess;
	while (fgets(line,100, input)!=NULL &&error==0&&counter<512) {
		parser(line,i);
		i++;
		counter++;
	}
			
	printf("prerun EXcounter %d\n", exCounter);
	i=0;
	
	while((IFKill+IFKill+EXKill+MEMKill+WBKill)!=5 && error==0){ // run while loop off of all the kills sum to 5 and the error detection 
	
		printf("Clock Cycle: %d\n", simulationCycle); // insert this in loop for the single pass mode
		printf("\n");
	
		WB();
		
		printf("%s  ", "MEM_WB Latch:"" );
		printf("%s  ",MEM_WB.opcode);
		printf("%d  ",MEM_WB.destinationRegister);
		printf("%d  ",MEM_WB.R1);
		printf("%d  ",MEM_WB.R2);
		printf("%d  ",MEM_WB.EX_Value);
		printf("\n");
		printf("\n");
		
		MEM();
		
		printf("%s  ", "EX_MEM Latch: ");
		printf("%s  ",EX_MEM.opcode);
		printf("%d  ",EX_MEM.destinationRegister);
		printf("%d  ",EX_MEM.R1);
		printf("%d  ",EX_MEM.R2);
		printf("%d  ",EX_MEM.EX_Value);
		printf("\n");
		printf("\n");
		
		EX(n,m);
		
		printf("%s  ", "ID_EX Latch: ");
		printf("%s  ",ID_EX.opcode);
		printf("%d  ",ID_EX.destinationRegister);
		printf("%d  ",ID_EX.R1);
		printf("%d  ",ID_EX.R2);
		printf("%d  ",ID_EX.EX_Value);
		printf("\n");
		printf("\n");
		
		ID();
		
		printf("%s  ","IF_ID Latch: ");
		printf("%s  ",IF_ID.opcode);
		printf("%d  ",IF_ID.destinationRegister);
		printf("%d  ",IF_ID.R1);
		printf("%d  ",IF_ID.R2);
		printf("%d  ",IF_ID.EX_Value);
		printf("%s  ",IF_ID.instruction.opcode);
		printf("%d  ",IF_ID.instruction.destinationRegister);
		printf("%d  ",IF_ID.instruction.sourceReg1);
		printf("%d  ",IF_ID.instruction.sourceReg2);
		printf("%d  ",IF_ID.instructin.immediate);
		printf("\n");
		printf("\n");
		
		IF(simulationCycle, argumentTwo);
		
		i++;
		printf("Counter %d\n", IFCounter);
		
		printf("\n");
		printf("\n");
		printf("%s\n", "Instruction Memory");
		
		for (i=1;i<=10;i++){
			printf("%s  ",instructionMemory[i].opcode);
			printf("%d  ",instructionMemory[i].destinationRegister);
			printf("%d  ",instructionMemory[i].sourceReg1);
			printf("%d  ",instructionMemory[i].sourceReg2);
			printf("%d  ",instructionMemory[i].immediate);
			printf("\n");
			
		}
			
		printf("\n");
		printf("%s\n", "Register Values");
		
		for (i=1;i<=numberOfRegisters;i++){
			printf("%d  ", registers[i]);
		}
		
		printf("\n");
		printf("\n");
		printf("%s\n", "Data Memory");
		
		for (i=1;i<=numberOfRegisters;i++){
			printf("%d  ",dataMemory[i]);
		}
			
		printf("%d\n",programCounter);
		simulationCycle+=1;
		testCounter++;
		printf("press ENTER to continue\n");
		while(getchar() != '\n');
		
		printf("%s\n", "-----------------------------------------");
		printf("%s\n", "End of Cycle");
		printf("%s\n", "-----------------------------------------");
		printf("\n");
	}
	
	if(simulationMode==0){
		fprintf(output,"program name: %s\n",argv[5]);
		fprintf(output,"stage utilization: %f  %f  %f  %f  %f \n", IFU, IDU, EXU, MEMU, WBU);
	   // add the (double) stage_counter/simulationCycle for each 
	   // stage following sequence IF ID EX MEM WB
	   	fprintf(output,"register values ");
	   	for (i=1;i<numberOfRegisters;i++){
	   		fprintf(output,"%d  ", registers[i]);
		}
	   	fprintf(output,"%d\n",programCounter);
	}
	
	fclose(input);
	fclose(output);
	return 0;
}
	
void ID(){

	if(ID_EX.flag!=1){//if the ID_EX latch is not full
		if(IF_ID.flag ==1){ // there is an instruction in the IF ID latch
			IF_ID.flag =0; // the latch is loaded
			if( (strcmp(IF_ID.instruction.opcode,"add")==0)|| (strcmp(IF_ID.instruction.opcode,"sub")==0)||(strcmp(IF_ID.instruction.opcode,"mult")==0)){ // R type instructions
				if((RVB[IF_ID.instruction.sourceReg1]!=1)&&(RVB[IF_ID.instruction.sourceReg2]!=1)){ // checks that both registers are valid
					ID_EX.opcode=IF_ID.instruction.opcode;
					ID_EX.R1= registers[IF_ID.instruction.sourceReg1];
					ID_EX.R2= registers[IF_ID.instruction.sourceReg2];
					ID_EX.destinationRegister= IF_ID.instruction.destinationRegister;
					registerValidBit[IF_ID.instruction.destinationRegister]==1;// changes the status of the destination register to avoid data hazards undone at WB stage
					ID_EX.flag=1;// there is now a value in the ID_EX latch
					IDU++;// actually did work
					printf("R type ID Executed\n");
				}
				else {
					IF_ID.flag==1; // data hazard exits so the inst in the IF_ID register is left in the latch for later
				}
			}	
			else if(strcmp( IF_ID.instruction.opcode,"beq")==0){ // branch instructions only use two registers and we ignore labels
				if((registerValidBit[IF_ID.instruction.sourceReg1]!=1)&&(RVB[IF_ID.inatruction.sourceReg2]!=1)){
					ID_EX.opcode=IF_ID.instruction.opcode;
					ID_EX.R1= registers[IF_ID.instruction.sourceReg1];
					ID_EX.R2= registers[IF_ID.instruction.sourceReg2];
					ID_EX.flag=1;
					branchFlag=1;  
					IDU++;
					printf("BRANCH ID EXECUTED\n");
				} 
				else {
					IF_ID.flag==1; // data hazard
				}
			}
			else if(strcmp(IF_ID.instruction.opcode,"haltSimulation")==0){
				IDKill=1;
				ID_EX.opcode=IF_ID.instruction.opcode;
				ID_EX.flag=1;
				printf(" ID EXECUTED haltSimulation\n");
			}
			else if( (strcmp(IF_ID.instruction.opcode,"addi")==0)||(strcmp(IF_ID.instruction.opcode,"lw")==0)||(strcmp(IF_ID.instrution.opcode,"sw"))){ // I type instructions use one source reg and an immediate that goes into the second source reg since there is no mux
				if(RVB[IF_ID.instruction.sourceReg1]!=1){ // checks that both registers are valid
					ID_EX.opcode=IF_ID.instruction.opcode;
					ID_EX.R1= registers[IF_ID.instruction.sourceReg1];
					ID_EX.R2= IF_ID.instruction.immediate;
					ID_EX.destinationRegister= IF_ID.instruction.destinationRegister;
					registerValidBit[IF_ID.instruction.destinationRegister]==1;
					ID_EX.flag=1;
					IDU++;
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


void IF(){

//	if(IFKill ==0){								
		//if(IFCounter==0){						
	//		if(IF_ID.flag!=1){				
	//			if(branchFlag!=1){			
					IF_ID.instruction = instructionMemory[programCounter];				
					IFU++;
					printf("PC %d\n",programCounter);
					
					if(strcmp(instructionMemory[programCounter].opcode,"haltSimulation")==0){
						IFKill=1;
					}
					
					programCounter++;
					printf("IF excecuted %s\n",IF_ID.instruction.opcode);
					IF_ID.flag=1;
//				}
	//		}
	//	}
	//	else IFCounter--;
//	}
}

void EX(){	
			
	if(exhold==0&&strcmp(ID_EX.opcode,"nop")!=0&&ID_EX.flag==1&&EX_MEM.flag==0){	
	exDelay();
	exhold=1;
	}
	
	if(EXKill==0){																
		if (exCount==0){													
			if(EX_MEM.flag!=1){												
				if(ID_EX.flag==1){				
					if(strcmp(ID_EX.opcode,"add")==0){
											
				EX_MEM.EX_Value=ID_EX.R1+ID_EX.R2;			
				EX_MEM.DR=ID_EX.DR; 
				ID_EX.flag=0;
				EX_MEM.flag=1;
				EX_MEM.opcode=ID_EX.opcode;
				EXU++;
				exhold=0;
				printf("EX EXCUTED add\n");				
		}
			
			
		else if (strcmp(ID_EX.opcode,"sub")==0){
		
			EX_MEM.EX_Value=ID_EX.R1-ID_EX.R2;
			EX_MEM.destinationRegister=ID_EX.destinationRegister;
			ID_EX.flag=0;
			EX_MEM.flag=1;
			EX_MEM.opcode=ID_EX.opcode;
			EXU++;
			exhold=0;
			printf("EX EXCUTED sub\n");
		}
		
				
		else if (strcmp(ID_EX.opcode,"mult")==0){
				 // waits to see if a cycles has passed for multiplication 
			EX_MEM.EX_Value=ID_EX.R1*ID_EX.R2;
			EX_MEM.destinationRegister=ID_EX.destinationRegister;
			ID_EX.flag=0;
			EX_MEM.flag=1;
			EX_MEM.opcode=ID_EX.opcode;
			exhold=0;
			EXU++; 
			printf("EX EXCUTED mult\n");
		}
				
		else if (strcmp(ID_EX.opcode,"addi")==0){
			
			EX_MEM.EX_Value=ID_EX.R1+ID_EX.R2;
			EX_MEM.destinationRegister=ID_EX.destinationRegister;
			ID_EX.flag=0;
			EX_MEM.flag=1;
			EX_MEM.opcode=ID_EX.opcode;
			EXU++; 
			exhold=0;
			printf("EX EXCUTED addi\n");
		}
		
				
		else if (strcmp(ID_EX.opcode,"lw")==0){  // needs to check if its okay
	
			EX_MEM.EX_Value=ID_EX.R1+ID_EX.R2;
			EX_MEM.destinationRegister=ID_EX.destinationRegister;
			ID_EX.flag=0;
			EX_MEM.flag=1;;
			EX_MEM.opcode=ID_EX.opcode;
			exhold=0;
			EXU++;
			printf("EX EXCUTED lw\n");
		}
				
				
		else if (strcmp(ID_EX.opcode,"sw")==0){
	
			EX_MEM.EX_Value=ID_EX.R1+ID_EX.R2;
			EX_MEM.destinationRegister=ID_EX.R2;
			ID_EX.flag=0;
			EX_MEM.flag=1;
			EX_MEM.opcoe=ID_EX.opcode;
			EXU++; 
			exhold=0;
			printf("EX EXCUTED sw\n");
		}
			
				
		else if (strcmp(ID_EX.opcode,"beq")==0){
			
			if(ID_EX.R1-ID_EX.R2==0){
			
				EX_MEM.destinationRegister=ID_EX.R2+programCounter; 
				branchFlag=0;
				ID_EX.flag=0;
				RVB[MEM_WB.destinationRegister]=0; // return the status flag to 0 
				EX_MEM.opcode=ID_EX.opcode;
				EXU++; 
				exhold=0;
				EX_MEM.flag=1;
				printf("EX EXCUTED beq\n");
			}
		}
		
		else if (strcmp(ID_EX.opcode,"haltSimulation")==0){
			EXKill=1;
			ID_EX.flag=0;
			EX_MEM.flag=1;
			EX_MEM.opcode=ID_EX.opcode;
			printf("EX Executed haltSimulation \n");}
		
		else{
		
			printf( "instruction not recognised: %s\n ", ID_EX.opcode);
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


void MEM(){	
	
	if(memhold==0&&strcmp(EX_MEM.opcode,"nop")!=0&&MEM_WB.flag!=1&&EX_MEM.flag==1){ 
	memDelay();
	memhold=1;
	}
	
	if(MEMKill==0){ 
		if (memCount==0){ 
			if(MEM_WB.flag!=1){ 
				if(EX_MEM.flag==1){
					if((strcmp(EX_MEM.opcode,"add")==0)||(strcmp(EX_MEM.opcode,"sub")==0)||(strcmp(EX_MEM.opcode,"mult")==0)||(strcmp(EX_MEM.opcode,"addi")==0)||(strcmp(EX_MEM.opcode,"beq")==0)){
						MEM_WB.destinationRegister=EX_MEM.destinationRegister; // these instructions do not need to use the Data memory
						MEM_WB.EX_Value=EX_MEM.EX_Value;
						MEM_WB.opcode=EX_MEM.opcode;
						EX_MEM.flag=0;
						MEM_WB.flag=1;
						memhold=0;
						printf("MEM Executed R Type\n");
						MEMU++;
					}
			
				
					else if (strcmp(EX_MEM.opcode,"lw")==0){
						if(EX_MEM.EX_Value%4==0){
							MEM_WB.EX_Value=dataMemory[EX_MEM.EX_Value]; // gets the value from data memory
							MEM_WB.destinationRegister=EX_MEM.destinationRegister;
							MEM_WB.opcode=EX_MEM.opcode;
							memhold=0;
							EX_MEM.flag=0;
							MEMU++;
							printf("MEM Executed lw\n");
						}
						else {error=1; // the memory offset is not a factor of 4
							printf("Invalid Memory Offset");
						}
					}
			
				 
					else if (strcmp(EX_MEM.opcode,"sw")==0){
						if(EX_MEM.EX_Value%4==0){
							dataMemory[EX_MEM.EX_Value] = registers[EX_MEM.destinationRegister];// this is wrong 
							memhold=0;
							EX_MEM.flag=0;
							RVB[EX_MEM.destinationRegister]=0; // return the status flag to 0 
							MEMU++;
							printf("MEM Executed sw\n");}
							else {error=1; // the memory offset is not a factor of 4
								printf("Invalid Memory Offset");
							}
						}
			
					else if (strcmp(EX_MEM.opcode,"haltSimulation")==0){
						MEMKill=1;
					MEM_WB.flag=1;
					EX_MEM.flag=0;
					MEM_WB.opcode=EX_MEM.opcode;
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
		if((strcmp(MEM_WB.opcode,"add")==0)||(strcmp(MEM_WB.opcode,"sub")==0)||(strcmp(MEM_WB.opcode,"mult")==0)||(strcmp(MEM_WB.opcode,"addi")==0)||(strcmp(MEM_WB.opcode,"lw")==0)){
			registers[MEM_WB.destinationRegister]=MEM_WB.EX_Value; 
			WBU++;
			registerValidBit[MEM_WB.destinationRegister]=0; 
			MEM_WB.flag=0;
			printf("WB Executed R type");
		}	
		
		else if((strcmp(MEM_WB.opcode,"sw")==0)||(strcmp(MEM_WB.opcode,"beq")==0)){
			WBU++;
			MEM_WB.flag=0; 
			printf("WB executed sw or beq");
		}
			
		else if (strcmp(MEM_WB.opcode,"haltSimulation")==0){
		WBKill=1;
		printf("WB executed haltSimulation\n");}	
	}	
}
	
void parser(char* c, int i){
	 char **n =regNumberConverter(progScanner(c));
	// struct inst in;
   char *ptr;
	if( (strcmp("add",n[0])==0)|| (strcmp("sub",n[0])==0)||(strcmp("mult",n[0])==0)){ // R type instructions
	strcpy(instructionMemory[i].opcode,n[0]);
	instructionMemory[i].sourceReg1=(int)strtol(n[2],&ptr,10);
	instructionMemory[i].sourceReg2=(int)strtol(n[3],&ptr,10);
	instructionMemory[i].destinationRegister=(int)strtol(n[1],&ptr,10);
	}
	else if((strcmp("beq",n[0])==0)||(strcmp("addi",n[0])==0)){ // I type instructions
	strcpy(instructionMemory[i].opcode,n[0]);
	 instructionMemory[i].sourceReg1=(int)strtol(n[2],&ptr,10);
	 instructionMemory[i].immediate=(int)strtol(n[3],&ptr,10);
	instructionMemory[i].destinationRegister=(int)strtol(n[1],&ptr,10);
	if(instructionMemory[i].immediate>65535){// the immediate cannot be stored in the 16 bits 
		error=1;
		printf("Immediate Field Can Not Be Stored in 16 Bits");
	}
	}
	else if((strcmp("sw",n[0])==0)||(strcmp("lw",n[0])==0)){
	strcpy(instructionMemory[i].opcode,n[0]);
	 instructionMemory[i].sourceReg1=(int)strtol(n[3],&ptr,10);
	 instructionMemory[i].immediate=(int)strtol(n[2],&ptr,10);
	 instructionMemory[i].destinationRegister=(int)strtol(n[1],&ptr,10);
	if(instructionMemory[i].immediate>65535){// the immediate cannot be stored in the 16 bits 
		error=1;
		printf("Immediate Field Can Not Be Stored in 16 Bits");
	}	
	}
	else if((strcmp("haltSimulation",n[0])==0)){
	strcpy(instructionMemory[i].opcode,n[0]);

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
		if(n[i][0]=='$'){ 
			if(regValid(n[i])){ 
			x=converter(n[i]);
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
		 while(x!= NULL)  {
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
							while(x!= NULL)	{
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
						error =1;
					}
				
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
	return ar;
}

void memDelay(){
	if ((strcmp(EX_MEM.opcode,"mult")==0)||(strcmp(EX_MEM.opcode,"addi")==0)||(strcmp(EX_MEM.opcode,"beq")==0)||(strcmp(EX_MEM.code,"add")==0)||(strcmp(EX_MEM.opcode,"sub")==0)){
		memCount=0;
	}
	else if((strcmp(EX_MEM.opcode,"lw")==0)||(strcmp(EX_MEM.opcode,"sw")==0)){
		memCount=cyclesForMemoryAccess;
	}
}

void exDelay(){
	if ((strcmp(ID_EX.opcode,"sw")==0)||(strcmp(ID_EX.opcode,"lw")==0)||(strcmp(ID_EX.opcode,"addi")==0)||(strcmp(ID_EX.opcode,"beq")==0)||(strcmp(ID_EX.opcode,"add")==0)||(strcmp(ID_EX.opcode,"sub")==0)){
		exCount=cyclesForOtherInstructions;
	}
	else if(strcmp(ID_EX.opcode,"mult")==0){
		exCount=cyclesForMultInstructions;
	}
	printf("EXDelay excucute: counter is %d\n", exCount);
}
