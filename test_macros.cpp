#include "testlib.h"
#include "parmpars.hpp"

using namespace std;

/*
Example usage:
	$ ./test_macros s1='@var1, world!' s2='@var1@var2' s3='@var1!1' s4='@@@' s5='@!var1'
Output:
	s1 = hello, world!
	s2 = hello42
	s3 = hello1
	s4 = @@@
	s5 = @var1
*/

int main(int argc, char **argv) {
	initGenerator(argc, argv, 1);
	params.define("var1", "hello");
	params.define("var2", 42);
	
	DECLARE_D(string, s1, "");
	DECLARE_D(string, s2, "");
	DECLARE_D(string, s3, "");
	DECLARE_D(string, s4, "");
	DECLARE_D(string, s5, "");
	
	cout << "s1 = " << s1 << endl;
	cout << "s2 = " << s2 << endl;
	cout << "s3 = " << s3 << endl;
	cout << "s4 = " << s4 << endl;
	cout << "s5 = " << s5 << endl;
	
	return 0;
} 
