/*
	TO COMPILE IN WINDOWS (MinGW): compile.bat
	
	cd /d %~dp0
	g++ -Wall -std=c++11 pagereplace.cpp -o pagereplace
	PAUSE
*/

// Include libraries.
#include <algorithm>  // for copy_if()
#include <cctype>
#include <cmath>
#include <cstdlib> /* srand, rand */
#include <ctime>   /* time */
#include <functional>
#include <iostream>
#include <iterator>   // for the back_inserter
#include <list>
#include <locale>
#include <memory> /* smart pointers */
#include <sstream>
#include <stdexcept>  /* required for defining and catching standard exceptions */
#include <string>
#include <vector>

// Class(es).
//================================================================================================================
// This is the BASE/PARENT class where most of the data members for each algorithm are stored.
// It's best not to change anything in this class, as it's what all objects rely on for their functionality!
class algorithmType
{
	private:              // it's best practice to declare member variables here, directly in the private section of object.
		std::string name; // Initialize all non-static variables in initialization list and all static variables directly below object.
		unsigned int row; // Row of each objects' two dimensional, algorithm vector. Unknown until the end of each calculation.
		unsigned int miss; // Variable that stores each page fault that's found.
	    unsigned int myid; // "myid" assigns each object created with it's own identification number (starts at 1).
		// Set as static because only one instance is to be used for all objects.
		static unsigned int maxid; // Ensures if an object is deleted that no identical id is assigned to a new object.
		static unsigned int frame; // Keeps track of the number of frames. Stored as a column.
		static unsigned int numobj; // Keeps track of how many objects of this class were created.
		std::vector<int> currentcashline;
		std::vector<std::vector<int>> alg;

	public:
	    algorithmType() : algorithmType("<N/A>"){} // Delegating constructor (C++11 feature).
		algorithmType(std::string pname) : name(pname), row(0), miss(0) // never rely on non-member functions to initialize member variables.
		{
		 numobj++; // static variables were already initialized below this object, so now they can modify their values.
		 maxid++;
		 myid = maxid; // this non-static member variable can't be initialized in initialization list because "maxid" is not allowed there.
		}

		// Destructor.
		// It's good practice to make the destructor virtual in the case that a pointer object is destroyed,
		// there is no undefined behavior.
		virtual ~algorithmType() 
		{
			if (numobj > 0)
			{numobj--;}
		}

		// Pure virtual function that NEEDS to exist within every object of this class, as every object of this
		// class has that member function called for it automatically. Plus, without it, there would be nothing to display!
		virtual void calculateAlgorithm(const int& refstrcount, const std::vector<int>& refstr) = 0;

		virtual void setMiss (unsigned int smiss) final
		{miss = smiss;}

		// The "final" specifier (along with the needed "virtual" keyword) work together to ensure these functions are NEVER overridden!
	    virtual void setRow (unsigned int srow) final
		{row = srow;}

		// Returning member by reference.
		// This breaks encapsulation, but it saves space and it's the most important member, which defines the use of this entire program.
		virtual std::vector<std::vector <int>> &setVector () final
		{return alg;}

		// This function is static so it may be called without an object, as it applies to all objects.
		static void static_setFrameFinalSize (unsigned int sframe)
		{frame = sframe;}

		virtual std::vector<int> &setCurrentCacheLine () final
		{return currentcashline;}

		virtual unsigned int getFrameFinalSize () final
		{return frame;}

		virtual unsigned int getMiss () final
		{return miss;}

		virtual unsigned int getMyId () final // Every object created will have it's own unique I.D., accessible to view via this method.
		{return myid;}

		virtual const std::string &getName() final
		{return name;}

		virtual unsigned int getRow () final
		{return row;}

		virtual const std::vector<std::vector <int>> &getVector () final
		{return alg;}
		
		static unsigned int static_getFrameFinalSize ()
		{return frame;}

		static unsigned int static_getNumObj () // This function is static so the number of objects that exist can be displayed without the need of any one specific object.
		{return numobj;}

