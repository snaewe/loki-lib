///////////////////////////////////////////////////////////////
//Generates forwarding headers using the information
//in the header.lst & vendor.lst files
//
//To create a header list from an existing directory of headers
//dir *.h /b > headers.lst

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

struct Compiler
	{
	Compiler() {}
	Compiler(const string& VersionTest, const string& SubDir) : 
	 version_test(VersionTest), subdir(SubDir)
		{}
	string version_test;
	string subdir;
	};
	
typedef vector<Compiler> cv_t;

int main(int argc, char* argv[])
	{
	if(argc < 2)
		{
		cout <<"Usage: <Header List> [Version/Vender List]"<<endl;
		return -1;
		}
	
	cv_t vendors;
	char buf[1024];
	string check;
	string subdir;
	fstream vendor_file;
	if(argc >= 3)
		{
		vendor_file.open(argv[2], ios::in);
		if(!vendor_file.is_open())
			{
			cout << "Unable to open vendor file: " << argv[2] << endl;
			return -2;
			}
		else
			{
			vendors.reserve(10);
			while(!vendor_file.eof())
				{
				check  = "";
				subdir = "";
				vendor_file.getline(buf, 1024, '\n');
				check = buf;
				vendor_file.getline(buf, 1024, '\n');
				subdir = buf;
				vendor_file.getline(buf, 1024, '\n');
				if(!(check.empty() || subdir.empty()))
					vendors.push_back(Compiler(check, subdir));
				else
					{
					cout << "Error parsing vendors, check:" << check << "\tsubdir:" << subdir << endl;
					}
				}
			}
		}
	else //if(vendors.empty())
		{
		cout << "No vendor file provided, using defaults" << endl;
		vendors.reserve(10);
		vendors.push_back(Compiler("(_MSC_VER >= 1300)", "MSVC\\1300"));
		vendors.push_back(Compiler("(_MSC_VER >= 1200)", "MSVC\\1200"));
		vendors.push_back(Compiler("( (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ > 1)) )", "Reference"));
		}
	
	fstream header_list(argv[1]);
	string header;
	stringstream ss;
	
	if(!header_list.is_open())
		{
		cout << "Invalid header list file, " << argv[1] << endl;
		return -3;
		}
	while(!header_list.eof())
		{
		header_list >> header;
		cout << header << endl;
		fstream header_file(header.c_str(), ios::out);
		if(!header_file.is_open())
			{
			cout << "Unable to open header file for output: " << header << endl;
			}
		else
			{
			ss.str("");
			ss << "//Generated header: " << header << endl;
			size_t n = ss.str().size();
			for(size_t i=0; i<n; ++i)
				header_file << "/";
			header_file << endl;
			header_file << ss.str();
			header_file << "//Forwards to the appropriate code" << endl;
			header_file << "// that works on the detected compiler" << endl;
			header_file << endl << endl;
			
			cv_t::iterator it=vendors.begin(), itEnd = vendors.end();
			
			header_file << "#ifdef LOKI_USE_REFERENCE" << endl;
			header_file << "#\tinclude \".\\Reference\\" << header << "\"" << endl;
			header_file << "#else" << endl;

			header_file << "#\tif " << it->version_test << endl;
			header_file << "#\t\tinclude \".\\" << it->subdir;
				header_file << "\\" << header << "\""<< endl;
			++it;
			for(; it!=itEnd; ++it)
				{
				header_file << "#elif " << it->version_test << endl;
				header_file << "#\t\tinclude \".\\" << it->subdir;
				header_file << "\\" << header << "\""<< endl;
				}
			header_file << "#\telse" << endl;
			header_file << "\t\t//Define LOKI_USE_REFERENCE and get back to us on the results" << endl;
			header_file << "#\t\terror Compiler not tested with Loki, #define LOKI_USE_REFERENCE" << endl;
			header_file << "#\tendif" << endl;
			header_file << "#endif" << endl;
			}
		}
#ifdef _MSC_VER
	system("PAUSE");
#endif
	return 0;
	}