#pragma once
#include <iomanip> // for std::put_time
#include <vector>
#include <string>
#include <fstream>
#include "FileInfo.h"// VolumeInfo
#include "ListTable.h"
#include "Data.h"
#include "ListUtil.h"// FileUtil
#include "MD5.h"
#include "AES.h"
#include "Student.h"
#include "Teacher.h"
#include <string>

#define NUM_ENTRY 94	// number of entries, must be 32 * k - 2
#define ENTRY_SIZE 16	// size of an entry (bytes), can't change
#define PADDING MAX_JAMS * (MAX_JAM_LEN + 2) + 1	// reserve to encrypt

using namespace std;

class File
{
private:
	FileInfo* _fileInfo;
	vector<ListTable*> _listTable;
	vector<Data*> _dataTable;

	string _path;

public:
	// Constructor
	File(const string& path)
	{
		_fileInfo = new FileInfo();

		for (int i = 0; i < NUM_ENTRY; i++) {
			_listTable.push_back(new ListTable());
		}

		_path = path;
	}

	File(uint32_t size, const string& path)
	{
		_fileInfo = new FileInfo(size);
		//cout<<"step1"<<endl;
		for (int i = 0; i < NUM_ENTRY; i++) {
			_listTable.push_back(new ListTable());
		}
		//cout<<"list table size when create:"<<_listTable.size()<<endl;
		int dataSize = ceil((size * 1024.0 - (NUM_ENTRY + 2) * ENTRY_SIZE) / BLOCK_SIZE);
		//cout<<"data size :"<<dataSize<<endl;//2042
		for (int i = 0; i < dataSize; i++) {
			_dataTable.push_back(new Data());
		}

		_path = path;
	}

	// Getter - Setter
	FileInfo* fileInfo() { return _fileInfo; }
	vector<ListTable*> listTable() { return _listTable; }
	vector<Data*> dataTable() { return _dataTable; }

	// Method
	void writeFile()
	{
		fstream fo(_path, ios::out | ios::binary);

		// Write File Info
		ListUtil::writeBlock(fo, 0, (char*)_fileInfo);

		// Write List Table
		for (int i = 0; i < _listTable.size(); i++) {
			ListUtil::writeBlock(fo, (i + 2) * ENTRY_SIZE, (char*)_listTable[i]);
		}

		// Write Data Table
		for (int i = 0; i < _dataTable.size(); i++) {
			ListUtil::writeBlock(fo, (NUM_ENTRY + 2) * ENTRY_SIZE + i * BLOCK_SIZE, (char*)_dataTable[i]);
		}

		fo.close();
	}

	void readFile()
	{
		fstream fi(_path, ios::in | ios::binary);

		// Read file Info
		ListUtil::readBlock(fi, 0, (char*)_fileInfo, 2 * ENTRY_SIZE);

		// Read List Table
		for (int i = 0; i < NUM_ENTRY; i++) {
			ListUtil::readBlock(fi, (i + 2) * ENTRY_SIZE, (char*)_listTable[i], ENTRY_SIZE);
		}

		// Read Data Table
		int dataSize = ceil((_fileInfo->size() * 1024.0 - (NUM_ENTRY + 2) * ENTRY_SIZE) / BLOCK_SIZE);
		for (int i = 0; i < dataSize; i++) {
			_dataTable.push_back(new Data());
		}
		for (int i = 0; i < _dataTable.size(); i++) {
			ListUtil::readBlock(fi, (NUM_ENTRY + 2) * ENTRY_SIZE + i * BLOCK_SIZE, (char*)_dataTable[i]);
		}

		fi.close();
	}

	void setFilePassword(char* password)
	{
		if (_fileInfo->password() == "") {
			_fileInfo->setPassword((md5(password)));
			writeFile();
			cout << "\nSet password successfully!" << endl;
		}
		else {
			cout << "\nThis File already has password!" << endl;
		}
	}

