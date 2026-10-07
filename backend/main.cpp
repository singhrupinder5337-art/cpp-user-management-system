#include "crow.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <ctime>
#include <cstring>

struct LoginRecord
{
    std::string username;
    std::string loginTime;
};

std::stack<LoginRecord> loginHistory;
// ================================
// USER LINKED LIST
// ================================

struct UserNode
{
    std::string name;
    std::string email;
    std::string username;
    std::string password;

    UserNode* next;

    UserNode(
        std::string n,
        std::string e,
        std::string u,
        std::string p
    )
    {
        name = n;
        email = e;
        username = u;
        password = p;

        next = nullptr;
    }
};


// Head of linked list
UserNode* userHead = nullptr;
void addUserToList(
    const std::string& name,
    const std::string& email,
    const std::string& username,
    const std::string& password
)
{
    UserNode* newUser =
        new UserNode(
            name,
            email,
            username,
            password
        );

    // First user
    if (userHead == nullptr)
    {
        userHead = newUser;
        return;
    }

    // Move to last node
    UserNode* current = userHead;

    while (current->next != nullptr)
    {
        current = current->next;
    }

    // Connect new user
    current->next = newUser;
}
UserNode* findUser(
    const std::string& username
)
{
    UserNode* current = userHead;

    while (current != nullptr)
    {
        if (current->username == username)
        {
            return current;
        }

        current = current->next;
    }

    return nullptr;
}

