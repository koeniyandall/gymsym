#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <limits>
#include "gymSym.hpp"

using namespace std;

int main() {
    string s, username, password;
    Manager GM;
    int age = 0;
    int weight = 0;
    int height = 0;
    bool done;
    vector<string> sv;
    char decision;
    
    // Load existing user data
    GM.openInfo();

    cout << "------------ Welcome to your gym companion --------" << endl;
    cout << "-------------- Type 'help' for commands! ----------" << endl;

    // User login or registration flow
    while (true) {
        cout << "Are you a returning user? (y/n): ";
        cin >> decision;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

        if (decision == 'y') {
            // Attempt to log in
            cout << "Please enter your username: ";
            cin >> username;
            cout << "Please enter your password: ";
            cin >> password;

            if (GM.login(username, password)) {
                cout << "Welcome back, " << username << "!" << endl;
                break;
            } else {
                cout << "Incorrect username or password. Please try again." << endl;
            }
        } else if (decision == 'n') {
            // Register a new account
            cout << "Please enter a new username: ";
            cin >> username;
            cout << "Please enter a password: ";
            cin >> password;

            cout << "Enter your age: ";
            cin >> age;
            cout << "Enter your weight (in lbs): ";
            cin >> weight;
            cout << "Enter your height (in inches): ";
            cin >> height;

            if (GM.createUser(username, age, weight, height)) {
                cout << "Account created successfully! Welcome, " << username << "!" << endl;
                break;
            } else {
                cout << "Error: Failed to create account. Please try again." << endl;
            }
        } else {
            cout << "Invalid choice. Please enter 'y' for yes or 'n' for no." << endl;
        }
    }

    // Main command processing loop
    do {
		cout << "GymSym> ";
        done = !getline(cin, s); // Check if user is inputting into standard input
        if (!done) {
            sv.clear();
            istringstream ss(s);

            // Parse the command line arguments into vector `sv`
            string w;
            while (ss >> w) {
                sv.push_back(w);
            }

            // Process commands
            if (sv.empty()) continue;

            if (sv[0] == "help") {
                GM.help_msg();

            } else if (sv[0] == "LOG_STATS") {
                // Ensure the user is logged in
                if (GM.currUserCheck() == nullptr) {
                    cout << "Error: No user is currently logged in. Please log in first." << endl;
                } else {
                    if (!GM.logUserDetails()) {
                        cout << "Error: Failed to log stats for the current user." << endl;
                    }
                }

            } else if (sv[0] == "CREATE_USER") {
                if (sv.size() != 5) {
                    cout << "Error, usage: CREATE_USER <username> <age> <weight(in lbs)> <height(in inches)>" << endl;
                } else {
                    username = sv[1];
                    try {
                        age = stoi(sv[2]);
                        weight = stoi(sv[3]);
                        height = stoi(sv[4]);
                    } catch (const invalid_argument&) {
                        cout << "Error: Age, weight, and height must be integers." << endl;
                        continue;
                    }

                    if (GM.createUser(username, age, weight, height)) {
                        cout << "CREATE_USER successful!" << endl;
                    } else {
                        cout << "Error: Failed to create user." << endl;
                    }
                }

            } else if (sv[0] == "CHECK_STATS") {
                // Ensure the user is logged in
                if (GM.currUserCheck() == nullptr) {
                    cout << "Error: No user is currently logged in. Please log in first." << endl;
                } else {
                    if (GM.checkStats()) {
                        cout << "CHECK_STATS successful!" << endl;
                    } else {
                        cout << "Error: Failed to check stats for the current user." << endl;
                    }
                }
                
            } else if (sv[0] == "q" || sv[0] == "Q") {
                cout << "Thanks for using GymSym!" << endl;
                GM.saveInfo();
                exit(0);
            }else if(sv[0] == "LOG_PR"){
				if(GM.logPr()){
					cout << "Successfully logged stats!" << endl;
				}else{
					cout << "Error: failed to log PR for user" << endl;
				}
			}else if(sv[0] == "UPDATE_GOAL"){
				if(GM.updateGoal()){
					cout << "Successfully logged goal!" << endl;
				}else{
					cout << "Error: failed to log goal" << endl;
				}
			}else if(sv[0] == "CHECK_MACROS"){
				if(GM.foodCheck()){
					cout << "Successfully viewed macros" << endl;
				}else{
					cout << "Error: failed to look at macros" << endl;
				}
			}else if (sv[0] == "RECOMMEND_GOAL") {
                if (GM.recommendGoal()) {
                    cout << "Goal recommendation successful!" << endl;
                } else {
                    cout << "Error: Unable to recommend goal." << endl;
                }
            }else {
                cout << "Invalid command. Type 'help' for the list of commands." << endl;
            }
        }
    } while (!done);

    // Save user data on exit
    GM.saveInfo();
    return 0;
}

