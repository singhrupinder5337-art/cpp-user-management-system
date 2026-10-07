#include<iostream>
#include<fstream>
#include<limits>
using namespace std;

class Node
{
public:
    string name;
    string email;
    string password;

    Node* next;

    Node(string n, string e, string p)
    {
        name = n;
        email = e;
        password = p;
        next = nullptr;
    }
};


class temp
{
    string name, email, password;
    fstream file;

    Node* head;

public:
    temp()
    {
        head = nullptr;
    }
void addUser(string name, string email, string password);
    void login();
    void signup();
    void forgot();
};


void temp::addUser(string name, string email, string password)
{
    Node* newNode = new Node(name, email, password);

    if(head == nullptr)
    {
        head = newNode;
        return;
    }

    Node* temp = head;

    while(temp->next != nullptr)
    {
        temp = temp->next;
    }

    temp->next = newNode;
}

int main()
{
    temp obj;
    char choice;

    cout << "\n1- Login";
    cout << "\n2- Sign Up";
    cout << "\n3- Forgot Password";
    cout << "\n4- Exit";

    cout << "\nEnter your choice: ";
    cin >> choice;

    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    switch(choice)
    {
        case '1':
            obj.login();
            break;

        case '2':
            obj.signup();
            break;

        case '3':
            obj.forgot();
            break;

        case '4':
            cout << "\nThank you for using our service!";
            break;

        default:
            cout << "\nInvalid choice";
    }

    return 0;
}


void temp::signup()
{
    cout << "\n-------------SIGN UP-------------";

    cout << "\nEnter your name: ";
    getline(cin, name);

    cout << "Enter your email: ";
    getline(cin, email);

    cout << "Enter your password: ";
    getline(cin, password);

    addUser(name, email, password);

    file.open("login.txt", ios::out | ios::app);

    file << name << "*" << email << "*" << password << endl;

    file.close();

    cout << "\nAccount created successfully!";
}


void temp::login()
{
    string searchname, searchpass;

    cout << "\n-------------LOGIN-------------";

    cout << "\nEnter your name: ";
    getline(cin, searchname);

    cout << "Enter your password: ";
    getline(cin, searchpass);

    file.open("login.txt", ios::in);

    while(getline(file, name, '*') &&
          getline(file, email, '*') &&
          getline(file, password))
    {
        if(searchname == name && searchpass == password)
        {
            cout << "\nLogin successful!";

            file.close();
            return;
        }
    }

    cout << "\nInvalid name or password!";

    file.close();
}


void temp::forgot()
{
    string searchname, searchemail;

    cout << "\n-------------FORGOT PASSWORD-------------";

    cout << "\nEnter your name: ";
    getline(cin, searchname);

    cout << "Enter your email: ";
    getline(cin, searchemail);

    file.open("login.txt", ios::in);

    while(getline(file, name, '*') &&
          getline(file, email, '*') &&
          getline(file, password))
    {
        if(searchname == name && searchemail == email)
        {
            cout << "\nYour password is: " << password;

            file.close();
            return;
        }
    }

    cout << "\nInvalid username or email!";

    file.close();
}