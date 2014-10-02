// stringstreams
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <ctype.h>

using namespace std;
/*Type A ex: AB12. Type B ex: R1C2*/

int id(string entry);
string convertToA(string entry);//convert to type A from type B
string convertToB(string entry);

int main()
{
	string s;
	int numItems;
	int entryType;
	//get data
	getline(cin, s);
	numItems = stoi(s);
	vector<string> output;
	for (int i=numItems; i > 0; i--) {
		getline(cin, s);
		entryType = id(s);
		if (entryType == 0) {
			s=convertToB(s);
		}
		else {
			s=convertToA(s);
		}
		output.push_back(s);
	}

	for (vector<string>::iterator i = output.begin();
		i != output.end();
		++i)
	{
		cout << *i << "\n";
	}

	return 0;
}

int id(string entry) {
	//only case where an entry is type B is if firstIsR is true, scndIsLtr is false, and foundC is true. We have if/else to exit the loop early
	//for the sake off efficiency if we detect it is type A right away. We could just test is B conditions are met after processing though.
	//return 0 = A and 1 = B
	bool firstIsR=false, scndIsLtr=false, foundC=false;
vector<char> chars(entry.begin(), entry.end());

if (chars[0] == 'R') {
	firstIsR = true;
}
else {
	return 0;
}
//rz228

if (isalpha(chars[1])) return 0;

for (vector<char>::iterator i = chars.begin();
	i != chars.end();
	++i)
{
	if (*i == 'C') return 1;//report B if we get this far
}
return 0;
}

string convertToB(string orig) {
	string rslt = "R";
	vector<char> chars(orig.begin(), orig.end());

	//find the final letter, we will start processing from there
	int finPos = -1;
	for (vector<char>::iterator i = chars.begin();
		i != chars.end();
		++i)
	{
		if (!isalpha(*i)) {
			rslt += *i;
		}
		else {
			finPos++;
		}
	}
	//finPos now contains the number of letters. use this number to control the loop that processes the letters
	int total = 0;
	int exponent = 0;
	//int test;
	/*Basic idea is to view the string of letters as written in base 26, with the caveat that we start counting from 1 instead of
	0 (ie numbers in our base 26 system are 1-26 not 0-25). When viewed in this light its simple base 26 to base 10 conversion.*/
	
	for (int i = finPos; i != -1; --i) {//work backwards from the final letter, right to left.
		//test = chars[i] - 64;
		total += (chars[i]-64) * pow(26.0,exponent);//(chars[i]-64)converts from ascii code to base 26 coefficient, eg 65=1 -> 1=A. then multiply by appropriate power of 26
		exponent++;
	}

	rslt += "C";
	std::stringstream sstm;
	sstm << rslt << total;
	rslt = sstm.str();

	return rslt;
}

