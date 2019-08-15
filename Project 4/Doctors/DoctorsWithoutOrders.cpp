#include "DoctorsWithoutOrders.h"
#include "map.h"
#include <climits>
#include <iostream>
using namespace std;

/* * * * Doctors Without Orders * * * */

/**
 * Given a list of doctors and a list of patients, determines whether all the patients can
 * be seen. If so, this function fills in the schedule outparameter with a map from doctors
 * to the set of patients that doctor would see.
 *
 * @param doctors  The list of the doctors available to work.
 * @param patients The list of the patients that need to be seen.
 * @param schedule An outparameter that will be filled in with the schedule, should one exist.
 * @return Whether or not a schedule was found.
 */

void generatePossibleCombinations(Vector<Patient> patients, Vector<Patient>& chosen, Vector<Vector<Patient> >& combinations);
Vector<Vector<Patient> > filterPossibleCombinations(Vector<Vector<Patient> > possibleCombinations, Doctor doctor);
bool containsPatient(Patient patient, Vector<Patient> patients);
Vector<Patient> dropPatients(Vector<Patient> assigned, Vector<Patient> patients);
int sumHoursNeeded(Vector<Patient> patients);
int sumHoursFree(Vector<Doctor> doctors);

bool canAllPatientsBeSeen(Vector<Doctor> &doctors,
                          Vector<Patient> &patients,
                          Map<string, Set<string>>& schedule) {
    Vector<Doctor> doctors_copy = doctors;         // Need to keep doctors unchanged in the end
    if (doctors.size() == 1 && sumHoursNeeded(patients) <= doctors[0].hoursFree) {
        Set<string> patientNames;
        for (int i=0; i<patients.size(); i++) {
            patientNames.add(patients[i].name);
        }
        schedule[doctors[0].name] = patientNames;
        return true;
    } else if (doctors.size() == 0) {
        if (patients.size() <= 0) {
            return true;          // If doctors and patients both do not exist, return true
        } else {
            return false;         // If only doctors do not exist, return false
        }
    } else {
        int totalHoursFree = sumHoursFree(doctors);
        int totalHoursNeeded = sumHoursNeeded(patients);

        // If the total hours needed by patients is greater than the
        // hours free of doctors, then directly return false
        if (totalHoursNeeded <= totalHoursFree) {
            // Make a choice: assign patients to a doctor
            // For each doctor's schedule,
            // find all possible combinations of patients
            // so that their sum of "hours needed" is less than
            // the "hours free" of that doctor
            Vector<Patient> chosen;
            Vector<Vector<Patient> > possibleCombinations;
            generatePossibleCombinations(patients, chosen, possibleCombinations);
            Vector<Vector<Patient> > filteredCombinations = filterPossibleCombinations(possibleCombinations, doctors[0]);

            Doctor first_doctor = doctors[0];
            doctors.remove(0);

            // Explore choices:
            for (int i=0; i<filteredCombinations.size(); i++) {
                // Scheduling
                Set<string> patientNames;
                for (int j=0; j<filteredCombinations[i].size(); j++) {
                    patientNames.add(filteredCombinations[i][j].name);
                }
                schedule[first_doctor.name] = patientNames;

                // For the remaining doctors and patients, schedule them with
                // recursive backtracking
                Vector<Patient> patientsNotSeen = dropPatients(filteredCombinations[i], patients);
                if (canAllPatientsBeSeen(doctors, patientsNotSeen, schedule)) {
                    doctors = doctors_copy;
                    return true;
                }

                // Un-scheduling
                schedule.remove(first_doctor.name);
            }

            // Un-choose/Backtracking:
            doctors.insert(0, first_doctor);
        }
        return false;
    }
}

void generatePossibleCombinations(Vector<Patient> patients, Vector<Patient>& chosen, Vector<Vector<Patient> >& combinations) {
    // Find all combinations of given patients regardless of ordering
    if (patients.isEmpty()) {
        combinations.add(chosen);
    } else {
        // Two choices to make in this case:
        // to include or not to include the first element
        Patient first_patient = patients[0];
        patients.remove(0);

        // Choose/explore all combinations WITH first_patient
        chosen.add(first_patient);
        generatePossibleCombinations(patients, chosen, combinations);

        // Choose/explore all combinations WITHOUT first_patient
        chosen.remove(chosen.size()-1);
        generatePossibleCombinations(patients, chosen, combinations);

        // Un-choose/Backtracking
        patients.insert(0, first_patient);
    }
}

Vector<Vector<Patient> > filterPossibleCombinations(Vector<Vector<Patient> > possibleCombinations, Doctor doctor) {
    Vector<Vector<Patient> > filteredCombinations;
    // Filter out those patient combinations whose sum of
    // "hours needed" is greater than "hours free" of the doctor
    int totalHoursFree = doctor.hoursFree;
    for (int i=0; i<possibleCombinations.size(); i++) {
        int totalHoursNeeded = 0;
        for (int j=0; j<possibleCombinations[i].size(); j++) {
            totalHoursNeeded += possibleCombinations[i][j].hoursNeeded;
        }
        if (totalHoursNeeded <= totalHoursFree) {
            filteredCombinations.add(possibleCombinations[i]);
        }
    }
    return filteredCombinations;
}

bool containsPatient(Patient patient, Vector<Patient> patients) {
    for (int i=0; i<patients.size(); i++) {
        // Assume no two patients have the same name
        if (patient.name == patients[i].name) {
            return true;
        }
    }
    return false;
}

Vector<Patient> dropPatients(Vector<Patient> assigned, Vector<Patient> patients) {
    // Drop the patients that have already been assigned a doctor
    Vector<Patient> patientsNotSeen;
    for (int i=0; i<patients.size(); i++) {
        // Tried to use a Set here, but need to define operands first
        // according to Stanford library documents
        if (!containsPatient(patients[i], assigned)) {
            patientsNotSeen.add(patients[i]);
        }
    }
    return patientsNotSeen;
}

int sumHoursNeeded(Vector<Patient> patients) {
    int sum = 0;
    for (int i=0; i<patients.size(); i++) {
        sum += patients[i].hoursNeeded;
    }
    return sum;
}

int sumHoursFree(Vector<Doctor> doctors) {
    int sum = 0;
    for (int i=0; i<doctors.size(); i++) {
        sum += doctors[i].hoursFree;
    }
    return sum;
}