		virtual void clearAlg () final // This clears the vector's contents and size to zero.
		{
			for(std::vector<int> &i : alg)
			{
			 i.clear();
			 std::vector<int>().swap(i);
			}
		 alg.clear();
		 std::vector<std::vector<int>>().swap(alg);
		}

		virtual void displayAlgorithm() // All 3 algorithms can be displayed in the same manner using this member function. If not, override it in derived class.
		{
		 std::cout << std::endl << name << " page faults: " << getMiss() << std::endl;
			for (unsigned int a = 0; a < getFrameFinalSize(); a++) // this display method displays this multidimensional vector inversely to match textbooks etc.
			{
				for (unsigned int i = 0; i < getVector().size(); i++) // 'getVector().size()' is the same as 'getRow()'.
				{
					// getFrameFinalSize() doesn't account for the reference string possibly not having as many unique numbers
					// as there are frames, which was calculated in the 'getInput' function from the user's input.
					if (a < getVector()[i].size())
					{
					 std::cout << getVector()[i][a];
						if ((i + 1) < getVector().size())
						{
						 std::cout << " ";
						}
					}
				}
				// This works just fine, but had this been a jagged array/vector, we would want to instead make sure that 'a',
				// which is the outer loop, is less than the BIGGEST 'getVector()[i].size()' from the inner loop.
				if ((a + 1) < getVector()[0].size())
				{
				 std::cout << std::endl;
				}
				else
				{
				 // We know that if the row we're currently checking (indexed by 'a') is equal to the maximum column size,
				 // because it's zero indexed, then there won't be anything left in the column ('getVector()[0].size()').
					
					// Not really necessary, but without this check, this conditional statement would explicitly break out
					// of the outer loop on the last iteration, every single time, and not just when needed.
					if ((a + 1) < getFrameFinalSize())
					{
					 break;
					}
				}
			}
		}

		virtual int fillFirstCacheLine(const std::vector<int>& refstr, const int& refstrcount) final
		{
		 setCurrentCacheLine().clear();
		 std::vector<int>().swap(setCurrentCacheLine());
		 int currentrefstr = 0;
			for (int i = 0; i < refstrcount; i++) // we're looping through entire reference string just in case there are repeating numbers at beginning
			{
				if (std::find(setCurrentCacheLine().begin(), setCurrentCacheLine().end(), refstr[i]) == setCurrentCacheLine().end()) // As long as no duplicate pages were found in the first frame...
				{
				 setMiss(getMiss() + 1); // page miss, then decide what to do next depending on the algorithm.
				 setCurrentCacheLine().push_back(refstr[i]);
				}
				if (setCurrentCacheLine().size() >= getFrameFinalSize() || (i + 1 >= refstrcount)) // create first line of cache now that all pages have been added to all frames.
				{
				 currentrefstr = i + 1;
				 setRow(getRow() + 1);
				 setVector().push_back(setCurrentCacheLine()); // add finished frames to the cache.
				 break;
				}
			}
		 return currentrefstr;
		}

};

// Initializing the static variable 'row', but this is, and can ONLY be initialized once, which it is here.
unsigned int algorithmType :: frame = 0; // This can NOT be less than zero because variable "fcount" in function "calculateFifo" is an unsigned int.
unsigned int algorithmType :: numobj = 0; // No objects of this class have been created yet.
unsigned int algorithmType :: maxid = 0; // No objects of this class have been created yet.

// This is a DERIVED/CHILD class, signified by inheriting from class "algorithmType".
// For every new algorithm added, a new derived class that inherits "algorithmType" needs to be added.
// The only entity that needs to be added to these classes is the member function "calculateAlgorithm".
// This is because calculating each algorithm is unique to each algorithm. And any other unique functionality
// that these algorithms have needs to be added to it's specific (derived) class.
class Fifo : public algorithmType
{
 public:
 Fifo(){} // Default Constructor
 Fifo(std::string pname) : algorithmType(pname){} // Overloaded Constructor. Used if object was assigned a name when it was initialized.

