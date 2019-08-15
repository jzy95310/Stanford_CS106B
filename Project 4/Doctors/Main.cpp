#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "Common.h"
#include "DoctorsWithoutOrders.h"
#include "console.h"
#include "strlib.h"
#include "vector.h"
#include "simpio.h"
#include "filelib.h"
using namespace std;

/* Everything in here is private to this file. */
namespace {   

    /* * * * * General I/O Logic * * * * */
    /* Prompts the user for a filename. This should eventually be switched to use openFileDialog,
     * but it seems that doing so has some kind of data race on Macs?
     */
    string haveUserChooseFile(ifstream& input, const string& prompt, const string& directory) {
        while (true) {
            string filename = getLine(prompt);
            string fullname = directory + "/" + filename;

            /* First, try to open the file using the updated path name. */
            {
                ifstream fileReader(fullname);
                if (fileReader) {
                    input = std::move(fileReader);
                    return fullname;
                }
            }

            /* Try to open the file just using the name given. */
            {
                ifstream fileReader(filename);
                if (fileReader) {
                    input = std::move(fileReader);
                    return filename;
                }
            }

            cerr << "Sorry, I can't open that file." << endl;
        }
    }

    /* Prompts the user for an answer, reprompting as necessary until they enter something
     * that starts with 'y' or 'n'.
     */
    bool askNicely(const string& prompt) {
        while (true) {
            string line = toLowerCase(getLine(prompt));
            if (startsWith(line, "y")) return true;
            if (startsWith(line, "n")) return false;

            cerr << "Sorry, I didn't understand." << endl;
        }
    }

    /* Given a filename and an expected suffix, checks whether the file
     * ends with that suffix. If not, issues a warning and asks if the
     * user wants to proceed anyway.
     */
    bool proceedWithTest(const string& filename, const string& suffix) {
        if (endsWith(getTail(filename), suffix)) return true;

        cerr << "Warning: This doesn't look like a test case for this part" << endl;
        cerr << "of the assignment. Test cases for this part of the assignment" << endl;
        cerr << "should have the " << suffix << " suffix." << endl;

        return askNicely("Do you want to continue anyway? ");
    }

    /* * * * * Logic for Doctors Without Orders * * * * */

    /* Prompts the user to choose a hospital test case, returning the contents of that test. */
    HospitalTestCase userChooseHospitalTest() {
        while (true) {
            ifstream input;
            string filename = haveUserChooseFile(input, "Enter test file name: ", "DoctorsWithoutOrders");

            if (!filename.empty() && input && proceedWithTest(filename, ".dwo")) {
                return loadHospitalTestCase(input);
            }
        }
    }

    /* Prints out a human-readable representation of the schedule. */
    void printScheduleNicely(const Set<string>& toSee, const HospitalTestCase& testCase, int hoursFree) {
        /* Convert the patients into a map from patients to hours. */
        Map<string, int> patientsToHours;
        for (const auto& patient: testCase.patients) {
            patientsToHours[patient.name] = patient.hoursNeeded;
        }

        /* List all the patients with their hours needed */
        int totalHours = 0;
        for (const auto& patient: toSee) {
            cout << "    Patient " << patient << " (" << pluralize(patientsToHours[patient], "hour") << " needed)" << endl;
            totalHours += patientsToHours[patient];
        }

        if (totalHours > hoursFree) {
            cerr << "  This doctor's demands exceed their available hours!" << endl;
        }
    }

    /* Given a schedule and the list of all doctors and patients, reports whether all patients
     * were actually seen.
     */
    bool allPatientsSeen(const HospitalTestCase& testCase, const Map<string, Set<string>>& schedule) {
        Set<string> seen;
        for (const string& doctor: schedule) {
            seen += schedule[doctor];
        }

        Set<string> allPatients;
        for (const auto& patient: testCase.patients) {
            allPatients += patient.name;
        }

        return seen == allPatients;
    }

    /* Given a test case, runs the student code on that test case, reporting the result and any
     * mismatches with the file.
     */
    void processHospitalTestCase(HospitalTestCase& testCase) {
        Map<string, Set<string>> schedule;
        if (canAllPatientsBeSeen(testCase.doctors, testCase.patients, schedule)) {
            /* Report the answer either to stdout or to stderr depending
             * on whether they got the answer right
             */
            cout << "Good news! Everyone can indeed be seen." << endl;
            for (const auto& doctor: testCase.doctors) {
                cout << "  Doctor " << doctor.name << " (" << pluralize(doctor.hoursFree, "hour") << " free) sees these patients: " << endl;
                printScheduleNicely(schedule[doctor.name], testCase, doctor.hoursFree);
            }

            if (!allPatientsSeen(testCase, schedule)) {
                cerr << "Not all patients were scheduled to be seen!" << endl;
            }
            if (!testCase.isPossible) {
                cerr << "The test case reports that there should not be a way to schedule everyone!" << endl;
            }
        } else {
            cout << "Oh no! Not all patients can be seen." << endl;

            if (testCase.isPossible) {
                cerr << "The test case reports that a schedule should exist!" << endl;
            }
        }
    }

    /* Displays the doctors and patients in a given hospital test case. */
    void showHospitalTest(const HospitalTestCase& test) {
        cout << "Here are the available doctors: " << endl;
        for (const auto& doctor: test.doctors) {
            cout << "   Doctor " << doctor.name << " (" << pluralize(doctor.hoursFree, "hour") << " free)" << endl;
        }
        cout << "Here are the patients that need to be seen: " << endl;
        for (const auto& patient: test.patients) {
            cout << "  Patient " << patient.name << " (" << pluralize(patient.hoursNeeded, "hour") << " needed)" << endl;
        }
    }

    /* Prompts the user for a hospital test, runs it, then asks to continue. */
    void testCanPatientsBeSeen() {
        do {
            HospitalTestCase testCase = userChooseHospitalTest();
            showHospitalTest(testCase);
            processHospitalTestCase(testCase);
        } while (askNicely("Another test case? "));
    }
}

int main() {
    testCanPatientsBeSeen();
    return 0; // Unreachable; silences warnings
}
