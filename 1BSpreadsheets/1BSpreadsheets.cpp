#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <ctype.h>

using namespace std;
/*This program accepts a series of row/column coordinates in one of two formats. It converts each inputted coordinate from one format to the other
then outputs the results. The two coordinate formats will be known at type A and type B.

Type A ex: AB12 : AB=column, 12 = rows. Type B ex: R1C2: 1 is rows, 2 is columns.*/

int id(string entry); //determines which category the argument coordinate belongs to
string convertToA(string entry);//convert to type A from type B
string convertToB(string entry);

int main()
{
	string s;
	unsigned int numItems;
	int entryType; //stores the return values of the id() function. 0 for A 1 for B

	//get number of items
	getline(cin, s);
	numItems = stoi(s);

	vector<string> output;

	//get items
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
	/*we identify an entry as type B by determining that firstIsR is true, scndIsLtr is false, and foundC is true. 
	A truth table shows that they are all we need.
	return 0 = A and 1 = B*/

	bool firstIsR=false, scndIsLtr=false, foundC=false;
	vector<char> chars(entry.begin(), entry.end());

	if (chars[0] == 'R') {
		firstIsR = true;
	}
	else {
		return 0;
	}

	if (isalpha(chars[1])) return 0;

	for (vector<char>::iterator i = chars.begin();
		i != chars.end();
		++i)
	{
		if (*i == 'C') return 1;//if we're here firstIsR is true, scndIsLtr is false, and foundC is true. Report type B.
	}
	return 0;
}

string convertToB(string orig) {
	//going from XY## to R#C#
	string rslt = "R";//if we're coverting to B from A, we know that the first character will be R, since the format is R#C#
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
	//finPos now contains the number of letters (and the index of the final letter). Use this number to control the loop that processes the letters
	int total = 0;
	int exponent = 0;
	
	/*Basic idea is to view the string of letters as written in base 26, with the caveat that we start counting from 1 instead of
	0 (ie numbers in our base 26 system are 1-26 not 0-25). When viewed in this light it' almost simple base 26 to base 10 conversion.*/
	
	for (int i = finPos; i != -1; --i) {//work backwards from the final letter, right to left.
		total += (chars[i]-64) * pow(26.0,exponent);//(chars[i]-64)converts from ascii code to base 26 coefficient, eg 65=1 -> 1=A. then multiply by appropriate power of 26
		exponent++;
	}

	rslt += "C";

	//add total to the rslt string using stringstreams (works a bit like file i/o streams).
	std::stringstream sstm;
	sstm << rslt << total;
	rslt = sstm.str();

	return rslt;
}

string convertToA(string orig) {
	/*Converting from R#C# to XY##*/
	string rslt;
	string rows, cols;
	vector<char> chars(orig.begin(), orig.end());

	//isolate the integers representing rows and columns
	int i = 1;//skip the R
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

	/*Again we're taking a "convert between different bases" route, this time from base 10 to base 26. Take
	as large a bite out of our base 10 total using base 26 as possible, without going over. Repeat this with
	whats left over, then again... until our original base 10 number, colsInt, is gone.*/
	int colsInt = stoi(cols);
	int exponent = 4;//exponent is used to size our bites. A bite is always of the form coeff*26^exponent. 4 is the largest exponent
	//we will could possibly need (since column int is > 10^6).
		
	int letter = 26; //26 = z ... 1 = a. start at z

	int charbuf; //used to test for special cases, see below
	
	//start taking bites
	while (colsInt != 0) {//while the remainder!=0
		rerunCurrExponent:
		while (colsInt - (pow(26.0, exponent) * letter) < 0) { //if we've taken too big a bite, take a step back. first way we can step back is by shrinking the coefficient.
			letter--;
			if (letter == 0) { //if we've tried all coefficients, its a special case. We deal with it below.
				break;			
			}
		}

		/*Test for our two special cases. First case is that we find a coeff that works but we're not left with a remainder to pass on.
		We require a remainder to pass on. Since we have no 0 in our notation system, we must always pass on a remainder
		unless we are at the last step (when the exponent is 0).
		The other case is we can't find a coefficient that works. The way to fix both these problems is the same: decrement the coefficient.
		If this is not allowable because the coefficient is already as small as possible, ie 'A', or we have tried all the possibile coefficients
		and none of them work, we decrement the coefficient tied to the previous exponent. Note that at this point we still
		haven't recorded anything for the current exponent so we don't need to worry about deleting anything from the result for the current exponent. Also note that
		it is possible that the coefficient of the previous exponent is also 'A', in which case we can't decrement it. In this case we have to
		remove it, and move along to the next-earliest exponent. If we get all the way to exponent=4 and we encounter this problem we have no choice but to
		decrement the exponent and to start the result from the smallest exponent, 3.*/

		if (exponent != 0 && (colsInt - (pow(26.0, exponent) * letter) == 0) || letter == 0) {
			if (letter != 0 && letter != 1) letter--; //if the letter isn't 'A', or we haven't run through all the letters, we can simply decrement the coeff
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