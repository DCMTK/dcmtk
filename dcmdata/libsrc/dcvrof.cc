/*
 *
 *  Copyright (C) 2002-2011, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Implementation of class DcmOtherFloat
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-12-02 15:46:29 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrof.h"
#include "dcmtk/dcmdata/dcvrfl.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for UID generation */


// ********************************


DcmOtherFloat::DcmOtherFloat(const DcmTag &tag,
                             const Uint32 len)
  : DcmFloatingPointSingle(tag, len)
{
}


DcmOtherFloat::DcmOtherFloat(const DcmOtherFloat &old)
  : DcmFloatingPointSingle(old)
{
}


DcmOtherFloat::~DcmOtherFloat()
{
}


DcmOtherFloat &DcmOtherFloat::operator=(const DcmOtherFloat &obj)
{
    DcmFloatingPointSingle::operator=(obj);
    return *this;
}


OFCondition DcmOtherFloat::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmOtherFloat &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmOtherFloat::ident() const
{
    return EVR_OF;
}


OFCondition DcmOtherFloat::checkValue(const OFString & /*vm*/,
                                      const OFBool /*oldFormat*/)
{
    /* currently no checks are performed */
    return EC_Normal;
}


unsigned long DcmOtherFloat::getVM()
{
    /* value multiplicity for OF is defined as 1 */
    return 1;
}


// ********************************


OFCondition DcmOtherFloat::writeXML(STD_NAMESPACE ostream &out,
                                    const size_t flags)
{
    /* always write XML start tag */
    writeXMLStartTag(out, flags);
    /* OF data requires special handling in the Native DICOM Model format */
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* for an empty value field, we do not need to do anything */
        if (getLengthField() > 0)
        {
            char uid[100];
            /* generate a new UID but the binary data is not (yet) written. */
            /* actually, it should be a UUID in hexadecimal representation. */
            dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT);
            out << "<BulkData UUID=\"" << OFSTRING_GUARD(uid) << "\"/>" << OFendl;
        }
    } else {
        /* write XML start tag */
        writeXMLStartTag(out, flags);
        /* write element value (if loaded) */
        if (valueLoaded())
        {
            Float32 *floatValues = NULL;
            /* get and check 32 bit float data */
            if (getFloat32Array(floatValues).good() && (floatValues != NULL))
            {
                /* increase default precision - see DcmFloatingPointSingle::print() */
                const int oldPrecision = out.precision(8);
                /* we cannot use getVM() since it always returns 1 */
                const size_t count = getLengthField() / sizeof(Float32);
                /* print float values with separators */
                out << (*(floatValues++));
                for (unsigned long i = 1; i < count; i++)
                    out << "\\" << (*(floatValues++));
                /* reset i/o manipulators */
                out.precision(oldPrecision);
            }
        }
    }
    /* always write XML end tag */
    writeXMLEndTag(out, flags);
    /* always report success */
    return EC_Normal;
}


/*
 * CVS/RCS Log:
 * $Log: dcvrof.cc,v $
 * Revision 1.9  2011-12-02 15:46:29  joergr
 * Made sure that the BulkData XML element is not written for empty values.
 *
 * Revision 1.8  2011-12-02 09:54:18  joergr
 * Added dedicated writeXML() method for OF elements because the VM is always 1.
 *
 * Revision 1.7  2010-10-20 16:44:17  joergr
 * Use type cast macros (e.g. OFstatic_cast) where appropriate.
 *
 * Revision 1.6  2010-10-14 13:14:10  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.5  2010-04-23 14:30:34  joergr
 * Added new method to all VR classes which checks whether the stored value
 * conforms to the VR definition and to the specified VM.
 *
 * Revision 1.4  2009-11-04 09:58:11  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.3  2008-07-17 10:31:32  onken
 * Implemented copyFrom() method for complete DcmObject class hierarchy, which
 * permits setting an instance's value from an existing object. Implemented
 * assignment operator where necessary.
 *
 * Revision 1.2  2005-12-08 15:41:58  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2002/12/06 12:07:02  joergr
 * Added support for new value representation Other Float String (OF).
 *
 *
 */