	void setKey(string key)
	{
		if (_fileInfo->key() == "") {
			_fileInfo->setKey(key);
			writeFile();
			cout << "\nSet key successfully!" << endl;
		}
		else {
			cout << "\nThis File already has key!" << endl;
		}
	}

	void changeFilePassword(char* oldPass, char* newPass)
	{
		string hashPass = _fileInfo->password();

		if (hashPass != "") {
			if (hashPass == md5(oldPass)) {
				_fileInfo->setPassword((md5(newPass)));
				writeFile();
				cout << "\nChange password successfully!" << endl;
			}
			else {
				cout << "\nWrong password!" << endl;
			}
		}
		else {
			cout << "\nThis file did not have a password." << endl
				<< "To set password, use: setpassword <pass>" << endl;
		}
	}

	int validlistTable() // for teacher list
	{
		// Find the listTable with status 0
		//cout << "here table with status 0 in 94 avaible:  " << _listTable.size() << endl;
		for (int i = 0; i < _listTable.size(); i++) {
			
			if (_listTable[i]->status() == 0)
				//cout<<"candidate listTable"<<i<<endl;
				return i;
		}
		//cout<<"you are here"<<endl;
		// If not listTable 0, find the first listTable deleted
		for (int i = 0; i < _listTable.size(); i++) {
			if (_listTable[i]->status() == _fileInfo->firstEntryDelete())
				return i;
		}

		// Else all listTables are in use
		return -1;
	}


	vector<int> validBlocks(int numBlocks)  // valid block for teacher
	{
		// Create a copy of dataStatus and first delete
		vector<uint32_t> dataStatus;
		//cout << "here data table size :" << _dataTable.size() << endl;
		for (int i = 0; i < _dataTable.size(); i++) {
			//cout<<"here data table size :"<<_dataTable.size()<<endl;
			dataStatus.push_back(_dataTable[i]->status());
		}

		uint32_t firstBlockDelete = _fileInfo->firstBlockDelete();

		// Work with a copy
		vector<int> result;

		for (int i = 0; i < numBlocks; i++) {
			int start = validData(dataStatus, firstBlockDelete);
			if (start != -1) {
				if (dataStatus[start] != 0) {
					firstBlockDelete = ListUtil::nextBlockValue(firstBlockDelete);
				} // if this is a deleted block, first delete ++

				result.push_back(start);
				dataStatus[start] = 1;  // Mark the block as in use
			}
			else {
				return {};
			}
		}
		return result;
	}

	int validlistTableStudent() // for student list: >50
	{
		// Find the listTable with status 0
		//cout << "here table with status 0 in 94 avaible:  " << _listTable.size() << endl;
		for (int i = 50; i < _listTable.size(); i++) {

			if (_listTable[i]->status() == 0)
			{
				cout << "Candidate could be use in listTable :" << i << endl;
				return i;
			}
		}
		//cout<<"you are here"<<endl;
		// If not listTable 0, find the first listTable deleted
		for (int i = 50; i < _listTable.size(); i++) {
			if (_listTable[i]->status() == _fileInfo->firstEntryDelete())
				return i;
		}

		// Else all listTables are in use
		return -1;
	}

