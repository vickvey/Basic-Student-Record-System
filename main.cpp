#include <iostream>
#include <cstring>
#include <string>
#include <sqlite3.h>

const char DATABASE_FILENAME[] = "students.db";

#define ER_NP -1 // NULL pointer
#define ER_CDB -2 // ERROR creating or opening database
#define ER_PRP_STMT -3 // ERROR preparing SQL statement
#define ER_EXC_STMT -4 // ERROR executing SQL statement
#define ER_NO_DATA -5 // ERROR no data found in the database
#define SUCCESS 1 // success

typedef struct {
    int id;
    std::string first_name;
    std::string last_name;
} Student;

class Database {
public:
    Database();
    ~Database(); // to close a database connection

    // database functions
    int openDB(const char* database_name);
    int createTable();

    // operation functions
    int addStudent(const Student& student);
    int viewStudent(int ID);
    int viewAllStudents();
private:
    sqlite3 *db;
};

void Welcome();
void Menu();

int main() {
    //code here
    int prompt = -1, cont = -1;
    Database database;
    Student student;
    if(database.openDB(DATABASE_FILENAME) != SUCCESS) {
        std::cerr << "ERROR: opening database\n" << std::endl;
        return 0;
    } // database is ready to write SQL queries

    // Create the students table if it doesn't exist
    if (database.createTable() != SUCCESS) {
        std::cerr << "ERROR: creating students table\n" << std::endl;
    }

    // program starts from here 
    Welcome();
    while(cont != 0) {
        Menu();
        std::cin >> prompt;

        switch (prompt) {
        case 1:
            // to add a student into the database
            std::cout << "\nTo add a student to database, follow the instructions : \n";
            std::cout << "\nEnter the firstName of student : \n";
            std::cin >> student.first_name;
            std::cout << "\nEnter the lastName of student : \n";
            std::cin >> student.last_name;
            student.id = -1;

            prompt = database.addStudent(student);
            if(prompt != SUCCESS) {
                std::cerr << "Error : adding student to database.\n";
                break;
            } else {
                std::cout << "Student added to database successfully.\n";
                break;
            }

        case 2:
            // to view a student from the database
            std::cout << "\n\nEnter the id of the student you want to view : ";
            std::cin >> student.id;

            prompt = database.viewStudent(student.id);
            if(prompt != SUCCESS) {
                std::cerr << "Error : viewing the student.\n";
                break;
            } else {
                std::cout << "\nYou have viewed the data successfully.\n";
                break;
            }

        case 3:
            // to view all students from the database
            std::cout << "\n\nHere is the complete database : \n\n";

            prompt = database.viewAllStudents();
            if(prompt != SUCCESS) {
                std::cerr << "Error : viewing the students.\n";
                break;
            } else {
                std::cout << "\nYou have viewed the database successfully.\n";
                break;
            }
            break;

        case 0:
            // to exit the program
            std::cout << "\n\nThanks for using the program!!\n";
            std::cout << "Have a nice data:)\n\n";
            cont = 0;
            break;
        
        default:
            // default for invalid input
            puts("Invalid input!!\n");
            break;
        }
    }
    return 0;
}

Database::Database() : db(nullptr) {};

int Database::
openDB(const char *database_name) {
    if(database_name == nullptr) {
        return ER_NP;
    }
    int rc = sqlite3_open(database_name, &db);
    if(rc) {
        std::cerr << "Can't open database: " \
        << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return ER_CDB;
    } else {
        std::cerr << "\n\nOpened database successfully\n\n" << std::endl;
    } 
    // ALL OK
    return SUCCESS;
}

int Database::createTable() {
    int rc = -1;
    const char *sql_stmt =
    "CREATE TABLE students (" \
    "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL DEFAULT 22001," \
    "FIRST_NAME TEXT NOT NULL," \
    "LAST_NAME TEXT NOT NULL);";

    // code here
    rc = sqlite3_exec(db, sql_stmt, 0, 0, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return ER_CDB;
    } else {
        std::cerr << "Table created successfully" << std::endl;
        // ALL OK
        return SUCCESS;
    }
}

