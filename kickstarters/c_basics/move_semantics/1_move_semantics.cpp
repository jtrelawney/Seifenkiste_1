//============================================================================
// Name        : 1_move_semantics.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// compile and grind with:
// g++ -std=c++11 1_move_semantics.cpp -o test.exe
// valgrind --leak-check=full --show-leak-kinds=all -v ./test.exe


#include <iostream>
#include <memory>

using namespace std;


class sensor{
private:
	unsigned int id;
	unsigned int size;
	int *data;
	unique_ptr<int> udata;

	sensor():id(0),size(0),data(nullptr), udata(nullptr){cout << "sensor private standard constructor call" << (void *) this << endl; };

public:
	sensor(unsigned int i, unsigned int s, int d, int ud):id(i),size(s),data(new int(d)), udata(unique_ptr<int> (new int(ud))){ cout << "sensor constructor call" << (void *) this << endl; };

	~sensor(){
		cout << "sensor de-constructor call" << (void *) this << endl;
		if (data != nullptr) {
			cout << "deleting data ptr" << endl;
			delete data;
		} else {
			cout << "data ptr is nullptr" << endl;
		}
		if (udata != nullptr) {
			cout << "deleting udata ptr" << endl;
			udata.reset();
		} else {
			cout << "udata ptr is nullptr" << endl;
		}
	};

	void print(){
		cout << "sensor " << (void *) this << "   state = " << id << " , " << size << " , ";
		if (data!=nullptr) cout << *data; else cout << "nullptr";
		cout << " , ";
		if (udata!=nullptr) cout << *udata; else cout << "nullptr";
		cout << endl;
	};

	void set_id(unsigned int val){ id = val; };
	void set_udata(int newval){ udata.reset(new int(newval)); }

	// copy not possible
	sensor& operator=(sensor& other) = delete;
	sensor(sensor& other) = delete;

	/*
	sensor(const sensor& other):id(0),size(0),data(nullptr){
		cout << "sensor copy constructor call" << (void *) this << endl;
		id = other.id;
		size = other.size;
		if (other.data == nullptr) {
			// practically obsolete because this is initialized with data = nullptr
			cout << "other data ptr = nullptr" << endl;
			data = nullptr;
		} else {
			//cout << "other data ptr = " << (void *) other.data << "   content = " << *(other.data)<< endl;
			if (data == nullptr) {
				data = new int(*(other.data));
				//delete other.data;
				//the compiler optimizes this away, so it would complain
				//other.data == nullptr;
			} else {
				cout << "data ptr is not nullptr, this is not good because this-data was initialized with nullptr" << endl;
				exit(0);
			}
		}
		if (other.udata == nullptr) {
			// practically obsolete because this is initialized with data = nullptr
			cout << "other udata ptr = nullptr" << endl;
			udata = nullptr;
		} else {
			//cout << "other data ptr = " << (void *) other.data << "   content = " << *(other.data)<< endl;
			if (udata == nullptr) {
				udata = unique_ptr<int> ( new int(*(other.udata)) );
				//the compiler optimizes this away, so it would complain
				//other.data == nullptr;
			} else {
				cout << "data ptr is not nullptr, this is not good because this-data was initialized with nullptr" << endl;
				exit(0);
			}
		}
	};
	*/

	// access to the move constructor and move assignement operator
	//sensor(sensor&& other) = default;
	//sensor& operator=(sensor&& other) = default;

	sensor(sensor&& other):id(0),size(0),data(nullptr), udata(unique_ptr<int> (nullptr)){
		cout << "sensor move constructor call" << (void *) this << endl;
		if (this == &other) { cout << "call with own object " << endl; return; }
		this->id = other.id;
		this->size = other.size;

		// data is nullptr as per init list
		if (other.data != nullptr) {
			if (this->data == nullptr) this->data = new int(*other.data);
			else *(this->data) = *(other.data);
		} else {
			if (this->data != nullptr) {
				delete this->data;
				this->data = nullptr;
			}
		}
		//cout << *(this->data) << " " << *other.data << endl;

		// udata is nullptr as per init list
		if (this->udata == nullptr) this->udata.reset(new int(*(other.udata) ) );
		else udata.reset(new int(*other.data));

		other.id = 0;
		other.size = 0;
		if (other.data != nullptr) {
			delete other.data;
			other.data = nullptr;
		}
		if (other.udata != nullptr) other.udata.reset();
	};