	vector<int> validBlocksStudent(int numBlocks) // valid blocks for student , >500
	{
		// Create a copy of dataStatus and first delete
		vector<uint32_t> dataStatus;
		//cout << "here data table size :" << _dataTable.size() << endl;
		for (int i = 50; i < _dataTable.size(); i++) {
			//cout<<"here data table size :"<<_dataTable.size()<<endl;
			dataStatus.push_back(_dataTable[i]->status());
		}

		uint32_t firstBlockDelete = _fileInfo->firstBlockDelete();

		// Work with a copy
		vector<int> result;

		for (int i = 0; i < numBlocks; i++) {
			int start = validDataStudent(dataStatus, firstBlockDelete);
			if (start != -1) {
				if (dataStatus[start] != 0) {
					firstBlockDelete = ListUtil::nextBlockValue(firstBlockDelete);
				} // if this is a deleted block, first delete ++

				result.push_back(start);
				dataStatus[start] = 1;  // Mark the block as in use
			}
			else {
				return {};
			}
		}
		return result;
	}
	int validDataStudent(vector<uint32_t> dataStatus, uint32_t firstBlockDelete)
	{
		// Find the block with status 0
		for (int i = 50; i < dataStatus.size(); i++) {
			if (dataStatus[i] == 0)
			{
				return i;
			}
		}

		// If not block 0, find the first block deleted
		for (int i = 50; i < dataStatus.size(); i++) {
			if (dataStatus[i] == firstBlockDelete)
			{
				return i;
			}
		}

		// Else all block are in use
		return -1;
	}

	int validData(vector<uint32_t> dataStatus, uint32_t firstBlockDelete)
	{
		// Find the block with status 0
		for (int i = 0; i < dataStatus.size(); i++) {
			if (dataStatus[i] == 0)
				return i;
		}

		// If not block 0, find the first block deleted
		for (int i = 0; i < dataStatus.size(); i++) {
			if (dataStatus[i] == firstBlockDelete)
				return i;
		}

		// Else all block are in use
		return -1;
	}

