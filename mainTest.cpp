/*
 * =====================================================================================
 *
 *       Filename:  mainTest.cpp
 *
 *    Description:  File to help test individual pieces of functionality before
 					using them in the main program
 *                  
 *
 *       Submitted: 13-Aug-2015
 *       Compiler:  g++
 *
 *         Author:  Kiran Kumar
 						kk2916@nyu.edu
 						N16979338
 *
 * =====================================================================================
 */

//Purpose: to modularly test functionality from linked classes/files

#include "phonemes.h"
#include "FIR-filter-class/filt.h"
#include <iostream>

using namespace std; 

int main()
{

	// Phoneme::Phoneme my_phoneme("[AH]", 730.0, 1110.0, 2450.0);

	// for (int i = 0; i < NUM_FORMANTS; i++)
	// {
	// 	printf("Formant %d frequency: %.2f\n", 
	// 		i + 1, my_phoneme.getFormant(i)->getFrequency() * 1000.0);
	// }

	string str1 = "What's up. F00!!";
	printf("Period found at position %d\n", (int)str1.find('.'));
	printf("String 2 beyond the period is %s\n", str1.substr(0, (int)str1.find('.') + 3).c_str());

	return 0;
}