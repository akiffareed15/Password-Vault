#include <iostream>
#include <fstream>
#include <string>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

const char XOR_KEY = 'K';

// Utility: Hide input while typing (for password)
string getHiddenInput() {
    string input;
    char ch;
#ifdef _WIN32
    while ((ch = _getch()) != '\r') {
        if (ch == '\b' && !input.empty()) {
            cout << "\b \b";
            input.pop_back();
        } else {
            input += ch;
            cout << '*';
        }
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    getline(cin, input);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    cout << endl;
    return input;
}

// Encrypt/Decrypt with XOR
string xorEncryptDecrypt(const string& data) {
    string result = data;
    for (char& c : result)
        c ^= XOR_KEY;
    return result;
}

// Create or Read master password
bool checkOrSetMasterPassword() {
    ifstream fin("master.txt");
    string savedPassword;

    if (getline(fin, savedPassword)) {
        fin.close();
        int attempts = 3;
        while (attempts--) {
            cout << "Enter Master Password (" << (attempts + 1) << " attempts left): ";
            string input = getHiddenInput();
            if (input == xorEncryptDecrypt(savedPassword)) {
                return true;
            } else {
                cout << " Incorrect Password!\n";
            }
        }
        return false; // All attempts failed
    } else {
        fin.close();
        cout << "No master password found. Create one now.\n";
        cout << "Set New Master Password: ";
        string newPass = getHiddenInput();

        ofstream fout("master.txt");
        fout << xorEncryptDecrypt(newPass);
        fout.close();
        cout << " Master password set successfully!\n";
        return true;
    }
}


// Change master password
void changeMasterPassword() {
    cout << "\n Change Master Password\n";
    cout << "Enter Current Password: ";
    string current = getHiddenInput();

    ifstream fin("master.txt");
    string savedPassword;
    getline(fin, savedPassword);
    fin.close();

    if (xorEncryptDecrypt(savedPassword) == current) {
        cout << "Enter New Password: ";
        string newPass = getHiddenInput();

        ofstream fout("master.txt");
        fout << xorEncryptDecrypt(newPass);
        fout.close();

        cout << "Master password changed!\n";
    } else {
        cout << "Incorrect current password!\n";
    }
}

// Add new credential
void addCredential() {
    string website, username, password;

    cout << "\n--- Add Credential ---\n";
    cout << "Enter Website: ";
    cin >> website;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    ofstream fout("vault.txt", ios::app);
    string entry = website + "," + username + "," + password;
    fout << xorEncryptDecrypt(entry) << endl;
    fout.close();

    cout << "Credential Saved Successfully!\n";
}

// View all credentials
void viewCredentials() {
    ifstream fin("vault.txt");
    string line;

    cout << "\n Saved Credentials:\n";
    while (getline(fin, line)) {
        string decrypted = xorEncryptDecrypt(line);
        size_t pos1 = decrypted.find(",");
        size_t pos2 = decrypted.rfind(",");

        string website = decrypted.substr(0, pos1);
        string username = decrypted.substr(pos1 + 1, pos2 - pos1 - 1);
        string password = decrypted.substr(pos2 + 1);

        cout << " Website: " << website << endl;
        cout << " Username: " << username << endl;
        cout << " Password: " << password << endl;
        cout << "--------------------------\n";
    }
    fin.close();
}

// Search for credential
void searchCredential() {
    string searchSite;
    cout << "\n Search Credential\n";
    cout << "Enter Website to search: ";
    cin >> searchSite;

    ifstream fin("vault.txt");
    string line;
    bool found = false;

    while (getline(fin, line)) {
        string decrypted = xorEncryptDecrypt(line);
        size_t pos1 = decrypted.find(",");
        size_t pos2 = decrypted.rfind(",");
        string website = decrypted.substr(0, pos1);

        if (website == searchSite) {
            string username = decrypted.substr(pos1 + 1, pos2 - pos1 - 1);
            string password = decrypted.substr(pos2 + 1);
            cout << " Website: " << website << endl;
            cout << " Username: " << username << endl;
            cout << " Password: " << password << endl;
            cout << "--------------------------\n";
            found = true;
        }
    }
    fin.close();

    if (!found)
        cout << " No entry found for that website.\n";
}

// Delete credential
void deleteCredential() {
    string searchSite;
    cout << "\n Delete Credential\n";
    cout << "Enter website to delete: ";
    cin >> searchSite;

    ifstream fin("vault.txt");
    ofstream fout("temp.txt");
    bool found = false;
    string line;

    while (getline(fin, line)) {
        string decrypted = xorEncryptDecrypt(line);
        size_t pos1 = decrypted.find(",");
        string website = decrypted.substr(0, pos1);

        if (website != searchSite) {
            fout << line << endl;
        } else {
            found = true;
        }
    }

    fin.close();
    fout.close();

    remove("vault.txt");
    rename("temp.txt", "vault.txt");

    if (found)
        cout << " Credential deleted!\n";
    else
        cout << " Website not found!\n";
}

// Edit credential by website name
void editCredential() {
    string searchSite;
    cout << "\n🔧 Edit Credential\n";
    cout << "Enter website to edit: ";
    cin >> searchSite;

    ifstream fin("vault.txt");
    ofstream fout("temp.txt");
    bool found = false;
    string line;

    while (getline(fin, line)) {
        string decrypted = xorEncryptDecrypt(line);
        size_t pos1 = decrypted.find(",");
        size_t pos2 = decrypted.rfind(",");
        string website = decrypted.substr(0, pos1);

        if (website == searchSite) {
            found = true;
            string newUser, newPass;
            cout << "Enter New Username: ";
            cin >> newUser;
            cout << "Enter New Password: ";
            cin >> newPass;
            string newEntry = website + "," + newUser + "," + newPass;
            fout << xorEncryptDecrypt(newEntry) << endl;
        } else {
            fout << line << endl;
        }
    }

    fin.close();
    fout.close();

    remove("vault.txt");
    rename("temp.txt", "vault.txt");

    if (found)
        cout << " Credential Updated!\n";
    else
        cout << " Website not found!\n";
}

int main() {
    cout << "===============================\n";
    cout << "Password Vault Encryption exe\n";
    cout << "Devoloped By: akif-dev\n";
    cout << "===============================\n";

    if (!checkOrSetMasterPassword()) {
        cout << " Incorrect master password. Exiting...\n";
        return 0;
    }

    int choice;
    do {
        cout << "\n========== Menu ==========\n";
        cout << "1.  Add Credential\n";
        cout << "2.  View Credentials\n";
        cout << "3.  Edit Credential\n";
        cout << "4.  Search Credential\n";
        cout << "5.  Delete Credential\n";
        cout << "6.  Change Master Password\n";
        cout << "7.  Exit\n";
        cout << "==========================\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addCredential(); break;
            case 2: viewCredentials(); break;
            case 3: editCredential(); break;
            case 4: searchCredential(); break;
            case 5: deleteCredential(); break;
            case 6: changeMasterPassword(); break;
            case 7: cout << " Goodbye!\n"; break;
            default: cout << " Invalid choice. Try again.\n";
        }

    } while (choice != 7);

    return 0;
}