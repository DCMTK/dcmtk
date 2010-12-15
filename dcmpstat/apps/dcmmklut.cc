/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmpstat
 *
 *  Authors: Marco Eichelberg, Joerg Riesmeier
 *
 *  Purpose: This application reads a DICOM image, adds a Modality LUT,
 *    a VOI LUT or a Presentation LUT to the image and writes it back.
 *    The LUT has a gamma curve shape or can be imported from an external
 *    file.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-12-15 17:03:38 $
 *  CVS/RCS Revision: $Revision: 1.47 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmimgle/dicrvfit.h"
#include "dcmtk/dcmimgle/digsdfn.h"
#include "dcmtk/dcmimgle/diutils.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstd.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CMATH
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmmklut"

static OFLogger dcmmklutLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************


enum LUT_Type
{
    LUT_Modality,
    LUT_Presentation,
    LUT_VOI
};


OFCondition readMapFile(const char *filename,
                        double *&inputXData,
                        double *&inputYData,
                        unsigned long &inputEntries,
                        double &inputXMax,
                        double &inputYMax)
{
    OFCondition result = EC_IllegalCall;
    if ((filename != NULL) && (strlen(filename) > 0))
    {
        OFLOG_INFO(dcmmklutLogger, "reading map file ...");
        unsigned char buffer[1000];
        FILE *inf = fopen(filename, "rb");
        if (inf != NULL)
        {
            if (fread(buffer, 1, 264, inf) == 264)
            {
                if (fread(buffer + 264, 1, 1, inf) == 0)
                {
                    if ((buffer[0] == 0x8a) && (buffer[1] == 0x3f) && (buffer[2] == 0x0) && (buffer[3] == 0x0))
                    {
                        inputEntries = 256;
                        inputXMax = 255;
                        inputYMax = 255;
                        inputXData = new double[inputEntries];
                        inputYData = new double[inputEntries];
                        if ((inputXData != NULL) && (inputYData != NULL))
                        {
                            for (unsigned long i = 0; i < inputEntries; i++)
                            {
                                inputXData[i] = i;
                                inputYData[i] = (double)buffer[i + 8];
                            }
                            result = EC_Normal;
                        }
                    } else
                        OFLOG_WARN(dcmmklutLogger, "magic word wrong, not a map file ... ignoring !");
                } else
                    OFLOG_WARN(dcmmklutLogger, "file too large, not a map file ... ignoring !");
            } else
                OFLOG_WARN(dcmmklutLogger, "read error in map file ... ignoring !");
            fclose(inf);
        } else
            OFLOG_WARN(dcmmklutLogger, "cannot open map file ... ignoring !");
    }
    return result;
}


OFCondition readTextFile(const char *filename,
                         double *&inputXData,
                         double *&inputYData,
                         unsigned long &inputEntries,
                         double &inputXMax,
                         double &inputYMax)
{
    if ((filename != NULL) && (strlen(filename) > 0))
    {
        OFLOG_INFO(dcmmklutLogger, "reading text file ...");
#ifdef HAVE_IOS_NOCREATE
        STD_NAMESPACE ifstream file(filename, STD_NAMESPACE ios::in | STD_NAMESPACE ios::nocreate);
#else
        STD_NAMESPACE ifstream file(filename, STD_NAMESPACE ios::in);
#endif
        if (file)
        {
            inputEntries = 0;
            inputXMax = 0;
            inputYMax = 0;
            unsigned long count = 0;
            double xmax = 0;
            double ymax = 0;
            char c;
            while (file.get(c))
            {
                if (c == '#')                                               // comment character
                {
                    while (file.get(c) && (c != '\n') && (c != '\r'));      // skip comments
                }
                else if (!isspace(OFstatic_cast(unsigned char, c)))         // skip whitespaces
                {
                    file.putback(c);
                    if (inputEntries == 0)                                  // read number of entries
                    {
                        char str[6];
                        file.get(str, sizeof(str));
                        if (strcmp(str, "count") == 0)                      // check for key word: count
                        {
                            file >> inputEntries;
                            if (inputEntries > 0)
                            {
                                inputXData = new double[inputEntries];
                                inputYData = new double[inputEntries];
                                if ((inputXData == NULL) || (inputYData == NULL))
                                    return EC_IllegalCall;
                            } else {
                                OFLOG_ERROR(dcmmklutLogger, "invalid or missing value for number of entries in text file ... ignoring !");
                                return EC_IllegalCall;                      // abort
                            }
                        } else {
                            OFLOG_ERROR(dcmmklutLogger, "missing keyword 'count' for number of entries in text file ... ignoring !");
                            return EC_IllegalCall;                          // abort
                        }
                    }
                    else if ((inputXMax == 0.0) && (c == 'x'))              // read x maxvalue (optional)
                    {
                        char str[5];
                        file.get(str, sizeof(str));
                        if (strcmp(str, "xMax") == 0)                       // check for key word: xMax
                        {
                            file >> inputXMax;
                            if (inputXMax <= 0)
                            {
                                OFLOG_DEBUG(dcmmklutLogger, "invalid value for xMax in text file ...ignoring !");
                            }
                        } else {
                            OFLOG_ERROR(dcmmklutLogger, "invalid text file ... ignoring !");
                            return EC_IllegalCall;                          // abort
                        }
                    }
                    else if ((inputYMax == 0.0) && (c == 'y'))              // read y maxvalue (optional)
                    {
                        char str[5];
                        file.get(str, sizeof(str));
                        if (strcmp(str, "yMax") == 0)                       // check for key word: yMax
                        {
                            file >> inputYMax;
                            if (inputYMax <= 0)
                            {
                                OFLOG_DEBUG(dcmmklutLogger, "invalid value for yMax in text file ...ignoring !");
                            }
                        } else {
                            OFLOG_ERROR(dcmmklutLogger, "invalid text file ... ignoring !");
                            return EC_IllegalCall;                          // abort
                        }
                    } else {
                        if (count < inputEntries)
                        {
                            file >> inputXData[count];                      // read x value
                            file >> inputYData[count];                      // read y value
                            if (file.fail())
                            {
                                OFLOG_DEBUG(dcmmklutLogger, "missing y value in text file ... ignoring last entry !");
                            } else {
                                if ((count > 0) && (inputXData[count] <= xmax))
                                {
                                    OFLOG_DEBUG(dcmmklutLogger, "x values in text file not strictly monotonous ... ignoring entry #" << (count + 1) << " !");
                                } else {
                                    xmax = inputXData[count];
                                    if (inputYData[count] > ymax)
                                        ymax = inputYData[count];
                                    if ((inputXMax != 0) && (inputXData[count] > inputXMax))
                                    {
                                        OFLOG_DEBUG(dcmmklutLogger, "x value (" << inputXData[count] << ") exceeds maximum value ("
                                            << inputXMax << ") in text file ..." << OFendl << "         ... ignoring value !");
                                    }
                                    else if ((inputYMax != 0) && (inputYData[count] > inputYMax))
                                    {
                                        OFLOG_DEBUG(dcmmklutLogger, "y value (" << inputYData[count] << ") exceeds maximum value ("
                                            << inputYMax << ") in text file ..." << OFendl << "         ... ignoring value !");
                                    } else
                                        count++;
                                }
                            }
                        } else {
                            OFLOG_DEBUG(dcmmklutLogger, "too many values in text file ... ignoring last line(s) !");
                            break;
                        }
                    }
                }
            }
            if (count  < inputEntries)
            {
                inputEntries = count;
                OFLOG_DEBUG(dcmmklutLogger, "automatically setting number of entries in text file to " << inputEntries << " !");
            }
            if (inputXMax == 0)                                             // automatic calculation
                inputXMax = xmax;
            if (inputYMax == 0)
                inputYMax = ymax;
            if (/*(inputXMax > 0) && (inputYMax > 0) &&*/ (inputEntries > 0) && (inputXData != NULL) && (inputYData != NULL))
                return EC_Normal;
            else
                OFLOG_WARN(dcmmklutLogger, "invalid text file ... ignoring !");
        } else
            OFLOG_WARN(dcmmklutLogger, "can't open text file ... ignoring !");
    }
    return EC_IllegalCall;
}