	void importText(string data)
	{
		//cout<<"the data is :"<<data<<endl;
		if (validlistTable() != -1)
		{
			// Check if data available
			int numBlocks = ceil((data.length() + PADDING) * 1.0 / DATA_SIZE);
			//cout<<"number of blocks needed :"<<numBlocks << endl;

			vector<int> dataOffset = validBlocks(numBlocks);
			//cout<<"=> data offset size :"<<dataOffset.size()<<endl;
			if (dataOffset.empty()) {
				cout << "\nYou do not have enough data to import." << endl;
				return;
			}

			// Write Data
			bool isWrite = 1;

			for (int i = 0; i < dataOffset.size(); i++)
			{
				int start = dataOffset[i];

				if (_dataTable[start]->status() != 0) {
					_fileInfo->setFirstBlockDelete(ListUtil::nextBlockValue(_fileInfo->firstBlockDelete()));
				} // if this is a deleted block, first delete ++

				_dataTable[start]->reset(); // Reset before overwrite
				_dataTable[start]->setStatus(1); // Set status

				// Set next block
				if (i == dataOffset.size() - 1) {
					_dataTable[start]->setNextBlock(0xFFFFFFFF);
				}
				else {
					_dataTable[start]->setNextBlock(dataOffset[i + 1]);
				}

				// Set data
				if (isWrite)
				{
					int len = DATA_SIZE;
					if (data.length() < (i + 1) * DATA_SIZE) {
						len = data.length() % DATA_SIZE;
						isWrite = 0;
					}
					_dataTable[start]->setData((char*)data.c_str() + i * DATA_SIZE, len);
				}
			}

			// Write Entry
			int valid = validlistTable();

			if (_listTable[valid]->status() != 0) {
				_fileInfo->setFirstEntryDelete(ListUtil::nextEntryValue(_fileInfo->firstEntryDelete()));
			} // if this is a deleted entry, first delete ++

			_listTable[valid]->reset();
			_listTable[valid]->setUseMode(0x20, 0, dataOffset[0], data.length());

			// Save
			//cout << "\nNumber of blocks: " << dataOffset.size() << "\n" << "Write to block  : ";
			//for (int i = 0; i < dataOffset.size(); i++) std::cout << dataOffset[i] << " ";
			//std::cout << std::endl;

			writeFile();
			std::cout << "\nImport text successfully!" << std::endl;
		}
		else {
			std::cout << "\nYou reached the limit of files/folders." << std::endl;
		}
	};
	void importStudent()
	{
		if (validlistTableStudent() != -1) {
			string keyEc = _fileInfo->key();
			//check if key is empty then set key to default: 123456
			if (keyEc == "") {
				keyEc = "123456";
				_fileInfo->setKey(keyEc);
			}
			Student newStudent;
			// Read Teacher information
			std::cout << "\n- Please Enter Student Information:" << std::endl;
			newStudent.addStudent(keyEc);

			// Check if data available
			int numBlocks = ceil((newStudent.toString().length() + PADDING) * 1.0 / DATA_SIZE);
			//cout << "number of blocks needed :" << numBlocks << endl;

			vector<int> dataOffset = validBlocksStudent(numBlocks);
			//cout << "=> data offset size :" << dataOffset.size() << endl;
			if (dataOffset.empty()) {
				cout << "\nYou do not have enough data to import." << endl;
				return;
			}

			// Write Data
			bool isWrite = 1;

			for (int i = 0; i < dataOffset.size(); i++)
			{
				int start = dataOffset[i];

				if (_dataTable[start]->status() != 0) {
					_fileInfo->setFirstBlockDelete(ListUtil::nextBlockValue(_fileInfo->firstBlockDelete()));
				} // if this is a deleted block, first delete ++

				_dataTable[start]->reset(); // Reset before overwrite
				_dataTable[start]->setStatus(1); // Set status

				// Set next block
				if (i == dataOffset.size() - 1) {
					_dataTable[start]->setNextBlock(0xFFFFFFFF);
				}
				else {
					_dataTable[start]->setNextBlock(dataOffset[i + 1]);
				}

				// Set data
				if (isWrite)
				{
					int len = DATA_SIZE;
					if (newStudent.toString().length() < (i + 1) * DATA_SIZE) {
						len = newStudent.toString().length() % DATA_SIZE;
						isWrite = 0;
					}
					_dataTable[start]->setData((char*)newStudent.toString().c_str() + i * DATA_SIZE, len);
				}
			}

			// Write Entry
			int valid = validlistTableStudent();

			if (_listTable[valid]->status() != 0) {
				_fileInfo->setFirstEntryDelete(ListUtil::nextEntryValue(_fileInfo->firstEntryDelete()));
			} // if this is a deleted entry, first delete ++

			_listTable[valid]->reset();
			_listTable[valid]->setUseMode(0x20, 0, dataOffset[0], newStudent.toString().length());

			// Save
			/*cout << "\nNumber of blocks: " <<
				dataOffset.size() << "\n" << "Write to block  : ";
			for (int i = 0; i < dataOffset.size(); i++) std::cout << dataOffset[i] << " ";
			std::cout << std::endl;*/

			writeFile();
			std::cout << "\nImport student successfully!" << std::endl;
		}
		else {
			std::cout << "\nYou reached the limit of files." << std::endl;
		}
	}


