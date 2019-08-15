// This is the CPP file you will edit and turn in. (TODO: Remove this comment!)

#include "patientqueue.h"

PatientQueue::PatientQueue(bool isMinHeap) {
    // TODO: write this constructor
    myPatients = new Patient[10];
    // Patientqueue is empty at index 0
    myPatients[0] = Patient();
    mySize = 1;
    myCapacity = 10;
    heapType = isMinHeap;
}

PatientQueue::~PatientQueue() {
    // Delete the queue
    delete[] myPatients;
}

void PatientQueue::changePriority(string value, int newPriority) {
    // Throw a string exception if the given patient does not exist
    int currentIndex;
    Patient previousPatient;
    if (!patientExists(value)) {
        throw "The specified patient does not exist in the queue.";
    } else {
        currentIndex = getPatientIndex(value);
        previousPatient = getPatientByName(value);
    }

    // Modify the priority of an existing patient in the queue
    Patient updatedPatient = Patient(value, newPriority);
    myPatients[currentIndex] = updatedPatient;

    if (updatedPatient <= previousPatient && heapType) {
        // Min-heap Case: If the priority of the patient gets smaller, bubble it up
        bubbleUpMinHeap(updatedPatient, currentIndex);
    } else if (updatedPatient > previousPatient && heapType) {
        // Min-heap Case: If the priority of the patient gets bigger, bubble it down
        bubbleDownMinHeap(currentIndex);
    } else if (updatedPatient <= previousPatient && !heapType) {
        // Max-heap Case: If the priority of the patient gets smaller, bubble it down
        bubbleDownMaxHeap(currentIndex);
    } else {
        // Max-heap Case: If the priority of the patient gets bigger, bubble it up
        bubbleUpMaxHeap(updatedPatient, currentIndex);
    }
}

void PatientQueue::clear() {
    // Clear all patients in the queue (but not delete the queue)
    for (int i=0; i<mySize; i++) {
        myPatients[i] = Patient();
    }
    mySize = 1;
}

void PatientQueue::debug() {
    // empty
}

string PatientQueue::dequeue() {
    // Throw a string exception if the queue is empty
    if (mySize == 1) {
        throw "Attempting to dequeue from an empty PatientQueue.";
    }

    // Return the name of the removed patient
    string dequeuedName = myPatients[1].name;

    // Remove the frontmost patient by first replacing it with the last patient in the queue
    myPatients[1] = Patient();
    swapPatients(1, mySize - 1);
    mySize--;

    // Check if the replaced patient breaks the heap order
    // if yes, bubble it down
    if (heapType) {
        // Min-heap Case:
        bubbleDownMinHeap(1);
    } else {
        // Max-heap Case:
        bubbleDownMaxHeap(1);
    }

    return dequeuedName;   // remove this
}

void PatientQueue::enqueue(string value, int priority) {
    // Check if the capacity will be exceeded
    checkResize();

    // Put the new patient at the end of the queue,
    Patient pat(value, priority);
    myPatients[mySize] = pat;
    mySize++;

    // Bubble up the new Patient if needed
    if (heapType) {
        // Min-heap Case:
        bubbleUpMinHeap(pat, mySize - 1);
    } else {
        // Max-heap Case:
        bubbleUpMaxHeap(pat, mySize - 1);
    }

}

bool PatientQueue::isEmpty() const {
    // Return if the PatientQueue is empty or not
    // Here "empty" means we only have an empty patient at index 0
    return (mySize == 1);
}

string PatientQueue::peek() const {
    // Return the name of the frontmost patient in the queue
    return myPatients[1].name;
}

int PatientQueue::peekPriority() const {
    // Return the priority of the frontmost patient in the queue
    return myPatients[1].priority;
}

int PatientQueue::size() const {
    // Return the number of patients (not including empty patient) in the queue
    return mySize - 1;
}

ostream& operator <<(ostream& out, const PatientQueue& queue) {
    // Display the PatientQueue
    if (queue.mySize <= 1) {
        out << "{}";
    } else {
        out << "{";
        for (int i=1; i<queue.mySize-1; i++) {
            out << queue.getPatient(i).name << " (" << queue.getPatient(i).priority << "), ";
        }
        out << queue.getPatient(queue.mySize-1).name << " (" << queue.getPatient(queue.mySize-1).priority << ")}";
    }
    return out;
}