	virtual void calculateAlgorithm(const int& refstrcount, const std::vector<int>& refstr) override // Required for all derived classes of "algorithmType".
	{
	 unsigned int fcount = 0; // keeps track of index of the first page added to cache.
	 int start = fillFirstCacheLine(refstr, refstrcount); // fill in first cache line of frames
		for (int i = start; i < refstrcount; i++)
		{
			if (std::find(setCurrentCacheLine().begin(), setCurrentCacheLine().end(), refstr[i]) == setCurrentCacheLine().end()) // If nothing in current cache of frames matches the current element in reference string...
			{
			 setMiss(getMiss() + 1); // page miss, then decide what to do next depending on the algorithm.
			 setCurrentCacheLine()[fcount] = refstr[i]; // replace next frame in the count with the current reference string.
			 ((fcount + 1 < getFrameFinalSize()) ? fcount++ : fcount = 0);
			 setRow(getRow() + 1);
			 setVector().push_back(setCurrentCacheLine()); // add finished frames to the cache.
			}
		}
	}

};

class Lru : public algorithmType
{
 public:
 Lru(){} // Default Constructor
 Lru(std::string pname) : algorithmType(pname){} // Overloaded Constructor. Used if object was assigned a name when it was initialized.

	virtual void calculateAlgorithm(const int& refstrcount, const std::vector<int>& refstr) override // Required for all derived classes of "algorithmType".
	{
	 // This algorithm uses the "stack" method, as it requires less code and is easier to understand. However, it is slightly slower than "counters".
	 std::list<int> lst;
	 int start = fillFirstCacheLine(refstr, refstrcount);  // fill in first cache line of frames
	 std::copy(setCurrentCacheLine().begin(), setCurrentCacheLine().end(), std::back_inserter(lst));
		for (int i = start; i < refstrcount; i++)
		{
			if (std::find(setCurrentCacheLine().begin(), setCurrentCacheLine().end(), refstr[i]) == setCurrentCacheLine().end()) // If nothing in current cache of frames matches the current element in reference string...
			{
			 setMiss(getMiss() + 1); // page miss, then decide what to do next depending on the algorithm.
			 std::replace(setCurrentCacheLine().begin(), setCurrentCacheLine().end(), lst.front(), refstr[i]);
			 lst.pop_front();
			 setRow(getRow() + 1);
			 setVector().push_back(setCurrentCacheLine()); // add finished frames to the cache.
			}
			else
			{
			 lst.erase(std::find(lst.begin(), lst.end(), refstr[i]));
			}
		 lst.push_back(refstr[i]);
		}
	}
 
};

class Opt : public algorithmType
{
 public:
 Opt(){} // Default Constructor
 Opt(std::string pname) : algorithmType(pname){} // Overloaded Constructor. Used if object was assigned a name when it was initialized.