int main()
{
    crow::SimpleApp app;
    // ==========================================
// LOAD USERS FROM FILE INTO LINKED LIST
// ==========================================

std::ifstream userFile("users.txt");

if (userFile)
{
    std::string line;

    while (std::getline(userFile, line))
    {
        std::stringstream ss(line);

        std::string name;
        std::string email;
        std::string username;
        std::string password;

        std::getline(ss, name, '|');
        std::getline(ss, email, '|');
        std::getline(ss, username, '|');
        std::getline(ss, password);

        if (!username.empty())
        {
            addUserToList(
                name,
                email,
                username,
                password
            );
        }
    }

    userFile.close();

    std::cout
        << "Users loaded into linked list successfully!"
        << std::endl;
}
else
{
    std::cout
        << "users.txt not found. Starting with empty user list."
        << std::endl;
}

    // ================================
    // LOAD LOGIN HISTORY FROM FILE
    // ================================

    std::ifstream historyInput("login_history.txt");

    if (historyInput)
    {
        std::string historyLine;

        while (std::getline(historyInput, historyLine))
        {
            std::stringstream ss(historyLine);

            std::string username;
            std::string loginTime;

            std::getline(ss, username, '|');
            std::getline(ss, loginTime);

            if (!username.empty() && !loginTime.empty())
            {
                LoginRecord record;

                record.username = username;
                record.loginTime = loginTime;

                loginHistory.push(record);
            }
        }

        historyInput.close();

        std::cout << "Login history loaded successfully!" << std::endl;
    }

    // ================================
// SERVE FRONTEND
// ================================

// Serve index.html
CROW_ROUTE(app, "/")
([]()
{
    crow::response res;

    std::ifstream file("../../frontend/index.html");

    if (!file)
    {
        res.code = 404;
        res.body = "index.html not found";
        return res;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    res.set_header("Content-Type", "text/html");
    res.body = buffer.str();

    return res;
});


// Serve CSS
CROW_ROUTE(app, "/style.css")
([]()
{
    crow::response res;

    std::ifstream file("../../frontend/style.css");

    if (!file)
    {
        res.code = 404;
        res.body = "style.css not found";
        return res;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    res.set_header("Content-Type", "text/css");
    res.body = buffer.str();

    return res;
});


// Serve JavaScript
CROW_ROUTE(app, "/script.js")
([]()
{
    crow::response res;

    std::ifstream file("../../frontend/script.js");

    if (!file)
    {
        res.code = 404;
        res.body = "script.js not found";
        return res;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    res.set_header(
        "Content-Type",
        "application/javascript"
    );

    res.body = buffer.str();

    return res;
});
// ================================
// SIGNUP API
// ================================

CROW_ROUTE(app, "/api/signup")
.methods(crow::HTTPMethod::POST)
([](const crow::request& req)
{
    auto body = crow::json::load(req.body);

    // Check valid JSON
    if (!body)
    {
        crow::json::wvalue response;
        response["message"] = "Invalid JSON data.";
        return crow::response(400, response);
    }

    // Check required fields
    if (!body.has("name") ||
        !body.has("email") ||
        !body.has("username") ||
        !body.has("password"))
    {
        crow::json::wvalue response;
        response["message"] = "All fields are required.";
        return crow::response(400, response);
    }

    std::string name =
        std::string(body["name"].s());

    std::string email =
        std::string(body["email"].s());

    std::string username =
        std::string(body["username"].s());

    std::string password =
        std::string(body["password"].s());


    // Check empty fields
    if (name.empty() ||
        email.empty() ||
        username.empty() ||
        password.empty())
    {
        crow::json::wvalue response;
        response["message"] = "All fields are required.";
        return crow::response(400, response);
    }


    // ================================
// CHECK USERNAME IN LINKED LIST
// ================================

UserNode* existingUser = findUser(username);

if (existingUser != nullptr)
{
    crow::json::wvalue response;

    response["message"] =
        "Username already exists.";

    return crow::response(
        409,
        response
    );
}

    // ================================
    // SAVE NEW USER
    // ================================

    std::ofstream file(
        "users.txt",
        std::ios::app
    );

    if (!file)
    {
        crow::json::wvalue response;
        response["message"] =
            "Could not save user.";

        return crow::response(
            500,
            response
        );
    }

file
    << name << "|"
    << email << "|"
    << username << "|"
    << password
    << '\n';

file.close();


// ================================
// ADD USER TO LINKED LIST
// ================================

addUserToList(
    name,
    email,
    username,
    password
);


// ================================
// SIGNUP SUCCESS
// ================================

crow::json::wvalue response;

response["message"] =
    "Account created successfully.";

response["username"] =
    username;

return crow::response(
    201,
    response
);

});

// ================================
// FORGOT PASSWORD API
// ================================// ================================
// FORGOT PASSWORD API
// ================================

CROW_ROUTE(app, "/api/forgot")
.methods(crow::HTTPMethod::POST)
([](const crow::request& req)
{
    auto body = crow::json::load(req.body);

    crow::json::wvalue response;

    // Check JSON
    if (!body ||
        !body.has("username") ||
        !body.has("email"))
    {
        response["message"] =
            "Username and email are required.";

        return crow::response(400, response);
    }

    std::string username =
        std::string(body["username"].s());

    std::string email =
        std::string(body["email"].s());

    if (username.empty() || email.empty())
    {
        response["message"] =
            "Username and email cannot be empty.";

        return crow::response(400, response);
    }


    // Open users.txt
    std::ifstream file("users.txt");

    if (!file)
    {
        response["message"] =
            "User database not found.";

        return crow::response(500, response);
    }


    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        std::string storedName;
        std::string storedEmail;
        std::string storedUsername;
        std::string storedPassword;

        std::getline(ss, storedName, '|');
        std::getline(ss, storedEmail, '|');
        std::getline(ss, storedUsername, '|');
        std::getline(ss, storedPassword);


        // Username AND email must match
        if (storedUsername == username &&
            storedEmail == email)
        {
            file.close();

            response["message"] =
                "Account verified successfully.";

            response["username"] =
                storedUsername;

            response["name"] =
                storedName;

            return crow::response(
                200,
                response
            );
        }
    }


    file.close();

    response["message"] =
        "Username and email do not match.";

    return crow::response(
        404,
        response
    );
});
// ================================
// RESET PASSWORD API
// ================================

CROW_ROUTE(app, "/api/reset-password/<string>")
.methods(crow::HTTPMethod::PUT)
([](const crow::request& req, std::string username)
{
    auto body = crow::json::load(req.body);

    crow::json::wvalue response;

    if (!body ||
        !body.has("email") ||
        !body.has("newPassword"))
    {
        response["message"] =
            "Email and new password are required.";

        return crow::response(400, response);
    }

    std::string email =
        std::string(body["email"].s());

    std::string newPassword =
        std::string(body["newPassword"].s());

    if (email.empty() || newPassword.empty())
    {
        response["message"] =
            "Email and new password cannot be empty.";

        return crow::response(400, response);
    }

    if (newPassword.length() < 8)
    {
        response["message"] =
            "Password must be at least 8 characters.";

        return crow::response(400, response);
    }

    std::ifstream file("users.txt");

    if (!file)
    {
        response["message"] =
            "User database not found.";

        return crow::response(500, response);
    }

    std::vector<std::string> records;

    std::string line;

    bool found = false;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        std::string name;
        std::string storedEmail;
        std::string storedUsername;
        std::string password;

        std::getline(ss, name, '|');
        std::getline(ss, storedEmail, '|');
        std::getline(ss, storedUsername, '|');
        std::getline(ss, password);

        if (storedUsername == username &&
            storedEmail == email)
        {
            line =
                name + "|" +
                storedEmail + "|" +
                storedUsername + "|" +
                newPassword;

            found = true;
        }

        records.push_back(line);
    }

    file.close();

    if (!found)
    {
        response["message"] =
            "Account verification failed.";

        return crow::response(404, response);
    }

    std::ofstream output(
        "users.txt",
        std::ios::trunc
    );

    if (!output)
    {
        response["message"] =
            "Could not update user database.";

        return crow::response(500, response);
    }

    for (const auto& record : records)
{
    output << record << '\n';
}

output.close();


// ================================
// UPDATE PASSWORD IN LINKED LIST
// ================================

UserNode* user = findUser(username);

if (user != nullptr)
{
    user->password = newPassword;
}


response["message"] =
    "Password reset successfully.";

return crow::response(200, response);

});
// ================================
// LOGIN API
// ================================

CROW_ROUTE(app, "/api/login")
.methods(crow::HTTPMethod::POST)
([](const crow::request& req)
{
    auto body = crow::json::load(req.body);

    if (!body)
    {
        crow::json::wvalue response;
        response["message"] = "Invalid JSON data.";

        return crow::response(400, response);
    }

    // Check required fields
    if (!body.has("username") ||
        !body.has("password") ||
        body["username"].t() != crow::json::type::String ||
        body["password"].t() != crow::json::type::String)
    {
        crow::json::wvalue response;
        response["message"] = "Username and password are required.";

        return crow::response(400, response);
    }

    std::string username = body["username"].s();
    std::string password = body["password"].s();

    if (username.empty() || password.empty())
    {
        crow::json::wvalue response;
        response["message"] = "Username and password are required.";

        return crow::response(400, response);
    }

    // ================================
// SEARCH USER IN LINKED LIST
// ================================

UserNode* user = findUser(username);

// Check whether username exists
if (user != nullptr && user->password == password)
{
    // ================================
    // CREATE LOGIN HISTORY RECORD
    // ================================

    std::time_t now = std::time(nullptr);

    char timeBuffer[100];

    std::tm* localTime = std::localtime(&now);

    if (localTime != nullptr)
    {
        std::strftime(
            timeBuffer,
            sizeof(timeBuffer),
            "%Y-%m-%d %H:%M:%S",
            localTime
        );
    }
    else
    {
        std::strcpy(
            timeBuffer,
            "Unknown time"
        );
    }


    LoginRecord record;

    record.username = user->username;
    record.loginTime = timeBuffer;


    // ================================
    // STORE LOGIN IN STACK
    // ================================

    loginHistory.push(record);


    // ================================
    // SAVE LOGIN PERMANENTLY
    // ================================

    std::ofstream historyFile(
        "login_history.txt",
        std::ios::app
    );

    if (historyFile)
    {
        historyFile
            << record.username
            << "|"
            << record.loginTime
            << '\n';

        historyFile.close();
    }


    // ================================
    // LOGIN SUCCESS
    // ================================

    crow::json::wvalue response;

    response["message"] =
        "Login successful.";

    response["username"] =
        user->username;

    response["name"] =
        user->name;

    response["email"] =
        user->email;

    return crow::response(
        200,
        response
    );
}

    // ================================
    // LOGIN FAILED
    // ================================

    crow::json::wvalue response;

    response["message"] = "Invalid username or password.";

    return crow::response(401, response);
});
    // =========================
// PROFILE
// =========================
// ================================
// GET PROFILE FROM LINKED LIST
// ================================

CROW_ROUTE(app, "/api/profile/<string>")
([](std::string username)
{
    // Search user in linked list
    UserNode* user = findUser(username);

    // User not found
    if (user == nullptr)
    {
        crow::json::wvalue response;

        response["message"] =
            "User not found.";

        return crow::response(
            404,
            response
        );
    }

    // User found
    crow::json::wvalue response;

    response["name"] =
        user->name;

    response["email"] =
        user->email;

    response["username"] =
        user->username;

    return crow::response(
        200,
        response
    );
});
// ================================
// EDIT PROFILE API
// ================================

CROW_ROUTE(app, "/api/profile/update/<string>")
.methods(crow::HTTPMethod::PUT)
([](const crow::request& req, std::string username)
{
    auto body = crow::json::load(req.body);

    crow::json::wvalue response;

    if (!body || !body.has("name") || !body.has("email") ||
        body["name"].t() != crow::json::type::String ||
        body["email"].t() != crow::json::type::String)
    {
        response["message"] = "Valid name and email are required.";
        return crow::response(400, response);
    }

    std::string newName = body["name"].s();
    std::string newEmail = body["email"].s();

    if (newName.empty() || newEmail.empty() ||
        newName.find('|') != std::string::npos ||
        newEmail.find('|') != std::string::npos ||
        newName.find('\n') != std::string::npos ||
        newEmail.find('\n') != std::string::npos)
    {
        response["message"] = "Invalid name or email.";
        return crow::response(400, response);
    }

    std::ifstream file("users.txt");

    if (!file)
    {
        response["message"] = "User database not found.";
        return crow::response(500, response);
    }

    std::vector<std::string> records;
    std::string line;
    bool found = false;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        std::string name, email, storedUsername, password;

        std::getline(ss, name, '|');
        std::getline(ss, email, '|');
        std::getline(ss, storedUsername, '|');
        std::getline(ss, password);

        if (storedUsername == username)
        {
            line = newName + "|" + newEmail + "|" +
                   storedUsername + "|" + password;

            found = true;
        }

        records.push_back(line);
    }

    file.close();

    if (!found)
    {
        response["message"] = "User not found.";
        return crow::response(404, response);
    }

    std::ofstream output("users.txt", std::ios::trunc);

    if (!output)
    {
        response["message"] = "Could not update user database.";
        return crow::response(500, response);
    }
for (const auto& record : records)
{
    output << record << '\n';
}

if (!output)
{
    response["message"] = "Failed to save profile.";
    return crow::response(500, response);
}

output.close();


// ================================
// UPDATE USER IN LINKED LIST
// ================================

UserNode* user = findUser(username);

if (user != nullptr)
{
    user->name = newName;
    user->email = newEmail;
}


// ================================
// PROFILE UPDATE SUCCESS
// ================================

response["message"] =
    "Profile updated successfully.";

return crow::response(
    200,
    response
);
});
// EDIT PROFILE API
// ================================

// ================================
// CHANGE PASSWORD API
// ================================

CROW_ROUTE(app, "/api/password/<string>")
.methods(crow::HTTPMethod::PUT)
([](const crow::request& req, std::string username)
{
    crow::json::wvalue response;

    auto body = crow::json::load(req.body);

    if (!body ||
        !body.has("oldPassword") ||
        !body.has("newPassword") ||
        body["oldPassword"].t() != crow::json::type::String ||
        body["newPassword"].t() != crow::json::type::String)
    {
        response["message"] = "Old password and new password are required.";
        return crow::response(400, response);
    }

    std::string oldPassword = body["oldPassword"].s();
    std::string newPassword = body["newPassword"].s();

    if (newPassword.empty() ||
        newPassword.find('|') != std::string::npos ||
        newPassword.find('\n') != std::string::npos)
    {
        response["message"] = "Invalid new password.";
        return crow::response(400, response);
    }

    std::ifstream file("users.txt");

    if (!file)
    {
        response["message"] = "User database not found.";
        return crow::response(500, response);
    }

    std::vector<std::string> records;

    std::string line;
    bool userFound = false;
    bool passwordCorrect = false;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        std::string name;
        std::string email;
        std::string storedUsername;
        std::string password;

        std::getline(ss, name, '|');
        std::getline(ss, email, '|');
        std::getline(ss, storedUsername, '|');
        std::getline(ss, password);

        if (storedUsername == username)
        {
            userFound = true;

            if (password == oldPassword)
            {
                passwordCorrect = true;

                line = name + "|" +
                       email + "|" +
                       storedUsername + "|" +
                       newPassword;
            }
        }

        records.push_back(line);
    }

    file.close();

    if (!userFound)
    {
        response["message"] = "User not found.";
        return crow::response(404, response);
    }

    if (!passwordCorrect)
    {
        response["message"] = "Old password is incorrect.";
        return crow::response(401, response);
    }

    std::ofstream output("users.txt", std::ios::trunc);

    if (!output)
    {
        response["message"] = "Could not update user database.";
        return crow::response(500, response);
    }
for (const auto& record : records)
{
    output << record << '\n';
}

output.close();


// ================================
// UPDATE PASSWORD IN LINKED LIST
// ================================

UserNode* user = findUser(username);

if (user != nullptr)
{
    user->password = newPassword;
}


response["message"] = "Password changed successfully.";

return crow::response(200, response);

});

