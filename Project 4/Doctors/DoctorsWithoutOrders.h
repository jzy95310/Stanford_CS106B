#ifndef DoctorsWithoutOrders_Included
#define DoctorsWithoutOrders_Included

#include <istream>
#include <string>
#include <ostream>
#include "vector.h"
#include "set.h"
#include "map.h"


/* * * * * Doctors Without Orders * * * * */

/**
 * A type representing a doctor and the number of hours they can work.
 */
struct Doctor {
    std::string name; // Their name, for testing purposes.
    int hoursFree;    // How many hours they have free to work.
};

/**
 * A type representing a patient and the number of hours they need to be seen for.
 */
struct Patient {
    std::string name; // Their name, for testing purposes.
    int hoursNeeded;  // How many hours they must be seen for.
};

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
bool canAllPatientsBeSeen(Vector<Doctor>& doctors,
                          Vector<Patient>& patients,
                          Map<std::string, Set<std::string>>& schedule);

/* * * * * Printing Routines * * * * */

/* These functions allow you to print out our structs using cout. You don't need to use these
 * functions, but they might be helpful during testing. They're implemented in Printing.cpp
 * in case you're curious how they work.
 */
std::ostream& operator<< (std::ostream& out, const Doctor& doc);
std::ostream& operator<< (std::ostream& out, const Patient& patient);

/* * * * * Testing Materials * * * * */

/**
 * Type representing a test case for the Doctors Without Orders problem.
 */
struct HospitalTestCase {
    Vector<Doctor>  doctors;
    Vector<Patient> patients;
    bool isPossible;
};

/**
 * Given a stream containing a hospital test case file, reads the
 * data from that file and returns a HospitalTestCase containing
 * the relevant information.
 *
 * @param source The stream to read from.
 * @return The test case from that file.
 * @throws ErrorException If the test case is malformed.
 */
HospitalTestCase loadHospitalTestCase(std::istream& source);

#endif