	virtual void calculateAlgorithm(const int& refstrcount, const std::vector<int>& refstr) override // Required for all derived classes of "algorithmType".
	{
	 int maxindex = 0, maxdistance = 0, currentdistance;
	 std::vector<int>::const_iterator cnst_itr_pos;
	 int start = fillFirstCacheLine(refstr, refstrcount);  // fill in first cache line of frames
		for (int i = start; i < refstrcount; i++)
		{
			if (std::find(setCurrentCacheLine().begin(), setCurrentCacheLine().end(), refstr[i]) == setCurrentCacheLine().end()) // If nothing in current cache of frames matches the current element in reference string...
			{
			 setMiss(getMiss() + 1); // page miss, then decide what to do next depending on the algorithm.
			 maxdistance = 0; // must reset the max value found for each new iteration of the reference string
				for (unsigned int c = 0; c < getFrameFinalSize(); c++) // loop through each frame in the current cache
				{
				 // even though "(refstr.begin() + i)" is the current element in the reference string (refstr), it is guaranteed not to be in the current cache of frames (setCurrentCacheLine())
				 // at this point, so including "(refstr.begin() + i)" will NOT return any false positive distances of zero
					 
				 // get position of where the first occurrence of the current frame is in the reference string (refstr)
				 cnst_itr_pos = std::find ((refstr.begin() + (i)), refstr.end(), setCurrentCacheLine()[c]);
				 currentdistance = std::distance((refstr.begin() + (i)), cnst_itr_pos); // gets the distance of the next number in the reference string (refstr) that matches the current element in the cache (setCurrentCacheLine())
					if (i < (refstrcount - 1)) // If the main loop (iterating through the reference string) is NOT at the last element yet... (last element MIGHT need handled differently)
					{
						if (cnst_itr_pos == refstr.end()) // if current element in setCurrentCacheLine() is NOT in any future element of refstr...
						{
						 setCurrentCacheLine()[c] = refstr[i]; // the number in "setCurrentCacheLine()[c]" was not found anywhere else in the reference string, first, so it is selected to store the current element of the reference string (refstr)
						 setRow(getRow() + 1);
						 setVector().push_back(setCurrentCacheLine()); // add finished frames to the cache.
						 break; // break out of looping through each frame in the cache line, since we are changing a frame
						}
							if (currentdistance > maxdistance) // if distance of current element of setCurrentCacheLine() in reference string is greater than the max distance so far...
							{
							 maxdistance = currentdistance; // if there are two elements with same max distance, the first element that had it is chosen
							 maxindex = c;
							}
								if (c >= (getFrameFinalSize() - 1)) // if this loop IS at the last element of the cache line (setCurrentCacheLine())...
								{
								 setCurrentCacheLine()[maxindex] = refstr[i];
								 setRow(getRow() + 1);
								 setVector().push_back(setCurrentCacheLine()); // add finished frames to the cache.
								 // no need to 'break' out of for loop because we're at the last element anyway
								}
					}
					else // at the last element of reference string... simply replaces first element in cache line (setCurrentCacheLine()) with the last element in the reference string (refstr)
					{
					 setCurrentCacheLine()[0] = refstr[i];
					 setRow(getRow() + 1);
					 setVector().push_back(setCurrentCacheLine()); // add finished frames to the cache.
					 break;
					}
				}
			}
		}
	}

};

class Opt_Fifo : public algorithmType
{
 public:
 Opt_Fifo(){} // Default Constructor
 Opt_Fifo(std::string pname) : algorithmType(pname){} // Overloaded Constructor. Used if object was assigned a name when it was initialized.