	void importTeacher()
	{
		if (validlistTable() != -1) {
			string keyEc = _fileInfo->key();
			//check if key is empty then set key to default: 123456
			if (keyEc == "") {
				keyEc = "123456";
				_fileInfo->setKey(keyEc);
			}
			Teacher newTeacher;
			// Read Teacher information
			std::cout << "\n- Please Enter Teacher Information:" << std::endl;
			newTeacher.addTeacher(keyEc);

			// Check if data available
			int numBlocks = ceil((newTeacher.toString().length() + PADDING) * 1.0 / DATA_SIZE);
			//cout << "number of blocks needed :" << numBlocks << endl;

			vector<int> dataOffset = validBlocks(numBlocks);
			//cout << "=> data offset size :" << dataOffset.size() << endl;
			if (dataOffset.empty()) {
				cout << "\nYou do not have enough data to import." << endl;
				return;
			}

			// Write Data
			bool isWrite = 1;

			for (int i = 0; i < dataOffset.size(); i++)
			{
				int start = dataOffset[i];

				if (_dataTable[start]->status() != 0) {
					_fileInfo->setFirstBlockDelete(ListUtil::nextBlockValue(_fileInfo->firstBlockDelete()));
				} // if this is a deleted block, first delete ++

				_dataTable[start]->reset(); // Reset before overwrite
				_dataTable[start]->setStatus(1); // Set status

				// Set next block
				if (i == dataOffset.size() - 1) {
					_dataTable[start]->setNextBlock(0xFFFFFFFF);
				}
				else {
					_dataTable[start]->setNextBlock(dataOffset[i + 1]);
				}

				// Set data
				if (isWrite)
				{
					int len = DATA_SIZE;
					if (newTeacher.toString().length() < (i + 1) * DATA_SIZE) {
						len = newTeacher.toString().length() % DATA_SIZE;
						isWrite = 0;
					}
					_dataTable[start]->setData((char*)newTeacher.toString().c_str() + i * DATA_SIZE, len);
				}
			}

			// Write Entry
			int valid = validlistTable();

			if (_listTable[valid]->status() != 0) {
				_fileInfo->setFirstEntryDelete(ListUtil::nextEntryValue(_fileInfo->firstEntryDelete()));
			} // if this is a deleted entry, first delete ++

			_listTable[valid]->reset();
			_listTable[valid]->setUseMode(0x20, 0, dataOffset[0], newTeacher.toString().length());

			// Save
			/*cout << "\nNumber of blocks: " <<
				dataOffset.size() << "\n" << "Write to block  : ";
			for (int i = 0; i < dataOffset.size(); i++) std::cout << dataOffset[i] << " ";
			std::cout << std::endl;*/

			writeFile();
			std::cout << "\nImport teacher successfully!" << std::endl;
		}
		else {
			std::cout << "\nYou reached the limit of files." << std::endl;
		}
	}

	// Function to read all student data from the file
	std::vector<Student> readAllStudentData()
	{
		std::vector<Student> students;

		for (int i = 50; i < NUM_ENTRY; i++)
		{
			//cout<<"list table status :"<<_listTable[i]->status()<<endl;
			if (_listTable[i]->status() == 1)
			{
				//cout<<"step 1"<<endl;
				uint32_t firstBlockOfData = _listTable[i]->firstBlockOfData();
				uint32_t size = _listTable[i]->size();

				std::string studentData;
				for (int j = 0; j < ceil(size * 1.0 / DATA_SIZE); j++)
				{
					int len = DATA_SIZE;
					if (j == ceil(size * 1.0 / DATA_SIZE) - 1) len = size % DATA_SIZE;

					studentData += _dataTable[firstBlockOfData]->data();
					firstBlockOfData = _dataTable[firstBlockOfData]->nextBlock();
				}

				// cout << "Student data before decrypt:\n" << studentData << endl;
				// Create a Student object from the decrypted data
				
				Student student = Student::fromString(studentData);
				// Decrypt sensitive information
				if (_fileInfo->key() == "")
				{
					string defauldKey = "123456";
					student.decryptSensitiveInfo(defauldKey);
				}
				else
				{ 
					student.decryptSensitiveInfo(_fileInfo->key());
				}
				//student.displayStudent();

				//Add the student to the vector
				students.push_back(student);
			}
		}

		return students;
	}

	// print all student data
	void printAllStudentData()
	{
			std::vector<Student> students = readAllStudentData();
			cout<<"\nNumber of students: "<<students.size()<<endl;

			for (int i = 0; i < students.size(); i++)
			{
				cout << "\n*) Student " << i + 1 << ":" << endl;
				students[i].displayStudent();
				cout << endl;
			}
	}