int Database::
addStudent(const Student& student) {
    int rc = -1;
    const char *sql_stmt = 
    "INSERT INTO students (FIRST_NAME, LAST_NAME) VALUES (?, ?);";

    sqlite3_stmt *stmt; // Declare a pointer to a SQLite prepared statement
    // Prepare the SQL statement.
    rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        /**
        If preparing the statement fails, print an error message 
        and return an error code.
        */
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return ER_PRP_STMT;
    }

    // Bind the values from the student struct to the placeholders in the prepared statement.
    sqlite3_bind_text(stmt, 1, student.first_name.c_str(), -1, SQLITE_STATIC); // Bind the FIRST_NAME field.
    sqlite3_bind_text(stmt, 2, student.last_name.c_str(), -1, SQLITE_STATIC); // Bind the LAST_NAME field.

    rc = sqlite3_step(stmt); // Execute the SQL statement

    if (rc != SQLITE_DONE) {
        /** 
        If executing the statement fails, print an error message,
        finalize the statement, and return an error code.
         */
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt); // Clean up and finalize the prepared statement.
        return ER_EXC_STMT;
    }

    sqlite3_finalize(stmt); // Clean up and finalize the prepared statement.
    // ALL OK
    return SUCCESS;
}

int Database::viewStudent(int id) {
    int rc = -1;
    const char *sql_stmt = 
    "SELECT * FROM students "\
    "WHERE ID = ?;";

    sqlite3_stmt *stmt; // Declare a pointer to a SQLite prepared statement.
    // Prepare the SQL statement.
    rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        /**
        If preparing the statement fails, print an error message
        and return an error code.
        */
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return ER_PRP_STMT;
    }

    // Bind the ID value to the placeholder in the prepared statement.
    sqlite3_bind_int(stmt, 1, id);

    // Execute the SQL statement.
    rc = sqlite3_step(stmt);

    if(rc == SQLITE_ROW) {
        // If a row is returned, print the student info
        int studentID = sqlite3_column_int(stmt, 0); // Get the ID
        // Get the FIRST_NAME.
        const char *firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)); 
         // Get the LAST_NAME.
        const char *lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        std::cout << "\n\nStudent ID: " << studentID << std::endl;
        std::cout << "First Name: " << firstName << std::endl;
        std::cout << "Last Name: " << lastName << std::endl;

        sqlite3_finalize(stmt); // Clean up and finalize the prepared statement.
        // ALL OK 
        return SUCCESS;
    } else if(rc == SQLITE_DONE) {
        // If no rows match the given ID, print a message and return a code to indicate no data found.
        std::cout << "No student found with ID: " << id << std::endl;
        sqlite3_finalize(stmt); // Clean up and finalize the prepared statement.
        return ER_NO_DATA;
    } else {
        // If executing the statement fails for another reason, print an error message,
        // finalize the statement, and return an error code.
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt); // Clean up and finalize the prepared statement.
        return ER_EXC_STMT;
    }
}

int Database::viewAllStudents() {
    int rc = -1;
    const char *sql_stmt = "SELECT * FROM students;";

    sqlite3_stmt *stmt; // Declare a pointer to a SQLite prepared statement.
    // Prepare the SQL statement.
    rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        /**
        If preparing the statement fails, print an error message
        and return an error code.
        */
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return ER_PRP_STMT;
    }

    // Execute the SQL statement.
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // If a row is returned, print the student info
        int studentID = sqlite3_column_int(stmt, 0); // Get the ID
        // Get the FIRST_NAME.
        const char *firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)); 
        // Get the LAST_NAME.
        const char *lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        std::cout << "\n\nStudent ID: " << studentID << std::endl;
        std::cout << "First Name: " << firstName << std::endl;
        std::cout << "Last Name: " << lastName << std::endl;
    }

    if (rc != SQLITE_DONE) {
        // If executing the statement fails for another reason, print an error message,
        // finalize the statement, and return an error code.
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt); // Clean up and finalize the prepared statement.
        return ER_EXC_STMT;
    }

    sqlite3_finalize(stmt); // Clean up and finalize the prepared statement.
    // ALL OK 
    return SUCCESS;
}

Database::~Database() {
    sqlite3_close(db);
}

void Welcome() {
    puts("\n\n");
    puts("Hello User !!\n");
    puts("Welcome to Database system!!\n");
}

void Menu() {
    puts("\n\n");
    puts("Press 1: to add a student to database.\n");
    puts("Press 2: to view about a student in the database.\n");
    puts("Press 3: to view all students in the database.\n");
    puts("Press 0: to exit the program.\n");
}

