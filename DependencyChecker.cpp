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
  register rs, rt, rd;
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
    if(i.getImmLabel()){
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
{
  // iterate the list of registers that dependence involoved
  struct Dependence myDep;
  myDep.dependenceType = RAW;
  myDep.registerNumber = reg;
  myDep.previousInstructionNumber = myDependence.currentInstructionNumber;
  myDep.currentInstructionNumber = myDependence.currentInstructionNumber + 1;

  for(int i = 0; i < myDependence.size(); i++{
    if(myDep.registerNumber == (myDependence.get(i)).registerNumber)
    myDependence.push_back(myDep);

  }

  //update the RegisterInfo entry
  r.lastInstructionToAccess = reg;
  r.accessType = WRITE;   
}


void DependencyChecker::checkForWriteDependence(unsigned int reg)
  /* Determines if a write data dependence occurs when reg is written by the current
   * instruction.  If so, adds an entry to the list of dependences. Also updates 
   * the appropriate RegisterInfo entry regardless of dependence detection.
   */
{
  
  // Your code here

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