// ================================
// GET LOGIN HISTORY API
// ================================

CROW_ROUTE(app, "/api/login-history/<string>")
.methods(crow::HTTPMethod::GET)
([](std::string username)
{
    crow::json::wvalue response;

    // Make a copy because popping the original stack
    // would permanently remove login records.
    std::stack<LoginRecord> tempStack = loginHistory;

    crow::json::wvalue::list history;

    while (!tempStack.empty())
    {
        LoginRecord record = tempStack.top();
        tempStack.pop();

        // Show records only for requested user
        if (record.username == username)
        {
            crow::json::wvalue item;

            item["username"] = record.username;
            item["loginTime"] = record.loginTime;

            history.push_back(std::move(item));
        }
    }

    response["username"] = username;
    response["history"] = std::move(history);

    return crow::response(200, response);
});
// ================================
// ==========================================
// MULTIPLE USER NOTES - CRUD API
// File format:
// id|title|content
// ==========================================


// ==========================================
// CREATE NEW NOTE
// POST /api/notes/<username>
// ==========================================

CROW_ROUTE(app, "/api/notes/<string>")
.methods(crow::HTTPMethod::POST)
([](const crow::request& req, std::string username)
{
    crow::json::wvalue response;

    UserNode* user = findUser(username);

    if (user == nullptr)
    {
        response["message"] = "User not found.";
        return crow::response(404, response);
    }

    auto body = crow::json::load(req.body);

    if (!body ||
        !body.has("title") ||
        !body.has("content") ||
        body["title"].t() != crow::json::type::String ||
        body["content"].t() != crow::json::type::String)
    {
        response["message"] =
            "Title and content are required.";

        return crow::response(400, response);
    }

    std::string title = body["title"].s();
    std::string content = body["content"].s();

    if (title.empty())
    {
        response["message"] =
            "Note title cannot be empty.";

        return crow::response(400, response);
    }

    // Prevent our file separator from breaking records
    if (title.find('|') != std::string::npos ||
        content.find('|') != std::string::npos ||
        title.find('\n') != std::string::npos ||
        content.find('\n') != std::string::npos)
    {
        response["message"] =
            "Notes cannot contain | or line breaks yet.";

        return crow::response(400, response);
    }

    std::string filename =
        "notes_" + username + ".txt";


    // Find next note ID
    int nextId = 1;

    std::ifstream input(filename);
    std::string line;

    while (std::getline(input, line))
    {
        std::stringstream ss(line);

        std::string idText;

        std::getline(ss, idText, '|');

        try
        {
            int id = std::stoi(idText);

            if (id >= nextId)
            {
                nextId = id + 1;
            }
        }
        catch (...)
        {
            // Ignore invalid lines
        }
    }

    input.close();


    // Append new note
    std::ofstream output(
        filename,
        std::ios::app
    );

    if (!output)
    {
        response["message"] =
            "Could not save note.";

        return crow::response(500, response);
    }

    output
        << nextId << "|"
        << title << "|"
        << content
        << '\n';

    output.close();

    response["message"] =
        "Note created successfully.";

    response["id"] = nextId;

    return crow::response(
        201,
        response
    );
});


