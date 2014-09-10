#include "MidSide.h"

#include <math.h>
#include <float.h>
#include <stdio.h>

void int2strng(VstInt32 value, char *string) { sprintf(string, "%d", value); }
void float2strng(float value, char *string) { sprintf(string, "%.2f", value); }
float linear2dB(float signal) { return 10.f * (float) log10(fabs(signal)); }

AudioEffect *createEffectInstance(audioMasterCallback audioMaster) {
  return new wolfMidSide(audioMaster);
}

wolfMidSide::~wolfMidSide() {
  if(buffer1) delete [] buffer1;
  if(buffer2) delete [] buffer2;
}

wolfMidSide::wolfMidSide(audioMasterCallback audioMaster) : AudioEffectX(audioMaster, 1, 4) // programs, parameters
{
  //inits here!
  fParam1 = (float) 0.5; // Mid Volume
  fParam2 = (float) 0.5; //Side Volume
  fParam3 = (float) 0.0; // Mid Delay
  fParam4 = (float) 0.0; //Side Delay

  size = 262143;
  bufpos = 0;
  buffer1 = new float[size];
  buffer2 = new float[size];

  setNumInputs(2);
  setNumOutputs(2);
  setUniqueID('wlfM');    // identify here
  DECLARE_VST_DEPRECATED(canMono) ();
  canProcessReplacing();
  strcpy(programName, "Mid-Side Fx");
  suspend();  // flush buffer

  //calcs here!
  del3 = (VstInt32) (fParam3 * getSampleRate());
  del4 = (VstInt32) (fParam4 * getSampleRate());
}

bool wolfMidSide::getProductString(char* text) { strcpy(text, "MidSide"); return true; }
bool wolfMidSide::getVendorString(char* text)  { strcpy(text, "Wolf Sound"); return true; }
bool wolfMidSide::getEffectName(char* name)    { strcpy(name, "MidSide"); return true; }

void wolfMidSide::setProgramName(char *name) { strcpy(programName, name); }
void wolfMidSide::getProgramName(char *name) { strcpy(name, programName); }
bool wolfMidSide::getProgramNameIndexed (VstInt32 category, VstInt32 index, char* name) {
  if (index == 0) { strcpy(name, programName); return true; }
  return false;
}

float wolfMidSide::getParameter(VstInt32 index) {
  float v = 0;
  switch(index) {
    case 0: v = fParam1; break;
    case 1: v = fParam2; break;
    case 2: v = fParam3; break;
    case 3: v = fParam4; break;
  }
  return v;
}

void wolfMidSide::getParameterName(VstInt32 index, char *label) {
  switch(index) {
    case 0: strcpy(label, "Mid Gain"); break;
    case 1: strcpy(label, "Side Gain"); break;
    case 2: strcpy(label, "Mid Delay"); break;
    case 3: strcpy(label, "Side Delay"); break;
  }
}

void wolfMidSide::setParameter(VstInt32 index, float value) {
  switch(index)
  {
    case 0: fParam1 = value; break;
    case 1: fParam2 = value; break;
    case 2: fParam3 = value; break;
    case 3: fParam4 = value; break;
  }
  //calcs here for further constants dependent on parameters
  del3 = (VstInt32) (fParam3 * getSampleRate());
  del4 = (VstInt32) (fParam4 * getSampleRate());
}

void wolfMidSide::getParameterDisplay(VstInt32 index, char *text) {
  switch(index) {
    case 0:
      if (fParam1 > 0.f) {
        float2strng(linear2dB(fParam1 * 2.f), text);
      } 
      else {
        strcpy(text, "-Inf");
      }
      break;
    case 1: 
      if (fParam2 > 0.f) {
        float2strng(linear2dB(fParam2 * 2.f), text);
      } 
      else {
        strcpy(text, "-Inf");
      }
      break;
    case 2: float2strng(1000.f * fParam3, text); break;
    case 3: float2strng(1000.f * fParam4, text); break;
  }
}

void wolfMidSide::getParameterLabel(VstInt32 index, char *label) {
  switch(index) {
    case 0: strcpy(label, "dB"); break;
    case 1: strcpy(label, "dB"); break;
    case 2: strcpy(label, "ms"); break;
    case 3: strcpy(label, "ms"); break;
  }
}

void wolfMidSide::suspend() {
  memset(buffer1, 0, size * sizeof(float));
  memset(buffer2, 0, size * sizeof(float));
}

//--------------------------------------------------------------------------------
// process

void wolfMidSide::process(float **inputs, float **outputs, VstInt32 sampleFrames) {
  float *in1 = inputs[0];
  float *in2 = inputs[1];
  float *out1 = outputs[0];
  float *out2 = outputs[1];

  VstInt32 delpos1, delpos2;
  while(--sampleFrames >= 0) {
    buffer1[bufpos & size] = (*in1 + *in2) * fParam1; //  mid and gain
    buffer2[bufpos & size] = (*in1++ - *in2++) * fParam2; // side and gain
    delpos1 = (bufpos - del3) & size; // delay position in the buffer
    delpos2 = (bufpos - del4) & size; // delay position in the buffer
    *out1++ += buffer1[delpos1] + buffer2[delpos2];
    *out2++ += buffer1[delpos1] - buffer2[delpos2];
    ++bufpos;
  }
}

void wolfMidSide::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames) {
  float *in1 = inputs[0];
  float *in2 = inputs[1];
  float *out1 = outputs[0];
  float *out2 = outputs[1];

  VstInt32 delpos1, delpos2;
  while(--sampleFrames >= 0) {
    buffer1[bufpos & size] = (*in1 + *in2) * fParam1; //  mid and gain
    buffer2[bufpos & size] = (*in1++ - *in2++) * fParam2; // side and gain
    delpos1 = (bufpos - del3) & size; // delay position in the buffer
    delpos2 = (bufpos - del4) & size; // delay position in the buffer
    *out1++ = buffer1[delpos1] + buffer2[delpos2];
    *out2++ = buffer1[delpos1] - buffer2[delpos2];
    ++bufpos;
  }
}
