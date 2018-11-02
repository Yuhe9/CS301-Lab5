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
  Opcode o = i.getOpcode();
  switch(iType){
  
  case RTYPE:
    // get register fields for the R-type instruction
    rs = i.getRS();
    rt = i.getRT();
    rd = i.getRD();
    
    // check if the R-type instruction has registers rs, rd, rt    
    if(myOpcodeTable.RSposition(o) != -1) {   
        checkForReadDependence(rs);
    }
    if(myOpcodeTable.RTposition(o) != -1) {
        checkForReadDependence(rt);
    }
    if(myOpcodeTable.RDposition(o) != -1) { 
        checkForWriteDependence(rd);
    }

    break;
  //get register fields for the I-type instruction
  case ITYPE:
    rs = i.getRS();
    rt = i.getRT();

    //check if the I-type instruction contains an immediate field
    if(i.getImmediate() != -1){
        checkForReadDependence(rs);
        checkForWriteDependence(rt);
    }
    break;

  case JTYPE:
    break;//J type instructions do not have registers
  default:
    // do nothing
    break;
  }
  // add the instruction to the list
  myInstructions.push_back(i);

}

void DependencyChecker::checkForReadDependence(unsigned int reg)
  /* Determines if a read data dependence occurs when reg is read by the current
   * instruction.  If so, adds an entry to the list of dependences. Also updates
   * the appropriate RegisterInfo entry regardless of dependence detection.
   */
{ 
   // check if the register is valid
   if(reg >= 0 && reg < NumRegisters){
       int i = myInstructions.size();
           // check for the previous access type to determine if it is a RAW dependence               
	   if(myCurrentState[reg].accessType == WRITE) {
                addDependEntry(reg, RAW);
       }
   
   //update the RegisterInfo entry              
   RegisterInfo updateInfo;
   updateInfo.lastInstructionToAccess = i;
   updateInfo.accessType = READ;
   myCurrentState.at(reg) = updateInfo;
   }
}     
       

void DependencyChecker::addDependEntry(unsigned int reg, DependenceType type)
//helper method to add an entry to the list of dependences
{   
    // constructe a dependency entry and add it into list of dependences
    Dependence dep;
    dep.dependenceType = type;
    dep.registerNumber = reg;
    dep.previousInstructionNumber = myCurrentState.at(reg).lastInstructionToAccess;
    dep.currentInstructionNumber = myInstructions.size() ;
    myDependences.push_back(dep);
}



void DependencyChecker::checkForWriteDependence(unsigned int reg)
  /* Determines if a write data dependence occurs when reg is written by the current
   * instruction.  If so, adds an entry to the list of dependences. Also updates 
   * the appropriate RegisterInfo entry regardless of dependence detection.
   */
{
   // check if the register is valid
   if(reg >= 0 && reg < NumRegisters){
      int i = myInstructions.size();
      //check for the previous access type to determine if it is a WAW or WAR dependency
      if(myCurrentState[reg].accessType == WRITE) {
         addDependEntry(reg,WAW);
      }
      if(myCurrentState[reg].accessType == READ) {
         addDependEntry(reg,WAR);
      }
   
      //update the RegisterInfo entry
      RegisterInfo updateInfo;
      updateInfo.lastInstructionToAccess = i;
      updateInfo.accessType = WRITE;
      myCurrentState.at(reg) = updateInfo;
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
