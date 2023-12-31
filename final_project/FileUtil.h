#pragma once
#include "File.h"
#include <time.h>

class FileUtil
{
public:
    static File* createFile(int args, char* arg1, char* arg2)
    {
        if (args == 2) {
            if (ListUtil::isExist(arg1)) {
                cout << "\nFile has already existed!" << endl;
                return nullptr;
            }

            File* vol = new File(512, string(arg1));
            vol->writeFile();
            cout << "\nCreate new File successfully!" << endl;
            vol->printInfo();

            return vol;
        }
        else {
            cout << "\nUsage: create <file-name>\n";
            return nullptr;
        }
    }

    static File* openFile(int args, char* arg1, char* arg2)
    {
        if (args > 1) {
            if (!ListUtil::isExist(arg1)) {
                cout << "\nFile has not existed!" << endl;
                return nullptr;
            }

            File* vol = new File(string(arg1));
            vol->readFile();
            string password = vol->fileInfo()->password();

            if (args == 2) {
                if (password == "") {
                    cout << "\nOpen file successfully!" << endl;
                    return vol;
                }
                else {
                    cout << "\nThis file has a password. Usage: open <file-name> <pass>" << endl;
                    return nullptr;
                }
            }
            else if (args == 3) {
                time_t mytime = time(NULL);
                char strtime[26];
                ctime_s(strtime, sizeof strtime, &mytime);
                
                string temp = string(arg2);
                
                string getPassWord = temp.substr(2, temp.length() - 4);// get password from input
                //cout<<getPassWord<<endl;


                // check condition of time(dymamic
                //temp[0] = strtime[11];
                //temp[1] = strtime[12];
                //temp[temp.length() - 2] = strtime[14];
                //temp[temp.length() - 1] = strtime[15];
                if (password == md5(getPassWord)&& 
                    temp[0]== strtime[11] && 
                    temp[1]== strtime[12] && 
                    temp[temp.length() - 2]== strtime[14] && 
                    temp[temp.length() - 1]== strtime[15])
                {
                    cout << "\nOpen file successfully!" << endl;
                    return vol;
                }
                else {
                    cout << "\nWrong password!" << endl;
                    return nullptr;
                }
            }
        }
        else {
            cout << "\nUsage: open <file-name> <pass>\n";
            cout << "OR: open <file-name> (if it don't have password)\n";
            return nullptr;
        }
    }

    static void setPassword(File* vol, int args, char* arg1, char* arg2)
    {
        if (args == 2) {
            vol->setFilePassword(arg1);
            vol->printInfo();
        }
        //else if (args == 3) {
         //  vol->setFilePassword(arg1, arg2);
        //}
        else {
            //cout << "\nUsage: setpassword <file-name> <pass>\n";
            cout << "\nUsage: setpassword <pass> (set password for volume)\n";
        }
    }

    static void changePassword(File* vol, int args, char* arg1, char* arg2, char* arg3)
    {
        if (args == 3) {
            vol->changeFilePassword(arg1, arg2);
        }
        /*else if (args == 4) {
            vol->changeFilePassword(arg1, arg2, arg3);
        }*/
        else {
            //cout << "\nUsage: changepassword <file-name> <old-pass> <new-pass>\n";
            cout << "\nUsage: changepassword <old-pass> <new-pass> (change password for volume)\n";
        }
    }

    static void setKey(File* vol, int args, char* arg1)
	{
		if (args == 2) {
			vol->setKey(string(arg1));
            //cout<<"\nSet key successfully! 01 "<<endl;
			vol->printInfo();
		}
		else {
			cout << "\nUsage: setkey <key> (set key for ecrypt data)\n";
		}
	}

    static void importText(File* vol, int args, char* arg1)
	{
		if (args == 2)
		{
			vol->importText(arg1);
		}
		else
		{
			cout << "\nUsage:import <text>\n";
			//cout << "OR:import <file-path> <inside-path>\n";
		}
	}

    static void importData(File* vol, int args, char* arg1)
    {
        //cout<<"args: "<<args<<endl;
        //cout<<"arg1: "<<arg1<<endl;
        if (args == 2) {
            if(string(arg1) == "1")
            { 			
                vol->importStudent();
			}
			else if(string(arg1) == "2")
			{
				vol->importTeacher();
                //cout<<"import teacher list 2"<<endl;
			}
			else
			{
				cout << "\nUsage: import <type>\n";
                cout << "type = 1: import student in list 1\n";
                cout << "type = 2: import teacher in list 2\n";
			}
			
		}
		else {
            cout << "\nUsage: import <type>\n";
            cout << "type = 1: import student in list 1\n";
            cout << "type = 2: import teacher in list 2\n";
		}
    }

