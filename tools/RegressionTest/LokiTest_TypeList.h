
#define LOKI_TEST_LENGTH\
	{\
	cout<<"Testing Length"<<endl;\
	cout<<"Length of List1: "<<Loki::TL::Length<Loki::RegressionTest::tyList1>::value<<endl;\
	cout<<"Length of List2: "<<Loki::TL::Length<Loki::RegressionTest::tyList2>::value<<endl;\
	cout<<"Length of List5: "<<Loki::TL::Length<Loki::RegressionTest::tyList5>::value<<endl;\
	if ( (Loki::TL::Length<Loki::RegressionTest::tyList1>::value==1) &&\
	     (Loki::TL::Length<Loki::RegressionTest::tyList2>::value==2) &&\
	     (Loki::TL::Length<Loki::RegressionTest::tyList5>::value==5) )\
		cout<<"Normal cases work"<<endl;\
	else\
		cout<<"Normal cases are ***BROKEN***"<<endl;\
	cout<<"Length of NullType: "<<Loki::TL::Length<Loki::NullType>::value<<endl;\
	cout<<"Length of NullList: "<<Loki::TL::Length<Loki::RegressionTest::tyNullList>::value<<endl;\
	if ( (Loki::TL::Length<Loki::NullType>::value==0) &&\
	     (Loki::TL::Length<Loki::RegressionTest::tyNullList>::value==0) )\
		cout<<"Pathological cases work"<<endl;\
	else\
		cout<<"Pathological cases are ***BROKEN***"<<endl;\
	cout<<endl<<endl;\
	}


/*
		void TypeAt()
			{
			cout<<endl<<"Testing TypeAt"<<endl;
			//typedef TypeAt<List2, -1>::Result FailsToCompile;
			typedef TypeAt<List2, 0>::Result FirstType;
			typedef TypeAt<List2, 1>::Result SecondType;
			typedef TypeAt<List2, 2>::Result ThirdType;
			typedef TypeAt<List2, 3>::Result FourthType;
			typedef TypeAt<List2, 4>::Result FifthType;
			cout<<typeid(FirstType).name()<<endl;
			cout<<typeid(SecondType).name()<<endl;
			cout<<typeid(ThirdType).name()<<endl;
			cout<<typeid(FourthType).name()<<endl;
			cout<<typeid(FifthType).name()<<endl;
			}
		/*
		void IndexOf()
			{
			cout<<endl<<"Testing IndexOf"<<endl;
			cout<<"IndexOf     int: "<<IndexOf<List2, int>::value<<endl;
			cout<<"IndexOf  double: "<<IndexOf<List2, double>::value<<endl;
			cout<<"IndexOf  vector: "<<IndexOf<List2, vector<int> >::value<<endl;
			cout<<"IndexOf Complex: "<<IndexOf<List2, Complex>::value<<endl;
			cout<<"IndexOf    long: "<<IndexOf<List2, long>::value<<endl;
			}
		/*
		void Append()
			{
			cout <<endl<<"Testing Append" << endl;
			typedef Append<NullType, NullType>::Result List0;
			typedef Append<List0, bool>::Result List1;
			typedef Append<List1, double>::Result List2;
			typedef Append<List2, NullType>:: Result List3;
			cout << Length<List0>::value << endl;
			cout << Length<List1>::value << "\t";
			cout << typeid(TypeAt<List1, Length<List1>::value -1 >::Result).name() << endl;
			cout << Length<List2>::value << "\t";
			cout << typeid(TypeAt<List2, Length<List2>::value -1>::Result).name() << endl;
			cout << Length<List3>::value << "\t";
			cout << typeid(TypeAt<List3, Length<List3>::value -1>::Result).name() << endl;
			}
		/*
		void Erase()
			{
			cout <<endl<< "Testing Erase" << endl;
			cout <<"Erase is broken, notice the size goes up"<<endl;
			cout << Length<List1>::value << "\t" 
				<< typeid(TypeAt<List1, Length<List1>::value -1 >::Result).name() << endl;
			cout << Length<List2>::value << "\t" 
				<< typeid(TypeAt<List2, Length<List2>::value -1>::Result).name() << endl;
			cout << Length<List3>::value << "\t" 
				<< typeid(TypeAt<List3, Length<List3>::value -1>::Result).name() << endl;
			}
			*/