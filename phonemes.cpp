/*
 * =====================================================================================
 *
 *       Filename:  phonemes.cpp
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

#include "phonemes.h"

using namespace std;

 /*
	Class:		Formant
	Purpose:	Hold filter data for a single formant frequency
 */

/***** Constructors *****/
Formant::Formant()
{
	frequency = 0;
	formantFilter = NULL;
}

Formant::Formant(float freqValue, float bandwidth)
{
	//freqValue and bandwidth are in Hz! The constructor converts them to
	//kHz in order to work with the Filter class

	frequency = freqValue/1000.0;
	formantFilter = new Filter(BPF, 50, (float)(SAMPLE_RATE/1000.0), 
		frequency - bandwidth/2000.0, frequency + bandwidth/2000.0);

}

/***** Accessors *****/
float Formant::getFrequency() { return frequency; }
Filter* Formant::getFormantFilter() { return formantFilter; }

/***** Mutators *****/
void Formant::setFilter(Filter *filterObject)
{
	formantFilter = filterObject;
}

/*******************************************************************************/

 /*
	Class:		Phoneme
	Purpose:	Hold multiple formant filters to 
				comprise a single phoneme sound
 */

/***** Constructors *****/
Phoneme::Phoneme(string desc, int numberOfFormants, Formant **formBuffer)
{
	name = desc;
	formants = formBuffer;
}

Phoneme::Phoneme(string desc, float freq1, float freq2, float freq3)
{
	name = desc;
	formants = new Formant*[NUM_FORMANTS];
	formants[0] = new Formant(freq1, BW_FORMANT_1);
	formants[1] = new Formant(freq2, BW_FORMANT_2);
	formants[2] = new Formant(freq3, BW_FORMANT_3);
}

/***** Accessors *****/
string Phoneme::getName()
{
	return name;
}

Formant* Phoneme::getFormant(int index)
{
	return formants[index];
}

Formant** Phoneme::getFormantBuffer()
{
	return formants;
}

/***** Mutators *****/
void Phoneme::setFormant(int index, Formant* formantObject)
{
	formants[index] = formantObject;
}