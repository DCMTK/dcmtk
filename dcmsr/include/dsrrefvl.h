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
 *    classes: DSRReferenceValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-19 16:02:37 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRREFVL_H
#define DSRREFVL_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"

#include "ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for (composite) reference values
 */
class DSRReferenceValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default contructor
     */
    DSRReferenceValue();

    /** constructor.
     *  The UID pair is only set if it passed the validity check (see setValue()).
     ** @param  sopClassUID     referenced SOP class UID of the composite object.
     *                          (VR=UI, type 1)
     *  @param  sopInstanceUID  referenced SOP instance UID of the composite object.
     *                          (VR=UI, type 1)
     */
    DSRReferenceValue(const OFString &sopClassUID,
                      const OFString &sopInstanceUID);

    /** copy constructor
     ** @param  referenceValue  reference value to be copied (not checked !)
     */
    DSRReferenceValue(const DSRReferenceValue &referenceValue);

    /** destructor
     */
    virtual ~DSRReferenceValue();

    /** assignment operator
     ** @param  referenceValue  reference value to be copied (not checked !)
     ** @return reference to this reference value after 'referenceValue' has been copied
     */
    DSRReferenceValue &operator=(const DSRReferenceValue &referenceValue);

    /** clear all internal variables.
     *  Since an empty reference value is invalid the reference becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the current reference value is valid.
     *  The reference value is valid if SOP class UID and SOP instance UID are valid (see
     *  checkSOP...UID() for details).
     ** @return OFTrue if reference value is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the current reference value is empty.
     *  Checks whether both UIDs of the reference value are empty.
     ** @return OFTrue if value is empty, OFFalse otherwise
     */
    virtual OFBool isEmpty() const;

    /** print reference value.
     *  The output of a typical composite reference value looks like this: (BasicTextSR,"1.2.3").
     *  If the SOP class UID is unknown the UID is printed instead of the related name.
     ** @param  stream  output stream to which the reference value should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     */
    virtual E_Condition print(ostream &stream,
                              const size_t flags) const;

    /** read referenced SOP sequence from dataset.
     *  The number of items within the sequence is checked.  If error/warning output are
     *  enabled a warning message is printed if the sequence is absent or contains more than
     *  one item.
     ** @param  dataset    DICOM dataset from which the sequence should be read
     *  @param  type       value type of the sequence (valid value: "1", "2", something else)
     *                     This parameter is used for checking purpose, any difference is reported.
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition readSequence(DcmItem &dataset,
                                     const OFString &type,
                                     OFConsole *logStream);

    /** write referenced SOP sequence to dataset.
     *  If the value is empty an empty sequence (without any items) is written.
     ** @param  dataset    DICOM dataset to which the sequence should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition writeSequence(DcmItem &dataset,
                                      OFConsole *logStream) const;

    /** render composite reference value in HTML format
     ** @param  docStream    output stream to which the main HTML document is written
     *  @param  annexStream  output stream to which the HTML document annex is written
     *  @param  annexNumber  reference to the variable where the current annex number is stored.
     *                       Value is increased automatically by 1 after a new entry has been added.
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  logStream    pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition renderHTML(ostream &docStream,
                                   ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags,
                                   OFConsole *logStream) const;

    /** get SOP class UID
     ** @return current SOP class UID (might be invalid or an empty string)
     */
    inline const OFString &getSOPClassUID() const
    {
        return SOPClassUID;
    }

    /** get SOP instance UID
     ** @return current SOP instance UID (might be invalid or an empty string)
     */
    inline const OFString &getSOPInstanceUID() const
    {
        return SOPInstanceUID;
    }

    /** get reference to composite reference value
     ** @return reference to composite reference value
     */
    inline const DSRReferenceValue &getValue() const
    {
        return *this;
    }

    /** get copy of composite reference value
     ** @param  referenceValue  reference to variable in which the value should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getValue(DSRReferenceValue &referenceValue) const;

    /** set composite reference value.
     *  Before setting the reference it is checked (see check...()).  If the value is
     *  invalid the current value is not replaced and remains unchanged.
     ** @param  referenceValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setValue(const DSRReferenceValue &referenceValue);

    /** set SOP class UID and SOP instance UID value.
     *  Before setting the values they are checked (see check...()).  If the value pair is
     *  invalid the current value pair is not replaced and remains unchanged.
     ** @param  sopClassUID     referenced SOP class UID to be set
     *  @param  sopInstanceUID  referenced SOP instance UID to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setReference(const OFString &sopClassUID,
                             const OFString &sopInstanceUID);

    /** set SOP class UID value.
     *  Before setting the value is is checked (see checkSOPClassUID()).  If the value is
     *  invalid the current value is not replaced and remains unchanged.
     ** @param  sopClassUID  SOP class UID to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setSOPClassUID(const OFString &sopClassUID);

    /** set SOP instance UID value.
     *  Before setting the value is is checked (see checkSOPInstanceUID()).  If the value is
     *  invalid the current value is not replaced and remains unchanged.
     ** @param  sopInstanceUID  SOP instance UID to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setSOPInstanceUID(const OFString &sopInstanceUID);


  protected:

    /** get pointer to reference value
     ** @return pointer to reference value (never NULL)
     */
    inline DSRReferenceValue *getValuePtr()
    {
        return this;
    }

    /** read reference value from dataset
     ** @param  dataset    DICOM dataset from which the value should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition readItem(DcmItem &dataset,
                                 OFConsole *logStream);

    /** write reference value to dataset
     ** @param  dataset    DICOM dataset to which the value should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual E_Condition writeItem(DcmItem &dataset,
                                  OFConsole *logStream) const;

    /** check the specified SOP class UID for validity.
     *  The only check that is currently performed is that the UID is not empty.  Derived
     *  classes might overwrite this method for more specific tests (e.g. allowing only
     *  particular SOP classes).
     ** @param  sopClassUID   SOP class UID to be checked
     ** @return OFTrue if SOP class UID is valid, OFFalse otherwise
     */
    virtual OFBool checkSOPClassUID(const OFString &sopClassUID) const;

    /** check the specified SOP instance UID for validity.
     *  The only check that is currently performed is that the UID is not empty.  Derived
     *  classes might overwrite this method for more specific tests.
     *  @param  sopInstanceUID  SOP instance UID to be checked
     ** @return OFTrue if SOP instance UID is valid, OFFalse otherwise
     */
    virtual OFBool checkSOPInstanceUID(const OFString &sopInstanceUID) const;

    /// reference SOP class UID (VR=UI, type 1)
    OFString SOPClassUID;
    /// reference SOP instance UID (VR=UI, type 1)
    OFString SOPInstanceUID;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrrefvl.h,v $
 *  Revision 1.4  2000-10-19 16:02:37  joergr
 *  Renamed some set methods.
 *
 *  Revision 1.3  2000/10/18 17:06:00  joergr
 *  Added methods allowing direct access to certain content item values.
 *  Added doc++ comments.
 *  Made some functions inline.
 *
 *  Revision 1.2  2000/10/16 11:57:23  joergr
 *  Added methods allowing direct access to certain content item values.
 *
 *  Revision 1.1  2000/10/13 07:49:30  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
