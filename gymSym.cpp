#include "gymSym.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <cstdio>    // For popen and fgets
#include <memory>
#include <stdexcept>
#include <string>

using namespace std;

// Pointer to the currently logged-in user
User* currUser = nullptr;
User* Manager::currUserCheck(){
	User* user = currUser;
	return user;
}

unsigned int djb_hash(const string &s)//function for hashing passwords
{
size_t i;
unsigned int h;

h = 5381;

for (i = 0; i < s.size(); i++) {
    h = (h << 5) + h + static_cast<unsigned int>(s[i]);
}
return h;
}

string Manager::getGoalRecommendation(int age, int weight, int height, int benchM, int squatM, int deadliftM) {
    string command = "python3 gymsym_predictor.py " +
                    to_string(age) + " " + to_string(weight) + " " + to_string(height) + " " +
                    to_string(benchM) + " " + to_string(squatM) + " " + to_string(deadliftM);

    string result;
    char buffer[128];
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        throw runtime_error("Failed to start Python script.");
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);

    if (!result.empty() && result[result.size() - 1] == '\n') {
        result.pop_back();
    }

    return result;
}

// Method to recommend goal based on user stats
bool Manager::recommendGoal() {
    if (currUser == nullptr) {
        cout << "Error: No user is currently logged in." << endl;
        return false;
    }

    int age = currUser->age;
    int weight = currUser->weight;
    int height = currUser->height;
    int benchM = currUser->benchM;
    int squatM = currUser->squatM;
    int deadliftM = currUser->deadliftM;

    string recommendation;
    try {
        recommendation = getGoalRecommendation(age, weight, height, benchM, squatM, deadliftM);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return false;
    }

    cout << "Based on your stats, the recommended goal is: " << recommendation << endl;
    return true;
}

bool Manager::foodCheck() {
    string gender;
    double bmr = 0.0;
    bool correctGender = false;

    // Get and validate gender input
    while (!correctGender) {
        cout << "Please enter your gender (male/female): ";
        cin >> gender;

        if (gender == "male" || gender == "female") {
            correctGender = true;
        } else {
            cout << "Invalid input. Please specify 'male' or 'female'." << endl;
        }
    }

    // Convert weight and height to metric for BMR calculation
    double weightKg = currUser->weight * 0.453592;  // 1 lb = 0.453592 kg
    double heightCm = currUser->height * 2.54;      // 1 inch = 2.54 cm

    // Calculate BMR based on gender
    if (gender == "male") {
        bmr = 10.0 * weightKg + 6.25 * heightCm - 5.0 * currUser->age + 5.0;
    } else if (gender == "female") {
        bmr = 10.0 * weightKg + 6.25 * heightCm - 5.0 * currUser->age - 161.0;
    }

    // Assume a moderately active lifestyle for TDEE calculation
    double tdee = bmr * 1.55;

    // Adjust TDEE based on fitness goal
    if (currUser->goal == "lean") {
        tdee -= 400;  // Caloric deficit for weight loss
    } else if (currUser->goal == "bulk") {
        tdee += 300;  // Caloric surplus for muscle gain
    }

    // Display calorie goal
    cout << "Based on your goal, weight, and height, your caloric goal is: " << static_cast<int>(tdee) << " calories." << endl;

    // Macronutrient distribution based on goal
    double proteinRatio = 0.0, carbRatio = 0.0, fatRatio = 0.0;
    if (currUser->goal == "lean") {
        proteinRatio = 0.40;
        carbRatio = 0.40;
        fatRatio = 0.20;
    } else if (currUser->goal == "bulk") {
        proteinRatio = 0.30;
        carbRatio = 0.40;
        fatRatio = 0.30;
    }

    // Calculate grams for each macronutrient based on TDEE and their calorie ratios
    double proteinGrams = (tdee * proteinRatio) / 4.0;  // 4 calories per gram of protein
    double carbGrams = (tdee * carbRatio) / 4.0;        // 4 calories per gram of carbohydrate
    double fatGrams = (tdee * fatRatio) / 9.0;          // 9 calories per gram of fat

    // Display macronutrient goals, converting from double to integer after calculation
    cout << "Protein goal: " << static_cast<int>(proteinGrams) << "g" << endl;
    cout << "Carb goal: " << static_cast<int>(carbGrams) << "g" << endl;
    cout << "Fat goal: " << static_cast<int>(fatGrams) << "g" << endl;

    return true;
}


