#ifndef __MidSide_H
#define __MidSide_H

#include "audioeffectx.h"

class wolfMidSide : public AudioEffectX
{
  public:
    wolfMidSide(audioMasterCallback audioMaster);
    ~wolfMidSide();

    virtual bool getEffectName(char *name);
    virtual bool getVendorString(char *text);
    virtual bool getProductString(char *text);
    virtual VstInt32 getVendorVersion() { return 1000; }

    virtual void setProgramName(char *name);
    virtual void getProgramName(char *name);
    virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* name);

    virtual float getParameter(VstInt32 index);
    virtual void getParameterName(VstInt32 index, char *text);
    virtual void setParameter(VstInt32 index, float value);
    virtual void getParameterDisplay(VstInt32 index, char *text);
    virtual void getParameterLabel(VstInt32 index, char *label);

    virtual void suspend();
    virtual void process(float **inputs, float **outputs, VstInt32 sampleFrames);
    virtual void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames);



  protected:
    char programName[32];

    float fParam1;
    float fParam2;
    float fParam3;
    float fParam4;
    float fParam5;
    float fParam6;
    float fParam7;
    

    float panL, panR;
    float *buffer1, *buffer2;
    float *bfHaasL, *bfHaasR;
    int bufpos, hpos;
    VstInt32 size, hsize;
    VstInt32 del3, del4;
    VstInt32 hadL, hadR;
};

#endif
