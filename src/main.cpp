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
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);

std::vector<int> pid_list;

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
        char cmd[tokens[0].length()];
        strcpy(cmd, tokens[0].c_str());

        /*
        // use this for displaying  input line tokens
        for(std::string st : tokens){
            std::cout << st << ' ';
        }
        */

        // Handle command
        if(strncmp(cmd,"create", 6) == 0){ //if first 6 digits match to 'create'
            printf("entered the CRE8 func call\n");

            int text_size= std::stoi(tokens[1]); 
            int data_size = std::stoi(tokens[2]);
            
            //printf("ts: %d, ds: %d\n", text_size, data_size); //test tokens cast to ints
            createProcess(text_size, data_size, mmu, page_table);
        }
        
        else if(strncmp(cmd,"allocate", 8) == 0){
            printf("entered the ALLOC8 func call\n");
        }
        else if(strncmp(cmd,"set", 3) == 0){
            printf("entered the SET func call\n");
        }
        else if(strncmp(cmd,"print", 5) == 0){
            printf("entered the PRINT func call\n");
        }
        else if(strncmp(cmd,"free", 4) == 0){
            printf("entered the FREE func call\n");
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
    uint32_t pid = mmu->createProcess(); //create process and save pid
    pid_list.push_back(pid);    //add pid to list

    //   2- allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
    allocateVariable(pid, "<TEXT>", DataType::Char, text_size, mmu, page_table);
    allocateVariable(pid, "<GLOBALS>", DataType::Char, data_size, mmu, page_table);
    allocateVariable(pid, "<STACK>", DataType::Char, 65536, mmu, page_table); //65536 is bytes size for this project

    //   3- print pid
    printf("%d\n",pid);
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - find first free space within a page already allocated to this process that is large enough to fit the new variable
    //   - if no hole is large enough, allocate new page(s)
    //   - insert variable into MMU
    //   - print virtual memory address 
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
    // TODO: implement this!
    //   - look up physical address for variable based on its virtual address / offset
    //   - insert `value` into `memory` at physical address
    //   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
    //           multiple elements of an array) 
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove entry from MMU
    //   - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
    // TODO: implement this!
    //   - remove process from MMU
    //   - free all pages associated with given process

}