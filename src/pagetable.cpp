#include "pagetable.h"

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
}

PageTable::~PageTable()
{
}

std::vector<std::string> PageTable::sortedKeys()
{
    std::vector<std::string> keys;

    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        keys.push_back(it->first);
    }

    std::sort(keys.begin(), keys.end(), PageTableKeyComparator());

    return keys;
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    std::vector<std::string> keys = sortedKeys();
    int frame = 0;
    bool frames[keys.size()];

    // Find free frame
    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        frames[it->second] = true;
    }
    frame = keys.size();
    for(int i = 0; i < keys.size(); i++) 
    {
        if(!frames[i]){
            frame = i;
            break;
        }
    }
    _table[entry] = frame;
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{
    // Convert virtual address to page_number and page_offset. (like in class)
    int page_number = virtual_address / _page_size;
    int page_offset = virtual_address % _page_size;

    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    if (_table.count(entry) > 0){ //if it exists in table
        address = _table[entry] * _page_size + page_offset;
    }

    return address;
}

void PageTable::print()
{
    int i;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    std::vector<std::string> keys = sortedKeys();

    for (i = 0; i < keys.size(); i++)
    {
        // TODO: print all pages.
        //the position of "|" in the key can be found using: keys[i].find("|")
        //get pid. AKA keys[i] from beginning to "|"
        std::string pid = keys[i].substr(0, keys[i].find("|"));
        //get page num. AKA keys[i] from "|" to end
        std::string page_number = keys[i].substr(keys[i].find("|") + 1);
        //get frame num
        int frame_number = _table[keys[i]];
        //print
        printf(" %4s | %11s | %12d \n", pid.c_str(), page_number.c_str(), frame_number);
    }
}

int PageTable::getPageSize(){
    return _page_size;
}

int PageTable::getNextPageIndex(uint32_t pid){
    int i = 0;
    std::string temp_entry = std::to_string(pid) + "|" + std::to_string(i);
    while(_table.count(temp_entry) > 0){
        i++;
        temp_entry = std::to_string(pid) + "|" + std::to_string(i);
    }
    return i;
}
