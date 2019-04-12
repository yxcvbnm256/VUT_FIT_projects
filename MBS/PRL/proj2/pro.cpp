/*
 * algorithm: preorder algorithm
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
#include <cmath>
#include <string>
#include <map>
 
using namespace std;

#define TAG 0
#define OFFSET 1

//for suffix sum communication
typedef struct {
	int weight;
	int next;
	int isLastOne;
} suffixSumMsg;

//structure describing each edge
typedef struct {
	char fromNode;
	char toNode;
	bool forwarding;
	int owner;
	int toIndex;
	int fromIndex;
} edge;

typedef struct {
	char parent;
	char leftSon;
	char rightSon;
	int index;
} rec;

/* function that returns the next euler tour's edge */
edge next(edge edg, string tree) {
	rec record;
	edge next;
	char node = edg.fromNode;
	record.parent = record.leftSon = record.rightSon = '-';
	record.index = edg.fromIndex + 1;
	int parentIndex = floor(record.index/2);
	int sonIndex;
	if (node != tree[0]) {
		record.parent = tree[parentIndex - 1];
	}
	if ((sonIndex = 2*(record.index)) <= tree.length()) {
		record.leftSon = tree[sonIndex-1];
	}
	if ((sonIndex = 2*(record.index)+1) <= tree.length()) {
		record.rightSon = tree[sonIndex-1];
	}
	if (record.parent == edg.toNode) {
		if (record.leftSon != '-') {
			next.fromNode = node;
			next.toNode = record.leftSon;
			return next;
		}
		else if (record.rightSon != '-') {
			next.fromNode = edg.fromNode;
			next.toNode = record.rightSon;
			return next;
		}
		else {
			if (record.parent != '-') {
				next.fromNode = node;
				next.toNode = record.parent;
			}
			else if (record.leftSon != '-') {
				next.fromNode = node;
				next.toNode = record.leftSon;
			}
			else {
				next.fromNode = node;
				next.toNode = record.rightSon;
			}
		}
	}
	else if (record.leftSon == edg.toNode) {		
		next.fromNode = edg.fromNode;
		if (record.rightSon == '-') {
			if (record.parent == '-')
				next.toNode = record.leftSon;
			else
				next.toNode = record.parent;
		}
		else
			next.toNode = record.rightSon;
		return next;		
	}
	else if (record.rightSon == edg.toNode) {
		next.fromNode = edg.fromNode;
		if (record.parent == '-')
			next.toNode = record.leftSon;
		else
			next.toNode = record.parent;
	}
	return next;
}

/* function that computes the suffix sum */
int suffixSum(int limit, int myid, int suffixNext, bool lastEdge, int weight) {
	int rcvdWeight, rcvdNext = 0;
	suffixSumMsg msg, recvMsg;
	bool lastIteration = false;
	MPI_Status stat;
	for (int i = 0; i<limit; i++) {
		MPI_Barrier(MPI_COMM_WORLD);
		//if not the "last" processor, send and receive			
		if (myid != 1) {
			msg.weight = -1;
			msg.next = -1;
			msg.isLastOne = lastEdge ? 1 : 0;
			if (suffixNext != 1) {
				MPI_Send(&msg, 3, MPI_INT, suffixNext-1, TAG, MPI_COMM_WORLD);
				MPI_Recv(&recvMsg, 3, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &stat);
				if (recvMsg.weight == -1) {
					msg.weight = weight;
					msg.next = suffixNext;
					lastIteration = recvMsg.isLastOne == 0 ? false : true;
					MPI_Send(&msg, 3, MPI_INT, stat.MPI_SOURCE, TAG, MPI_COMM_WORLD);
				}
				else {

					rcvdNext = recvMsg.next;
					rcvdWeight = recvMsg.weight;
				}
			} else {
				rcvdWeight = 0;
				rcvdNext = 1;
			}
			if (!lastEdge) {
				MPI_Recv(&recvMsg, 3, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &stat);
				if (recvMsg.weight == -1) {
					msg.weight = weight;
					msg.next = suffixNext;					
					lastIteration = recvMsg.isLastOne == 0 ? false : true;
					MPI_Send(&msg, 3, MPI_INT, stat.MPI_SOURCE, TAG, MPI_COMM_WORLD);
				}
				else {

					rcvdNext = recvMsg.next;
					rcvdWeight = recvMsg.weight;
				}
			}
			lastEdge = lastEdge ? true : lastIteration;
			suffixNext = rcvdNext;
			weight += rcvdWeight;
		}
	}
	return weight;
}


