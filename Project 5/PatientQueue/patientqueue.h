// This is the H file you will edit and turn in. (TODO: Remove this comment!)

#ifndef _patientqueue_h
#define _patientqueue_h

#include <iostream>
#include <string>
#include "patient.h"
using namespace std;

class PatientQueue {
public:
    PatientQueue(bool isMinHeap = true);
    ~PatientQueue();
    void changePriority(string value, int newPriority);
    void clear();
    void debug();
    string dequeue();
    void enqueue(string value, int priority);
    bool isEmpty() const;
    string peek() const;
    int peekPriority() const;
    int size() const;
    friend ostream& operator <<(ostream& out, const PatientQueue& queue);

private:
    // TODO: add specified member variables and any needed helper functions
    Patient* myPatients;
    int mySize;
    int myCapacity;
    bool heapType;
    void checkResize();        // Check of the heap size will exceed its capacity
    void swapPatients(int index1, int index2);       // Swap two patients in PatientQueue
    Patient getPatient(int index) const;
    int getSmallerChildIndex(int index1, int index2);
    int getLargerChildIndex(int index1, int index2);
    bool patientExists(string name);
    int getPatientIndex(string name);
    Patient getPatientByName(string name);

    void bubbleUpMinHeap(Patient pat, int currentIndex);
    void bubbleDownMinHeap(int currentIndex);
    void bubbleUpMaxHeap(Patient pat, int currentIndex);
    void bubbleDownMaxHeap(int currentIndex);
};

#endif