	virtual void calculateAlgorithm(const int& refstrcount, const std::vector<int>& refstr) override // Required for all derived classes of "algorithmType".
	{
	 int maxindex = 0, maxdistance = 0, currentdistance, nomatch, fifoindex, fifodistance, fifomindistance;
	 std::vector<int>::const_iterator cnst_itr_pos;
	 std::vector<int>::const_iterator cnst_itr_match;
	 int start = fillFirstCacheLine(refstr, refstrcount);  // fill in first cache line of frames
		for (int i = start; i < refstrcount; i++)
		{
			if (std::find(setCurrentCacheLine().begin(), setCurrentCacheLine().end(), refstr[i]) == setCurrentCacheLine().end()) // If nothing in current cache of frames matches the current element in reference string...
			{
			 setMiss(getMiss() + 1); // page miss, then decide what to do next depending on the algorithm.
			 maxdistance = 0; // must reset the max value found for each new iteration of the reference string
			 nomatch = 0; // reset so new frames with no match in reference string (refstr) start fresh for each iteration of the reference string
				for (unsigned int c = 0; c < getFrameFinalSize(); c++) // loop through each frame in the current cache
				{
				 // even though "(refstr.begin() + i)" is the current element in the reference string (refstr), it is guaranteed not to be in the current cache of frames (setCurrentCacheLine())
				 // at this point, so including "(refstr.begin() + i)" will NOT return any false positive distances of zero
					 
				 // get position of where the first occurrence of the current frame is in the reference string (refstr)
				 cnst_itr_pos = std::find ((refstr.begin() + (i)), refstr.end(), setCurrentCacheLine()[c]);
				 currentdistance = std::distance((refstr.begin() + (i)), cnst_itr_pos); // gets the distance of the next number in the reference string (refstr) that matches the current element in the cache (setCurrentCacheLine())
					if (cnst_itr_pos == refstr.end()) // if current element in setCurrentCacheLine() is NOT in any future element of refstr...
					{nomatch++;}
					else // current element in setCurrentCacheLine() IS in any future element of refstr...
					{
						if (currentdistance > maxdistance) // if distance of current element of setCurrentCacheLine() in reference string is greater than the max distance so far...
						{
						 maxdistance = currentdistance; // if there are two elements with same max distance, the first element that had it is chosen
						 maxindex = c;
						}
					}
				}
			 fifoindex = 0;
			 fifomindistance = refstrcount; // because refstrcount is always going to be 1 element passed the last element in refstr, we will know that the value has NOT changed yet
				if (nomatch > 1) //  if there were two or more elements of the cache line that were not found anywhere in the future of the reference string...
				{
					for (unsigned int d = 0; d < getFrameFinalSize(); d++) // at least 2 frames were not found in future refstr, so loop over frames again and use to Fifo to determine which frame comes first
					{
						if (std::find ((refstr.begin() + (i)), refstr.end(), setCurrentCacheLine()[d]) == refstr.end()) // only frames that had no match will be considered
						{
						 cnst_itr_match = std::find (refstr.begin(), (refstr.begin() + (i)), setCurrentCacheLine()[d]); // find position of the first occurence of the current frame inside of the reference string, starting from beginning
						 fifodistance = std::distance(refstr.begin(), cnst_itr_match); // find distance of recently found position of "cnst_itr_match" in refstr
							if (fifodistance < fifomindistance)
							{
							 fifomindistance = fifodistance;
							 fifoindex = d;
							}
						}
					}
				 setCurrentCacheLine()[fifoindex] = refstr[i];
				}
				else if (nomatch == 1)
				{
					for (unsigned int e = 0; e < getFrameFinalSize(); e++) // because there's only one frame that didn't have a match in refstr, we know that frame will be the last frame in the cache to be replaced next
					{
						if (std::find ((refstr.begin() + (i)), refstr.end(), setCurrentCacheLine()[e]) == refstr.end()) // once we find the one and only frame that has no match, we're done looking
						{
						 setCurrentCacheLine()[e] = refstr[i];
						 break; // break from searching the one and only frame with no match and go ahead with next element in the reference string
						}
					}
				}
				else // if "nomatch" is zero... then use the original max distance found between all frames in cache
				{
				 setCurrentCacheLine()[maxindex] = refstr[i];
				}
			 setRow(getRow() + 1);
			 setVector().push_back(setCurrentCacheLine()); // add finished frames to the cache.
			}
		}
	}

};

// Functions.
//================================================================================================================

// This function removes all whitespace completely.
//================================================================================================================
void remove_all_whitespaces(std::string& str)
{str.erase(remove_if(str.begin(), str.end(), [](char ch){return std::isspace<char>(ch, std::locale::classic());} ), str.end());}

// These functions trim the white spaces from either side of the input.
//================================================================================================================
// trim from start
std::string& ltrim(std::string& s)
{
 //s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char c) {return (bool)!(std::isspace(c));}));
 s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char ch){return (bool)!(std::isspace<char>(ch, std::locale::classic()));} ));
 return s;
}

// trim from end
std::string& rtrim(std::string& s)
{
 //s.erase(std::find_if(s.rbegin(), s.rend(), [](char c) {return (bool)!(std::isspace(c));}).base(), s.end());
 s.erase(std::find_if(s.rbegin(), s.rend(), [](char ch){return (bool)!(std::isspace<char>(ch, std::locale::classic()));} ).base(), s.end());
 return s;
}

// trim from both ends
std::string& trim(std::string& s)
{return ltrim(rtrim(s));}