	sensor& operator=(sensor&& other){
		cout << "sensor move assignment call" << (void *) this << endl;
		if (this == &other) { cout << "call with own object " << endl; return *this; }

		this->id = other.id;
		this->size = other.size;

		// data is nullptr as per init list
		if (other.data != nullptr) {
			if (this->data == nullptr) this->data = new int(*other.data);
			else *(this->data) = *(other.data);
		} else {
			if (this->data != nullptr) {
				delete this->data;
				this->data = nullptr;
			}
		}
		//cout << *(this->data) << " " << *other.data << endl;

		int i = *(other.udata);
		if (this->udata != nullptr) this->udata.reset(new int(i) );
		else this->udata = unique_ptr<int> ( new int(i) );

		//cout << *(this->udata) << " " << *(other.udata) << endl;

		other.id = 0;
		other.size = 0;
		if (other.data != nullptr) {
			delete other.data;
			other.data = nullptr;
		}

		if (other.udata != nullptr) other.udata.reset();
		return *this;
	};
};

sensor create_sensor(){
	sensor s(3,1,7,9);
	cout << "create_sensor call, handing back the sensor" << endl;
	return s;
}

int main() {

	cout << "init sensor object - s(1,5,1)" << endl;
	sensor s(1,1,5,6);
	s.print();

	cout << "\ninit ptr to sensor object and delete it - t = new sensor(2,6,1)" << endl;
	sensor *t = new sensor(2,1,7,8);
	t->print();
	delete t;

	// copy the existing s to a new s1
	cout << "\nassign s to new s1 , s1=s" << endl;
	sensor s1 = move(s);
	s1.print();
	s.print();

	// copy the existing s to a new s1
	// this compiles and executes but crashes during data access, the s object is invalid of course
	cout << "\nrecreate s" << endl;
	s = sensor(3,1,9,10);
	s.print();

	cout << "\ncopy s into new s2 - s2(s)" << endl;
	sensor s2(move(s));
	s2.print();

	// create a new sensor using a function
	// at first sight this should call the copy assignment operator
	// but because the functions local sensor object goes out of scope it should call the move assignment operator
	// but it really calls the sensor constructor, because it seems to inline the code ....
	cout << "\ncreate sensor in a function call - s3=create_sensor()" << endl;
	sensor s3 = create_sensor();
	cout << "this is what I got back" << endl;
	s3.print();

	cout << "\ncreate unique ptr to sensor t1" << endl;
	unique_ptr<sensor> t1 = unique_ptr<sensor> (new sensor(6,1,6,19) );
	t1->print();

	//cout << "\nextract the object from t1 and change the id" << endl;
	//cout << "\nthis doesnt change the original t1" << endl;
	//cout << "\nthe compiler complains once the copy constructor has been deleted" << endl;
	//sensor ti = *t1.get();
	//ti.set_id(1000);
	//ti.print();

	cout << "\nextract the object ptr from t1 into tii and change the id to 2000" << endl;
	cout << "this changes tiii and the original t1" << endl;
	sensor *tii = t1.get();
	tii->set_id(2000);

	cout << "\nt1 is now " << endl;
	t1->print();

	cout << "\ncreate unique ptr to sensor t2" << endl;
	unique_ptr<sensor> t2 = unique_ptr<sensor> ( new sensor(5,1,5,5) );
	t2->print();

	cout << "\nmove t1 into t2" << endl;
	t2 = move(t1);
	if (t2) t2->print();
	cout << "the t1 sensor now looks like this" << endl;
	if (t1) t1->print(); else cout << "t1 = nullptr" << endl;

	// see above
	//cout << "\nsee if the extracted object still exists" << endl;
	//ti.print();
	//ti.set_udata(3000);
	//ti.print();

	// this delete points to t1 -> hence first free t1, then this
	//tii = nullptr;
	//delete tii;

	cout << "\n\nthe end" << endl;
	return 0;
}