OFCondition writeTextOutput(const char *filename,
                            const unsigned long numberOfEntries,
                            const signed long firstMapped,
                            const Uint16 *outputData,
                            const OFString &header)
{
    OFCondition result = EC_IllegalCall;
    if ((filename != NULL) && (strlen(filename) > 0))
    {
        if ((outputData != NULL) && (numberOfEntries > 0))
        {
            OFLOG_INFO(dcmmklutLogger, "writing text file ...");
            STD_NAMESPACE ofstream file(filename);
            if (file)
            {
                file << header << OFendl;
                for (unsigned long i = 0; i < numberOfEntries; i++)
                    file << (firstMapped + (signed long)i) << "\t" << outputData[i] << OFendl;
            } else
                OFLOG_WARN(dcmmklutLogger, "can't create output text file ... ignoring !");
        }
    }
    return result;
}


OFCondition convertInputLUT(const unsigned int numberOfBits,
                            const unsigned long numberOfEntries,
                            const signed long firstMapped,
                            double *inputXData,
                            double *inputYData,
                            const unsigned long inputEntries,
                            const double inputXMax,
                            const double inputYMax,
                            const unsigned int order,
                            Uint16 *outputData,
                            OFString &header,
                            char *explanation)
{
    OFCondition result = EC_IllegalCall;
    if ((inputXData != NULL) && (inputYData != NULL) && (inputEntries > 0) && (inputYMax > 0) && (outputData != NULL))
    {
        OFOStringStream oss;
        if (explanation !=NULL)
        {
            if (strlen(explanation) == 0)
            {
                sprintf(explanation, "LUT created from %u->%u bit data, descriptor %u/%ld/%u",
                    DicomImageClass::tobits((int)inputXMax), DicomImageClass::tobits((int)inputYMax),
                    (numberOfEntries < 65536) ? (Uint16)numberOfEntries : 0, firstMapped, numberOfBits);
            }
            oss << "# " << explanation << OFendl;
        }
        const double factor = (double)(DicomImageClass::maxval(numberOfBits)) / inputYMax;
        if (factor != 1.0)
        {
            OFLOG_INFO(dcmmklutLogger, "multiplying input values by "
                << STD_NAMESPACE ios::fixed << factor << " ...");
            for (unsigned long i = 0; i < inputEntries; i++)
                inputYData[i] *= factor;
        }
        if (numberOfEntries == inputEntries)
        {
            OFLOG_INFO(dcmmklutLogger, "importing LUT data directly ...");
            for (unsigned long i = 0; i < numberOfEntries; i++)
                outputData[i] = (Uint16)inputYData[i];
            result = EC_Normal;
        } else {
            OFLOG_INFO(dcmmklutLogger, "using polynomial curve fitting algorithm ...");
            double *coeff = new double[order + 1];
            if (DiCurveFitting<double, double>::calculateCoefficients(inputXData, inputYData, (unsigned int)inputEntries, order, coeff))
            {
                if (DiCurveFitting<double, Uint16>::calculateValues(0, inputXMax, outputData, (unsigned int)numberOfEntries, order, coeff))
                {
                    oss << "# using polynomial curve fitting algorithm (order = " << order << ")" << OFendl;
                    oss << "# equation: y = C0 + C1*x + C2*x^2 + C3*x^3 + ... + Cn*x^n" << OFendl;
                    oss << "# where: ";
                    for (unsigned int i = 0; i <= order; i++)
                    {
                        oss << "C" << i << " = ";
                        oss.setf(STD_NAMESPACE ios::fixed, STD_NAMESPACE ios::floatfield);
                        //oss.setf(ios::showpos);
                        oss.precision(5);
                        oss.width(10);
                        oss << coeff[i] << OFendl;
                        if (i < order)
                        {
                            oss << "#       ";
                            if (i < 9)
                                oss << " ";
                        }
                    }
                    result = EC_Normal;
                }
            }
            delete[] coeff;
        }
        if (result == EC_Normal)
        {
            oss << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            header += tmpString;
            OFSTRINGSTREAM_FREESTR(tmpString)
        } else
            OFLOG_WARN(dcmmklutLogger, "can't create lookup table from text file ... ignoring !");
    }
    return result;
}


