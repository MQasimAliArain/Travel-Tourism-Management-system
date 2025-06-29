#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <conio.h>  // For getch()

using namespace std;

struct Destination {
    string name;
    int cost;
    int days;
    int maxPersons;
};

struct Booking {
    string customerName;
    string phone;
    string email;
    Destination destination;
};

struct Admin {
    string username;
    string password;
};

vector<Destination> destinations;
vector<Booking> bookings;
Admin admin;

static string toLowerCase(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return (first == string::npos || last == string::npos) ? "" : str.substr(first, last - first + 1);
}

static void saveBookingsToFile() {
    ofstream outFile("bookings.txt");
    if (outFile.is_open()) {
        for (const auto& booking : bookings) {
            outFile << booking.customerName << "\n"
                << booking.phone << "\n"
                << booking.email << "\n"
                << booking.destination.name << "\n"
                << booking.destination.cost << "\n"
                << booking.destination.days << "\n"
                << booking.destination.maxPersons << "\n";
        }
        outFile.close();
    }
}

static void loadBookingsFromFile() {
    ifstream inFile("bookings.txt");
    if (inFile.is_open()) {
        Booking booking;
        while (getline(inFile, booking.customerName) &&
            getline(inFile, booking.phone) &&
            getline(inFile, booking.email) &&
            getline(inFile, booking.destination.name)) {
            inFile >> booking.destination.cost;
            inFile >> booking.destination.days;
            inFile >> booking.destination.maxPersons;
            inFile.ignore(); // Ignore newline after integer
            bookings.push_back(booking);
        }
        inFile.close();
    }
}

static void saveDestinationsToFile() {
    ofstream outFile("destinations.txt");
    if (outFile.is_open()) {
        for (const auto& dest : destinations) {
            outFile << dest.name << "\n"
                << dest.cost << "\n"
                << dest.days << "\n"
                << dest.maxPersons << "\n";
        }
        outFile.close();
    }
}

static void loadDestinationsFromFile() {
    ifstream inFile("destinations.txt");
    if (inFile.is_open()) {
        Destination dest;
        while (getline(inFile, dest.name)) {
            inFile >> dest.cost;
            inFile >> dest.days;
            inFile >> dest.maxPersons;
            inFile.ignore(); // Ignore newline after integer
            destinations.push_back(dest);
        }
        inFile.close();
    }
}

static bool confirmAction(const string& action) {
    char choice;
    cout << "Are you sure you want to " << action << "? (y/n): ";
    cin >> choice;
    return (choice == 'y' || choice == 'Y');
}

static bool loginAdmin() {
    string username, password;
    cout << "Enter username: ";
    cin >> username;

    cout << "Enter password: ";
    password = "";
    char ch;
    while (true) {
        ch = _getch(); // Get character without showing on screen
        if (ch == 13) break; // Enter key to finish input
        else if (ch == 8) { // Backspace key to remove last character
            if (password.length() > 0) {
                password.pop_back();
                cout << "\b \b"; // Remove the '*' character on screen
            }
        }
        else {
            password.push_back(ch);
            cout << "*"; // Display '*' on screen
        }
    }

    ifstream inFile("admin.txt");
    if (inFile.is_open()) {
        string storedUsername, storedPassword;
        getline(inFile, storedUsername);
        getline(inFile, storedPassword);
        inFile.close();

        if (username == storedUsername && password == storedPassword) {
            cout << "\n                       Login successful!\n";
            return true;
        }
    }
    cout << "\n                       Invalid credentials!\n";
    return false;
}

static void displayBookingTable(const vector<Booking>& bookings) {
    cout << left << setw(25) << "Customer Name"
        << setw(15) << "Phone"
        << setw(25) << "Email"
        << setw(25) << "Destination"
        << setw(10) << "Cost"
        << setw(10) << "Days"
        << setw(15) << "Max Persons" << "\n";
    cout << string(120, '-') << "\n";

    for (const auto& booking : bookings) {
        cout << left << setw(25) << booking.customerName
            << setw(15) << booking.phone
            << setw(25) << booking.email
            << setw(25) << booking.destination.name
            << setw(10) << booking.destination.cost
            << setw(10) << booking.destination.days
            << setw(15) << booking.destination.maxPersons << "\n";
    }
}