// ==========================================
// GET ALL NOTES
// GET /api/notes/<username>
// ==========================================

CROW_ROUTE(app, "/api/notes/<string>")
.methods(crow::HTTPMethod::GET)
([](std::string username)
{
    crow::json::wvalue response;

    UserNode* user = findUser(username);

    if (user == nullptr)
    {
        response["message"] =
            "User not found.";

        return crow::response(404, response);
    }

    std::string filename =
        "notes_" + username + ".txt";

    std::ifstream input(filename);

    crow::json::wvalue::list notes;


    // No file means user has no notes yet
    if (!input)
    {
        response["notes"] =
            std::move(notes);

        return crow::response(
            200,
            response
        );
    }


    std::string line;

    while (std::getline(input, line))
    {
        std::stringstream ss(line);

        std::string idText;
        std::string title;
        std::string content;

        std::getline(ss, idText, '|');
        std::getline(ss, title, '|');
        std::getline(ss, content);

        try
        {
            int id = std::stoi(idText);

            crow::json::wvalue note;

            note["id"] = id;
            note["title"] = title;
            note["content"] = content;

            notes.push_back(
                std::move(note)
            );
        }
        catch (...)
        {
            // Ignore invalid lines
        }
    }

    input.close();

    response["notes"] =
        std::move(notes);

    return crow::response(
        200,
        response
    );
});