		// Function to read all teacher data from the file
	std::vector<Teacher> readAllTeacherData()
	{
		std::vector<Teacher> teachers;

		for (int i = 0; i < 50; i++)
		{
			//cout<<"list table status :"<<_listTable[i]->status()<<endl;
			if (_listTable[i]->status() == 1)
			{
				//cout<<"step 1"<<endl;
				uint32_t firstBlockOfData = _listTable[i]->firstBlockOfData();
				uint32_t size = _listTable[i]->size();

				std::string teacherData;
				for (int j = 0; j < ceil(size * 1.0 / DATA_SIZE); j++)
				{
					int len = DATA_SIZE;
					if (j == ceil(size * 1.0 / DATA_SIZE) - 1) len = size % DATA_SIZE;

					teacherData += _dataTable[firstBlockOfData]->data();
					firstBlockOfData = _dataTable[firstBlockOfData]->nextBlock();
				}

				// cout << "Teacher data before decrypt:\n" << studentData << endl;
				// Create a Teacher object from the decrypted data

				Teacher teacher = Teacher::fromString(teacherData);
				// Decrypt sensitive information
				if (_fileInfo->key() == "")
				{
					string defauldKey = "123456";
					teacher.decryptSensitiveInfo(defauldKey);
				}
				else
				{
					teacher.decryptSensitiveInfo(_fileInfo->key());
				}

				//Add the teacher to the vector
				teachers.push_back(teacher);
			}
		}

		return teachers;
	}

	// print all teacher data
	void printAllTeacherData()
	{
		std::vector<Teacher> teachers = readAllTeacherData();
		cout << "\nNumber of teachers: " << teachers.size() << endl;

		for (int i = 0; i < teachers.size(); i++)
		{
			cout << "\n*) Teacher " << i + 1 << ":" << endl;
			teachers[i].displayTeacher();
			cout << endl;
		}
	}

	// remove student by name
	void removeStudent(const std::string& studentName, bool permanently)
	{
		for (int i = 0; i < _listTable.size(); i++)
		{
			if (_listTable[i]->status() == 1)
			{
				uint32_t firstBlockOfData = _listTable[i]->firstBlockOfData();
				uint32_t size = _listTable[i]->size();

				std::string studentData;
				for (int j = 0; j < ceil(size * 1.0 / DATA_SIZE); j++)
				{
					int len = DATA_SIZE;
					if (j == ceil(size * 1.0 / DATA_SIZE) - 1) len = size % DATA_SIZE;

					studentData += _dataTable[firstBlockOfData]->data();
					firstBlockOfData = _dataTable[firstBlockOfData]->nextBlock();
				}

				// Create a Student object from the decrypted data
				Student student = Student::fromString(studentData);

				if (student.getName() == studentName) {
					if (permanently)
					{
						// Delete data
						int firstBlock = _listTable[i]->firstBlockOfData();

						while (true) {
							int nextBlock = _dataTable[firstBlock]->nextBlock();
							_dataTable[firstBlock]->reset();

							if (nextBlock == 0xFFFFFFFF) break;
							else firstBlock = nextBlock;
						}

						// Delete entry
						_listTable[i]->reset();
					}
					else
					{
						// Delete entry
						uint16_t nextE = _fileInfo->firstEntryDelete();
						_listTable[i]->setStatus(nextE);
						_fileInfo->setFirstEntryDelete(ListUtil::nextEntryValue(nextE));

						// Delete data
						int firstBlock = _listTable[i]->firstBlockOfData();
						while (true) {
							uint16_t nextB = _fileInfo->firstBlockDelete();
							_dataTable[firstBlock]->setStatus(nextB);
							_fileInfo->setFirstBlockDelete(ListUtil::nextBlockValue(nextB));

							if (_dataTable[firstBlock]->nextBlock() == 0xFFFFFFFF) {
								break;
							}
							else {
								firstBlock = _dataTable[firstBlock]->nextBlock();
							}
						}
					}

					// Save changes to the file
					writeFile();
					std::cout << "\nRemove student '" << studentName << "' successfully!" << std::endl;
					return;
				}
			}
		}

		std::cout << "\nStudent not found!" << std::endl;
	}

