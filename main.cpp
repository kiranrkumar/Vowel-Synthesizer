/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  Main driver for voice synthesizer
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

// Define important compile-time parameters
#define FRAMES_PER_BUFFER	128
#define NUM_IN_CHANNELS		0
#define NUM_OUT_CHANNELS	1 		//only need to worry about mono here
#define MONO 				1
#define STEREO				2
#define PI 3.14159265358979323846264338327950288
#define NUM_GENDER          2
#define NUM_PHONEMES        5

/*** Rosenberg Values ***/
#define GLOT_OPEN           0.25
#define GLOT_CLOSE          0.10

#include <math.h>
#include <time.h>
#include <portaudio.h>		// for audio output & processing
#include <ncurses.h>		// for user input
#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include "phonemes.h"
#include "FIR-filter-class/filt.h"

//Allow user to increase or decrease F0 by semitones
static int freqInc = -16; //start at a comfortable note for a male speaker - based on A440

//Global buffer of phonemes
//  phonemeBuffer[gender (0=male/1=female)][numPhoneme][Phoneme ptr]
Phoneme ***phonemeBuffer;


/****************************** IMPORTANT!! *************************************
*** Change NUM_PHONEMES and phon_enum whenever phonemes are added or removed! ***
*********************************************************************************/

enum phon_enum
{
    A,
    E,
    I,
    O,
    U,
    DEFAULT
};

enum gender
{
    MALE,
    FEMALE
};

//Indicates the currently selected phoneme and gender
static phon_enum whichSound = DEFAULT;
static gender curGender = MALE;

//PortAudio data struct (for audio callback)
typedef struct 
{
	float freq;
} 
paData;

//Use the Filter class's functionality to filter a sample in a signal
double calculateSample(Phoneme* currentPhoneme, double orgSample);


/*
    Function:       paCallback
    Purpose:        Audio callback (where the audio processing happens)
    Parameters:     <Defined by the PortAudio Library>
    Returns:        paContinue - if the callback succesfully finished
*/
static int paCallback( const void *inputBuffer,
        void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags, void *userData ) 
{
	
	float *out = (float *)outputBuffer; //create output buffer and cast to float*
	paData *data = (paData *)userData; //cast user data pointer to a PortAudio struct pointer

	//general processing vars
    static float sample = -1;
    static int period = 0;
    static int contSampleCount = 0;

    //Rosenberg glottal pulse vars
    static int glottalOpen = 0;
    static int glottalClose = 0;


    //Recalculate period, glottal opening, and glottal closure for every buffer
    period = (int)(SAMPLE_RATE / data->freq);
    glottalOpen = period * GLOT_OPEN;
    glottalClose = period * GLOT_CLOSE;

    /***** Iterate through each sample in the buffer *****/
	for (int i = 0; i < FRAMES_PER_BUFFER; i++)
	{

        /*** Rosenberg Glottal Pulse Calculation ***/

        if ( contSampleCount <= glottalOpen )
        {
           sample = 0.5 * (1 - cos(PI * contSampleCount / glottalOpen));
        }
        else if (contSampleCount <= (glottalClose + glottalClose) )
        {
           sample = cos(PI * (contSampleCount - glottalOpen) / (2 * glottalClose));
        }
        else
        {
            sample = 0;
        }

        /*** Filter sample with the appropriate phoneme ***/
        if (whichSound == DEFAULT)
        {
            out[i] = sample * 0.07;
        }
        else
        {        
            out[i] = (float)calculateSample(phonemeBuffer[(int)curGender][(int)whichSound], (double)sample);
        }
	
        contSampleCount = (contSampleCount + 1) % period;

    }
    //write out buffer samples into the output file
    //sf_writef_float(data->outfile, out, framesPerBuffer);

    return paContinue;
}