static void viewBookings() {
    if (bookings.empty()) {
        cout << "                       No bookings available.\n";
        return;
    }
    displayBookingTable(bookings);
}

static void cancelBooking() {
    string phone;
    cout << "Enter the phone number associated with the booking to cancel: ";
    cin >> phone;

    auto it = find_if(bookings.begin(), bookings.end(), [&](const Booking& b) {
        return b.phone == phone;
        });

    if (it != bookings.end()) {
        cout << "\nBooking details to cancel:\n";
        displayBookingTable({ *it });

        if (confirmAction("cancel this booking")) {
            bookings.erase(it);
            saveBookingsToFile();
            cout << "                       Booking canceled successfully!\n";
        }
        else {
            cout << "                       Booking cancellation aborted.\n";
        }
    }
    else {
        cout << "                       Booking not found!\n";
    }
}

static void registerAdmin() {
    string username, password;
    cout << "Enter a new username for the admin: ";
    cin >> username;

    cout << "Enter a new password for the admin: ";
    password = "";
    char ch;
    while (true) {
        ch = _getch(); // Get character without showing on screen
        if (ch == 13) break; // Enter key to finish input
        else if (ch == 8) { // Backspace key to remove last character
            if (password.length() > 0) {
                password.pop_back();
                cout << "\b \b"; // Remove the '*' character on screen
            }
        }
        else {
            password.push_back(ch);
            cout << "*"; // Display '*' on screen
        }
    }

    ofstream outFile("admin.txt");
    if (outFile.is_open()) {
        outFile << username << "\n" << password << "\n";
        outFile.close();
        cout << "\n                       Admin account created successfully!\n";
    }
}

static void deleteAdminAccount() {
    ifstream inFile("admin.txt");
    if (inFile.is_open()) {
        string storedUsername, storedPassword;
        getline(inFile, storedUsername);
        getline(inFile, storedPassword);
        inFile.close();

        cout << "\n                       Admin account details to be deleted:\n";
        cout << "Username: " << storedUsername << "\n";
        cout << "Password: " << storedPassword << "\n";

        if (confirmAction("delete the admin account")) {
            ofstream outFile("admin.txt", ios::trunc);  // Open file in truncate mode to delete its content
            outFile.close();
            cout << "                       Admin account deleted successfully!\n";
        }
        else {
            cout << "                       Deletion aborted.\n";
        }
    }
    else {
        cout << "                       Admin account not found.\n";
    }
}

static void adminInterface() {
    bool loggedIn = false;
    int choice;
    do {
        cout << "********************************************************************\n";
        cout << "*                          ADMIN INTERFACE                         *\n";
        cout << "********************************************************************\n";
        cout << "\n1. Login as Admin\n2. Register Admin\n3. View Bookings\n4. Cancel Booking\n5. Delete Admin Account\n6. Exit\nChoose an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "********************************************************************\n";
            cout << "*                          LOGIN AS ADMIN                          *\n";
            cout << "********************************************************************\n";
            loggedIn = loginAdmin();
            break;
        case 2:
            cout << "********************************************************************\n";
            cout << "*                         REGISTER AS ADMIN                        *\n";
            cout << "********************************************************************\n";
            registerAdmin();
            break;
        case 3:
            if (loggedIn) {
                cout << "********************************************************************\n";
                cout << "*                         VIEW BOOKINGS                            *\n";
                cout << "********************************************************************\n";
                viewBookings();
            }
            else {
                cout << "                       You must be logged in to access this option.\n";
            }
            break;
        case 4:
            if (loggedIn) {
                cout << "********************************************************************\n";
                cout << "*                         CANCEL BOOKINGS                          *\n";
                cout << "********************************************************************\n";
                cancelBooking();
            }
            else {
                cout << "                       You must be logged in to access this option.\n";
            }
            break;
        case 5:
            if (loggedIn) {
                cout << "********************************************************************\n";
                cout << "*                        DELETE ADMIN ACCOUNT                      *\n";
                cout << "********************************************************************\n";
                deleteAdminAccount();
                return;
            }
            else {
                cout << "                       You must be logged in to access this option.\n";
            }
            break;
        case 6:
            cout << "                       Exiting Admin Interface...\n";
            break;
        default:
            cout << "                       Invalid option!\n";
        }
    } while (choice != 6);
}