string convertToA(string orig) {
	string rslt;
	string rows, cols;
	vector<char> chars(orig.begin(), orig.end());

	//isolate the integers representing rows and columns
	int i = 1;
	while (true)
	{
		if (orig[i] == 'C') {
			i++;
			for (unsigned int q = i; q < orig.length(); q++) {
				cols += orig[q];
			}
			break;
		}
		else {
			rows += orig[i];
			i++;
		}
	}

	//do the actual conversion, this is only neccesary for the columns.
	int colsInt = stoi(cols);
	/*What exponent is required for what number of digits? Eg is exp 1, ie 26^1 sufficient to take bites out of 3 digits? Too
	big or too small? 2 digits? It suffices for up to three since you can have a coefficient up to 26. If we used exp = 2,
	we start at 26*26. This is ok for 3 digits, but any larger coefficient than 1 (A) is too big a bite. Furthermore, we can use
	21^1 to accomplish the same thing with coeff of z (26*26 again). This approach also reduces number of special cases arising from
	haveing two different ways to get 26*26. This pattern also holds for 26^0 being good for 2 digits. Anything higher than exp = 3 is def
	good enough for more than three digits.*/
	
	/*What exponent should we start with? Find the "bliss point" and drop down by 1. Bliss point is the exponent that only
	has one viable bite: when the coefficient is A. Having a coeff of 1 is the same as bumping down to the next exp and
	having a coeff of 26. With A the expression is less concise, so we choose the smaller exp. 
	
	In summary, find the bliss point and bump down by 1.*/
	
	//4 is the maximum exponent, 26^5 > 10^6. this loop finds the bliss point.
	int exponent = 4; 
	
	int letter = 26; //26 = z ... 1 = a. start at z

	int charbuf; //used in special cases, see below
	int test;
	/*Again we're taking a "convert between different bases" route, this time from base 10 to base 26. Take
	as large a bite out of our base 10 total using base 26 as possible, without going over. Repeat this with 
	whats left over, then again... until our original base 10 number, colsInt, is gone.*/
	while (colsInt != 0) {
		rerunCurrExponent:
		while (colsInt - (pow(26.0, exponent) * letter) < 0) { //if we've taken too big a bite, take a step back. first way we can step back is by shrinking the coefficient.
		test = colsInt - (pow(26.0, exponent) * letter);
			letter--;
			if (letter == 0) { 
				break;			
			}
		}

		/*Test for our two special cases. First case is that we find a coeff that works but we're not left with a remainder to pass on.
		The other case is we can't find a coefficient that works. The way to fix both these problems is the same: decrement the coefficient.
		If this is not allowable because the coefficient is already as small as possible, ie 'A', or we have tried all the possibile coefficients
		and none of them work, we decrement the coefficient tied to the previous exponent. Note that at this point we still
		haven't recorded anything for the current exponent so we don't need to worry about deleting anything from the result. Also note that
		it is possible that the coefficient of the previous exponent is also 'A', in which case we can't decrement it. In this case we have to
		remove it, and move along to the next exponent. If we get all the way to exponent=4 and we encounter this problem we have no choice but to
		decrement the exponent and to start the result from the smallest exponent, 3.*/

		if (exponent != 0 && (colsInt - (pow(26.0, exponent) * letter) == 0) || letter == 0) {
			if (letter != 0 && letter != 1) letter--; //if the letter isn't 'A', or we haven't run through all the letters, we can simply decrement
			else {//if it is 'A', we need to roll back the exponent and decrement the corresponding coefficient. Same thing if letter == 0 and we've tried every coefficient for the current exponent.
				if (rslt.size() != 0){ //If the rslt string isn't empty we can backtrack. If it is empty we decrement the exponent and try at the next level down.
					
					do{
						if (rslt.size() == 0) break;
						exponent++;
						charbuf = (int)rslt[rslt.size() - 1];
						charbuf--;
						if (charbuf == 64) { //remove the 'A'
							rslt.resize(rslt.size() - 1);
							colsInt += pow(26.0, exponent);
						}
					} while (charbuf == 64);
						
					//remove the pre-incremented letter
					if (rslt.size() != 0){
						rslt.resize(rslt.size() - 1);
						colsInt += pow(26.0, exponent);

						//add the newly incremented letter
						rslt += (char)charbuf;
					}

					//reset, prepare to run the exponent loop again for the next exponent in the series
					exponent--; 
					letter = 26;
					goto rerunCurrExponent;
					
				
				}
				else {
					exponent--;
					letter = 26;
					goto rerunCurrExponent;
				}
			}
			
			//forcibily reduce bite size. consider 494: when exp is 1 and letter is S we would have no remainder (26*19=494). Without special
			//consideration, we would except S as our answer. This would work if we could say S0, but there is no 0 in our special bas 26 rep. S is for
			//column 19, not column 294. To fix this, reduce bite size and pass along the remainder.

		}

		colsInt = colsInt - ((int)pow(26.0, exponent) * letter);
		//record the corresponding letter
		letter += 64; //get the ascii code
		rslt += char(letter);
		//prepare for next phase
		letter = 26;
		exponent--;
	}
	std::stringstream sstm;
	sstm << rslt << rows;
	rslt = sstm.str();
	return rslt;
}