/*
    Function:       main
    Purpose:        Entry point for the program
    Parameters:     <None>
    Returns:        0
*/
int main()
{

    //Seed RNG
    srand (time(NULL)); 

    //initPhonemes(phonemeBuffer);

    //**** Allocate memory for the phoneme buffer
    phonemeBuffer = new Phoneme**[NUM_GENDER];
    
    for (int i = 0; i < NUM_GENDER; i++)
    {
        phonemeBuffer[i] = (Phoneme**)malloc(NUM_PHONEMES * sizeof(Phoneme*));
    }

    //**** Create the phonemes

    //Male bass
    phonemeBuffer[(int)MALE][(int)A] = new Phoneme("[A]", 600, 1040, 2250);
    phonemeBuffer[(int)MALE][(int)E] = new Phoneme("[E]", 400, 1620, 2400);
    phonemeBuffer[(int)MALE][(int)I] = new Phoneme("[I]", 250, 1750, 2600);
    phonemeBuffer[(int)MALE][(int)O] = new Phoneme("[O]", 400, 750, 2400);
    phonemeBuffer[(int)MALE][(int)U] = new Phoneme("[U]", 350, 600, 2400);

    //Female soprano
    phonemeBuffer[(int)FEMALE][(int)A] = new Phoneme("[A]", 800, 1150, 2900);
    phonemeBuffer[(int)FEMALE][(int)E] = new Phoneme("[E]", 350, 2000, 2800);
    phonemeBuffer[(int)FEMALE][(int)I] = new Phoneme("[I]", 270, 2140, 2950);
    phonemeBuffer[(int)FEMALE][(int)O] = new Phoneme("[O]", 450, 800, 2830);
    phonemeBuffer[(int)FEMALE][(int)U] = new Phoneme("[U]", 325, 700, 2700);    

    /**** Initialize PortAudio and begin audio stream ****/

    //declare PortAudio variables
	PaStream *stream;
	PaStreamParameters outputParameters;
	PaError err;
	paData data;

    //Initialize PortAudio
	Pa_Initialize();

    //Set output stream parameters
    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = NUM_OUT_CHANNELS;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = 
        Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    //Open audio stream and check for error
    err = Pa_OpenStream(&stream, NULL /* no input */,
            &outputParameters,
            SAMPLE_RATE, FRAMES_PER_BUFFER, paNoFlag,
            paCallback, &data);

    if (err != paNoError)
    {
        printf("Error in PortAudio: Pa_OpenStream: %s\n", Pa_GetErrorText(err));
        return -1;
    }

    err = Pa_StartStream(stream);

    if (err != paNoError)
    {
        printf("Error in PortAudio: Pa_StartStream: %s\n", Pa_GetErrorText(err));
    }

    /******** Terminal Display ********/
    initscr();
    keypad(stdscr, TRUE);

    char ch = '\0';
    data.freq = 440.0 * pow( pow(2.0, (1.0/12.0) ), freqInc);
    int row, col, prevRow, prevCol;
    getmaxyx(stdscr, row, col);

    mvprintw(0, col/2, "%s%f", "Frequency: ", data.freq);
    mvprintw(1, col/2, "%s%s", "Gender: ", (curGender == MALE) ? "Male" : "Female");
    mvprintw(3, col/2, "%s%s", "Vowel: ", 
                ((int)whichSound == NUM_PHONEMES) ? "<No Filter>" : 
                phonemeBuffer[(int)curGender][(int)whichSound]->getName().c_str());

    mvprintw(5, 0, "%s", "Next formant: 's'   Random formant: 'r'   Change gender: 'g'   Frequency down/up: 'o'/'p'   Quit: 'q'");

    mvprintw(7, 0, "%s", "1st Formant: ");
    mvprintw(8, 0, "%s", "2nd Formant: ");
    mvprintw(9, 0, "%s", "3rd Formant: ");

    mvprintw(13, col/2, "%s", "Pressed key: ");

    /******** User Input ********/

    while (ch != 'q')
    {
    	ch = getch();

        switch (ch)
        {
            case 'p':
                //data.freq += freqInc;
                data.freq = 440.0 * pow( pow(2.0, (1.0/12.0) ), ++freqInc);
                break;
            case 'o':
                data.freq = 440.0 * pow( pow(2.0, (1.0/12.0)), --freqInc);
                break;
            case 's': //switch to a different phoneme with the 's' key
                whichSound = (phon_enum)( ( (int)whichSound + 1) % (NUM_PHONEMES + 1) ); 
                //+ 1 is to cover the default case as well
                break;
            case 'r': //choose a random phoneme
                whichSound = (phon_enum)(rand() % NUM_PHONEMES + 1);
                break;
            case 'g': //change gender
                curGender = (gender)( ((int)curGender + 1) % NUM_GENDER);
                break;
        }

        clear();
        refresh();


        prevRow = row;
        prevCol = col;
        getmaxyx(stdscr, row, col);
        if ( (row != prevRow) || (col != prevCol) )
        {
            clear();
            refresh();
        }

        mvprintw(0, col/2, "%s%f", "Frequency: ", data.freq);
        mvprintw(1, col/2, "%s%s", "Gender: ", (curGender == MALE) ? "Male  " : "Female");
        mvprintw(3, col/2, "%s%s", "Vowel: ",
            ( ((int)whichSound) == NUM_PHONEMES) ? "<No Filter>" : 
            phonemeBuffer[(int)curGender][(int)whichSound]->getName().c_str() );

        mvprintw(5, 0, "%s", "Next formant: 's'   Random formant: 'r'   "
                        "Change gender: 'g'   Frequency down/up: 'o'/'p'   "
                        "Quit: 'q'");

        mvprintw(7, 0, "%s", "1st Formant: ");
        mvprintw(8, 0, "%s", "2nd Formant: ");
        mvprintw(9, 0, "%s", "3rd Formant: ");
        
        string tmpFreq;
        for (int i = 0; i < NUM_FORMANTS; i ++)
        {
            tmpFreq = ( (((int)whichSound) == NUM_PHONEMES) ? 
                        "None" :
                        to_string(phonemeBuffer[(int)curGender][(int)whichSound]->
                        getFormant(i)->getFrequency()*1000.0));

            mvprintw(7 + i, col/3, "%s", 
                ( ((int)whichSound) == NUM_PHONEMES) ? 
                    "None" : ( (((int)whichSound) == NUM_PHONEMES) ? 
                        "None" :
                        to_string(phonemeBuffer[(int)curGender][(int)whichSound]->
                        getFormant(i)->getFrequency()*1000.0)).c_str());
        }

        mvprintw(13, col/2, "%s%c", "Pressed key: ", ch);
        
    }

    endwin();

    /***** Close audio stream *****/

    err = Pa_StopStream(stream);

    if (err != paNoError)
    {
    	printf("Error in PortAudio: Pa_StopStream: %s\n", Pa_GetErrorText(err));
    }

    err = Pa_CloseStream(stream);

	if (err != paNoError)
    {
    	printf("Error in PortAudio: Pa_CloseStream: %s\n", Pa_GetErrorText(err));
    }	

    err = Pa_Terminate();

	if (err != paNoError)
    {
    	printf("Error in PortAudio: Pa_Terminate: %s\n", Pa_GetErrorText(err));
    }	

	return 0;
}


/*
    Function:       calculateSample
    Purpose:        Filter an audio sample based on a phoneme's
                    formant frequencies
    Parameters:     currentPhoneme      <Phoneme ptr>
                        - phoneme to use for the filtering
                    orgSample           <double>
                        - sample to filter
    Returns:        <a filtered sample> <double>
*/
double calculateSample(Phoneme* currentPhoneme, double orgSample)
{
    double newSample = 0;
    for (int i = 0; i < NUM_FORMANTS; i++)
    {
        newSample += currentPhoneme->getFormant(i)->getFormantFilter()->do_sample(orgSample);
    }

    return newSample;
}