// This function merges all multiple white spaces in a row, into one 'space', even if no 'space' character is found.
// For practical usability, this function allows ANY white spaces in a row ('space', tab, newline, etc.) to be used as a delimiter.
//================================================================================================================
void remove_extra_whitespaces(std::string& input) //, std::string &output)
{
 std::string output; // = "NO_OUTPUT_YET";
 output.clear();  // unless you want to add at the end of existing string, clear the output so it's ready to accept new input.
 //unique_copy (input.begin(), input.end(), std::back_insert_iterator<std::string>(output), [](char a,char b){ return std::isspace(a) && std::isspace(b);});
 unique_copy (input.begin(), input.end(), std::back_insert_iterator<std::string>(output), [](char a, char b){return std::isspace<char>(a, std::locale::classic()) && std::isspace<char>(b, std::locale::classic());} );
 replace_if(output.begin(), output.end(), [](char ch){return std::isspace<char>(ch, std::locale::classic());} , ' '); // makes all characters that are any type of whitespace into single 'space'.
 input = output;
}

// This function acts as "cin>>", but without the headache of having to worry about maintaining the input buffer.
// The use of "cin >>" requires "std::cin.clear()" and "std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n')"
// immediately after you use it. This method is cleaner, safer and more portable.
//================================================================================================================
void cin_custom(std::string& str)
{
 getline(std::cin, str); // Causes a newline to be created here as well.
 ltrim(str);
	for(unsigned int i = 0; i < str.length(); i++)
	{
		if (std::isspace(str[i]))
		{
		 str.erase(str.begin() + i, str.end());
		 break;
		}
	}
}

// This function takes care of setting up all objects for the next calculation by resetting certain values.
//================================================================================================================
void cleanUp (std::vector<std::shared_ptr<algorithmType>>& algvector)
{
	// Range based "for" loop that loops through each object of type "algorithmType" and resets everything for each new loop in "main".
	for (std::shared_ptr<algorithmType>& alg : algvector)
	{
	 alg->clearAlg();
	 alg->setRow(0);
	 alg->setMiss(0);
	}
}

// Function that uses a range based "for" loop to calculate every algorithm that was added to the vector "algvector".
//================================================================================================================
void calculateAllAlgorithms (std::vector<std::shared_ptr<algorithmType>>& algvector, const int& refstrcount, const std::vector<int>& refstr)
{
	for (std::shared_ptr<algorithmType>& alg : algvector)
	{alg->calculateAlgorithm(refstrcount, refstr);}
}

// Function that uses a range based "for" loop to display every algorithm that was added to the vector "algvector".
//================================================================================================================
void displayAllAlgorithms (std::vector<std::shared_ptr<algorithmType>>& algvector)
{
	for (std::vector<std::shared_ptr<algorithmType>>::iterator itr_pos = algvector.begin(); itr_pos != algvector.end(); ++itr_pos)
	{
	 (*itr_pos)->displayAlgorithm();
		if (std::next(itr_pos, 1) != algvector.end()) // Do not automatically put a space after the last algorithm output.
		{
		 std::cout << std::endl; // Space in between each algorithm output.
		}
	}
}

// This function is used by function "getInput" to prevent incorrect input from the user from crashing program.
//================================================================================================================
int strToInt(const std::string& str, int& i)
{
 int r_error = 0;
	try
	{
	 i = (int)std::trunc(stod(str)); // here is where you can change how to handle decimal numbers.
	                     // I elected to truncate any non integer type variables behind the scenes, without throwing an exception.
						 // Note, if you use 'stoi' instead, it will implicitly truncate any non integer type variable anyway.
	}
     catch(std::invalid_argument&)
	{
     // if no conversion could be performed
	 r_error = 1;
    }
    catch(std::out_of_range&)
	{
     // if the converted value would fall out of the range of the result type 
     // or if the underlying function (std::strtol or std::strtoull) sets errno 
     // to ERANGE.
	 r_error = 2;
    }
    catch(...)
	{
     // everything else
	 r_error = 3;
    }

 return r_error;
}