bool Manager::updateGoal(){
	bool done = false;
	int decision = 0;
	while(!done){
		cout << endl << "Would you like to bulk up of get lean?" << endl;
		cout << "1. Bulk" << endl;
		cout << "2. Lean" << endl;
		cin >> decision;
		if(decision == 1){
			currUser -> goal = "bulk";
			done = true;
		}else if(decision == 2){
			currUser -> goal = "lean";
			done = true;
		}else{
			cout << "Incorrect input, please enter valid decision" << endl;
		}
	}
	return true;
}

bool Manager::logPr(){
	int benchmax = 0;
	int squatmax = 0;
	int deadliftmax = 0;
	bool quit = false;
	int decision = 0;
	cout << "----------PR Logger----------" << endl;
	cout << "Please choose an option('0' to quit)" << endl;
	while(!quit){
		cout << endl << "0. quit" << endl;
		cout << "1. Log a bench PR" << endl;
		cout << "2. Log a squat PR" << endl;
		cout << "3. Log a deadlift PR" << endl;
		cin >> decision;
		if(decision == 1){
			cout << endl << "Please enter your bench PR (in lbs)" << endl;
			cin >> benchmax;
			currUser -> benchM = benchmax;
			cout << "Successfully Logged bench max!" << endl;
		}else if(decision == 2){
			cout << endl << "Please enter your squat PR (in lbs)" << endl;
			cin >> squatmax;
			currUser -> squatM = squatmax;
			cout << "Successfully Logged squat max!" << endl;
		}else if(decision ==3){
			cout << endl << "Please enter your deadlift PR (in lbs)" << endl;
			cin >> deadliftmax;
			currUser -> deadliftM = deadliftmax;
			cout << "Successfully logged deadlift max!" << endl;
		}else if(decision ==0){
			quit = true;
		}
		cin.clear();
	}
	return true;
}

bool Manager::login(string username, string password) {
    // Debugging: Print all users in the map for verification
    unordered_map<string, User*>::iterator it;
    for (it = Names.begin(); it != Names.end(); ++it) {
        cout << "Stored user - Username: " << it->first
            << ", Password: " << it->second->password << endl;
    }

    // Find the user by username
    it = Names.find(username);
    if (it == Names.end()) {
        // Username not found in the map
        cout << "Username not found." << endl;
        return false;
    } else {
        unsigned int hashedPassword = djb_hash(password);
        it -> second -> passwordHash = hashedPassword;
        // Username found; verify the password
        if (it->second->passwordHash == djb_hash(password)) {
            currUser = it->second;
            cout << "Login successful!" << endl << endl;
            return true;
        } else {
            cout << "Incorrect password for the given username." << endl;
            return false;
        }
    }
}


void Manager::saveInfo() {
    ofstream ofs("user.txt");

    if (!ofs.is_open()) {
        cerr << "Error: Could not open user data file 'user.txt' for saving." << endl;
        return;
    }
    unordered_map<string, User*>::iterator it;
    for (it = Names.begin(); it != Names.end(); ++it) {
        User* user = it->second;
        ofs << it->first << " "    //keyed by the username
            << djb_hash(user->password) << " "
			<< user -> goal << " "
            << user->age << " "
            << user->weight << " "
            << user->height << " "
            << user->benchM << " "
            << user->squatM << " "
            << user->deadliftM << endl;
    }

    ofs.close();
    cout << "save success!" << endl;
}


