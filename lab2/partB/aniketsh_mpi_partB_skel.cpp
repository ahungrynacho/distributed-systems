#include "mpi.h"
#include <algorithm>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
// Process information in order.
const static int ARRAY_SIZE = 130000;
using Lines = char[ARRAY_SIZE][16];

// To remove punctuations
struct letter_only: std::ctype<char> 
{
    letter_only(): std::ctype<char>(get_table()) {}

    static std::ctype_base::mask const* get_table()
    {
        static std::vector<std::ctype_base::mask> 
            rc(std::ctype<char>::table_size,std::ctype_base::space);

        std::fill(&rc['A'], &rc['z'+1], std::ctype_base::alpha);
        return &rc[0];
    }
};

void DoOutput(std::string word, int result)
{
    std::cout << "Word Frequency: " << word << " -> " << result << std::endl;
}

// ***************** Add your functions here *********************
// How do scoping rules work in MPI?
/*int scatter(char ** lines, int chunkSize, std::string word) {
    int local_count = 0;
    char * recv_buf = new char[chunkSize][16];
    
    MPI_Scatter(lines, chunkSize, MPI_CHAR, recv_buf, chunkSize, MPI_CHAR, 0, MPI_COMM_WORLD);
    for (int i = 0; i < chunkSize; ++i) {
        if (recv_buf[i] == word)
            local_count++;
    }
    return local_count;
}
*/

int main(int argc, char* argv[])
{
    int processId;
    int num_processes;
    int word_count;
    int *to_return = NULL;
    double start_time, end_time;
 
    // Setup MPI
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &processId);
    MPI_Comm_size( MPI_COMM_WORLD, &num_processes);
 
    // Three arguments: <input file> <search word> <part B1 or part B2 to execute>
    if(argc != 4)
    {
        if(processId == 0)
        {
            std::cout << "ERROR: Incorrect number of arguments. Format is: <filename> <word> <b1/b2>" << std::endl;
        }
        MPI_Finalize();
        return 0;
    }
	std::string word = argv[2];
    std::string version = argv[3];
 
    Lines lines;
	// Read the input file and put words into char array(lines)
    if (processId == 0) {
        std::ifstream file;
		file.imbue(std::locale(std::locale(), new letter_only()));
		file.open(argv[1]);
		std::string workString;
		int i = 0;
		while(file >> workString){
			memset(lines[i], '\0', 16);
			memcpy(lines[i++], workString.c_str(), workString.length());
            //std::cout << lines[i-1] << std::endl;
		}
        std::cout << "number of words: " << i << std::endl;
        word_count = i;
    }
	
	//***************** Add code as per your requirement below ***************** 
	
	start_time=MPI_Wtime();
    int chunkSize = floor((double) word_count/num_processes);
    std::cout << "processId: " << processId << std::endl;
    std::cout << "word_count: " << word_count << std::endl;
    std::cout << "num_processes: " << num_processes << std::endl;
    std::cout << "chunkSize: " << chunkSize << std::endl;
    std::cout << "--------------------" << std::endl; 
    char recv_buf[chunkSize][16];
    MPI_Scatter(lines, chunkSize, MPI_CHAR, recv_buf, chunkSize, MPI_CHAR, 0, MPI_COMM_WORLD);
    
    int local_count = 0;
    for (int i = 0; i < chunkSize; ++i) {
        if (recv_buf[i] == word)
            local_count++;
    }


	if( version == "b1")
	{
		// Reduction for Part B1
        std::cout << "b1 runs" << std::endl;
        int global_count; 
        MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        //std::cout << "num_processes: " << num_processes << std::endl;

	} else {
		// Point-To-Point communication for Part B2


        int global_count;
        if (processId != 0) {
            MPI_Recv(&global_count, 1, MPI_INT, processId-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        global_count += local_count;
        MPI_Send(&global_count, 1, MPI_INT, (processId+1) % num_processes, 0, MPI_COMM_WORLD);
        if (processId == 0) {
            MPI_Recv(&global_count, 1, MPI_INT, num_processes-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

	}
	
    if(processId == 0)
    {
        // Output the search word's frequency here
		end_time=MPI_Wtime();
        std::cout << "Time: " << ((double)end_time-start_time) << std::endl;
    }
 
    MPI_Finalize();
 
    return 0;
}
