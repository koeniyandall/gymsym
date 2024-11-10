#ifndef GYMSYM_HPP
#define GYMSYM_HPP

#include <unordered_map>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

class User {
    public:
        string username;
        string password;
        unsigned int passwordHash;
        int age;
        int weight;
        int height;
		int benchM;
		int squatM;
		int deadliftM;
		string goal;

        // Parameterized constructor
        User(const std::string& uname, string& pwd, int a, int w, int h, int bMax, int sMax, int dlM, string& persgoal)
            : username(uname), password(pwd), age(a), weight(w), height(h) , benchM(bMax), squatM(sMax), deadliftM(dlM), goal(persgoal){}

        // Default constructor
        User() : age(0), weight(0), height(0), benchM(0), squatM(0), deadliftM(0), goal("goal"){}
};

class Manager {
    public:
        //methods implemented:
		User* currUserCheck();
        bool login(string username, string password);
        bool help_msg();
        bool logUserDetails();
        bool checkStats();
        bool createUser(string username, int age, int weight, int height);
        void saveInfo();
        void openInfo();
        ~Manager();
		bool logPr();
		bool updateGoal();
		//TODO methods
		bool compareStats();
		bool foodCheck();
        bool recommendGoal();

    private:
        unordered_map<string, User*> Names;  // Map of username to User object
         string getGoalRecommendation(int age, int weight, int height, int benchM, int squatM, int deadliftM);  // Helper function
};

#endif
