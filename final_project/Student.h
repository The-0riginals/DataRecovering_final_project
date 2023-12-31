#pragma once
#include <iostream>
#include <string>
#include "AES.h"
#include "FileInfo.h"
#include <sstream>
#include <vector>

#define AES_BLOCK_SIZE 16

class Student {
private:
	int id;
	std::string name;
	std::string dateOfBirth;
	std::string dateJoinSchool;
	std::string phone;
	std::string personalID;

public:
	// Constructor
	Student() :id(0), name(""), dateOfBirth(""), dateJoinSchool(""), phone(""), personalID("") {}


	Student(int studentId, const std::string& studentName, const std::string& dob,
		const std::string& joinDate, const std::string& phoneNumber, const std::string& personalId)
		: id(studentId), name(studentName), dateOfBirth(dob),
		dateJoinSchool(joinDate), phone(phoneNumber), personalID(personalId) {}
	//getters and setters for phone and personalID
	void setPhone(const std::string& phoneNumber) { phone = phoneNumber; }
	void setPersonalID(const std::string& personalId) { personalID = personalId; }
	std::string getPhone() const { return phone; }
	std::string getPersonalID() const { return personalID; }

	std::string getName() const {
		return name;
	}
	//get student id
	int getId() const {
		return id;
	}

	// Function to add a student from keyboard
	void addStudent(const std::string& keyEncrypt)
	{
		std::cout << "+ Enter student ID: ";
		std::cin >> id;
		std::cout << "+ Enter student name: ";
		std::cin.ignore();
		std::getline(std::cin, name);
		std::cout << "+ Enter student date of birth: ";
		std::getline(std::cin, dateOfBirth);
		std::cout << "+ Enter student date join school: ";
		std::getline(std::cin, dateJoinSchool);
		std::cout << "+ Enter student phone number: ";
		std::getline(std::cin, phone);
		std::cout << "+ Enter student personal ID: ";
		std::getline(std::cin, personalID);

		// Encrypt Phone and PersonalID
		phone = encryptSensitiveInfo(phone, keyEncrypt);
		personalID = encryptSensitiveInfo(personalID, keyEncrypt);

		//setters for phone and personalID
		setPhone(phone);
		setPersonalID(personalID);

		// display student information
		displayStudent();

	};



	// Function to display a student
	void displayStudent()
	{
		std::cout << "\n- Student information: " << std::endl;
		std::cout << "  + Student ID: " << id << std::endl;
		std::cout << "  + Student name: " << name << std::endl;
		std::cout << "  + Student date of birth: " << dateOfBirth << std::endl;
		std::cout << "  + Student date join school: " << dateJoinSchool << std::endl;
		std::cout << "  + Student phone number: " << phone << std::endl;
		std::cout << "  + Student personal ID: " << personalID << std::endl;
	}
	// Convert student information to a string
	std::string toString()
	{
		std::string result = std::to_string(id) + "," + name + "," + dateOfBirth + "," + dateJoinSchool + "," + phone + "," + personalID;
		return result;
	}

	//// Encrypt function
	string encryptSensitiveInfo(string data,const std::string& keyEncrypt)
	{
		// Concatenate Phone and PersonalID for encryption
		std::string sensitiveInfoToEncrypt = addPKCS7Padding(data);

		// Convert string to unsigned char*
		unsigned char* plain = (unsigned char*)sensitiveInfoToEncrypt.c_str();
		unsigned char* key = (unsigned char*)keyEncrypt.c_str();
		unsigned int plainLen = sensitiveInfoToEncrypt.length();

		// Encrypt using AES
		AES aes(AESKeyLength::AES_128);
		unsigned char* cipher = aes.EncryptECB(plain, plainLen, key);

		// Update encrypted Phone/Personal ID Student object
		return(std::string(reinterpret_cast<char*>(cipher), plainLen));
	}

	std::string addPKCS7Padding(const std::string& data) {
		size_t paddingSize = AES_BLOCK_SIZE - (data.size() % AES_BLOCK_SIZE);
		char paddingChar = static_cast<char>(paddingSize);

		std::string paddedData = data;
		for (size_t i = 0; i < paddingSize; ++i) {
			paddedData.push_back(paddingChar);
		}

		return paddedData;
	}

	//// Function to decrypt sensitive information
	void decryptSensitiveInfo(const std::string& keyDecrypt)
	{
		// Decrypt Phone
	    std::string decryptedPhone = decryptSensitiveInfoPart(phone, keyDecrypt);
		//cout<<decryptedPhone<<endl;

		// Decrypt PersonalID
		std::string decryptedPersonalID = decryptSensitiveInfoPart(personalID, keyDecrypt);
		//cout<<decryptedPersonalID<<endl;

	    //Update decrypted Phone and PersonalID in the Student object
		setPhone(decryptedPhone);
		setPersonalID(decryptedPersonalID);
	}

	// Function to decrypt a part of sensitive information
	std::string decryptSensitiveInfoPart(const std::string& encryptedData, const std::string& keyDecrypt)
	{
		// Convert string to unsigned char*
		unsigned char* cipher = (unsigned char*)encryptedData.c_str();
		unsigned char* key = (unsigned char*)keyDecrypt.c_str();
		unsigned int cipherLen = encryptedData.length();

		// Decrypt using AES
		AES aes(AESKeyLength::AES_128);
		unsigned char* plain = aes.DecryptECB(cipher, cipherLen, key);

		// Remove PKCS7 padding
		// Remove PKCS7 padding
		size_t paddingSize = static_cast<size_t>(plain[cipherLen - 1]);
		std::string decryptedData(reinterpret_cast<char*>(plain), cipherLen - paddingSize);
		

		return decryptedData;
	}

	static Student fromString(const std::string& decryptedData) 
	{
		Student student;

		std::istringstream ss(decryptedData);
		std::string token;
		std::vector<std::string> tokens;

		while (getline(ss, token, ',')) {
			tokens.push_back(token);
		}

		if (tokens.size() == 6) { // Ensure all fields are present
			student.id = std::stoi(tokens[0]);
			student.name = tokens[1];
			student.dateOfBirth = tokens[2];
			student.dateJoinSchool = tokens[3];
			student.phone = tokens[4];
			student.personalID = tokens[5];
		}

		return student;
	}

	// Function to update student information
	void updateStudent(const std::string& keyEncrypt)
	{
		std::cout << "+ Enter updated student name: ";
		std::cin.ignore();
		std::getline(std::cin, name);
		std::cout << "+ Enter updated student date of birth: ";
		std::getline(std::cin, dateOfBirth);
		std::cout << "+ Enter updated student date join school: ";
		std::getline(std::cin, dateJoinSchool);
		std::cout << "+ Enter updated student phone number: ";
		std::getline(std::cin, phone);
		std::cout << "+ Enter updated student personal ID: ";
		std::getline(std::cin, personalID);

		// Encrypt Phone and PersonalID
		phone = encryptSensitiveInfo(phone, keyEncrypt);
		personalID = encryptSensitiveInfo(personalID, keyEncrypt);

		//setters for phone and personalID
		setPhone(phone);
		setPersonalID(personalID);

		// display student information
		displayStudent();

		std::cout << "\nStudent information updated successfully!" << std::endl;
	}

};