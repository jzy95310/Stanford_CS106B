/*
 * CS 106B/X Patient Queue
 * This file contains the main program and user interface for testing your
 * various patient queues.
 *
 * You can modify this file if you like, to add other tests and so on.
 * But your turned-in files should work properly with an unmodified version of
 * all provided code files.
 *
 * @author Marty Stepp and Julie Zelenski
 * @version 2019/05/03
 * - 19sp initial version; heap-based PQ only, supports min or max heap
 * @version 2017/10/31
 * - 17au version; supports multiple types of PQs
 */

#include <algorithm>  // For sort, reverse
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "console.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "patientqueue.h"
using namespace std;

static const bool RIG_RANDOM_NUMBERS = true;  // true to use same random sequence every time
static const int RIG_RANDOM_SEED = 106;       // seed for fixing random numbers
static const int RANDOM_STRING_LENGTH = 6;    // max length of random strings in bulk en/deQ

// function prototype declarations
string randomString(int maxLength);
void menu(PatientQueue& queue);
void bulkDequeue(PatientQueue& queue, int count);
void bulkEnqueue(PatientQueue& queue, int count);

int main() {
    cout << "CS 106B Hospital Patient Check-in System" << endl;
    cout << "========================================" << endl;

    if (RIG_RANDOM_NUMBERS) {
        setRandomSeed(RIG_RANDOM_SEED);
    }

    bool isMinHeap = getYesOrNo("Process the minimum priority patients first (Y/N)? ");

    // these {} braces are so that your queue's destructor will be called sooner
    {
        PatientQueue queue(isMinHeap);
        menu(queue);
    }
    // your queue's destructor will be called here

    cout << endl;
    cout << "Exiting." << endl;
    return 0;
}

/*
 * Returns a randomly generated string of letters up to the given length.
 */
string randomString(int maxLength) {
    int length = randomInteger(1, maxLength);
    ostringstream out;
    for (int i = 0; i < length; i++) {
        char ch = (char) ('a' + randomInteger(0, 25));
        out << ch;
    }
    return out.str();
}

/*
 * Prompts the user to perform tests on your patient queue.
 * Each method of the queue has a corresponding letter or symbol to type that
 * will call that method on the queue and display the results as appropriate.
 */
void menu(PatientQueue& queue) {
    while (true) {
        cout << endl;
        queue.debug();
        cout << "Current patient queue:" << endl;
        cout << queue;
        cout << "  (size=" << queue.size() << ")";
        if (queue.isEmpty()) {
            cout << " (empty)" << endl;
        } else {
            cout << " (not empty)" << endl;
        }

        string prompt = "(E)nqueue, (D)equeue, (P)eek, (C)hange, c(L)ear, (B)ulk, (Q)uit?";
        string choice = toUpperCase(trim(getLine(prompt)));
        if (choice.empty() || choice == "Q") {
            break;
        } else if (choice == "E") {
            string value = getLine("Name? ");
            int priority = getInteger("Priority? ");
            queue.enqueue(value, priority);
        } else if (choice == "D") {
            string value = queue.dequeue();
            cout << "Dequeued patient: \"" << value << "\"" << endl;
        } else if (choice == "P") {
            string name = queue.peek();
            int pri = queue.peekPriority();
            cout << "Front of queue is \"" << name << "\" with priority " << pri << endl;
        } else if (choice == "C") {
            string value = getLine("Name? ");
            int newPriority = getInteger("New priority? ");
            queue.changePriority(value, newPriority);
        } else if (choice == "L") {
            queue.clear();
        } else if (choice == "B") {
            int count = getInteger("How many patients? ");
            string choice2 = toUpperCase(trim(getLine("(E)nqueue or (D)equeue: ")));
            if (choice2 == "E") {
                bulkEnqueue(queue, count);
            } else if (choice2 == "D") {
                bulkDequeue(queue, count);
            }
        }
    }
}

/*
 * Dequeues the given number of patients from the queue.
 * Helpful for bulk testing.
 */
void bulkDequeue(PatientQueue& queue, int count) {
    for (int i = 1; i <= count; i++) {
        string value = queue.dequeue();
        cout << "#" << i << ", processing patient: \"" << value << "\"" << endl;
    }
}

/*
 * Enqueues the given number of patients into the queue, priorities either random
 * or in ascending or descending order.
 * Helpful for bulk testing.
 */
void bulkEnqueue(PatientQueue& queue, int count) {
    string choice2;

    while (true) {
        choice2 = trim(toUpperCase(getLine("R)andom, A)scending, D)escending? ")));
        if (choice2 == "R" || choice2 == "A" || choice2 == "D") {
            break;
        }
        cout << "Your choice was invalid. Please try again." << endl;
    }

   Vector<int> priorities;
   if (choice2 == "R") {
        for (int i = 0; i < count; i++) {
            priorities.add(randomInteger(1, count));
        }
    } else if (choice2 == "A") {
        for (int i = 1; i <= count; i++) {
            priorities.add(i);
        }
    } else if (choice2 == "D") {
        for (int i = count; i > 0; i--) {
            priorities.add(i);
        }
    }
    for (int p : priorities) {
        string value = randomString(RANDOM_STRING_LENGTH);
        queue.enqueue(value, p);
        cout << "New patient \"" << value << "\" with priority " << p << endl;
    }
}
