#include "MidSide.h"

#include <math.h>
#include <float.h>
#include <stdio.h>

void int2strng(VstInt32 value, char *string) { sprintf(string, "%d", value); }
void float2strng(float value, char *string) { sprintf(string, "%.2f", value); }
float linear2dB(float signal) { return 10.f * (float) log10(fabs(signal)); }
void fpan2strng(float pan, char *string) {
  if (pan == 0.f) strcpy(string, "Center");
  if (pan < 0.f) sprintf(string, "L %.2f%%", -100.f * pan);
  if (pan > 0.f) sprintf(string, "R %.2f%%", 100.f * pan);
}

AudioEffect *createEffectInstance(audioMasterCallback audioMaster) {
  return new wolfMidSide(audioMaster);
}

wolfMidSide::~wolfMidSide() {
  if(buffer1) delete [] buffer1;
  if(buffer2) delete [] buffer2;
  if(bfHaasL) delete [] bfHaasL;
  if(bfHaasR) delete [] bfHaasR;
}

wolfMidSide::wolfMidSide(audioMasterCallback audioMaster) : AudioEffectX(audioMaster, 1, 7) // programs, parameters
{
  //inits here!
  fParam1 = (float) 0.5; // Mid Volume (post)
  fParam2 = (float) 0.5; //Side Volume (post)
  fParam3 = (float) 0.0; // Mid Delay (post)
  fParam4 = (float) 0.0; //Side Delay (post)
  fParam5 = (float) 0.0; // Haas Left (pre)
  fParam6 = (float) 0.0; // Haas Right (pre)
  fParam7 = (float) 0.0; // Pan (-1 L, +1 R) (pre)

  size = 262143;
  hsize = 16383;
  bufpos = 0;
  hpos = 0;
  buffer1 = new float[size];
  buffer2 = new float[size];
  bfHaasL = new float[hsize];
  bfHaasR = new float[hsize];

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
  hadL = (VstInt32) (fParam5 * getSampleRate() * 0.04f);
  hadR = (VstInt32) (fParam6 * getSampleRate() * 0.04f);
  if (fParam7 == 0.5f) { panL = 1.f; panR = 1.f; }
  if (fParam7 < 0.5f) { panL = 1.f; panR = fParam7 * 2.f; }
  if (fParam7 > 0.5f) { panL = 2.f - fParam7 * 2.f; panR = 1.f; }
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
    case 4: v = fParam5; break;
    case 5: v = fParam6; break;
    case 6: v = fParam7; break;
  }
  return v;
}

void wolfMidSide::getParameterName(VstInt32 index, char *label) {
  switch(index) {
    case 0: strcpy(label, "Mid Gain"); break;
    case 1: strcpy(label, "Side Gain"); break;
    case 2: strcpy(label, "Mid Delay"); break;
    case 3: strcpy(label, "Side Delay"); break;
    case 4: strcpy(label, "Left Delay"); break;
    case 5: strcpy(label, "Right Delay"); break;
    case 6: strcpy(label, "Pan"); break;
  }
}

void wolfMidSide::setParameter(VstInt32 index, float value) {
  switch(index)
  {
    case 0: fParam1 = value; break;
    case 1: fParam2 = value; break;
    case 2: fParam3 = value; break;
    case 3: fParam4 = value; break;
    case 4: fParam5 = value; break;
    case 5: fParam6 = value; break;
    case 6: fParam7 = value; break;
    
  }
  //calcs here for further constants dependent on parameters
  del3 = (VstInt32) (fParam3 * getSampleRate());
  del4 = (VstInt32) (fParam4 * getSampleRate());
  hadL = (VstInt32) (fParam5 * getSampleRate() * 0.04f);
  hadR = (VstInt32) (fParam6 * getSampleRate() * 0.04f);
  if (fParam7 == 0.5f) { panL = 1.f; panR = 1.f; }
  if (fParam7 < 0.5f) { panL = 1.f; panR = fParam7 * 2.f; }
  if (fParam7 > 0.5f) { panL = 2.f - fParam7 * 2.f; panR = 1.f; }
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
    case 4: float2strng(40.f * fParam5, text); break;
    case 5: float2strng(40.f * fParam6, text); break;
    case 6: fpan2strng(2.f * fParam7 - 1.f, text); break;
  }
}

void wolfMidSide::getParameterLabel(VstInt32 index, char *label) {
  switch(index) {
    case 0: strcpy(label, "dB"); break;
    case 1: strcpy(label, "dB"); break;
    case 2: strcpy(label, "ms"); break;
    case 3: strcpy(label, "ms"); break;
    case 4: strcpy(label, "ms"); break;
    case 5: strcpy(label, "ms"); break;
    case 6: break;
  }
}

void wolfMidSide::suspend() {
  memset(buffer1, 0, size * sizeof(float));
  memset(buffer2, 0, size * sizeof(float));
  memset(bfHaasL, 0, hsize * sizeof(float));
  memset(bfHaasR, 0, hsize * sizeof(float));
}

//--------------------------------------------------------------------------------
// process

void wolfMidSide::process(float **inputs, float **outputs, VstInt32 sampleFrames) {
  float *in1 = inputs[0];
  float *in2 = inputs[1];
  float *out1 = outputs[0];
  float *out2 = outputs[1];

  VstInt32 delpos1, delpos2, delhaasL, delhaasR;
  while(--sampleFrames >= 0) {
    // store panning on the buffer
    bfHaasL[hpos & hsize] = *in1++ * panL; // left pan
    bfHaasR[hpos & hsize] = *in2++ * panR; // right pan
    // apply left and right delay
    delhaasL = (hpos - hadL) & hsize; // left delay position in the buffer
    delhaasR = (hpos - hadR) & hsize; // right delay position in the buffer
    buffer1[bufpos & size] = (bfHaasL[delhaasL] + bfHaasR[delhaasR]) * fParam1; //  mid and gain
    buffer2[bufpos & size] = (bfHaasL[delhaasL] - bfHaasR[delhaasR]) * fParam2; // side and gain
    delpos1 = (bufpos - del3) & size; // delay position in the buffer
    delpos2 = (bufpos - del4) & size; // delay position in the buffer
    *out1++ += buffer1[delpos1] + buffer2[delpos2];
    *out2++ += buffer1[delpos1] - buffer2[delpos2];
    ++bufpos;
    ++hpos;
  }
}

void wolfMidSide::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames) {
  float *in1 = inputs[0];
  float *in2 = inputs[1];
  float *out1 = outputs[0];
  float *out2 = outputs[1];

  VstInt32 delpos1, delpos2, delhaasL, delhaasR;
  while(--sampleFrames >= 0) {
    // store panning on the buffer
    bfHaasL[hpos & hsize] = *in1++ * panL; // left pan
    bfHaasR[hpos & hsize] = *in2++ * panR; // right pan
    // apply left and right delay
    delhaasL = (hpos - hadL) & hsize; // left delay position in the buffer
    delhaasR = (hpos - hadR) & hsize; // right delay position in the buffer
    buffer1[bufpos & size] = (bfHaasL[delhaasL] + bfHaasR[delhaasR]) * fParam1; //  mid and gain
    buffer2[bufpos & size] = (bfHaasL[delhaasL] - bfHaasR[delhaasR]) * fParam2; // side and gain
    delpos1 = (bufpos - del3) & size; // delay position in the buffer
    delpos2 = (bufpos - del4) & size; // delay position in the buffer
    *out1++ = buffer1[delpos1] + buffer2[delpos2];
    *out2++ = buffer1[delpos1] - buffer2[delpos2];
    ++bufpos;
    ++hpos;
  }
}
