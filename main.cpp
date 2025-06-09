#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const string MASTER_PASSWORD = "admin123";
const char XOR_KEY = 'K';

string xorEncryptDecrypt(const string& data) {
    string result = data;
    for (char& c : result) {
        c ^= XOR_KEY;
    }
    return result;
}

bool login() {
    string input;
    cout << "Enter Master Password: ";
    cin >> input;
    return input == MASTER_PASSWORD;
}

void addCredential() {
    string website, username, password;
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

void viewCredentials() {
    ifstream fin("vault.txt");
    string line;

    cout << "\nSaved Credentials:\n";
    while (getline(fin, line)) {
        string decrypted = xorEncryptDecrypt(line);
        size_t pos1 = decrypted.find(",");
        size_t pos2 = decrypted.rfind(",");

        string website = decrypted.substr(0, pos1);
        string username = decrypted.substr(pos1 + 1, pos2 - pos1 - 1);
        string password = decrypted.substr(pos2 + 1);

        cout << "Website: " << website << endl;
        cout << "Username: " << username << endl;
        cout << "Password: " << password << endl;
        cout << "--------------------------\n";
    }

    fin.close();
}

int main() {
    cout << "==== Password Vault ====\n";

    if (!login()) {
        cout << "Wrong password. Try again later.\n";
        return 0;
    }

    int choice;
    do {
        cout << "\n1. Add Credential\n2. View Credentials\n3. Exit\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addCredential(); break;
            case 2: viewCredentials(); break;
            case 3: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice.\n";
        }

    } while (choice != 3);

    return 0;
}