// ==========================================
// EDIT ONE NOTE
// PUT /api/notes/<username>/<id>
// ==========================================

CROW_ROUTE(
    app,
    "/api/notes/<string>/<int>"
)
.methods(crow::HTTPMethod::PUT)
([](
    const crow::request& req,
    std::string username,
    int noteId
)
{
    crow::json::wvalue response;

    UserNode* user = findUser(username);

    if (user == nullptr)
    {
        response["message"] =
            "User not found.";

        return crow::response(404, response);
    }


    auto body =
        crow::json::load(req.body);

    if (!body ||
        !body.has("title") ||
        !body.has("content") ||
        body["title"].t() != crow::json::type::String ||
        body["content"].t() != crow::json::type::String)
    {
        response["message"] =
            "Title and content are required.";

        return crow::response(400, response);
    }


    std::string newTitle =
        body["title"].s();

    std::string newContent =
        body["content"].s();


    if (newTitle.empty() ||
        newTitle.find('|') != std::string::npos ||
        newContent.find('|') != std::string::npos ||
        newTitle.find('\n') != std::string::npos ||
        newContent.find('\n') != std::string::npos)
    {
        response["message"] =
            "Invalid note title or content.";

        return crow::response(400, response);
    }


    std::string filename =
        "notes_" + username + ".txt";

    std::ifstream input(filename);

    if (!input)
    {
        response["message"] =
            "No notes found.";

        return crow::response(404, response);
    }


    std::vector<std::string> records;

    std::string line;

    bool found = false;


    while (std::getline(input, line))
    {
        std::stringstream ss(line);

        std::string idText;

        std::getline(ss, idText, '|');

        try
        {
            int id =
                std::stoi(idText);

            if (id == noteId)
            {
                line =
                    std::to_string(noteId)
                    + "|" +
                    newTitle
                    + "|" +
                    newContent;

                found = true;
            }
        }
        catch (...)
        {
        }

        records.push_back(line);
    }

    input.close();


    if (!found)
    {
        response["message"] =
            "Note not found.";

        return crow::response(404, response);
    }


    std::ofstream output(
        filename,
        std::ios::trunc
    );

    if (!output)
    {
        response["message"] =
            "Could not update note.";

        return crow::response(500, response);
    }


    for (const auto& record : records)
    {
        output << record << '\n';
    }

    output.close();


    response["message"] =
        "Note updated successfully.";

    return crow::response(
        200,
        response
    );
});


