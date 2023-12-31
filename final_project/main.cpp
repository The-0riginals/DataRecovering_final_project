#include <conio.h>
#include "FileUtil.h"
#include <thread>

#pragma warning(disable : 4996)

#define streq(a, b) (strcmp((a), (b)) == 0)

int main(int argc, char* argv)
{
    File* vol = nullptr;
    string currPath = "";
    bool isShowAddr = true;

    cout << "Welcome to My File !\nMade by 19120659-20120382." << endl;

    while (true)
    {
        char* password = NULL;
        char line[BUFSIZ], cmd[BUFSIZ], arg1[BUFSIZ], arg2[BUFSIZ], arg3[BUFSIZ];

        if (isShowAddr) cout << "\nMyFile@F:~$ ";

        fflush(stderr);
        if (fgets(line, BUFSIZ, stdin) == NULL) break;
        int args = sscanf(line, "%s %s %s %s", cmd, arg1, arg2, arg3);

        if (args <= 0) {
            isShowAddr = false;
            continue;
        }
        else {
            isShowAddr = true;
        }

        if (streq(cmd, "create"))
        {
            vol = FileUtil::createFile(args, arg1, arg2);
        }
        else if (streq(cmd, "open")) {
            int attempts = 1;
            int maxAttempts = 5;
            int waitTime = 1;
            vol = FileUtil::openFile(args, arg1, arg2);
            if (vol == nullptr)
            {

                do {
                    waitTime *= 2;
                    std::cout << "Waiting for " << waitTime << "  seconds...\n";
                    std::this_thread::sleep_for(std::chrono::seconds(waitTime));

                    // Prompt for password
                    std::cout << "  + Please ReEnter password: ";
                    char enteredPassword[BUFSIZ];
                    fgets(enteredPassword, BUFSIZ, stdin);
                    enteredPassword[strlen(enteredPassword) - 1] = '\0';

                    vol = FileUtil::openFile(args, arg1, enteredPassword);

                    if (vol == nullptr) {
                        attempts++;
                        std::cout << "Attempt " << attempts << " of " << maxAttempts << ":" << std::endl;

                        if (attempts == maxAttempts) {
                            std::cout << "Authentication failed after " << maxAttempts << " attempts.\n";
                            // Add any additional logic or exit the program if needed
                            break;
                        }

                    }
                    else {
                        std::cout << "\nit did return!." << std::endl;
                        break;
                    }
                } while (attempts < maxAttempts && vol == nullptr);
            }
        }
        else if (streq(cmd, "setpassword"))
        {
            if (vol != nullptr) {
                FileUtil::setPassword(vol, args, arg1, arg2);
            }
            else cout << "\nYou do not work with any file." << endl;
        }
        else if (streq(cmd, "changepassword"))
        {
            if (vol != nullptr) {
                FileUtil::changePassword(vol, args, arg1, arg2, arg3);
            }
            else cout << "\nYou do not work with any file." << endl;
        }
        else if (streq(cmd, "setkey"))
        {
            if (vol != nullptr) FileUtil::setKey(vol, args, arg1);
			else cout << "\nYou do not work with any file." << endl;
        }
        else if (streq(cmd, "import"))
        {
            if (vol != nullptr) {
                //FileUtil::importText(vol, args, arg1);
                FileUtil::importData(vol, args, arg1);
            }
            else cout << "\nYou do not work with any file." << endl;
        }
        else if (streq(cmd, "readdata"))
        {
            if (vol != nullptr) {
                //FileUtil::importText(vol, args, arg1);
                FileUtil::readData(vol, args, arg1);
            }
            else cout << "\nYou do not work with any file." << endl;
        }
        else if (streq(cmd, "rm"))
        {
            if (vol != nullptr) {
                FileUtil::remove(vol, args, arg1, arg2);
            }
            else cout << "\nYou do not work with any file." << endl;
        }
        else if (streq(cmd, "editdata"))
        {
            if (vol != nullptr) {
                
                FileUtil::editData(vol, args, arg1);
            }
            else cout << "\nYou do not work with any file." << endl;
        }
        else if (streq(cmd, "help"))
        {
            FileUtil::help();
        }
        else if (streq(cmd, "exit") || streq(cmd, "quit")) {
            break;
        }
        else {
            printf("\nUnknown command: %s", line);
            printf("Type 'help' for a list of commands.\n");
        }
    }

    system("pause");
    return 0;
}