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
		backtrack:
		while (colsInt - (pow(26.0, exponent) * letter) < 0) { //if we've taken too big a bite, take a step back. first way we can step back is by shrinking the coefficient.
		test = colsInt - (pow(26.0, exponent) * letter);
			letter--;
			if (letter == 0) { //0 does not exist in our number system, we can't insert it. what we have to do instead is backtrack and take the next smallest possible letter...				
				letter = 26;//reset our stuff... don't decrement exponent as we will be running it again after we backtrack
				if (rslt.size() != 0) {//and backtrack, assuming we can
					
					if (charbuf = (int)rslt[rslt.size() - 1] == 65) {
						//if we've run an entire exponent without finding a coeff, that means prev coeff is A.
						do {
							rslt.resize(rslt.size() - 1);//chop off the offending A
							colsInt += pow(26.0, exponent + 1);
							charbuf = (int)rslt[rslt.size() - 1];
							charbuf--;
							exponent++;
						} while (charbuf == 64);

						//if charbuf=64, we have a problem. We've backtracked, but next highest exp coeff is A. Can't decrem A, its lowest coeff. Soln is to decrem the coeff at the level above this one.
						rslt.resize(rslt.size() - 1);
						//26^2*1 is equiv to 26^1 * 26. In cases like this we take the smaller exponent, if we don't
						//we will go through the smaller exponent without getting a result, effecively inserting a blank
						rslt += (char)charbuf;
						colsInt += pow(26.0, exponent + 1);//return the values we took from colsInt to complete the backtrack. exp+1 because we decremented at line 195
						//if we are trying to decrement an "A", we need a different approach. We can't decrement it, so we have to decrement the exponent and choose Z (it's equivalent).
						//then we need to decrement exponent again, as there is no need to run the loop for the "Z" exponent. We already know we will be choosing Z as our coefficient.
					}
				}
				else exponent--;
			}
			
		}
		//if we're here, we've found an exponent/letter combo that doesn't take too big a bite. Deal with special cases, then 
		//start the process again with the remainder

		//test for special cases: what if we don't have a remainder to pass along? can't pass along 0, we can't represent it!
		if (exponent != 0 && (colsInt - (pow(26.0, exponent) * letter) == 0)) {
			letter--;
			if (letter == 0) {//if the problem coefficient is A, we have a special case. Can't just decrement letter, that would make it '@'. Soln
				//is to go back up a level and decrement the letter. In rare cases, we might encounter a string of 'A's as we backtrack. We'd need to backtrack
				//out of the As, which may take mutliple steps. In a word, backtrack until we can take a smaller bite.
				
				if (exponent != 4){
					exponent++;
					//if we're subtracting from A, can't just make it Z! Need to decrement the previous coefficient and run the current exponent loop again
					if (rslt.size() != 0) {
						
						charbuf = (int)rslt[rslt.size() - 1];//decrement the previous exponent's coefficient, then take a run at the current exponent again (with th new remainder)
						charbuf--;
			
						rslt.resize(rslt.size() - 1);

						rslt += (char)charbuf; //attach the newly decremented coefficent

						colsInt += pow(26.0, exponent);//add the total we subtracted in error prior to the backtrack
						
						exponent--; //get back to the current exponent
						letter = 26;//reset
						goto backtrack;//and run the loop
					}
					else {
						exponent--;
						letter = 26;
						goto backtrack;
					}
				}
				else {//if exponent 4 for there is no higher level to backtrack to. Here we simply ignore exponent of 4 and move on to the next smaller one.
					exponent--;
					letter = 26;
					goto backtrack;
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