    static void readData(File* vol, int args, char* arg1)
	{
		if (args == 2) {
			if(string(arg1) == "1")
			{ 			
				vol->printAllStudentData();
            }
            else if(string(arg1) == "2")
			{
                vol->printAllTeacherData();
			}
            else
			{
                cout << "\nUsage: readdata <type>\n";
				cout << "type = 1: read student in list 1\n";
				cout << "type = 2: read teacher in list 2\n";
            }
        }
        else 
        {
			cout << "\nUsage: readdata <type>\n";
            cout << "type = 1: read student in list 1\n";
            cout << "type = 2: read teacher in list 2\n";
		}
    }

    static void remove(File* vol, int args, char* arg1, char* arg2)
    {
        if (args > 1)
        {
            if (string(arg1) == "1")
            {
                // get the student name from input
                cout <<"Please enter the student name you want to remove: ";
                string studentName;
                getline(cin, studentName);
				//cout<<studentName<<endl;
                vol->removeStudent(studentName, args == 3 && (string(arg2) == "true" || string(arg2) == "1"));
            }
            else if (string(arg1) == "2")
            {
                // get the teacher name from input
                cout <<"Please enter the teacher name you want to remove: ";
				string teacherName;
				getline(cin, teacherName);
                //cout<<teacherName<<endl;
				vol->removeTeacher(teacherName, args == 3 && (string(arg2) == "true" || string(arg2) == "1"));

            }
            else
            {
                cout << "\nUsage:rm <type-list> <permanently-delete: true/false>\n";
                cout << "type = 1: remove student in list 1\n";
                cout << "type = 2: remove teacher in list 2\n";
            }
           
        }
        else
        {
            cout << "\nUsage:rm <type-list> <permanently-delete: true/false>\n";
        }
    }

    //edit data
    static void editData(File* vol, int args, char* arg1)
	{
		if (args == 2) {
			if(string(arg1) == "1")
			{ 			
				//get the student ID from input
				cout <<"Please enter the student ID you want to edit: ";
				string studentID;
				getline(cin, studentID);
                //cout<<studentID<<endl;
                vol->editStudent(studentID);
            }
			else if(string(arg1) == "2")
            {
                 //get the teacher ID from input
                cout <<"Please enter the teacher ID you want to edit: ";
				string teacherID;
                getline(cin, teacherID);
				//cout<<teacherID<<endl;
				vol->editTeacher(teacherID);
            }
            else
            {
                cout << "\nUsage: editdata <type>\n";
                cout << "type = 1: edit student in list 1\n";
                cout << "type = 2: edit teacher in list 2\n";
            }
        }
        else {
            cout << "\nUsage: editdata <type>\n";
            cout << "type = 1: edit student in list 1\n";
            cout << "type = 2: edit teacher in list 2\n";
        }
    }

    static void help()
    {
        cout << "\nCommands are:\n";
        cout << "  create <file-name>\n";
        cout << "    + Purpose: Create a new File.\n";
        cout << "    + <file-name>: File's name (maximum: 256 ascii character).\n";
        cout << "    + Ex: create myFile \n";

        cout << " setpassword <pass> \n";
        cout << "    + Purpose: Set password for the file.\n";
        cout << "    + <pass>: input password\n";

        cout << "  open <file-name> <pass>\n";
        cout << "    + Purpose: Open a file already exists.\n";
        cout << "    + <file-name>: file's name (maximum: 256 ascii character).\n";
        cout << "    + <pass>: Dynamic Password (detail in report).\n";

        cout << "  setkey <key>\n";
        cout << "    + Purpose: Set key for encrypt data.\n";
        cout << "    + <key>: Key.\n";
        cout << "    + Ex: setkey 666666\n";

        cout << " import <type>\n";
        cout << "   + Purpose: Import data for file.\n";
        cout << "   + type = 1: import student in list 1\n";
        cout << "   + type = 2: import teacher in list 2\n";

        cout << " readdata <type>\n";
        cout << "   + type = 1: read student in list 1\n";
        cout << "   + type = 2: read teacher in list 2\n";

        cout << " rm <type-list> <permanently-delete: true/false>\n";
        cout << "   + type = 1: remove student in list 1 by id\n";
        cout << "   + type = 2: remove teacher in list 2 by id\n";
        cout << "   + permanently-delete: true/false\n";
        cout << "   + Ex: rm 1 true\n";

        cout << " editdata <type>\n";
        cout << "   + type = 1: edit student in list 1\n";
        cout << "   + type = 2: edit teacher in list 2\n";

        cout << "  help\n";
        cout << "    + Purpose: View all commands\n";

        cout << "  exit\n";
        cout << "    + Purpose: Exit the program\n";

        cout << "  quit\n";
        cout << "    + Purpose: Exit the program\n";
    }
};