// This function gets the input from the user, which is the reference string and how many rows (frames) it should be.
//================================================================================================================
int getInput(int& refstrcount, std::vector<int>& refstr, const int& maxrefstrsize, const int& maxframesize)
{
 refstr.clear(); // this reference string is being cleared from the last inputted values, to accept the new reference string.
 std::vector<int>().swap(refstr); // also shrink down the capacity of vector for each new request, so it doesn't cause any memory problems.
 std::string line, selectionnumber;
 int numint, randnum, errorcount;
 refstrcount = 0; // initialization.
 
 numint = 0;
 std::cout << "Would you like to generate a random reference string (1) or to enter a reference string (2)?\n";
	while (numint != 1 && numint != 2)
	{
	 cin_custom(selectionnumber);
		if (strToInt(selectionnumber, numint) != 0)
		{std::cout << "This is not a valid input. Please select either \"1\" or \"2\":\n";}
		else 
		{
			if (numint != 1 && numint != 2)
			{std::cout << "The number you selected was not \"1\" or \"2\". Please try again:\n";}
		}
	}
	
	if (numint == 1)
	{
	 numint = 0;
	 std::cout << "\nPlease enter the number of reference string elements (between 1 and " << maxrefstrsize << ")\n";
		while (numint < 1 || numint > maxrefstrsize)
		{
		 cin_custom(selectionnumber);
			if (strToInt(selectionnumber, numint) != 0)
			{std::cout << "This is not a valid input. Please select number between \"1\" and \"" << maxrefstrsize << "\":\n";}
			else
			{
				if (numint < 1 || numint > maxrefstrsize)
				{std::cout << "The number you selected was not between \"1\" and \"" << maxrefstrsize << "\". Please try again:\n";}
			}
		}
	 refstrcount = numint;
	 std::srand((unsigned int)std::time(NULL));
	 std::cout << std::endl;
		for (int i = 0; i < refstrcount; i++)
		{
		 randnum = std::rand() % 10;
		 refstr.push_back(randnum);
		 std::cout << refstr[i] << " ";
		}
	 std::cout << "\n\n";
	}
	
	else // numint = 2
	{
	 errorcount = 1; // Temporarily set to one so while loop works, without the need of a do-while loop.
		while (errorcount)
		{
		 refstr.clear(); // this reference string is being cleared from the last inputted values, to accept the new reference string.
		 std::vector<int>().swap(refstr); // also shrink down the capacity of vector for each new request, so it doesn't cause any memory problems.
		 refstrcount = 0;
		 errorcount = 0; // Now that we're inside the while loop, we start with zero errors.
		 std::string num;
		 std::cout << "Enter numbers separated by spaces:\n\n";
		 getline(std::cin, line);
		 remove_extra_whitespaces(line);
		 trim(line);
		 std::stringstream  lineStream(line);
			while (getline(lineStream, num, ' ')) // the 'space' is the delimiter
			{
				if (strToInt(num, numint) != 0)
				{
				 errorcount++;
				 std::cout << "\"" << num << "\"" << " is not an integer!\n";
				}
				else
				{
					if (num.size() > 4) // Number is most likely too big to fit in an 'int'.
				    {
					 errorcount++;
					 std::cout << "\"" << num << "\"" << " is too big!\n";
				    }
					else
					{
					    if (numint >= 0 && numint <= 9)
					    {
						 refstr.push_back(numint); // place next number on the back of the stack, which will be used in alogoithm calculations.
						 refstrcount++;
					    }
					    else
					    {
						 errorcount++;
						 std::cout << "\"" << numint << "\"" << " is out of range! Number must be between 0-9.\n";
					    }
					}
				}
			}
			if (refstrcount < 1 || refstrcount > maxrefstrsize)
			{
			 errorcount++;
			 std::cout << "The amount of reference strings you entered was not between \"1\" and \"" << maxrefstrsize << "\". Please try again:\n";
			}
			if (errorcount)
			{std::cout << "There were " << errorcount << " errors! Please try again!\n";}
		}
	}
 
 numint = 0;
 std::cout << "\nPlease enter the number of the frames (between 1 and " << maxframesize << ")\n";
	while (numint < 1 || numint > maxframesize)
	{
	 cin_custom(selectionnumber);
		if (strToInt(selectionnumber, numint) != 0)
		{std::cout << "This is not a valid input. Please select number between \"1\" and \"" << maxframesize << "\":\n";}
		else
		{
			if (numint < 1 || numint > maxframesize)
			{std::cout << "The number you selected was not between \"1\" and \"" << maxframesize << "\". Please try again:\n";}
		}
	}
 // min() is used here just in case refstrcount (# of elements in reference string) is less than the number of frames in each cache line.
 algorithmType :: static_setFrameFinalSize(std::min(numint, refstrcount));
 return 0; // there were no problems with the number of reference strings.
}

