#include "mpi.h"
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

int * compute(int * buf, int length, int value) {
    int * result = new int[length];
    for (int i = 0; i < length; ++i) {
        result[i] = buf[i];
        result[i] += value;
    }
    return result;
}

void fill(int * buf, int length, int value) {
    for (int i = 0; i < length; ++i) {
        buf[i] = value;
    }
}

std::string out_array(int * buf, int length) {
    std::string result = "";
    for (int i = 0; i < length; ++i) {
        result += std::to_string(buf[i]) + " ";
    }
    return result;
}

int main(int argc, char ** argv) {
    int processId, num_processes;
    int chunkSize = 0;
    int globalSize = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    // error here
    if (processId == 0) {
        globalSize = 12;
        
        for (int i = 1; i < num_processes; ++i) {
            MPI_Send(&globalSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(&globalSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    chunkSize = floor((double) globalSize/num_processes);
    /*
    globalSize = 12;
    chunkSize = floor((double) globalSize/num_processes);
    */
    int * total_input = nullptr;
    if (processId == 0) {
        total_input = new int[globalSize];
        fill(total_input, globalSize, 5);
        std::cout << "before" << std::endl;
        std::cout << out_array(total_input, globalSize) << std::endl;
    }

    int * recv_buf = new int[chunkSize];
    MPI_Scatter(total_input, chunkSize, MPI_INT, recv_buf, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
    int * subtotal = compute(recv_buf, chunkSize, 15);

    int * total_output = nullptr;
    if (processId == 0) {
        total_output = new int[chunkSize * num_processes]; // Same as globalSize
    }
    MPI_Gather(subtotal, chunkSize, MPI_INT, total_output, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);


    if (processId == 0) {
        std::cout << "after" << std::endl;
        std::cout << out_array(total_output, globalSize) << std::endl;
    }
    MPI_Finalize();
    return 0;
}
