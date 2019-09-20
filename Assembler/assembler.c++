/*
Name: Dakoda Patterson
Class: CS 3421 
Assignment: Assignment 2
Compile: "g++ -o assem assembler.c++" 
Run: "./assem <file.asm "
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <string.h>
#include <cctype>
using namespace std;

typedef std::pair<std::string,unsigned int> codePair;

const size_t MAXLINE = 80;
const size_t MAXREG  = 5;  
const size_t MAXIMM  = 20;
const size_t MAXTARG = 30;
const size_t MAXLABEL = 30;
const size_t MAXPROGRAM = 32768;

void skipLabel (char *, size_t); 
void printLabelSummary(std::map<std::string,unsigned int> &, std::map<std::string,unsigned int> &); 

int main()
{
    std::map <std::string,int> argTable;
    argTable.insert(codePair("zero",0)); argTable.insert(codePair("at",1)); argTable.insert(codePair("v0",2));
    argTable.insert(codePair("v1",3)); argTable.insert(codePair("a0",4)); argTable.insert(codePair("a1",5));
    argTable.insert(codePair("a2",6)); argTable.insert(codePair("a3",7)); argTable.insert(codePair("t0",8));
    argTable.insert(codePair("t1",9)); argTable.insert(codePair("t2",10)); argTable.insert(codePair("t3",11));
    argTable.insert(codePair("t4",12)); argTable.insert(codePair("t5",13)); argTable.insert(codePair("t6",14));
    argTable.insert(codePair("t7",15)); argTable.insert(codePair("s0",16)); argTable.insert(codePair("s1",17));
    argTable.insert(codePair("s2",18)); argTable.insert(codePair("s3",19)); argTable.insert(codePair("s4",20));
    argTable.insert(codePair("s5",21)); argTable.insert(codePair("s6",22)); argTable.insert(codePair("s7",23));
    argTable.insert(codePair("t8",24)); argTable.insert(codePair("t9",25)); argTable.insert(codePair("k0",26));
    argTable.insert(codePair("k1",27)); argTable.insert(codePair("gp",28)); argTable.insert(codePair("sp",29));
    argTable.insert(codePair("fp",30)); argTable.insert(codePair("ra",31));
    
    std::map <std::string,int> opcodeTable;
    opcodeTable.insert(codePair("addiu",9)); opcodeTable.insert(codePair("addu",0)); opcodeTable.insert(codePair("and",0));
    opcodeTable.insert(codePair("beq",4)); opcodeTable.insert(codePair("bne",5)); opcodeTable.insert(codePair("div",0));
    opcodeTable.insert(codePair("j",2)); opcodeTable.insert(codePair("lw",35)); opcodeTable.insert(codePair("mfhi",0));
    opcodeTable.insert(codePair("mflo",0)); opcodeTable.insert(codePair("mult",0)); opcodeTable.insert(codePair("or",0));
    opcodeTable.insert(codePair("slt",0)); opcodeTable.insert(codePair("subu",0)); opcodeTable.insert(codePair("sw",43));
    opcodeTable.insert(codePair("syscall",0));
    
    std::map <std::string,int> funcTable;
    funcTable.insert(codePair("addu",33)); funcTable.insert(codePair("and",36)); funcTable.insert(codePair("div",26));
    funcTable.insert(codePair("mfhi",16)); funcTable.insert(codePair("mflo",18)); funcTable.insert(codePair("mult",24));
    funcTable.insert(codePair("or",37)); funcTable.insert(codePair("slt",42)); funcTable.insert(codePair("subu",35));
    funcTable.insert(codePair("syscall",12));
    
    std::map <std::string,unsigned int> textOffset;
   
    std::map <std::string,unsigned int> dataOffset;
    
    struct {
        union   {
            struct {
                        unsigned int funct:6;
                        unsigned int shamt:5;
                        unsigned int rd:5;
                        unsigned int rt:5;
                        unsigned int rs:5;
                        unsigned int opcode:6;
                    } rFormat;
            struct  {
                        unsigned int imm:16;
                        unsigned int rt:5;
                        unsigned int rs:5;
                        unsigned int opcode:6;
                    } iFormat;
            struct  {
                        unsigned int address:26;
                        unsigned int opcode:6;
                    } jFormat;
            unsigned int encoding; 
                } u;
            } instructions[MAXPROGRAM] = {0}; 
    
   
    int dataArray[MAXPROGRAM] = {0};
    
    
    unsigned int textLineNumber = 0;
    unsigned int dataLineNumber = 0;
    
    bool isData = 0;
    
    char line[MAXLINE]={0};
    char oper[MAXLINE] ={0};
    char rd[MAXREG] = {0}, rs[MAXREG] = {0}, rt[MAXREG] = {0};
    char imm[MAXIMM] = {0};
    char targ[MAXTARG] = {0};
    char label[MAXLABEL] = {0};
    
    char progInstructions[MAXPROGRAM][MAXLINE]; 
  
    size_t numLines = 0;
    while (fgets(progInstructions[numLines],MAXLINE,stdin))
        ++numLines; 
    
    size_t labelIter = 0;
    while (progInstructions[labelIter][0] != '\0') 
    {
        strcpy(line,progInstructions[labelIter]);
    
        if (sscanf(line, " #%s", oper) == 1)
        {
            ++labelIter;
            continue; 
        }
        
        sscanf(line, " %s", label);              
        size_t labelLength = strlen(label);    
        
        
        if (label[labelLength-1] == ':') 
        {
            skipLabel(line, labelLength); 
            
            char labelPair[MAXLABEL] = {0};
            sscanf(label, " %[^:]", labelPair); 
            
            if (!isData) 
            {
                textOffset.insert(codePair(labelPair,textLineNumber)); 
            }
            else 
            {
                dataOffset.insert(codePair(labelPair,dataLineNumber));
            }
            
        } 
        if (sscanf(line, " .%s", oper) == 1)
        {
            if (!strcmp(oper,"data")) 
            {
                isData = 1;
                ++labelIter;
                continue; 
            }
            if (!strcmp(oper,"text")) 
            {
                ++labelIter;
                continue; 
            }
        }
        
        
        if (!isData)
        {
            ++textLineNumber;
        }
        else 
        {
            int arg;
            sscanf(line, " .%s %d",oper,&arg);
            if (!strcmp(oper,"word"))
            {
                
                while(1)
                {
                    size_t lineIter = 0;
                    while (line[lineIter] != ',' && line[lineIter] != '\0')
                    {
                        ++lineIter;
                    }
                    
                    if (line[lineIter] == '\0')
                        break; 
						
                    size_t lineLength = strlen(line);
                    ++lineIter;
                 
                    for (size_t i = lineIter; i < lineLength; ++i)
                    {
                        line[i-lineIter] = line[i];
                    }
                    line[lineLength-lineIter] = '\0';
                    ++dataLineNumber;
                } 
                ++dataLineNumber; 
            }
            else if (!strcmp(oper,"space"))
            {
                dataLineNumber += arg;
            }
            else
            {
                std::cerr << "Unsupported directive!\n";
            }
        }
        
        ++labelIter;
    } 

  
    textLineNumber  = 0; 
    dataLineNumber  = 0;  
    isData          = 0;  
    
    size_t instIter = 0;
    while (progInstructions[instIter][0] != '\0') 
    {
        strcpy(line,progInstructions[instIter]);
		
        if (sscanf(line, " #%s", oper) == 1)
        {
            ++instIter;
            continue; 
        }
        
        sscanf(line, "%s", label);
        size_t labelLength = strlen(label);
        if (label[labelLength-1] == ':')
            skipLabel(line, labelLength);
        
        
        if (sscanf(line, " .%s", oper) == 1)
        {
            if (!strcmp(oper,"data"))
            {
                isData = 1;
                ++instIter;
                continue; 
            }
            else if (!strcmp(oper,"text"))
            {
                ++instIter;
                continue; 
            }
            else if (!strcmp(oper,"word"))
            {
                int arg;
                
                sscanf(line, " .%s %d",oper,&arg); 
                dataArray[dataLineNumber] = arg; 
                ++dataLineNumber;
                
                while(1)
                {
                    size_t lineIter = 0;
                    while (line[lineIter] != ',' && line[lineIter] != '\0')
                    {
                        ++lineIter;
                    }
                
                    if (line[lineIter] == '\0')
                        break; 
                
                   
                    size_t lineLength = strlen(line);
                    ++lineIter; 
                    
                    for (size_t i = lineIter; i < lineLength; ++i)
                    {
                        line[i-lineIter] = line[i];
                    }
                    line[lineLength-lineIter] = '\0';
                
                    sscanf(line, " %d",&arg);
                    dataArray[dataLineNumber] = arg;
                    ++dataLineNumber;
                } 

            }
            else if (!strcmp(oper,"space"))
            {
                int arg;
                
                sscanf(line, " .%s %d",oper,&arg); 
				
                for (size_t i = 0; i < arg; ++i)
                {
                dataArray[dataLineNumber+i] = 0;
                }
                dataLineNumber += arg; 
            }

        }
        
        else if (sscanf(line, "%s $%[^, \n] , $%[^, \n] , $%s",oper,rd,rs,rt) == 4)
        {
 
            instructions[textLineNumber].u.rFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.rFormat.rs = argTable[rs];
            instructions[textLineNumber].u.rFormat.rd = argTable[rd];
            instructions[textLineNumber].u.rFormat.rt = argTable[rt];
            instructions[textLineNumber].u.rFormat.shamt = 0;
            instructions[textLineNumber].u.rFormat.funct = funcTable[oper];
            
            ++textLineNumber; 
            
        }
        else if (sscanf(line, "%s $%[^, \n] , $%[^, \n] , %s", oper, rt, rs, imm) == 4)
        {
            
            instructions[textLineNumber].u.iFormat.opcode = opcodeTable[oper];
            
            if (!strcmp(oper,"addiu")) 
            {
                instructions[textLineNumber].u.iFormat.rt = argTable[rt];
                instructions[textLineNumber].u.iFormat.rs = argTable[rs];
                instructions[textLineNumber].u.iFormat.imm = atoi(imm);
            }
            else
            {
                if (textOffset.count(imm) == 0)
                {
                    std::cerr << "could not find label " << imm;
                    exit(1);
                }
                instructions[textLineNumber].u.iFormat.rt = argTable[rs];
                instructions[textLineNumber].u.iFormat.rs = argTable[rt];
                instructions[textLineNumber].u.iFormat.imm = textOffset[imm] - textLineNumber;
            }
            ++textLineNumber; 
        }
        else if (sscanf(line, "%s $%[^, ] , %[^( ] ( $%[^) ])", oper, rt, imm, rs) == 4)
        {
           
            bool isNumber = 0;
            if (dataOffset.count(imm) == 0)
            {
                
                size_t immLength = strlen(imm);
                for (size_t i = 0; i < immLength; ++i)
                {
                    if (!isdigit(imm[i]))
                    {
                        isNumber = 0; 
                        std::cerr << "could not find label " << imm;
                        exit(1); 
                    }
                    ++i; 
                }
                isNumber = 1; 
            }
            
            instructions[textLineNumber].u.iFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.iFormat.rt = argTable[rt];
            instructions[textLineNumber].u.iFormat.rs = argTable[rs];
            if (isNumber)
            {
                instructions[textLineNumber].u.iFormat.imm = atoi(imm);
            }
            else
            {
                instructions[textLineNumber].u.iFormat.imm = dataOffset[imm];
            }
            
            ++textLineNumber;
        }
        else if (sscanf(line, "%s $%[^,],$%s", oper, rs, rt) == 3)
        {
         
            instructions[textLineNumber].u.rFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.rFormat.rs = argTable[rs];
            instructions[textLineNumber].u.rFormat.rd = 0;
            instructions[textLineNumber].u.rFormat.rt = argTable[rt];
            instructions[textLineNumber].u.rFormat.shamt = 0;
            instructions[textLineNumber].u.rFormat.funct = funcTable[oper];
            
            ++textLineNumber; 
        }
        else if (sscanf(line, "%s $%s", oper, rd) == 2)
        {
           
            instructions[textLineNumber].u.rFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.rFormat.rs = 0;
            instructions[textLineNumber].u.rFormat.rd = argTable[rd];
            instructions[textLineNumber].u.rFormat.rt = 0;
            instructions[textLineNumber].u.rFormat.shamt = 0;
            instructions[textLineNumber].u.rFormat.funct = funcTable[oper];
            
            ++textLineNumber; 
        }
        else if (sscanf(line, "%s %s",oper,targ) == 2)
        {
          
            if (textOffset.count(targ) == 0)
            {
                std::cerr << "could not find label " << targ;
                exit(1);
            }
            
            instructions[textLineNumber].u.jFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.jFormat.address = textOffset[targ];
            
            ++textLineNumber;
        }
        else if (sscanf(line,"%s",oper) == 1)
        {
           
            instructions[textLineNumber].u.rFormat.opcode = opcodeTable[oper];
            instructions[textLineNumber].u.rFormat.rs = 0;
            instructions[textLineNumber].u.rFormat.rd = 0;
            instructions[textLineNumber].u.rFormat.rt = 0;
            instructions[textLineNumber].u.rFormat.shamt = 0;
            instructions[textLineNumber].u.rFormat.funct = funcTable[oper];
            
            ++textLineNumber;
        }
        else
        {
            std::cout << "Unsupported input type.\n\n";
        }
        
        ++instIter;
    } 
    
    std::cout << textLineNumber << " "; 
    std::cout << dataLineNumber << "\n";
    for (size_t i = 0; i < textLineNumber; ++i)
    {
        printf("%08x", instructions[i].u.encoding);
        printf("\n");
    }
    for (size_t i = 0; i < dataLineNumber; ++i) 
    {
        printf("%08x", dataArray[i]);
        printf("\n");
    }
}


void skipLabel(char * line, size_t labelLength)
{
    size_t lineLength = strlen(line);
  
    for (size_t i = labelLength; i < lineLength; ++i)
    {
        line[i-labelLength] = line[i]; 
    }
    line[lineLength-labelLength] = '\0';
}