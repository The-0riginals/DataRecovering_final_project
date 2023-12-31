#pragma once
#include <iostream>
#include <string>
#include "AES.h"
#include "FileInfo.h"

#define AES_BLOCK_SIZE 16

class Teacher {
private:
	int id;
	std::string name;
	std::string dateOfBirth;
	std::string dateJoinSchool;
	std::string phone;
	std::string personalID;

public:
	// Constructor
	Teacher() :id(0), name(""), dateOfBirth(""), dateJoinSchool(""), phone(""), personalID("") {}

	Teacher(int teacherId, const std::string& teacherName, const std::string& dob,
		const std::string& joinDate, const std::string& phoneNumber, const std::string& personalId)
		: id(teacherId), name(teacherName), dateOfBirth(dob),
		dateJoinSchool(joinDate), phone(phoneNumber), personalID(personalId) {}

	// Getters and setters for phone and personalID
	void setPhone(const std::string& phoneNumber) { phone = phoneNumber; }
	void setPersonalID(const std::string& personalId) { personalID = personalId; }
	std::string getPhone() const { return phone; }
	std::string getPersonalID() const { return personalID; }

	std::string getName() const {
		return name;
	}

	//get teacher id
	int getId() const {
		return id;
	}

	// Function to add a teacher from keyboard
	void addTeacher(const std::string& keyEncrypt)
	{
		std::cout << "+ Enter teacher ID: ";
		std::cin >> id;
		std::cout << "+ Enter teacher name: ";
		std::cin.ignore();
		std::getline(std::cin, name);
		std::cout << "+ Enter teacher date of birth: ";
		std::getline(std::cin, dateOfBirth);
		std::cout << "+ Enter teacher date join school: ";
		std::getline(std::cin, dateJoinSchool);
		std::cout << "+ Enter teacher phone number: ";
		std::getline(std::cin, phone);
		std::cout << "+ Enter teacher personal ID: ";
		std::getline(std::cin, personalID);

		// Encrypt Phone and PersonalID
		phone = encryptSensitiveInfo(phone, keyEncrypt);
		personalID = encryptSensitiveInfo(personalID, keyEncrypt);

		// Setters for phone and personalID
		setPhone(phone);
		setPersonalID(personalID);

		// Display teacher information
		displayTeacher();
	}

	// Function to display a teacher
	void displayTeacher()
	{
		std::cout << "\n- Teacher information: " << std::endl;
		std::cout << "  + Teacher ID: " << id << std::endl;
		std::cout << "  + Teacher name: " << name << std::endl;
		std::cout << "  + Teacher date of birth: " << dateOfBirth << std::endl;
		std::cout << "  + Teacher date join school: " << dateJoinSchool << std::endl;
		std::cout << "  + Teacher phone number: " << phone << std::endl;
		std::cout << "  + Teacher personal ID: " << personalID << std::endl;
	}

	// Convert teacher information to a string
	std::string toString()
	{
		std::string result = std::to_string(id) + "," + name + "," + dateOfBirth + "," + dateJoinSchool + "," + phone + "," + personalID;
		return result;
	}

	// Encrypt function
	std::string encryptSensitiveInfo(const std::string& data, const std::string& keyEncrypt)
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

		// Update encrypted Phone/Personal ID Teacher object
		return(std::string(reinterpret_cast<char*>(cipher), plainLen));
	}

	// Padding function
	std::string addPKCS7Padding(const std::string& data)
	{
		size_t paddingSize = AES_BLOCK_SIZE - (data.size() % AES_BLOCK_SIZE);
		char paddingChar = static_cast<char>(paddingSize);

		std::string paddedData = data;
		for (size_t i = 0; i < paddingSize; ++i) {
			paddedData.push_back(paddingChar);
		}

		return paddedData;
	}

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
		std::string decryptedData((char*)plain);
		size_t paddingSize = decryptedData.back();
		decryptedData = decryptedData.substr(0, decryptedData.length() - paddingSize);

		return decryptedData;
	}

	//// Function to decrypt sensitive information
	void decryptSensitiveInfo(const std::string& keyDecrypt)
	{
		// Decrypt Phone
		std::string decryptedPhone = decryptSensitiveInfoPartII(phone, keyDecrypt);
		//cout<<decryptedPhone<<endl;

		// Decrypt PersonalID
		std::string decryptedPersonalID = decryptSensitiveInfoPartII(personalID, keyDecrypt);
		//cout<<decryptedPersonalID<<endl;

		//Update decrypted Phone and PersonalID in the Student object
		setPhone(decryptedPhone);
		setPersonalID(decryptedPersonalID);
	}

	// Function to decrypt a part of sensitive information
	std::string decryptSensitiveInfoPartII(const std::string& encryptedData, const std::string& keyDecrypt)
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

	// Function to convert a string to a Teacher object
	static Teacher fromString(const std::string& decryptedData)
	{
		Teacher teacher;

		std::istringstream ss(decryptedData);
		std::string token;
		std::vector<std::string> tokens;

		while (getline(ss, token, ',')) {
			tokens.push_back(token);
		}

		if (tokens.size() == 6) { // Ensure all fields are present
			teacher.id = std::stoi(tokens[0]);
			teacher.name = tokens[1];
			teacher.dateOfBirth = tokens[2];
			teacher.dateJoinSchool = tokens[3];
			teacher.phone = tokens[4];
			teacher.personalID = tokens[5];
		}

		return teacher;
	}

	// Function to update teacher information
	void updateTeacher(const std::string& keyEncrypt)
	{
		std::cout << "+ Enter updated teacher name: ";
		std::cin.ignore();
		std::getline(std::cin, name);
		std::cout << "+ Enter updated teacher date of birth: ";
		std::getline(std::cin, dateOfBirth);
		std::cout << "+ Enter updated teacher date join school: ";
		std::getline(std::cin, dateJoinSchool);
		std::cout << "+ Enter updated teacher phone number: ";
		std::getline(std::cin, phone);
		std::cout << "+ Enter updated teacher personal ID: ";
		std::getline(std::cin, personalID);

		// Encrypt Phone and PersonalID
		phone = encryptSensitiveInfo(phone, keyEncrypt);
		personalID = encryptSensitiveInfo(personalID, keyEncrypt);

		//setters for phone and personalID
		setPhone(phone);
		setPersonalID(personalID);

		// display teacher information
		displayTeacher();

		std::cout << "\nTeacher information updated successfully!" << std::endl;
	}
};
