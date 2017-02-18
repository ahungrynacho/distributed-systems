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
using Lines = char[ARRAY_SIZE][16]; // Lines is already linearized under-the-hood in memory

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
/*
std::string output_words(char * buf[16], int length) {
    std::string result = "";
    for (int i = 0; i < length; ++i) {
        result += buf[i] + ' ';
    }
    return result;
}
*/
int main(int argc, char* argv[])
{
    int processId;
    int num_processes;
    int total_words;
    int chunkSize;
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
		}
        total_words = i;
        chunkSize = floor((double) total_words /  num_processes);

        for (int i = 1; i < num_processes; ++i) {
            MPI_Send(&chunkSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }   
    }
    else {
        MPI_Recv(&chunkSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
	
	//***************** Add code as per your requirement below ***************** 
	
	start_time=MPI_Wtime();
    /*
    std::cout << "processId: " << processId << std::endl;
    std::cout << "word_count: " << word_count << std::endl;
    std::cout << "num_processes: " << num_processes << std::endl;
    std::cout << "chunkSize: " << chunkSize << std::endl;
    std::cout << "--------------------" << std::endl;
    */
    char recv_buf[chunkSize * 16];
    // initialize array
    for (int i = 0; i < chunkSize * 16; ++i) {
        recv_buf[i] = '\0';
    }
    MPI_Scatter(lines, chunkSize * 16, MPI_CHAR, recv_buf, chunkSize * 16, MPI_CHAR, 0, MPI_COMM_WORLD);
    std::vector<std::string> recv_buf_str;

    
    // Convert char* to std::string.
    for (int i = 0; i < chunkSize; ++i) {
        std::string temp = "";
        for (int j = 0; j < 16; ++j) {
            if (recv_buf[(i * 16) + j] == '\0')
                break;
            temp += recv_buf[(i * 16) + j];
        }
        recv_buf_str.push_back(temp);
    }
    
    int local_count = 0;
    for (std::string w : recv_buf_str) {
        if (w.compare(word) == 0)
            local_count++;
    }



    int global_count = 0;
	if( version == "b1")
	{
		// Reduction for Part B1
        MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	} else {
		// Point-To-Point communication for Part B2

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
        if (version == "b1")
            std::cout << "Reduction" << std::endl;
        else
            std::cout << "Point-to-Point" << std::endl;

        std::cout << "Number of Processes: " << num_processes << std::endl;
        DoOutput(word, global_count);
        std::cout << "Time: " << ((double)end_time-start_time) << std::endl;
    }
 
    MPI_Finalize();
 
    return 0;
}