	// remove teacher by name
	void removeTeacher(const std::string& teacherName, bool permanently)
	{
		for (int i = 0; i < _listTable.size(); i++)
		{
			if (_listTable[i]->status() == 1)
			{
				uint32_t firstBlockOfData = _listTable[i]->firstBlockOfData();
				uint32_t size = _listTable[i]->size();

				std::string teacherData;
				for (int j = 0; j < ceil(size * 1.0 / DATA_SIZE); j++)
				{
					int len = DATA_SIZE;
					if (j == ceil(size * 1.0 / DATA_SIZE) - 1) len = size % DATA_SIZE;

					teacherData += _dataTable[firstBlockOfData]->data();
					firstBlockOfData = _dataTable[firstBlockOfData]->nextBlock();
				}

				// Create a Teacher object from the decrypted data
				Teacher teacher = Teacher::fromString(teacherData);

				if (teacher.getName() == teacherName) {
					if (permanently)
					{
						// Delete data
						int firstBlock = _listTable[i]->firstBlockOfData();

						while (true) {
							int nextBlock = _dataTable[firstBlock]->nextBlock();
							_dataTable[firstBlock]->reset();

							if (nextBlock == 0xFFFFFFFF) break;
							else firstBlock = nextBlock;
						}

						// Delete entry
						_listTable[i]->reset();
					}
					else
					{
						// Delete entry
						uint16_t nextE = _fileInfo->firstEntryDelete();
						_listTable[i]->setStatus(nextE);
						_fileInfo->setFirstEntryDelete(ListUtil::nextEntryValue(nextE));

						// Delete data
						int firstBlock = _listTable[i]->firstBlockOfData();
						while (true) {
							uint16_t nextB = _fileInfo->firstBlockDelete();
							_dataTable[firstBlock]->setStatus(nextB);
							_fileInfo->setFirstBlockDelete(ListUtil::nextBlockValue(nextB));

							if (_dataTable[firstBlock]->nextBlock() == 0xFFFFFFFF) {
								break;
							}
							else {
								firstBlock = _dataTable[firstBlock]->nextBlock();
							}
						}
					}

					// Save changes to the file
					writeFile();
					std::cout << "\nRemove teacher '" << teacherName << "' successfully!" << std::endl;
					return;
				}
			}
		}

		std::cout << "\nTeacher not found!" << std::endl;
	}

	// Edit student by ID
	void editStudent(const std::string& studentID)
	{
		for (int i = 50; i < NUM_ENTRY; i++)
		{
			if (_listTable[i]->status() == 1)
			{
				uint32_t firstBlockOfData = _listTable[i]->firstBlockOfData();
				uint32_t size = _listTable[i]->size();

				std::string studentData;
				for (int j = 0; j < ceil(size * 1.0 / DATA_SIZE); j++)
				{
					int len = DATA_SIZE;
					if (j == ceil(size * 1.0 / DATA_SIZE) - 1) len = size % DATA_SIZE;

					studentData += _dataTable[firstBlockOfData]->data();
					firstBlockOfData = _dataTable[firstBlockOfData]->nextBlock();
				}

				// Create a Student object from the decrypted data
				Student student = Student::fromString(studentData);

				if (student.getId() == std::stoi(studentID))
				{
					string keyEc = _fileInfo->key();
					//check if key is empty then set key to default: 123456
					if (keyEc == "") {
						keyEc = "123456";
						_fileInfo->setKey(keyEc);
					}
					// Edit the student information 
					student.updateStudent(keyEc);

					// Encrypt the updated information
					std::string encryptedData = student.toString(); // Assuming toString returns encrypted data

					// Update the file data
					std::vector<uint32_t> dataStatus(_dataTable.size(), 0); // Create a copy of dataStatus)
					int start = validDataStudent(dataStatus, 0); // Find a valid data block
					if (start != -1)
					{
						_dataTable[start]->reset();
						_dataTable[start]->setStatus(1);

						// Set next block
						_dataTable[start]->setNextBlock(0xFFFFFFFF);

						// Set data
						_dataTable[start]->setData((char*)encryptedData.c_str(), encryptedData.length());

						// Update the list table entry
						_listTable[i]->setUseMode(0x20, 0, start, encryptedData.length());

						// Save changes to the file
						writeFile();
						std::cout << "\nEdit student '" << studentID << "' successfully!" << std::endl;
						return;
					}
					else
					{
						std::cout << "\nNo available data block to edit student." << std::endl;
						return;
					}
				}
			}
		}

		std::cout << "\nStudent not found!" << std::endl;
	}