static void viewDestinations() {
    cout << left << setw(20) << "Destination" << setw(10) << "Cost" << setw(10) << "Days" << setw(15) << "Max Persons" << "\n";
    cout << string(55, '-') << "\n";

    for (const auto& dest : destinations) {
        cout << left << setw(20) << dest.name
            << setw(10) << dest.cost
            << setw(10) << dest.days
            << setw(15) << dest.maxPersons
            << "\n";
    }
}

static void bookDestination() {
    Booking booking;
    cout << "Enter your name: ";
    cin.ignore();
    getline(cin, booking.customerName);
    cout << "Enter your phone (11 digits): ";
    cin >> booking.phone;
    cout << "Enter your email: ";
    cin >> booking.email;

    viewDestinations();

    cout << "Enter the name of the destination you want to book: ";
    cin.ignore();
    string destName;
    getline(cin, destName);
    destName = toLowerCase(trim(destName));

    auto it = find_if(destinations.begin(), destinations.end(), [&](const Destination& d) {
        return toLowerCase(trim(d.name)) == destName;
        });

    if (it != destinations.end()) {
        booking.destination = *it;

        cout << "\n                       Booking details:\n";
        displayBookingTable({ booking });

        if (confirmAction("book this destination")) {
            bookings.push_back(booking);
            saveBookingsToFile();
            cout << "                       Booking successful!\n";
        }
        else {
            cout << "                       Booking aborted.\n";
        }
    }
    else {
        cout << "                           Destination not found!\n";
    }
}

static void filterDestinations() {
    int budget, days, persons;
    cout << "Enter your budget: ";
    cin >> budget;
    cout << "Enter number of days: ";
    cin >> days;
    cout << "Enter number of persons: ";
    cin >> persons;

    cout << left << setw(20) << "Destination" << setw(10) << "Cost" << setw(10) << "Days" << setw(15) << "Max Persons" << "\n";
    cout << string(55, '-') << "\n";

    bool found = false;

    for (const auto& dest : destinations) {
        if (dest.cost == budget && dest.days == days && dest.maxPersons == persons) {
            cout << left << setw(20) << dest.name
                << setw(10) << dest.cost
                << setw(10) << dest.days
                << setw(15) << dest.maxPersons
                << "\n";
            found = true;
        }
        else if (abs(dest.cost - budget) <= 0.1 * budget ||
            abs(dest.days - days) <= 2 ||
            abs(dest.maxPersons - persons) <= 2) {
            cout << left << setw(20) << dest.name
                << setw(10) << dest.cost
                << setw(10) << dest.days
                << setw(15) << dest.maxPersons
                << " (Nearby match)\n";
            found = true;
        }
    }

    if (!found) {
        cout << "                       No destinations found matching your criteria.\n";
    }
}

static void userInterface() {
    int choice;
    do {
        cout << "********************************************************************\n";
        cout << "*                          USER INTERFACE                          *\n";
        cout << "********************************************************************\n";
        cout << "\n1. View Destinations\n2. Book Destination\n3. Filter Destinations\n4. Exit\nChoose an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "********************************************************************\n";
            cout << "*                        VIEW DESTINATIONS                         *\n";
            cout << "********************************************************************\n";
            viewDestinations();
            break;
        case 2:
            cout << "********************************************************************\n";
            cout << "*                        BOOK DESTINATION                          *\n";
            cout << "********************************************************************\n";
            bookDestination();
            break;
        case 3:
            cout << "********************************************************************\n";
            cout << "*                      FILTER DESTINATIONS                         *\n";
            cout << "********************************************************************\n";
            filterDestinations();
            break;
        case 4:
            cout << "                       Exiting User Interface...\n";
            break;
        default:
            cout << "                       Invalid option!\n";
        }
    } while (choice != 4);
}

int main() {
    loadDestinationsFromFile();
    loadBookingsFromFile();

    int choice;
    do {
        cout << "********************************************************************\n";
        cout << "*                        MAIN MENU                                 *\n";
        cout << "********************************************************************\n";
        cout << "\n1. User Interface\n2. Admin Interface\n3. Exit\nChoose an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            userInterface();
            break;
        case 2:
            adminInterface();
            break;
        case 3:
            cout << "                       Exiting program...\n";
            break;
        default:
            cout << "                       Invalid option!\n";
        }
    } while (choice != 3);

    return 0;
}

