#include <iostream>
#include <string>
#include <cstring>
#include "mmu.h"
#include "pagetable.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, bool printBool);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);

DataType convertStringToDatatype(std::string input);
uint32_t findOffset(uint32_t address, uint32_t element_size, uint32_t num_elements, uint32_t page_size);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint32_t mem_size = 67108864;
    void *memory = malloc(mem_size); // 64 MB (64 * 1024 * 1024)

    // Create MMU and Page Table
    Mmu *mmu = new Mmu(mem_size);
    PageTable *page_table = new PageTable(page_size);

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);

    while (command != "exit") {
        //set command to a char* for using strncmp
        /*
        char cmd[command.length() + 1];
        strcpy(cmd, command.c_str());   
        */

        //tokenize the string
        std::istringstream buffer(command);
        std::istream_iterator<std::string> beg(buffer), end;
        std::vector<std::string> tokens(beg,end);

        //set first token to a char*, called 'cmd', for using strncmp
        char cmd[tokens[0].length()+1];
        strcpy(cmd, tokens[0].c_str());

        /*
        // use this for displaying  input line tokens
        for(std::string st : tokens){
            std::cout << st << ' ';
        }
        */

        // Handle command
        if(strncmp(cmd,"create", 6) == 0){ //if first 6 digits match to 'create'
            //printf("entered the CRE8 func call\n");

            //get text and data sizes
            int text_size = std::stoi(tokens[1]); 
            int data_size = std::stoi(tokens[2]);

            //printf("ts: %d, ds: %d\n", text_size, data_size); //test tokens cast to ints
            createProcess(text_size, data_size, mmu, page_table);
            //printf("flagg");
        }
        else if(strncmp(cmd,"allocate", 8) == 0){
            //printf("entered the ALLOC8 func call\n");

            int pid = std::stoi(tokens[1]);
            std::string var_name = tokens[2];
            DataType type = convertStringToDatatype(tokens[3]);
            int num_elements = std::stoi(tokens[4]);
            //error check then call allocateVariable
            //if(pid);
            allocateVariable(pid, var_name, type, num_elements, mmu, page_table, true);
        }
        else if(strncmp(cmd,"set", 3) == 0){
            printf("entered the SET func call\n");
        }
        else if(strncmp(cmd,"print", 5) == 0){
            //printf("entered the PRINT func call\n");
            if(tokens[1] == "mmu"){
                mmu->print();
            }
            else if(tokens[1] == "page"){
                page_table->print();
            }
        }
        else if(strncmp(cmd,"free", 4) == 0){
            //printf("entered the FREE func call\n");
            //get pid and var_name
            //printf("flag\n");
            uint32_t pid = std::stoi(tokens[1]);
            printf("pid = %d \n", pid);
            //printf("flag2\n");
            std::string var_name = tokens[2];
            printf("var_name = %s \n", var_name.c_str());
            printf("flag3\n");
            //ERROR CHECK here for process not exists AND for variable not exists
            mmu->getProc(pid);
            printf("flag3\n");
            freeVariable(pid, var_name, mmu, page_table);
        }
        else if(strncmp(cmd,"terminate", 9) == 0){
            printf("entered the TERMIN8 func call\n");
        }
        else{ //else, unrecognized command. ERROR.
            printf("error: command not recognized\n");
        }
        
        // Get next command
        std::cout << "> ";
        std::getline (std::cin, command);
        
        //printf("\n %s \n",command);
    }

    // Clean up
    free(memory);
    delete mmu;
    delete page_table;

    return 0;
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}

