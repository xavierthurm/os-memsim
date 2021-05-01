#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->type = DataType::FreeSpace;
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

void Mmu::addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
{
    int i;
    Process *proc = NULL;
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }

    Variable *var = new Variable();
    var->name = var_name;
    var->type = type;
    var->virtual_address = address;
    var->size = size;
    if (proc != NULL)
    {
        proc->variables.push_back(var);
    }
}

void Mmu::print()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;

    /* use for format checking on print
    printf(" %4u | %-13s | 0x%08X   | %10u \n", 1024, "temperature", 0,  300);
    printf(" %4u | %-13s | 0x%08X   | %10u \n", 1024, "test", 76,  1);
    */

    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            // TODO: print all variables (excluding <FREE_SPACE> entries)
            if(_processes[i]->variables[j]->type != FreeSpace){
                printf(" %4u | %-13s | 0x%08X   | %10u \n", _processes[i]->pid, _processes[i]->variables[j]->name.c_str(), _processes[i]->variables[j]->virtual_address, _processes[i]->variables[j]->size);
            }
        }
    }
}


void Mmu::mergeFreeSpace(uint32_t pid, Variable* var){
    Process *proc = getProc(pid);
    Variable *var_before = NULL;
    Variable *var_after = NULL;
    int before_index, after_index;

    for(int i = 0; i < proc->variables.size(); i++){
        if(proc->variables[i]->virtual_address == var->virtual_address){
            //store the variables before and after for potential merging
            if(i > 0){ //if there is a var before input var
                var_before = proc->variables[i-1];
                int before_index = i-1;
            }
            if(i < proc->variables.size()-1){
                var_after = proc->variables[i+1];
                int after_index = i+1;
            }
        }    
    }
    
    //merge, if necessary
    if(var_before != NULL && var_before->type == FreeSpace){
        //size is now size of var plus the free space before it
        var->size += var_before->size;
        //var must now begin where var_before began
        var->virtual_address = var_before->virtual_address;
        //erase var_before. it is part of var now
        proc->variables.erase(proc->variables.begin() + before_index); 
    }
    if(var_after != NULL && var_after->type == FreeSpace){ //if free space after
        //size is now size of var plus the free space after it
        var->size += var_after->size;
        //erase var_after. it is part of var now
        proc->variables.erase(proc->variables.begin() + after_index);
    }
    
}   
std::vector<Variable*> Mmu::getVarList(uint32_t pid) {
    int i;
    Process *proc = getProc(pid);
    for (i = 0; i < _processes.size(); i++)
    {
        if (_processes[i]->pid == pid)
        {
            proc = _processes[i];
        }
    }
    return proc->variables;
}

Process* Mmu::getProc(uint32_t pid){
    for(int i = 0; i < _processes.size(); i++){
        if(_processes[i]->pid == pid){ //process (pid) found!
            return _processes[i];
        }
    }
    //this should not execute. only if input pid is bad.
    printf("error: getProc() in mmu.cpp FAILED!");
    exit(-1);
}

Variable* Mmu::getVar(uint32_t pid, std::string var_name){
    Process *proc = getProc(pid);

    for(int i =0; i < proc->variables.size(); i++){
        if(proc->variables[i]->name == var_name){ //variable found!
            return proc->variables[i];
        }
    }
    //this should not execute. only if input pid or var_name is bad.
    printf("error: getVar() in mmu.cpp FAILED!");
    exit(-1);
}