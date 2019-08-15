#include "DoctorsWithoutOrders.h"
#include "Common.h"
#include <sstream>
#include <climits>
using namespace std;

/* Prints a human-readable version of a Doctor struct. */
ostream& operator<< (ostream& out, const Doctor& doc) {
    ostringstream result;
    result << "[ Doctor " << doc.name << " ("
           << pluralize(doc.hoursFree, "hour") << " free) ]";
    return out << result.str();
}

/* Prints a human-readable version of a Patient struct. */
ostream& operator<< (ostream& out, const Patient& patient) {
    ostringstream result;
    result << "[ Patient " << patient.name << " ("
           << pluralize(patient.hoursNeeded, "hour") << " needed) ]";
    return out << result.str();
}