/*
 * =====================================================================================
 *
 *       Filename:  phonemes.h
 *
 *    Description:  Collection of phoneme data for voice synthesizer
 					Advanced Acoustics Final Project
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

#define SAMPLE_RATE			44100

//Only dealing with the first three formants
#define NUM_FORMANTS		3
#define	BW_FORMANT_1		50
#define	BW_FORMANT_2		80
#define	BW_FORMANT_3		100

#include <string>
#include "FIR-filter-class/filt.h"

using namespace std;


 
 /*
	Class:		Formant
	Purpose:	Hold filter data for a single formant frequency
 */
 class Formant
 {
 	private:
 		float frequency;
 		Filter* formantFilter;

 	public:

 		/***** Constructors *****/
 		Formant();
 		Formant(float freqValue, float bandwidth);

 		/***** Accessors *****/
 		float getFrequency();
 		Filter* getFormantFilter();

 		/***** Mutators *****/
 		void setFilter(Filter *filterObject);
 };

 /*
	Class:		Phoneme
	Purpose:	Hold multiple formant filters to 
				comprise a single phoneme sound
 */
 class Phoneme
 {
	 private:
	 	string name;
	 	Formant **formants;

	 public:
	 	
	 	/***** Constructors *****/
	 	Phoneme(string desc, int numberOfFormants, Formant **formBuffer);
	 	Phoneme(string desc, float freq, float freq2, float freq3);

	 	/***** Accessors *****/
	 	string getName();
	 	Formant* getFormant(int index);
	 	Formant** getFormantBuffer();

	 	/***** Mutators *****/
	 	void setFormant(int index, Formant* formantObject);
};

/***** Other methods *****/

// void initPhonemes(Phoneme*** phonBuffer);