//no error check needed in createProcess
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
{
    //   1- create new process in the MMU
    //printf("flg");
    uint32_t pid = mmu->createProcess(); //create process and save pid
    //printf("flg");

    //   2- allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    allocateVariable(pid, "<TEXT>", DataType::Char, text_size, mmu, page_table, false);
    allocateVariable(pid, "<GLOBALS>", DataType::Char, data_size, mmu, page_table, false);
    allocateVariable(pid, "<STACK>", DataType::Char, 65536, mmu, page_table, false); 

    //   3- print pid
    printf("%d\n",pid);
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, bool printBool)
{
    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    // find total size to be allocated
    uint32_t element_size;
    if(type == Char){
        element_size = 1;
    }
    else if(type == Short){
        element_size = 2;
    }
    else if((type == Int) || (type == Float)){
        element_size = 4;
    }
    else if((type == Long) || (type == Double)){
        element_size = 8;
    }
    uint32_t total_size = element_size * num_elements;
    //find free space in MMU. 
    //std::vector<Variable*> var_list = mmu->getProc(pid)->variables; //get var_list for this PID
    /*std::vector<Variable*> var_list;
    for(int i = 0; i < mmu->getProc(pid)->variables.size(); i++){ //for each variable in proc
        var_list.push_back(mmu->getProc(pid)->variables[i]);
    }*/
    Process* proc = mmu->getProc(pid);
    
    uint32_t index_in_var_list = -1;
    for(int i = 0; i < proc->variables.size(); i++){ //loop through var_list
        if((proc->variables[i]->type == FreeSpace) && proc->variables[i]->size >= total_size){ //if valid spot to allocate
            index_in_var_list = i;
            break; //break here since FIRST spot is selected
        }
    }
    if(index_in_var_list == -1){ //no spot found
        printf("error. no open space in process '%d' with enough memory\n", pid);
        //exit(-1) //exit program
        exit(-1);
    }

    Variable* var_to_replace = proc->variables[index_in_var_list]; //this is the free space
    
    //adjust page table accordingly
    int page_size = page_table->getPageSize();
    int page_start = page_table->getNextPageIndex(pid);
    int page_index = page_start; //this starts at the 'start_page' for allocation
    int memory_allocated = page_index * page_size;
    int offset = findOffset(var_to_replace->virtual_address, element_size, num_elements, page_size);

    while(var_to_replace->virtual_address + offset + total_size > memory_allocated){
        //printf("y");
        page_table->addEntry(pid, page_index);
        memory_allocated += page_size;
        page_index++; //in case there is multiple pages to be allocated.
    }
    if(offset > 0){ //needed to shift, so a freespace needed beforehand.
        mmu->addVariableToProcess(pid, "<FREE_SPACE>", FreeSpace, offset, var_to_replace->virtual_address);
    }
    
    //if free space > space needed for var, add new freespace variable in mmu for leftovers
    if(var_to_replace->size > total_size){
        uint32_t leftover_free_space = var_to_replace->size - total_size;
        uint32_t new_address_for_free_space = var_to_replace->virtual_address + total_size;

        mmu->addVariableToProcess(pid, "<FREE_SPACE>", FreeSpace, leftover_free_space, new_address_for_free_space);
    }

    //   - insert variable into MMU
    var_to_replace->name = var_name;
    var_to_replace->type = type;
    var_to_replace->size = total_size;
    var_to_replace->virtual_address += offset; //might be 0.


    //   - print virtual memory address 
    if(printBool){
        printf("%d\n", var_to_replace->virtual_address);
    }
    /*
     how can we NOT have address print out for createVariable() calls of allocateVariable()?
     ---could duplicate function and have no print at end
     OR 
     ---could add a parameter that signals if a print is necessary and exclude
        createVariable() calls to allocateVariable().
    */
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    //   - look up physical address for variable based on its virtual address / offset
    //   - insert `value` into `memory` at physical address
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array) 
}

//almost done w freeVariable. 
/*
weird issue. can free variables created
*/
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    //   - remove entry from MMU
    // Change the variable name and type to represent free space
    printf("flag"); 
    Variable *var = mmu->getVar(pid, var_name);
    var->name = "<FREE_SPACE>";
    var->type = FreeSpace;
    // Check if either the variable just before it and/or just after it are also free space - if so merge them into one larger free space
    printf("flag2");
    mmu->mergeFreeSpace(pid, var);


    // !!  - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process
}

DataType convertStringToDatatype(std::string input){
    DataType type;
    if(input == "char"){
        type = Char;
    }
    else if(input == "short"){
        type = Short;
    }
    else if(input == "int"){
        type = Int;
    }
    else if(input == "float"){
        type = Float;
    }
    else if(input == "long"){
        type = Long;
    }
    else if(input == "double"){
        type = Double;
    }
    else{
        //if this is hit, invalid request
        printf("failure in convertStringToDataType() in main.cpp\n");
    }
    return type;
}

uint32_t findOffset(uint32_t address, uint32_t element_size, uint32_t num_elements, uint32_t page_size){
    for(int i = 0; i < num_elements; i++){
        int element_address = (i * element_size) + address;
        for(int j = 1; j < element_size; j++){
            int temp = element_address + j;
            if(temp % page_size == 0){
                return j;
            }
        }
    }
    return 0;
}