int main(int argc, char *argv[])
{
	//number of cpus
	int numprocs;
	//rank
	int myid;
	MPI_Status stat;
	//MPI INIT
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	char myNode = argv[1][myid];
	bool root = false;
	bool lastEdge = false;
	string tree = string(argv[1]);
	myid++;
	edge myedge;
	edge euedge;
	edge temp;
	std::map<string,int> edgeList;
	int edges = strlen(argv[1])-1;
	int edgeCount = 0;
	chrono::time_point<std::chrono::system_clock> now;
	root = myid == 1;
	string index;
	//assign each edge to each processor
	for (int i = 1; i <= strlen(argv[1]); i++) {
		index = "";
		char node = argv[1][i-1];
		int leftSon, rightSon;
		if ((leftSon = 2*i) <= strlen(argv[1])) {
			edgeCount++;
			if (myid == edgeCount) {
				myedge.fromIndex = i-1;
				myedge.fromNode = node;
				myedge.forwarding = true;
				myedge.toIndex = leftSon-1;
				myedge.toNode = argv[1][leftSon-1];
				myedge.owner = edgeCount;
			}
			index += node;
			index += argv[1][leftSon-1];		
			edgeList[index] = edgeCount;
			reverse(index.begin(), index.end());
			edgeList[index] = edgeCount + edges;
			if (myid == (edgeCount + edges)) {
				myedge.fromIndex = leftSon-1;
				myedge.fromNode = argv[1][leftSon-1];
				myedge.forwarding = false;
				myedge.toIndex = i-1;
				myedge.toNode = node;
				myedge.owner = edgeCount + edges;
			}
		}
		if ((rightSon = 2*i+1) <= strlen(argv[1])) {
			edgeCount++;
			index = "";
			index += node;
			index += argv[1][rightSon-1];
			edgeList[index] = edgeCount;
			reverse(index.begin(), index.end());
			edgeList[index] = edgeCount + edges;
			if (myid == edgeCount) {
				myedge.fromIndex = i-1;
				myedge.fromNode = node;
				myedge.forwarding = true;
				myedge.toIndex = rightSon-1;
				myedge.toNode = argv[1][rightSon-1];
				myedge.owner = edgeCount;
			}			
			if (myid == (edgeCount + edges)) {
				myedge.fromIndex = rightSon-1;
				myedge.fromNode = argv[1][rightSon-1];
				myedge.forwarding = false;
				myedge.toIndex = i-1;
				myedge.toNode = node;
				myedge.owner = edgeCount + edges;
			}
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);	
	edge revEdge;
	int suffixNext;
	revEdge.fromNode = myedge.toNode;
	revEdge.toNode = myedge.fromNode;
	revEdge.toIndex = myedge.fromIndex;
	revEdge.fromIndex = myedge.toIndex;	
	//get the next edge in the euler tour
	edge nxt = next(revEdge, tree);		
	index = "";
	index += nxt.fromNode;
	index += nxt.toNode;
	//get the next edge's owner from map
	nxt.owner = edgeList[index];
	int receivedNext;
	int weight = myedge.forwarding ? 1 : 0;
	int receivedWeight;
	suffixSumMsg msg;
	suffixSumMsg recvMsg = msg;   
    if (nxt.owner == 1)
		lastEdge = true;

	//"list reverse" - find out your predecessor
	MPI_Send(&myid, 1, MPI_INT, nxt.owner-1, TAG, MPI_COMM_WORLD);
	MPI_Recv(&suffixNext, 1, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &stat);
	int limit = ceil(log2(numprocs));
	//do the suffix sum
	weight = suffixSum(limit, myid, suffixNext, lastEdge, weight);
	suffixSumMsg tmp;
	if (myid != 1)
		weight++;
	/*double elapsedTime = MPI_Wtime() - tbeg;
    double totalTime;*/
	if (myid == 1){
		vector<char> vec;
		vec.resize(edges-1);		
		cout << myedge.fromNode << myedge.toNode;
		for (int i = 0; i < edges - 1; i++) {
			MPI_Recv(&tmp, 3, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &stat);
			vec[tmp.weight-1] = tmp.isLastOne;
		}
		for (int i = 1; i < edges;i++) {
			cout << vec[i];
		}
		cout << endl;
	}
	else if (myedge.forwarding) {
		tmp.weight = weight;
		tmp.isLastOne = myedge.toNode;
		MPI_Send(&tmp, 3, MPI_INT, 0, TAG, MPI_COMM_WORLD);
	}
	MPI_Finalize(); 
	return 0;
}