void gammaLUT(const unsigned int numberOfBits,
              const unsigned long numberOfEntries,
              const signed long firstMapped,
              const OFBool byteAlign,
              const double gammaValue,
              Uint16 *outputData,
              OFString &header,
              char *explanation)
{
    if (outputData != NULL)
    {
        OFLOG_INFO(dcmmklutLogger, "computing gamma function ...");
        OFOStringStream oss;
        if (explanation != NULL)
        {
            if (strlen(explanation) == 0)
            {
                char gammabuf[16];
                OFStandard::ftoa(gammabuf, sizeof(gammabuf), gammaValue, OFStandard::ftoa_format_f, 3, 1);

                sprintf(explanation, "LUT with gamma %s, descriptor %u/%ld/%u", gammabuf,
                    (numberOfEntries < 65536) ? (Uint16)numberOfEntries : 0, firstMapped, numberOfBits);
            }
            oss << "# " << explanation << OFendl;
        }
        Uint16 maxValue = 0xFFFF >> (16 - numberOfBits);
        double step = (double)maxValue / ((double)numberOfEntries - 1.0);
        double gammaExp = 1.0 / gammaValue;
        double factor = (double)maxValue / pow((double)maxValue, gammaExp);
        double val;
        unsigned long i = 0;
        if (byteAlign)
        {
            Uint8 *data8 = (Uint8 *)outputData;
            for (i = 0; i < numberOfEntries; i++)
            {
                val = factor * pow(i * step, gammaExp);
                data8[i] = (Uint8)val;
            }
            data8[numberOfEntries]=0; //create padding value
        } else {
            for (i = 0; i < numberOfEntries; i++)
            {
                val = factor * pow(i * step, gammaExp);
                outputData[i]= (Uint16)val;
            }
        }
        oss << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        header += tmpString;
        OFSTRINGSTREAM_FREESTR(tmpString)
    }
}


void applyInverseGSDF(const unsigned int numberOfBits,
                      const unsigned long numberOfEntries,
                      const OFBool byteAlign,
                      const unsigned int minDensity,
                      const unsigned int maxDensity,
                      const unsigned int illumination,
                      const unsigned int reflection,
                      Uint16 *outputData,
                      OFString &header,
                      char *explanation)
{
    if (outputData != NULL)
    {
        OFLOG_INFO(dcmmklutLogger, "applying inverse GSDF ...");
        OFOStringStream oss;
        if ((explanation != NULL) && (strlen(explanation) > 0))
            strcat(explanation, ", inverse GSDF");
        const double l0 = (double)illumination;
        const double la = (double)reflection;
        const double dmin = (double)minDensity / 100;
        const double dmax = (double)maxDensity / 100;
        const double lmin = la + l0 * pow((double)10, -dmax);
        const double lmax = la + l0 * pow((double)10, -dmin);
        const double jmin = DiGSDFunction::getJNDIndex(lmin);
        const double jmax = DiGSDFunction::getJNDIndex(lmax);
        const double factor = (double)DicomImageClass::maxval(numberOfBits) / (jmax - jmin);
        const double density = (dmax - dmin) / (double)DicomImageClass::maxval(numberOfBits);
        unsigned long i;
        if (byteAlign)
        {
            Uint8 *data8 = (Uint8 *)outputData;
            for (i = 0; i < numberOfEntries; i++)
                data8[i] = (Uint8)((DiGSDFunction::getJNDIndex(la + l0 * pow((double)10, -(dmin + (double)data8[i] * density))) - jmin) * factor);
        } else {
            for (i = 0; i < numberOfEntries; i++)
                outputData[i] = (Uint16)((DiGSDFunction::getJNDIndex(la + l0 * pow((double)10, -(dmin + (double)outputData[i] * density))) - jmin) * factor);
        }
        oss << "# applied inverse GSDF with Dmin/max = " << minDensity << "/" << maxDensity << ", L0/La = "
            << illumination << "/" << reflection << OFendl;
        oss << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        header += tmpString;
        OFSTRINGSTREAM_FREESTR(tmpString)
    }
}

#ifndef RAND_MAX
/* some brain-dead systems such as SunOS 4.1.3 do not define any constant
 * for the upper limit of rand() calls. We hope that such systems at least
 * keep within the SysV specs and return values up to 2^15-1.
 */
#define RAND_MAX 32767
#endif

