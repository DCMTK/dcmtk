/*
 *
 *  Copyright (C) 2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRNumericMeasurementValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:49:29 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRNUMVL_H
#define DSRNUMVL_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrcodvl.h"

#include "ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for numerical measurement values
 */
class DSRNumericMeasurementValue
{

  public:

    DSRNumericMeasurementValue();

    DSRNumericMeasurementValue(const OFString &numericValue,
                               const DSRCodedEntryValue &measurementUnit);

    DSRNumericMeasurementValue(const DSRNumericMeasurementValue &numericMeasurement);

    virtual ~DSRNumericMeasurementValue();

    DSRNumericMeasurementValue &operator=(const DSRNumericMeasurementValue &numericMeasurement);

    virtual void clear();

    virtual OFBool isValid() const;

    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    virtual E_Condition readSequence(DcmItem &dataset,
                                     OFConsole *logStream);

    virtual E_Condition writeSequence(DcmItem &dataset,
                                      OFConsole *logStream) const;

    virtual E_Condition renderHTML(ostream &docStream,
                                   ostream &annexStream,
                                   size_t &annexNumber,
                                   OFConsole *logStream) const;

    const OFString &getNumericValue() const
    {
        return NumericValue;
    }

    DSRCodedEntryValue &getMeasurementUnit()
    {
        return MeasurementUnit;
    }

    const DSRNumericMeasurementValue &getValue() const
    {
        return *this;
    }

    E_Condition getMeasurementUnit(DSRCodedEntryValue &measurementUnit) const;

    E_Condition getValue(DSRNumericMeasurementValue &numericMeasurement) const;

    E_Condition setNumericValue(const OFString &numericValue);

    E_Condition setMeasurementUnit(const DSRCodedEntryValue &measurementUnit);
    
    E_Condition setValue(const DSRNumericMeasurementValue &numericMeasurement);

    E_Condition setValue(const OFString &numericValue,
                         const DSRCodedEntryValue &measurementUnit);
  

  protected:

    virtual E_Condition readItem(DcmItem &dataset,
                                 OFConsole *logStream);

    virtual E_Condition writeItem(DcmItem &dataset,
                                  OFConsole *logStream) const;

    virtual OFBool checkNumericValue(const OFString &numericValue) const;

    virtual OFBool checkMeasurementUnit(const DSRCodedEntryValue &measurementUnit) const;


  private:
  
    OFString           NumericValue;

    DSRCodedEntryValue MeasurementUnit;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrnumvl.h,v $
 *  Revision 1.1  2000-10-13 07:49:29  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
