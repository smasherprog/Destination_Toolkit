// Threading.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Threading_Lib/Thread.h"
#include "concurrent_vector.h"
#include "../Threading_Lib/mutex_rw.h"
#include <string>

using namespace std;
unsigned int global=0;
std::vector<int> testvec;
Threading::mutex_rw spin_mutex;//

class somerandomclass {
public:

	unsigned int classobject_1, classobject_2, classobject_3;
	std::string name;

	void someclassfunction(){
		cout<<"starting " + name<<endl;
		Threading::Parallel_For(classobject_1, classobject_2, classobject_3, [](unsigned int beg, unsigned int end){
			for(unsigned int b(beg); b < end; b++){
				Threading::lock_guard<Threading::mutex_rw> lock(spin_mutex, false);// aquire a write lock. This will release the lock when it goes out of scope
				cout<<"entered write lock . . . "<<b<<endl;
				testvec.push_back(b);
			}
		});
	}
};
class someotherclass{
public:

	somerandomclass object;
	void workisdone(){
		Threading::lock_guard<Threading::mutex_rw> lock(spin_mutex);// aquire a read lock. This will release the lock when it goes out of scope
		cout<<"Finished with the work on"<<object.name<<"    reading something from the vector"<<testvec[object.classobject_2 -1]<<std::endl; 
	}

};
int main(int argc, char* argv[])
{

	Threading::Internal_Use::cThreadStarter thrdstarter;
	
	// I recommend using 0 for beg, 1000 for end and 5-10 for splits. This demo is to prove that adding for loops inside of for loops works correctly and the waiting functionality also works correctly
	cout << "Parallel for! First number is the beginning, second is the end, the last is the number is splits."<<endl;
	unsigned int beg, end, splits;
	cin>>beg>>end>>splits;

	cout<<"Starting Parallel For Work"<<endl;
	Threading::Parallel_For(beg, end, splits, [](unsigned int beg, unsigned int end){
		for(unsigned int b(beg); b < end; b++){
			Threading::lock_guard<Threading::mutex_rw> lock(spin_mutex, false);// aquire a write lock. This will release the lock when it goes out of scope
			cout<<"entered write lock . . . "<<b<<endl;
			testvec.push_back(b);
		}
	});
	cout<<"Finished Parallel For Work "<<std::endl;

	
	somerandomclass object;
	object.classobject_1 =beg;
	object.classobject_2 =end;
	object.classobject_3 =splits;
	object.name = "object numero uno";

	somerandomclass object1;
	object1.classobject_1 =beg;
	object1.classobject_2 =end;
	object1.classobject_3 =splits;
	object1.name = "object numero dos";

	somerandomclass object2;
	object2.classobject_1 =beg;
	object2.classobject_2 =end;
	object2.classobject_3 =splits;
	object2.name = "object numero three!!";
	someotherclass obj;
	obj.object = object2;

	cout<<"Functions can be bound and object to call a specific function!";
	// be carefull with the capture on lambda's. I did a capture by copy, but I could have done it by reference, I just need to be sure the object will last the until the callback is called
	Threading::RunTask(std::bind(&somerandomclass::someclassfunction, object), [](){ cout<<"Finished with the work on"<<endl; });
	Threading::RunTask(std::bind(&somerandomclass::someclassfunction, object1), [](){ cout<<"Finished with the work on"<<endl; });
	std::shared_ptr<Threading::Base_Task_Work> waiter;// create an empty shared_ptr to pass to the library

	Threading::RunTask(std::bind(&somerandomclass::someclassfunction, object2), waiter, std::bind(&someotherclass::workisdone, obj));
	//now, you can hold onto waiter as long as you want. The library will release it when the work is done, but since it was passed into the RunTask function, an extra reference was created so it will persist
	/*
	waiter->Wait();// you can do this if you want!
	if(waiter->GetWorkStatus() == Threading::Work_Status::FINISHED) cout<<"Work is done!!"<<endl;
*/
	
	cout<<"Starting to wait . . . . "<<std::endl;
	Threading::WaitForAll();// This waits for ALL WORK to complete that the library currently has

	cout<<"Finished WaitForAll "<<endl;

	system("pause");
}