void mixingUpLUT(const unsigned long numberOfEntries,
                 const OFBool byteAlign,
                 const unsigned long randomCount,
                 const unsigned int randomSeed,
                 Uint16 *outputData,
                 char *explanation)
{
    if (outputData != NULL)
    {
        OFLOG_INFO(dcmmklutLogger, "mixing up LUT entries ...");
        if ((explanation != NULL) && (strlen(explanation) > 0))
            strcat(explanation, ", mixed-up entries");
        srand(randomSeed);
        unsigned long i, i1, i2;
        const double factor = (double)(numberOfEntries - 1) / RAND_MAX;
        if (byteAlign)
        {
            Uint8 temp;
            Uint8 *data8 = (Uint8 *)outputData;
            for (i = 0; i < randomCount; i++)
            {
                i1 = (unsigned long)(rand() * factor);
                i2 = (unsigned long)(rand() * factor);
                if (i1 != i2)
                {
                    temp = data8[i1];
                    data8[i1] = data8[i2];
                    data8[i2] = temp;
                }
            }
        } else {
            Uint16 temp;
            for (i = 0; i < randomCount; i++)
            {
                i1 = (unsigned long)(rand() * factor);
                i2 = (unsigned long)(rand() * factor);
                if (i1 != i2)
                {
                    temp = outputData[i1];
                    outputData[i1] = outputData[i2];
                    outputData[i2] = temp;
                }
            }
        }
    }
}


OFCondition createLUT(const unsigned int numberOfBits,
                      const unsigned long numberOfEntries,
                      const signed long firstMapped,
                      const OFBool byteAlign,
                      DcmEVR lutVR,
                      DcmItem &item,
                      Uint16 *data,
                      const char *explanation = NULL)
{
    OFCondition result = EC_Normal;
    Uint16 numEntries16 = 0;

    if (numberOfEntries == 0)
        OFLOG_WARN(dcmmklutLogger, "creating LUT without LUT data");
    if (numberOfEntries > 65536)
    {
        OFLOG_ERROR(dcmmklutLogger, "cannot create LUT with more than 65536 entries");
        return EC_IllegalCall;
    }
    if (numberOfEntries < 65536)
        numEntries16 = (Uint16)numberOfEntries;
    if ((numberOfBits < 8) || (numberOfBits > 16))
    {
        OFLOG_ERROR(dcmmklutLogger, "cannot create LUT with " << numberOfBits << " bit entries, only 8..16");
        return EC_IllegalCall;
    }

    DcmElement *descriptor = NULL;
    if (firstMapped < 0)
    {
        // LUT Descriptor is SS
        if (firstMapped < -32768)
        {
            OFLOG_ERROR(dcmmklutLogger, "cannot create LUT - first value mapped < -32768");
            return EC_IllegalCall;
        }
        descriptor = new DcmSignedShort(DcmTag(DCM_LUTDescriptor, EVR_SS));
        if (descriptor)
        {
            if (EC_Normal==result) result = descriptor->putSint16((Sint16)numEntries16, 0);
            if (EC_Normal==result) result = descriptor->putSint16((Sint16)firstMapped, 1);
            if (EC_Normal==result) result = descriptor->putSint16((Sint16)numberOfBits, 2);
            if (EC_Normal==result) result = item.insert(descriptor, OFTrue /*replaceOld*/);
        } else
            return EC_MemoryExhausted;
    } else {
        // LUT Descriptor is US
        if (firstMapped > 0xFFFF)
        {
            OFLOG_ERROR(dcmmklutLogger, "cannot create LUT - first value mapped > 65535");
            return EC_IllegalCall;
        }
        descriptor = new DcmUnsignedShort(DcmTag(DCM_LUTDescriptor, EVR_US));
        if (descriptor)
        {
            if (EC_Normal==result) result = descriptor->putUint16(numEntries16, 0);
            if (EC_Normal==result) result = descriptor->putUint16((Uint16)firstMapped, 1);
            if (EC_Normal==result) result = descriptor->putUint16(numberOfBits, 2);
            if (EC_Normal==result) result = item.insert(descriptor, OFTrue /*replaceOld*/);
        } else
            return EC_MemoryExhausted;
    }

    unsigned long wordsToWrite = 0;
    if (byteAlign)
    {
        // the array is now in little endian byte order. Swap to local byte order if neccessary.
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, (Uint8 *)data, numberOfEntries + 1, sizeof(Uint16));
        wordsToWrite = (numberOfEntries + 1) / 2;
    } else
        wordsToWrite = numberOfEntries;
    if ((wordsToWrite > 32767) && (lutVR != EVR_OW))
    {
        OFLOG_WARN(dcmmklutLogger, "LUT data >= 64K, writing as OW");
        lutVR = EVR_OW;
    }

    // write LUT Data as OW, US, or SS
    DcmElement *lutdata = NULL;
    switch (lutVR)
    {
        case EVR_US:
            lutdata = new DcmUnsignedShort(DcmTag(DCM_LUTData, EVR_US));
            if (lutdata)
            {
                if (EC_Normal==result) result = lutdata->putUint16Array(data, wordsToWrite);
                if (EC_Normal==result) result = item.insert(lutdata, OFTrue /*replaceOld*/);
            } else
                return EC_MemoryExhausted;
            break;
        case EVR_OW:
            lutdata = new DcmOtherByteOtherWord(DcmTag(DCM_LUTData, EVR_OW));
            if (lutdata)
            {
                if (EC_Normal==result) result = lutdata->putUint16Array(data, wordsToWrite);
                if (EC_Normal==result) result = item.insert(lutdata, OFTrue /*replaceOld*/);
            } else
                return EC_MemoryExhausted;
            break;
        case EVR_SS:
            lutdata = new DcmSignedShort(DcmTag(DCM_LUTData, EVR_SS));
            if (lutdata)
            {
                if (EC_Normal==result) result = lutdata->putSint16Array((Sint16 *)data, wordsToWrite);
                if (EC_Normal==result) result = item.insert(lutdata, OFTrue /*replaceOld*/);
            } else
                return EC_MemoryExhausted;
            break;
        default:
            OFLOG_ERROR(dcmmklutLogger, "unsupported VR for LUT Data");
            return EC_IllegalCall;
            /* break; */
  }

  // create LUT explanation
  if (explanation != NULL)
  {
      DcmElement *explItem = new DcmLongString(DCM_LUTExplanation);
      if (explItem)
      {
          if (result == EC_Normal) result = explItem->putString(explanation);
          if (EC_Normal == result) result = item.insert(explItem, OFTrue /*replaceOld*/);
      } else
        return EC_MemoryExhausted;
  }
  return result;
}


