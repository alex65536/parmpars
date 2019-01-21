/*
 * MIT License
 * 
 * Copyright (c) 2018 Alexander Kernozhitsky <sh200105@mail.ru>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "testlib.h"
#include "parmpars.hpp"
#include <iostream>
#include <sstream>

using namespace std;

/*
Example usage:
	$ ./test i=1 l=2000111222 ll=-31513975719571 f=3.931 d=-13841.313 ld=-180e433 \
	  c=a b=true s=hello ichk=20 schk=gepardo-debian 'ra=[1;5]' 'rachk=[1;100]' \
	  're=lin[a-z]{1,2}-(mint|u[a-z]u[a-z][a-z]u)' unused=45 'igen=[1; 200]' wuovnowvwim
Output:
	i = 1
	l = 2000111222
	ll = -31513975719571
	f = 3.931
	d = -13841.3
	ld = -1.8e+435
	c = a
	b = 1
	s = hello
	ichk = 20
	schk = gepardo-debian
	ra = [1; 5] 5
	rachk = [1; 100] 73
	re = lin[a-z]{1,2}-(mint|u[a-z]u[a-z][a-z]u) linry-mint
	id = 10
	idchk = 10
	sd = hello
	sdchk = hello
	sgen = uayiiuie
	igen = 108
	Warning: Variable unused is unused!
*/

int main(int argc, char **argv) {
	initGenerator(argc, argv, 1);
	DECLARE(int, i);
	DECLARE(long, l);
	DECLARE(long long, ll);
	DECLARE(float, f);
	DECLARE(double, d);
	DECLARE(long double, ld);
	DECLARE(char, c);
	DECLARE(bool, b);
	DECLARE(string, s);
	DECLARE(int, ichk, range(1, 100));
	DECLARE(string, schk, "gepardo.[a-zA-Z]{1,10}");
	DECLARE(GenRange<int>, ra);
	DECLARE(GenRange<int>, rachk, range(1, 100));
	DECLARE(GenRegex, re);
	DECLARE_D(int, id, 10);
	DECLARE_D(int, idchk, 10, range(1, 100));
	DECLARE_D(string, sd, "hello");
	DECLARE_D(string, sdchk, "hello", "h.l.{1,5}");
	DECLARE_GEN_D(GenRegex, sgen, GenRegex("[aoueiy]{3,8}"));
	DECLARE_GEN(GenRange<int>, igen, range(1, 200));
	cout << "i = " << i << endl;
	cout << "l = " << l << endl;
	cout << "ll = " << ll << endl;
	cout << "f = " << f << endl;
	cout << "d = " << d << endl;
	cout << "ld = " << ld << endl;
	cout << "c = " << c << endl;
	cout << "b = " << b << endl;
	cout << "s = " << s << endl;
	cout << "ichk = " << ichk << endl;
	cout << "schk = " << schk << endl;
	cout << "ra = " << ra << " " << ra.generate() << endl;
	cout << "rachk = " << rachk << " " << rachk.generate() << endl;
	cout << "re = " << re << " " << re.generate() << endl;
	cout << "id = " << id << endl;
	cout << "idchk = " << idchk << endl;
	cout << "sd = " << sd << endl;
	cout << "sdchk = " << sdchk << endl;
	cout << "sgen = " << sgen << endl;
	cout << "igen = " << igen << endl;
	return 0;
}