//======================================== MAIN =============================================================MAIN=
//================================================================================================================
int main()
{
 std::vector<std::shared_ptr<algorithmType>> algvector;
 std::shared_ptr<algorithmType> fifo_obj (new Fifo("Fifo")); algvector.push_back(fifo_obj);
 std::shared_ptr<algorithmType> lru_obj (new Lru("LRU")); algvector.push_back(lru_obj);
 std::shared_ptr<algorithmType> opt_obj (new Opt("Optimal")); algvector.push_back(opt_obj);
 std::shared_ptr<algorithmType> opt_fifo_obj (new Opt_Fifo("Optimal with Fifo")); algvector.push_back(opt_fifo_obj);
 std::string runagainstr;
 int refstrcount; // Number of elements of the reference string
 int maxrefstrsize = 50, maxframesize = 7;
 std::vector<int> refstr; // The reference string that's used by the "calculate" functions that the user defined.
	while (runagainstr != "no")
	{
	 cleanUp (algvector); // On each iteration of this loop, each "calculate" function expects an empty vector. So clean everything up and reset values.
	 getInput (refstrcount, refstr, maxrefstrsize, maxframesize); // Get input from user.
	 calculateAllAlgorithms (algvector, refstrcount, refstr); // just calculate one: fifo_obj->calculateAlgorithm(refstrcount, refstr);
	 displayAllAlgorithms (algvector);                        // just display one:   fifo_obj->displayAlgorithm();
	 std::cout << "\n\nWould you like to run again?";
	 cin_custom(runagainstr);
	 std::transform(runagainstr.begin(), runagainstr.end(), runagainstr.begin(), ::tolower); // change entire string to lower case.
	}
 return 0;
}

/* This block of code can optionally be included inside class "main". It shows the simplicity, flexibility and power that you have, solely using "main".

// THIS MUST BE PLACED AFTER ALL OF THE SHARED POINTER DECLARATIONS OF EACH ALGORITHM CLASS!!!
 // std::cout << "Number of objects: " << algorithmType :: static_getNumObj() << std::endl; // Displays the number of objects in  the class "algorithmType".
 // std::cout << "Fifo's unique I.D. is " << fifo_obj->getMyId() << std::endl; // example of an object displaying it's own unique I.D.

 // JUST FOR FUN - Let's sum up the last frame (column) of all algorithms and display each summed row on one line, before we display each algorithm's output!
 // THIS MUST BE PLACED AFTER THE FUNCTION "calculateAllAlgorithms"!!!
	 //================================================================================================================
	 std::vector<int> temp_vector(algorithmType :: static_getFrameFinalSize(), 0); // create temporary vector to store the sum of the last column of each algorithm.
	 std::cout << std::endl << "Sum of last cache line: ";
		for (unsigned int i = 0; i < algorithmType :: static_getFrameFinalSize(); i++)
		{
			for (std::shared_ptr<algorithmType> alg : algvector)
			{temp_vector.at(i) += alg->getVector()[alg->getRow()-1][i];} // .at() function is NOT used anywhere else in program, to improve performance
		 std::cout << temp_vector.at(i) << " ";
		}
	 std::cout << std::endl;
	 //================================================================================================================
 */