// ********************************************


#define SHORTCOL 3
#define LONGCOL 14

int main(int argc, char *argv[])
{
    const char *opt_inName = NULL;                     /* in file name */
    const char *opt_outName = NULL;                    /* out file name */
    const char *opt_outText = NULL;
    const char *opt_mapName = NULL;
    const char *opt_textName = NULL;
    const char *opt_explanation = NULL;
    OFCmdFloat opt_gammaValue = 1.0;
    OFCmdUnsignedInt opt_bits = 16;
    OFCmdUnsignedInt opt_entries = 256;
    OFCmdSignedInt opt_firstMapped = 0;
    OFCmdUnsignedInt opt_randomCount = 0;
    OFCmdUnsignedInt opt_randomSeed = 0;             /* default: for reproduceable results */
    OFCmdUnsignedInt opt_order = 5;
    OFCmdUnsignedInt opt_minDensity = 20;
    OFCmdUnsignedInt opt_maxDensity = 300;
    OFCmdUnsignedInt opt_illumination = 2000;
    OFCmdUnsignedInt opt_reflection = 10;
    LUT_Type opt_lutType = LUT_VOI;
    OFBool opt_inverseGSDF = OFFalse;
    OFBool opt_byteAlign = OFFalse;
    OFBool opt_replaceMode = OFTrue;
    DcmEVR opt_lutVR = EVR_OW;

    OFCommandLine cmd;
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Create DICOM look-up tables", rcsid);
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmimg-out",                   "DICOM output filename");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",           "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",                  "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

    cmd.addGroup("LUT creation options:");
      cmd.addSubGroup("LUT type:");
       cmd.addOption("--modality",     "+Tm",    "create as Modality LUT");
       cmd.addOption("--presentation", "+Tp",    "create as Presentation LUT");
       cmd.addOption("--voi",          "+Tv",    "create as VOI LUT (default)");
      cmd.addSubGroup("LUT placement:");
       cmd.addOption("--add",          "+Pa",    "add to existing transform\n(default for and only with --voi)");
       cmd.addOption("--replace",      "+Pr",    "replace existing transform\n(default for --modality and --presentation)");
      cmd.addSubGroup("LUT content:");
       cmd.addOption("--gamma",        "+Cg", 1, "[g]amma: float",
                                                 "use gamma value (default: 1.0)");
       cmd.addOption("--map-file",     "+Cm", 1, "[f]ilename: string",
                                                 "read input data from MAP file");
       cmd.addOption("--text-file",    "+Ct", 1, "[f]ilename: string",
                                                 "read input data from text file");
      cmd.addSubGroup("LUT options:");
       cmd.addOption("--inverse-gsdf", "+Og",    "apply inverse GSDF (print presentation LUT in OD)");
       cmd.addOption("--min-density",         1, "[v]alue: integer (0..65535, default: 20)",
                                                 "set min density to v (in hundreds of OD)");
       cmd.addOption("--max-density",         1, "[v]alue: integer (0..65535, default: 300)",
                                                 "set max density to v (in hundreds of OD)");
       cmd.addOption("--illumination", "+Oi", 1, "[v]alue: integer (0..65535, default: 2000)",
                                                 "set illumination to v (in cd/m^2)");
       cmd.addOption("--reflection",   "+Or", 1, "[v]alue: integer (0..65535, default: 10)",
                                                 "set reflected ambient light to v (in cd/m^2)");
      cmd.addSubGroup("LUT structure:");
       cmd.addOption("--bits",         "-b",  1, "[n]umber: integer",
                                                 "create LUT with n bit values (8..16, default: 16)");
       cmd.addOption("--entries",      "-e",  1, "[n]umber: integer",
                                                 "create LUT with n entries (1..65536, default: 256)");
       cmd.addOption("--first-mapped", "-f",  1, "[n]umber: integer",
                                                 "first input value mapped (-31768..65535, default: 0)");
       cmd.addOption("--random",       "-r",  1, "[n]umber: unsigned integer",
                                                 "perform n randomly selected permutations on the LUT");
       cmd.addOption("--random-seed",  "-rs", 1, "[n]umber: unsigned integer",
                                                 "initialize the random-number generator with n\n(default: 0, for reproducible results)");
       cmd.addOption("--order",        "-o",  1, "[n]umber: integer",
                                                 "use polynomial curve fitting algorithm with order n\n(0..99, default: 5)");
       cmd.addOption("--explanation",  "-E",  1, "[n]ame: string",
                                                 "LUT explanation (default: automatically created)");
      cmd.addSubGroup("LUT data alignment:");
       cmd.addOption("--byte-align",   "-a",     "create byte-aligned LUT\n(default for and only with 8 bit values)");
       cmd.addOption("--word-align",   "+a",     "create word-aligned LUT\n(default for 9-16 bit values)");
      cmd.addSubGroup("LUT data VR:");
       cmd.addOption("--data-ow",      "+Dw",    "write LUT Data as OW (default)");
       cmd.addOption("--data-us",      "+Du",    "write LUT Data as US");
       cmd.addOption("--data-ss",      "+Ds",    "write LUT Data as SS (minimal support)");
    cmd.addGroup("file options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--dicom-input",    "+Fi", 1, "[f]ilename: string",
                                                 "read dataset from DICOM file f");
     cmd.addOption("--text-output",    "+Fo", 1, "[f]ilename: string",
                                                 "write LUT data to tabbed text file f");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
    {
        /* check exclusive options first */
        if (cmd.hasExclusiveOption())
        {
          if (cmd.findOption("--version"))
          {
              app.printHeader(OFTrue /*print host identifier*/);
              COUT << OFendl << "External libraries used:";
#ifdef WITH_ZLIB
              COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
              COUT << " none" << OFendl;
#endif
              return 0;
           }
        }

        /* command line parameters */
        cmd.getParam(1, opt_outName);

        OFLog::configureFromCommandLine(cmd, app);

        cmd.beginOptionBlock();
        if (cmd.findOption("--modality"))
          opt_lutType = LUT_Modality;
        if (cmd.findOption("--presentation"))
          opt_lutType = LUT_Presentation;
        if (cmd.findOption("--voi"))
          opt_lutType = LUT_VOI;
        cmd.endOptionBlock();

        if ((opt_lutType == LUT_Modality) || (opt_lutType == LUT_Presentation))
            opt_replaceMode = OFTrue;
        else
            opt_replaceMode = OFFalse;
        cmd.beginOptionBlock();
        if (cmd.findOption("--add"))
        {
            app.checkConflict("--add","--modality", (opt_lutType == LUT_Modality));
            app.checkConflict("--add","--presentation", (opt_lutType == LUT_Presentation));
            opt_replaceMode = OFFalse;
        }
        if (cmd.findOption("--replace"))
            opt_replaceMode = OFTrue;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--gamma"))
            app.checkValue(cmd.getValue(opt_gammaValue));
        if (cmd.findOption("--map-file"))
            app.checkValue(cmd.getValue(opt_mapName));
        if (cmd.findOption("--text-file"))
            app.checkValue(cmd.getValue(opt_textName));
        cmd.endOptionBlock();

        if (cmd.findOption("--inverse-gsdf"))
            opt_inverseGSDF = OFTrue;
        if (cmd.findOption("--min-density"))
            app.checkValue(cmd.getValue(opt_minDensity));
        if (cmd.findOption("--max-density"))
            app.checkValue(cmd.getValue(opt_maxDensity));
        if (cmd.findOption("--illumination"))
            app.checkValue(cmd.getValue(opt_illumination));
        if (cmd.findOption("--reflection"))
            app.checkValue(cmd.getValue(opt_reflection));

        if (cmd.findOption("--bits"))
        {
            app.checkValue(cmd.getValueAndCheckMinMax(opt_bits, 8, 16));
            opt_byteAlign = (opt_bits == 8);   /* default */
        }
        if (cmd.findOption("--entries"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_entries, 1, 65536));
        if (cmd.findOption("--first-mapped"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_firstMapped, -32768, 65535));
        if (cmd.findOption("--explanation"))
            app.checkValue(cmd.getValue(opt_explanation));
        if (cmd.findOption("--random"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_randomCount, 1, 99999));
        if (cmd.findOption("--random-seed"))
            app.checkValue(cmd.getValue(opt_randomSeed));
        if (cmd.findOption("--order"))
            app.checkValue(cmd.getValueAndCheckMinMax(opt_order, 0, 99));

        cmd.beginOptionBlock();
        if (cmd.findOption("--byte-align"))
            opt_byteAlign = OFTrue;
        if (cmd.findOption("--word-align"))
            opt_byteAlign = OFFalse;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--data-ow"))
            opt_lutVR = EVR_OW;
        if (cmd.findOption("--data-us"))
            opt_lutVR = EVR_US;
        if (cmd.findOption("--data-ss"))
            opt_lutVR = EVR_SS;
        cmd.endOptionBlock();

        if (cmd.findOption("--dicom-input"))
            app.checkValue(cmd.getValue(opt_inName));
        if (cmd.findOption("--text-output"))
            app.checkValue(cmd.getValue(opt_outText));
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmmklutLogger, rcsid << OFendl);

    if ((opt_lutType == LUT_Modality) && (opt_bits != 8) && (opt_bits != 16))
    {
        OFLOG_FATAL(dcmmklutLogger, "--modality cannot be used with --bits other than 8 or 16");
        return 1;
    }
    if ((opt_bits != 8) && opt_byteAlign)
    {
        OFLOG_FATAL(dcmmklutLogger, "--byte-align cannot be used with --bits other than 8");
        return 1;
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
        OFLOG_WARN(dcmmklutLogger, "no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);

    E_TransferSyntax Xfer= EXS_LittleEndianExplicit;
    OFCondition result = EC_Normal;
    DcmFileFormat *fileformat = new DcmFileFormat();
    if (!fileformat)
    {
        OFLOG_FATAL(dcmmklutLogger, "memory exhausted");
        return 1;
    }
    DcmDataset *dataset = fileformat->getDataset();

    if (opt_inName != NULL)
    {
        OFCondition cond = fileformat->loadFile(opt_inName);
        if (! cond.good())
        {
            OFLOG_FATAL(dcmmklutLogger, "cannot open file: " << opt_inName);
            return 1;
        }
        Xfer = dataset->getOriginalXfer();
    }

    /* create Item with LUT */
    DcmItem *ditem = new DcmItem();
    if (ditem)
    {
        Uint16 *outputData = new Uint16[opt_entries];
        if (outputData == NULL)
            result = EC_MemoryExhausted;
        else
        {
            char explStr[1024];
            if (opt_explanation != NULL)
                strcpy(explStr, opt_explanation);
            else
                explStr[0] = 0;
            OFString headerStr;
            double *inputXData = NULL;
            double *inputYData = NULL;
            unsigned long inputEntries = 0;
            double inputXMax = 0;
            double inputYMax = 0;
            if (readMapFile(opt_mapName, inputXData, inputYData, inputEntries, inputXMax, inputYMax) == EC_Normal)
            {
                result = convertInputLUT((unsigned int)opt_bits, opt_entries, opt_firstMapped, inputXData, inputYData, inputEntries,
                    inputXMax, inputYMax, (unsigned int)opt_order, outputData, headerStr, explStr);
            }
            else if (readTextFile(opt_textName, inputXData, inputYData, inputEntries, inputXMax, inputYMax) == EC_Normal)
            {
                result = convertInputLUT((unsigned int)opt_bits, opt_entries, opt_firstMapped, inputXData, inputYData, inputEntries,
                    inputXMax, inputYMax, (unsigned int)opt_order, outputData, headerStr, explStr);
            } else {
                gammaLUT((unsigned int)opt_bits, opt_entries, opt_firstMapped, opt_byteAlign, opt_gammaValue, outputData, headerStr, explStr);
            }
            if (result == EC_Normal)
            {
                if (opt_inverseGSDF)
                    applyInverseGSDF((unsigned int)opt_bits, opt_entries, opt_byteAlign, (unsigned int)opt_minDensity, (unsigned int)opt_maxDensity,
                        (unsigned int)opt_illumination, (unsigned int)opt_reflection, outputData, headerStr, explStr);
                if (opt_randomCount > 0)
                    mixingUpLUT(opt_entries, opt_byteAlign, opt_randomCount, (unsigned int)opt_randomSeed, outputData, explStr);
                result = createLUT((unsigned int)opt_bits, opt_entries, opt_firstMapped, opt_byteAlign, opt_lutVR, *ditem,
                    outputData, explStr);
            }
            delete[] inputXData;
            delete[] inputYData;
            writeTextOutput(opt_outText, opt_entries, opt_firstMapped, outputData, headerStr);
        }
        delete[] outputData;
        if (EC_Normal != result)
        {
            OFLOG_FATAL(dcmmklutLogger, "could not create LUT, bailing out.");
            return 1;
        }
    } else
        result = EC_MemoryExhausted;

    if (opt_outName != NULL)
    {
        DcmSequenceOfItems *dseq = NULL;
        if (EC_Normal==result)
        {
            switch (opt_lutType)
            {
                case LUT_Modality:
                    {
                        DcmLongString modalityLUTType(DCM_ModalityLUTType);
                        modalityLUTType.putString("US"); // unspecified Modality LUT
                        DcmElement *delem = new DcmLongString(modalityLUTType);
                        if (delem)
                        {
                            ditem->insert(delem, OFTrue /*replaceOld*/);
                            dseq = new DcmSequenceOfItems(DCM_ModalityLUTSequence);
                            if (dseq)
                            {
                                dataset->insert(dseq, OFTrue);
                                dseq->insert(ditem);
                            } else
                                result = EC_MemoryExhausted;
                            delete dataset->remove(DCM_RescaleIntercept);
                            delete dataset->remove(DCM_RescaleSlope);
                            delete dataset->remove(DCM_RescaleType);
                        } else
                            result = EC_MemoryExhausted;
                    }
                    break;
                case LUT_Presentation:
                    if (!opt_replaceMode)
                    {
                        // search existing sequence
                        DcmStack stack;
                        if (EC_Normal == dataset->search(DCM_PresentationLUTSequence, stack, ESM_fromHere, OFFalse))
                            dseq=(DcmSequenceOfItems *)stack.top();
                    }
                    if (dseq == NULL)
                    {
                        dseq = new DcmSequenceOfItems(DCM_PresentationLUTSequence);
                        if (dseq)
                            dataset->insert(dseq, OFTrue);
                        else
                            result = EC_MemoryExhausted;
                    }
                    if (dseq)
                        dseq->insert(ditem);
                    if (opt_replaceMode)
                        delete dataset->remove(DCM_PresentationLUTShape);
                    break;
                case LUT_VOI:
                    if (!opt_replaceMode)
                    {
                        // search existing sequence
                        DcmStack stack;
                        if (EC_Normal == dataset->search(DCM_VOILUTSequence, stack, ESM_fromHere, OFFalse))
                            dseq=(DcmSequenceOfItems *)stack.top();
                    }
                    if (dseq == NULL)
                    {
                        dseq = new DcmSequenceOfItems(DCM_VOILUTSequence);
                        if (dseq)
                            dataset->insert(dseq, OFTrue);
                        else
                            result = EC_MemoryExhausted;
                    }
                    if (dseq)
                        dseq->insert(ditem);
                    if (opt_replaceMode)
                    {
                        delete dataset->remove(DCM_WindowCenter);
                        delete dataset->remove(DCM_WindowWidth);
                        delete dataset->remove(DCM_WindowCenterWidthExplanation);
                    }
                    break;
            }
        }

        if (result != EC_Normal)
        {
            OFLOG_FATAL(dcmmklutLogger, "Error while adding LUT to image dataset. Bailing out.");
            return 1;
        }

        OFLOG_INFO(dcmmklutLogger, "writing DICOM file ...");

        result = fileformat->saveFile(opt_outName, Xfer);
        if (result.bad())
        {
            OFLOG_FATAL(dcmmklutLogger, result.text() << ": writing file: " <<  opt_outName);
            return 1;
        }
    }

    return 0;
}


/*
 * CVS/RCS Log:
 * $Log: dcmmklut.cc,v $
 * Revision 1.47  2010-12-15 17:03:38  joergr
 * Added missing prefix "ios::" to I/O manipulator "fixed" (reported on HP-UX).
 *
 * Revision 1.46  2010-10-20 07:41:36  uli
 * Made sure isalpha() & friends are only called with valid arguments.
 *
 * Revision 1.45  2010-10-14 13:13:44  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.44  2009-11-24 14:12:56  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.43  2009-02-27 11:46:11  joergr
 * Added new command line option --word-align which was previously the default
 * for all bit values.
 * Changed behavior of option --byte-align which previously implied --bits 8.
 * Instead, --bits 8 now implies --byte-align (if not specified manually).
 *
 * Revision 1.42  2008-09-25 16:30:24  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.41  2006/08/15 16:57:01  meichel
 * Updated the code in module dcmpstat to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.40  2006/07/27 14:34:40  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.39  2005/12/08 15:46:03  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.38  2005/11/28 15:29:05  meichel
 * File dcdebug.h is not included by any other header file in the toolkit
 *   anymore, to minimize the risk of name clashes of macro debug().
 *
 * Revision 1.37  2004/02/04 15:44:38  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.36  2003/09/03 16:01:01  meichel
 * Fixed bug in command line evaluation
 *
 * Revision 1.35  2003/04/14 14:28:02  meichel
 * Added explicit typecasts in calls to pow(). Needed by Visual C++ .NET 2003.
 *
 * Revision 1.34  2002/12/04 10:41:33  meichel
 * Changed toolkit to use OFStandard::ftoa instead of sprintf for all
 *   double to string conversions that are supposed to be locale independent
 *
 * Revision 1.33  2002/11/27 15:47:48  meichel
 * Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 * Revision 1.32  2002/11/26 08:44:25  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.31  2002/09/23 18:26:05  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used (incl. preparation for future support of
 * 'config.guess' host identifiers).
 *
 * Revision 1.30  2002/08/20 12:21:52  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.29  2002/05/02 14:10:03  joergr
 * Added support for standard and non-standard string streams (which one is
 * supported is detected automatically via the configure mechanism).
 *
 * Revision 1.28  2002/04/16 14:01:25  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.27  2001/11/28 13:58:42  joergr
 * Check return value of DcmItem::insert() statements where appropriate to
 * avoid memory leaks when insert procedure fails.
 *
 * Revision 1.26  2001/11/09 16:06:03  joergr
 * Renamed some of the getValue/getParam methods to avoid ambiguities reported
 * by certain compilers.
 *
 * Revision 1.25  2001/09/28 13:47:36  joergr
 * Added check whether ios::nocreate exists.
 *
 * Revision 1.24  2001/09/26 15:36:00  meichel
 * Adapted dcmpstat to class OFCondition
 *
 * Revision 1.23  2001/06/07 14:34:08  joergr
 * Removed comment.
 *
 * Revision 1.21  2001/06/01 15:50:06  meichel
 * Updated copyright header
 *
 * Revision 1.20  2000/12/12 16:45:35  meichel
 * Minor changes to keep gcc 2.7.x on SunOS 4.1.3 happy
 *
 * Revision 1.19  2000/12/11 18:18:09  joergr
 * Added explicit typecast to keep SunCC 2.0.1 quiet.
 *
 * Revision 1.18  2000/10/16 12:26:05  joergr
 * Added new feature: create inverse GSDF (useful for printer output).
 *
 * Revision 1.17  2000/07/04 16:09:22  joergr
 * Added new option to command line tool allowing to specify the 'seed' value
 * for the random-number generator.
 *
 * Revision 1.16  2000/06/09 10:22:25  joergr
 * Added new commandline option allowing to mix-up the LUT entries based on
 * a random-number generator (useful to test correct implementation of LUTs).
 *
 * Revision 1.15  2000/05/30 14:02:43  joergr
 * Corrected typo/formatting.
 *
 * Revision 1.14  2000/03/08 16:28:41  meichel
 * Updated copyright header.
 *
 * Revision 1.13  2000/03/07 16:17:11  joergr
 * Added explicit type casts to make Sun CC 2.0.1 happy.
 *
 * Revision 1.12  2000/03/06 18:21:44  joergr
 * Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
 *
 * Revision 1.11  2000/03/03 14:13:24  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.10  2000/02/02 14:38:24  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.9  1999/10/18 10:21:31  joergr
 * Enlarged string buffer for output text file header.
 *
 * Revision 1.8  1999/10/15 09:35:05  joergr
 * Enhanced checking mechanism for input text files.
 *
 * Revision 1.7  1999/10/14 20:21:29  joergr
 * Fixed problems with MSVC.
 *
 * Revision 1.6  1999/10/14 19:08:48  joergr
 * Merged command line tool 'dconvmap' into 'dcmmklut' and enhanced its
 * facilities (e.g. integrated new polynomial curve fitting algorithm).
 *
 * Revision 1.5  1999/07/28 11:21:07  meichel
 * New options in dcmmklut:
 * - creation of LUTs with 65536 entries
 * - creation of LUT data with VR=OW, US or SS
 * - creation of LUT descriptor with VR=US or SS
 *
 * Revision 1.4  1999/05/04 15:27:22  meichel
 * Minor code purifications to keep gcc on OSF1 quiet.
 *
 * Revision 1.3  1999/05/03 14:16:37  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.2  1999/04/28 15:45:05  meichel
 * Cleaned up module dcmpstat apps, adapted to new command line class
 *   and added short documentation.
 *
 * Revision 1.1  1998/12/14 16:05:48  meichel
 * Added sample application that creates Modality and VOI LUTs
 *   with gamma curve characteristics.
 *
 * Revision 1.1  1998/11/27 14:50:19  meichel
 * Initial Release.
 *
 */
