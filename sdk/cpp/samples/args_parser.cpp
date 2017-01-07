#include "args_parser.h"
#include <iostream>

using namespace std;

void show_usage(string name)
{
	cerr << "\nUsage:\n\t"<< name << " [http|ssh]://user:password@host:port [-v]" <<endl<<endl;
}

vector<string> parse_args(int argc, char* argv[])
{
	if (argc < 2) {
		show_usage(argv[0]);
		return {};
	}
	string arg = argv[1];
	if ((arg == "-h") || (arg == "--help")) {
		show_usage(argv[0]);
		return {};
	}
	vector<string> ret;
	size_t s = arg.find("ssh://") + sizeof("ssh://")-1;
	if(s==string::npos)
	{
		s = arg.find("http://") + sizeof("http://")-1;
	}
	size_t col1 = arg.find(":",s);
	size_t amp = arg.find("@")-1;
	size_t col2 = arg.find(":",amp);
	ret.push_back(arg.substr(s,col1-s));
	ret.push_back(arg.substr(col1+1, amp-col1));
	ret.push_back(arg.substr(amp+2, col2-amp-2));
	ret.push_back(arg.substr(col2+1));

	bool verb = false;
	if(argc == 3)
	{
		string v = argv[2];
		if(v=="-v")
			verb = true;
	}

	if(verb)
	{
		ret.push_back("--verbose");
	}
	else
	{
		ret.push_back("--silent");
	}
	return ret;
}