	// Edit teacher by ID
	void editTeacher(const std::string& teacherID)
	{
		for (int i = 0; i < 50; i++)
		{
			if (_listTable[i]->status() == 1)
			{
				uint32_t firstBlockOfData = _listTable[i]->firstBlockOfData();
				uint32_t size = _listTable[i]->size();

				std::string teacherData;
				for (int j = 0; j < ceil(size * 1.0 / DATA_SIZE); j++)
				{
					int len = DATA_SIZE;
					if (j == ceil(size * 1.0 / DATA_SIZE) - 1) len = size % DATA_SIZE;

					teacherData += _dataTable[firstBlockOfData]->data();
					firstBlockOfData = _dataTable[firstBlockOfData]->nextBlock();
				}

				// Create a Teacher object from the decrypted data
				Teacher teacher = Teacher::fromString(teacherData);

				if (teacher.getId() == std::stoi(teacherID))
				{
					string keyEc = _fileInfo->key();
					//check if key is empty then set key to default: 123456
					if (keyEc == "") {
						keyEc = "123456";
						_fileInfo->setKey(keyEc);
					}
					// Edit the teacher information 
					teacher.updateTeacher(keyEc);

					// Encrypt the updated information
					std::string encryptedData = teacher.toString(); // Assuming toString returns encrypted data

					// Update the file data
					std::vector<uint32_t> dataStatus(_dataTable.size(), 0); // Create a copy of dataStatus)
					int start = validData(dataStatus, 0); // Find a valid data block
					if (start != -1)
					{
						_dataTable[start]->reset();
						_dataTable[start]->setStatus(1);

						// Set next block
						_dataTable[start]->setNextBlock(0xFFFFFFFF);

						// Set data
						_dataTable[start]->setData((char*)encryptedData.c_str(), encryptedData.length());

						// Update the list table entry
						_listTable[i]->setUseMode(0x20, 0, start, encryptedData.length());

						// Save changes to the file
						writeFile();
						std::cout << "\nEdit teacher '" << teacherID << "' successfully!" << std::endl;
						return;
					}
					else
					{
						std::cout << "\nNo available data block to edit teacher." << std::endl;
						return;
					}
				}
			}
		}

		std::cout << "\nTeacher not found!" << std::endl;
	}

	void printInfo()
	{
		auto formatDate = [](const std::chrono::system_clock::time_point& tp) {
			std::time_t time = std::chrono::system_clock::to_time_t(tp);
			std::tm tm = *std::localtime(&time);
			char buffer[20]; // Assumes the maximum required size for the formatted date
			std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
			return std::string(buffer);
		};

		cout << "\nfile Info" << endl
			<< "   Signature          : " << _fileInfo->signature() << endl
			<< "   Password           : " << _fileInfo->password() << endl
			<< "   Size               : " << _fileInfo->size() << endl
			<< "   First entry delete : " << _fileInfo->firstEntryDelete() << endl
			<< "   Next entry delete  : " << _fileInfo->nextEntryDelete() << endl
			<< "   First block delete : " << _fileInfo->firstBlockDelete() << endl
			<< "   Next block delete  : " << _fileInfo->nextBlockDelete() << endl
			<< "   Date created       : " << formatDate(_fileInfo->datecreate()) << endl
			<< "   Date modified      : " << formatDate(_fileInfo->datemodify()) << endl
			<< "   Key                : " << _fileInfo->key() << endl;
	}
};