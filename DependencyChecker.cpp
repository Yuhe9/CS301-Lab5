#include "DependencyChecker.h"

DependencyChecker::DependencyChecker(int numRegisters)
/* Creates RegisterInfo entries for each of the 32 registers and creates lists for
 * dependencies and instructions.
 */
{
  RegisterInfo r;

  // Create entries for all registers
  for(int i = 0; i < numRegisters; i++){
    myCurrentState.insert(make_pair(i, r));
  }
}

void DependencyChecker::addInstruction(Instruction i)
/* Adds an instruction to the list of instructions and checks to see if that 
 * instruction results in any new data dependencies.  If new data dependencies
 * are created with the addition of this instruction, appropriate entries
 * are added to the list of dependences.
 */
{
  InstType iType = i.getInstType();
  unsigned int  rs, rt, rd;
  rs = rt = rd = -1;
  
  switch(iType){
  
  case RTYPE:
    rs = i.getRS();
    rt = i.getRT();
    rd = i.getRD();
    
    checkForReadDependence(rs);
    checkForReadDependence(rt);
    checkForWriteDependence(rd);

    break;

  case ITYPE:
    rs = i.getRS();
    rt = i.getRT();
    if(i.getImmediate() != -1){
      checkForReadDependence(rs);
      checkForReadDependence(rt);
    } else{
      checkForReadDependence(rs);
      checkForWriteDependence(rt);
    }

    break;

  case JTYPE:
    break;
  default:
    // do nothing
    break;
  }

  myInstructions.push_back(i);

}

void DependencyChecker::checkForReadDependence(unsigned int reg)
  /* Determines if a read data dependence occurs when reg is read by the current
   * instruction.  If so, adds an entry to the list of dependences. Also updates
   * the appropriate RegisterInfo entry regardless of dependence detection.
   */
{ //check for the read dependence occur when reg is read
   list<Instruction>::iterator it;
   for(it = myInstructions.begin(); it !=  myInstructions.end(); it++){
      if((*it).getInstType() == RTYPE){
         if((int)reg == (*it).getRD())
		        addDependEntry(reg,RAW);
      } else if((*it).getInstType() == ITYPE){
	         if((int)reg == (*it).getRT()) {
	           addDependEntry(reg,RAW);	
           }
      }
}
   //update the RegisterInfo entry              
     myCurrentState[reg].lastInstructionToAccess = reg;             
     myCurrentState[reg].accessType = WRITE;  
}

void DependencyChecker::addDependEntry(unsigned int reg, DependenceType type)
{// iterate the list of registers that dependence involoved
  Dependence myDep;
  myDep.dependenceType = type;
  myDep.registerNumber = reg;
  myDep.previousInstructionNumber = myDep.currentInstructionNumber;
  myDep.currentInstructionNumber = myDep.currentInstructionNumber + 1;
  
  list<Dependence>::iterator it;
   for(it = myDependences.begin(); it != myDependences.end(); it++){
      if(myDep.registerNumber == (*it).registerNumber) {
        myDependences.push_back(myDep);
    }
  }
}



void DependencyChecker::checkForWriteDependence(unsigned int reg)
  /* Determines if a write data dependence occurs when reg is written by the current
   * instruction.  If so, adds an entry to the list of dependences. Also updates 
   * the appropriate RegisterInfo entry regardless of dependence detection.
   */
{
   list<Instruction>::iterator it;
   for(it = myInstructions.begin(); it !=  myInstructions.end(); it++){
      if((*it).getInstType() == RTYPE){
         if((int)reg == (*it).getRD()){
                addDependEntry(reg,WAW);
          }
         if((int)reg == (*it).getRS() || (int)reg == (*it).getRT()) {
		        addDependEntry(reg,WAR);
         }

      } else if((*it).getInstType() == ITYPE){
        if((int)reg == (*it).getRT()){
                addDependEntry(reg,WAW);
        }
	      if((int)reg == (*it).getRS()) {
		            addDependEntry(reg,WAR);
        }
    }

    myCurrentState[reg].lastInstructionToAccess = reg;
    myCurrentState[reg].accessType = WRITE;
  }
}

void DependencyChecker::printDependences()
  /* Prints out the sequence of instructions followed by the sequence of data
   * dependencies.
   */ 
{
  // First, print all instructions
  list<Instruction>::iterator liter;
  int i = 0;
  cout << "INSTRUCTIONS:" << endl;
  for(liter = myInstructions.begin(); liter != myInstructions.end(); liter++){
    cout << i << ": " << (*liter).getAssembly() << endl;
    i++;
  }

  // Second, print all dependences
  list<Dependence>::iterator diter;
  cout << "DEPENDENCES: \nType Register (FirstInstr#, SecondInstr#) " << endl;
  for(diter = myDependences.begin(); diter != myDependences.end(); diter++){
    switch( (*diter).dependenceType){
    case RAW:
      cout << "RAW \t";
      break;
    case WAR:
      cout << "WAR \t";
      break;
    case WAW:
      cout << "WAW \t";
      break;
    default:
      break;
    }

    cout << "$" << (*diter).registerNumber << " \t";
    cout << "(" << (*diter).previousInstructionNumber << ", ";
    cout << (*diter).currentInstructionNumber << ")" << endl;
  }


}