//*************************************Below are helper functions*******************************************
void PatientQueue::checkResize() {
    // Expand the array if the size of the PatientQueue will exceed its capacity

}
void PatientQueue::swapPatients(int index1, int index2) {
    // Swap two patients in PatientQueue
    Patient temp = myPatients[index1];
    myPatients[index1] = myPatients[index2];
    myPatients[index2] = temp;
}

Patient PatientQueue::getPatient(int index) const {
    return myPatients[index];
}

int PatientQueue::getSmallerChildIndex(int index1, int index2) {
    Patient child1 = myPatients[index1];
    Patient child2 = myPatients[index2];
    if (child1 < child2) {
        return index1;
    } else {
        return index2;
    }
}

int PatientQueue::getLargerChildIndex(int index1, int index2) {
    Patient child1 = myPatients[index1];
    Patient child2 = myPatients[index2];
    if (child1 > child2) {
        return index1;
    } else {
        return index2;
    }
}

bool PatientQueue::patientExists(string name) {
    // Check if a patient exists in the queue
    for (int i=1; i<mySize; i++) {
        if (myPatients[i].name == name) {
            return true;
        }
    }
    return false;
}

int PatientQueue::getPatientIndex(string name) {
    // Return the index of the first found patient with the given name
    for (int i=1; i<mySize; i++) {
        if (myPatients[i].name == name) {
            return i;
        }
    }
    return 0;
}

Patient PatientQueue::getPatientByName(string name) {
    // Return the priority of the first found patient with the given name
    for (int i=1; i<mySize; i++) {
        if (myPatients[i].name == name) {
            return myPatients[i];
        }
    }
    return Patient();
}

void PatientQueue::bubbleUpMinHeap(Patient pat, int currentIndex) {
    // Priority of parent node should always be smaller than its child
    int parentIndex = currentIndex / 2;
    while (parentIndex >= 1 && pat < myPatients[parentIndex]) {
        swapPatients(currentIndex, parentIndex);
        currentIndex = parentIndex;
        parentIndex = parentIndex / 2;
    }
}

void PatientQueue::bubbleDownMinHeap(int currentIndex) {
    // If any of the child of the replaced patient is smaller,
    // swap the replaced patient with its child with smaller priority until heap is in order
    int childIndex_smaller = 2*currentIndex;
    int childIndex_larger = 2*currentIndex + 1;
    while (childIndex_smaller <= mySize - 1) {
        if (childIndex_smaller == mySize - 1) {
            // Case 1: Only one child exists in queue
            if (myPatients[childIndex_smaller] < myPatients[currentIndex]) {
                swapPatients(currentIndex, childIndex_smaller);
            }
            break;
        } else {
            // Case 2: Both children exist in queue
            int smallerChildIndex = getSmallerChildIndex(childIndex_smaller, childIndex_larger);
            if (myPatients[smallerChildIndex] < myPatients[currentIndex]) {
                swapPatients(currentIndex, smallerChildIndex);
            }
            currentIndex = smallerChildIndex;
            childIndex_smaller = currentIndex*2;
            childIndex_larger = currentIndex*2 + 1;
        }
    }
}

void PatientQueue::bubbleUpMaxHeap(Patient pat, int currentIndex) {
    // Priority of parent node should always be larger than its child
    int parentIndex = currentIndex / 2;
    while (parentIndex >= 1 && pat > myPatients[parentIndex]) {
        swapPatients(currentIndex, parentIndex);
        currentIndex = parentIndex;
        parentIndex = parentIndex / 2;
    }
}

void PatientQueue::bubbleDownMaxHeap(int currentIndex) {
    // If any of the child of the replaced patient is larger,
    // swap the replaced patient with its child with larger priority until heap is in order
    int childIndex_smaller = 2*currentIndex;
    int childIndex_larger = 2*currentIndex + 1;
    while (childIndex_smaller <= mySize - 1) {
        if (childIndex_smaller == mySize - 1) {
            // Case 1: Only one child exists in queue
            if (myPatients[childIndex_smaller] > myPatients[currentIndex]) {
                swapPatients(currentIndex, childIndex_smaller);
            }
            break;
        } else {
            // Case 2: Both children exist in queue
            int largerChildIndex = getLargerChildIndex(childIndex_smaller, childIndex_larger);
            if (myPatients[largerChildIndex] < myPatients[currentIndex]) {
                swapPatients(currentIndex, largerChildIndex);
            }
            currentIndex = largerChildIndex;
            childIndex_smaller = currentIndex*2;
            childIndex_larger = currentIndex*2 + 1;
        }
    }
}
