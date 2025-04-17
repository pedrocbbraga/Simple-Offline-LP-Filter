/*
 * RAMTeCH Screening Test/Interview Questions for Audio Systems Software Engineer
 * Question 19
 * Simple Digital Filter
 * 
 * Author: Pedro Cajaty Barbosa Braga
 * Created on 04/16/2025
 *
 * Simple IIR Low-Pass Filter
 * Run with make
 *
 */

/* ------------------------------------------------------- */

 /*
- Read WAV file
    - Open input file
    - WAV struct to decode metadata
- Apply low pass IIR filter
    - While not at end of input file
        - Read sample from file
        - apply filter
        - write filtered sample to output
- Output filter WAV file
    - Copy original WAV header metadata to output file
        ** Do this before applying filter
    - Close input + output files
*/

/* ------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>

float cutOffFreq = 500.0;
int sampleRate = 44100.0;

typedef struct
{
    int sampleRate;
    int numChannels;
    int bitDepth;
    sf_count_t numFrames;
    sf_count_t totalSamples;
    float* data;
    SF_INFO originalInfo;
} WAV;

WAV readFile(const char* fileName)
{
    WAV wav;
    SF_INFO info;
    SNDFILE* file = sf_open(fileName, SFM_READ, &info);

    // If unable to open file, launch error
    if (!file)
    {
        fprintf(stderr, "can't open file\n");
        exit(1);
    }

    int subformat = info.format & SF_FORMAT_SUBMASK;

    // Detect bit depth from format -> subformat
    switch (subformat) {
        case SF_FORMAT_PCM_S8:
            wav.bitDepth = 8;
            break;
        case SF_FORMAT_PCM_16:
            wav.bitDepth = 16;
            break;
        case SF_FORMAT_PCM_24:
            wav.bitDepth = 24;
            break;
        case SF_FORMAT_PCM_32:
            wav.bitDepth = 32;
            break;
        case SF_FORMAT_FLOAT:
            wav.bitDepth = 32;
            break;
        default:
            wav.bitDepth = -1;
            exit(1);
    }

    // Populate wav struct
    wav.sampleRate = info.samplerate;
    wav.numChannels = info.channels;
    wav.numFrames = info.frames;
    wav.totalSamples = wav.numChannels * wav.numFrames;
    wav.data = (float*)malloc(sizeof(float) * wav.totalSamples);
    wav.originalInfo = info;

    sf_readf_float(file, wav.data, wav.numFrames);
    sf_close(file);

    return wav;
}

void lowPass(WAV* wav, float cutOffFreq)
{
    // alpha = fc / fc + fs
    float a = cutOffFreq / (cutOffFreq + wav->sampleRate);

    // Init each memory for channel
    float* prevSum = calloc(wav->numChannels, sizeof(float));

    for (sf_count_t i = 0; i < wav->totalSamples; i++)
    {
        int channel = i % wav->numChannels;
        
        // y[n] = a * x[n] + (1 - a) * y[n-1]
        wav->data[i] = a * wav->data[i] + (1.0f - a) * prevSum[channel];
        prevSum[channel] = wav->data[i];
    }

    free(prevSum);
}

void writeFile(WAV* wav, const char* fileName)
{
    //Reconstruct WAV file
    SNDFILE* file = sf_open(fileName, SFM_WRITE, &wav->originalInfo);
    if (!file)
    {
        fprintf(stderr, "can't open file\n");
        exit(1);
    }

    sf_writef_float(file, wav->data, wav->numFrames);
    sf_close(file);

}

int main()
{
    WAV wav = readFile("test.wav");
    // printf("OPENED FILE\n");

    lowPass(&wav, cutOffFreq);
    // printf("LOW PASSED\n");

    writeFile(&wav, "outputLP.wav");
    // printf("DONE OUTPUT\n");

    free(wav.data);
    return 0;
}