// ==========================================
// DELETE ONE NOTE
// DELETE /api/notes/<username>/<id>
// ==========================================

CROW_ROUTE(
    app,
    "/api/notes/<string>/<int>"
)
.methods(crow::HTTPMethod::Delete)
([](
    std::string username,
    int noteId
)
{
    crow::json::wvalue response;

    UserNode* user = findUser(username);

    if (user == nullptr)
    {
        response["message"] =
            "User not found.";

        return crow::response(404, response);
    }


    std::string filename =
        "notes_" + username + ".txt";

    std::ifstream input(filename);

    if (!input)
    {
        response["message"] =
            "No notes found.";

        return crow::response(404, response);
    }


    std::vector<std::string> records;

    std::string line;

    bool found = false;


    while (std::getline(input, line))
    {
        std::stringstream ss(line);

        std::string idText;

        std::getline(ss, idText, '|');

        try
        {
            int id =
                std::stoi(idText);

            if (id == noteId)
            {
                found = true;
                continue;
            }
        }
        catch (...)
        {
        }

        records.push_back(line);
    }

    input.close();


    if (!found)
    {
        response["message"] =
            "Note not found.";

        return crow::response(404, response);
    }


    std::ofstream output(
        filename,
        std::ios::trunc
    );

    if (!output)
    {
        response["message"] =
            "Could not delete note.";

        return crow::response(500, response);
    }


    for (const auto& record : records)
    {
        output << record << '\n';
    }

    output.close();


    response["message"] =
        "Note deleted successfully.";

    return crow::response(
        200,
        response
    );
});
    // =========================
    // START SERVER
    // =========================
    app.port(18080)
       .multithreaded()
       .run();

    return 0;
}
