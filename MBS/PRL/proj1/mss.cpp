/*
 * algorithm: merge-splitting sort
 * author: jan pawlus, xpawlu00@stud.fit.vutbr.cz
 *
 */

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstdint>
 
using namespace std;

#define TAG 0

/* function that merges two sorted vectors into one in 2*n time. */
vector<int> mergeTwo(vector<int> first, vector<int> second) {
	vector<int> final;
	int size = first.size() + second.size();
	for (int i = 0; i < size; i++) {
		int firstNum = first.empty() ? 255 : first.front();
		int secondNum = second.empty() ? 255 : second.front();
		if (firstNum < secondNum) {
			final.push_back(firstNum);
			if (!first.empty())
                first.erase(first.begin());
		} else {
			final.push_back(secondNum);
            if (!second.empty())
                second.erase(second.begin());
		}
	}
	return final;
}

int main(int argc, char *argv[])
{
	//number of cpus
	int numprocs;
	//rank
	int myid;
	MPI_Status stat;
	//my numbers
	vector<int> mynumbers;
	//all the numbers
	vector<int> numbers;
	//numbers per cpu
	int numbersPerCpu;
	//chrono::time_point<std::chrono::system_clock> now;
	//MPI INIT
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	if (myid == 0) {
		//file name
		char input[]= "numbers";
		fstream fin;
        fin.open(input, ios::in);
		int c;
		while(fin.good()){
			c = fin.get();
			if (!fin.good())
				break;
			numbers.push_back(c);
			if (numbers.size() == 1)
				cout << c;
			else
				cout << " " << c;
		}
		fin.close(); 
		cout << endl;		
		//count number 
		for (int i = 0; i < numprocs; i++) {
            
			//numbers count for next cpu according to numbers left and cpus left
			numbersPerCpu = floor((double)numbers.size()/(double)(numprocs-i));
			//send number count
			MPI_Send(&numbersPerCpu, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
			//send numbers
			MPI_Send(&numbers.front(), numbersPerCpu, MPI_INT, i, TAG, MPI_COMM_WORLD);
            if (i + 1 == numprocs) {
                numbers.clear();
            } else {
			for (int j = 0; j < numbersPerCpu; j++)
				numbers.erase(numbers.begin());
            }
		}
	}
    //time measuring
    /*if (myid == 0)
        now = std::chrono::system_clock::now();*/
	//each processor receives numbers count and resizes its vector

	MPI_Recv(&numbersPerCpu, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
	mynumbers.resize(numbersPerCpu);
	//each processor gets its numbers
	MPI_Recv(&mynumbers.front(), numbersPerCpu, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
	//each processor sorts its numbers
	sort(mynumbers.begin(), mynumbers.end(), less_equal<int>());
    int oddlimit = 2*(numprocs/2)-1;
	int evenlimit = 2*((numprocs-1)/2);
	int halfcycles = ceil((double)numprocs/2);
	//temp vector where merged numbers will be
	vector<int> tempVec;
	//vector where received numbers are
	vector<int> toReceive;
	//vector where numbers to send are
	vector<int> toSend;
	int toReceiveSize = 0;
	for (int j = 1; j <= halfcycles; j++) {
		if (numprocs == 1)
			break;
		//even processors listen
		if ((!(myid % 2) || myid == 0) && (myid < oddlimit)) {
			//receive numbers count from the odd neighbour
			MPI_Recv(&toReceiveSize, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD, &stat);
			toReceive.resize(toReceiveSize);
			//receive numbers from the odd neighbour
			MPI_Recv(&toReceive.front(), toReceiveSize, MPI_INT, myid+1, TAG, MPI_COMM_WORLD, &stat);
			tempVec = mergeTwo(mynumbers, toReceive);
			mynumbers.clear();
			mynumbers.resize(numbersPerCpu);
			toSend = vector<int>(tempVec.end() - toReceiveSize, tempVec.end());
			MPI_Send(&toSend.front(), toReceiveSize, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);
			mynumbers = vector<int>(tempVec.begin(), tempVec.end() - toReceiveSize);

		} 
		//odd processors send their numbers
		else if (myid % 2) {
			MPI_Send(&numbersPerCpu, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);
			MPI_Send(&mynumbers.front(), numbersPerCpu, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);
			//receive the merged ordered list from neighbour
			MPI_Recv(&mynumbers.front(), numbersPerCpu, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat);
		}
		//even processors apart form 0 send their numbers   
		if (!(myid % 2) && myid != 0) {
			MPI_Send(&numbersPerCpu, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);
			MPI_Send(&mynumbers.front(), numbersPerCpu, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);
			//receive the merged ordered list from neighbour
			MPI_Recv(&mynumbers.front(), numbersPerCpu, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat);
		//odd processors listen
		} else if (myid % 2 != 0 && myid < evenlimit) {
			MPI_Recv(&toReceiveSize, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD, &stat);
			toReceive.resize(toReceiveSize);
			MPI_Recv(&toReceive.front(), toReceiveSize, MPI_INT, myid+1, TAG, MPI_COMM_WORLD, &stat);
			tempVec = mergeTwo(mynumbers, toReceive);
			mynumbers.clear();
			mynumbers.resize(numbersPerCpu);
			toSend = vector<int>(tempVec.end() - toReceiveSize, tempVec.end()); 
			MPI_Send(&toSend.front(), toReceiveSize, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);
			mynumbers = vector<int>(tempVec.begin(), tempVec.end() - toReceiveSize);
		}
	}
    mynumbers.clear();
    tempVec.clear();
    toSend.clear();
    toReceive.clear();
	//cpu 0 prints its numbers and listens for the rest cpus for their numbers
	if (myid == 0) { 
        //time measuring
        //auto milliseconds = chrono::duration_cast<std::chrono::microseconds>(chrono::system_clock::now() - now).count();
		for (int i = 0; i < numbersPerCpu; i++) {
			cout << mynumbers[i] << endl;
		}
		for (int i = 1; i < numprocs; i++) {
			MPI_Recv(&toReceiveSize, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat);
			toReceive.resize(toReceiveSize);
			MPI_Recv(&toReceive.front(), toReceiveSize, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat);
			for (int j = 0; j < toReceiveSize; j++) {
				cout << toReceive[j] << endl;
			}
		}		
	}
	//other processors than 0 send their numbers
	else {
		MPI_Send(&numbersPerCpu, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
		MPI_Send(&mynumbers.front(), numbersPerCpu, MPI_INT, 0, TAG, MPI_COMM_WORLD);
	}
	MPI_Finalize(); 
	return 0;
}