bool Manager::help_msg() {
    cout << "----------Commands------------" << endl;
    cout << "help" << endl;
    cout << "LOG_STATS" << endl;
    cout << "CREATE_USER <username> <age> <weight(in lbs)> <height(in inches)>" << endl;
    cout << "CHECK_STATS" << endl;
	cout << "LOG_PR" << endl;
	cout << "UPDATE_GOAL" << endl;
	cout << "CHECK_MACROS" << endl;
    cout << "RECCOMEND_GOAL" << endl;
	cout << endl << endl;
    return true;
}

bool Manager::logUserDetails() {
    if (currUser == nullptr) {
        cout << "Error: No user is currently logged in." << endl;
        return false;
    }

    int age, weight, height;
    cout << "Please enter your age: ";
    cin >> age;
    cout << "Please enter your weight (in lbs): ";
    cin >> weight;
    cout << "Please enter your height (in inches): ";
    cin >> height;

    currUser->age = age;
    currUser->weight = weight;
    currUser->height = height;

    cout << "Successfully updated user info for " << currUser->username << endl;
    return true;
}

bool Manager::checkStats() {
    if (currUser == nullptr) {
        cout << "Error: No user is currently logged in." << endl;
        return false;
    }

    cout << "----------Your Stats----------" << endl;
    cout << "Username: " << currUser->username << endl;
	cout << "Goal: " << currUser -> goal << endl;
    cout << "Age: " << currUser->age << endl;
    cout << "Weight: " << currUser->weight << " lbs" << endl;
    cout << "Height: " << currUser->height << " inches" << endl;
	cout << "Bench Max: " << currUser -> benchM << " lbs" << endl;
	cout << "Squat Max: " << currUser -> squatM << " lbs" << endl;
	cout << "Deadlift Max: " << currUser -> deadliftM << " lbs" << endl;
	cout << endl << endl;
    return true;
}

void Manager::openInfo() {
    ifstream inf("user.txt");

    if (!inf.is_open()) {
        cerr << "Error: Could not open user data file 'user.txt' for reading." << endl;
        return;
    }

    string line;
    int lineNum = 1;
    while (getline(inf, line)) {
        istringstream iss(line);
        string username, goal; // Default goal if missing
        int age, weight, height, benchM = 0, squatM = 0, deadliftM = 0, password;

        // Attempt to parse each line with optional fields for benchM, squatM, deadliftM, and goal
        if (iss >> username >> password >> goal >> age >> weight >> height) {
            // Parse optional fields
            if (!(iss >> benchM)) benchM = 0;
            if (!(iss >> squatM)) squatM = 0;
            if (!(iss >> deadliftM)) deadliftM = 0;

            // Create a new user and add to the map with username as the key
            User* user = new User();
            user->username = username;
            user->password = to_string(djb_hash(to_string((password))));
            user->age = age;
            user->weight = weight;
            user->height = height;
            user->benchM = benchM;
            user->squatM = squatM;
            user->deadliftM = deadliftM;
            user->goal = goal;

            Names[username] = user;
            cout << "Loaded user: " << username << " from line " << lineNum << endl;
        } else {
            cerr << "Error: Incorrect format in user data file on line " << lineNum << ": " << line << endl;
        }
        lineNum++;
    }

    inf.close();
}


bool Manager::createUser(string username, int age, int weight, int height) {
    string pass;
    bool uniqueUsername = false;

    // Check if username is unique
    while (!uniqueUsername) {
        cout << "Please enter a password you'd like to use: ";
        cin >> pass;
        uniqueUsername = true;

        // Check if the username already exists
        if (Names.find(username) != Names.end()) {
            cout << "Sorry, that username is already taken. Please try again." << endl;
            uniqueUsername = false;
        }
    }

    // Create and store the new user
    User* newUser = new User();
	newUser -> username = username;
	newUser -> age = age;
	newUser -> weight = weight;
	newUser -> height = height;
	newUser -> password = pass;
    Names[username] = newUser;
    currUser = newUser;
    cout << "User successfully created!" << endl;
    return true;
}

// Destructor to free dynamically allocated memory
Manager::~Manager() {
    unordered_map<string, User*>::iterator it;
    for (it = Names.begin(); it != Names.end(); ++it) {
        delete it->second;  // Free each User object
    }
    Names.clear();
}


