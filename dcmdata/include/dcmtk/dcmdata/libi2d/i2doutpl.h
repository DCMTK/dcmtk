/*
 *
 *  Copyright (C) 2001-2007, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Base class for converter from image file to DICOM
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2009-03-31 11:31:05 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef I2DOUTPL_H
#define I2DOUTPL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

class I2DOutputPlug
{

public:

  /** Constructor, initializes member variables
   *  @return none
   */
  I2DOutputPlug() : m_doAttribChecking(OFTrue), m_inventMissingType2Attribs(OFTrue),
                    m_inventMissingType1Attribs(OFTrue), m_debug(OFFalse),
                    m_logStream(NULL)
  {};

  /** Virtual function that returns a short name of the plugin.
   *  @return The name of the plugin
   */
  virtual OFString ident() =0;

  /** Virtual function that returns the Storage SOP class UID, the plugin writes.
   *  @param suppSOPs - [out] List containing supported output SOP classes
   *  @return String containing the Storage SOP class UID
   */
  virtual void supportedSOPClassUIDs(OFList<OFString> suppSOPs) =0;

  /** Outputs SOP class specific information into dataset
   * @param dataset - [in/out] Dataset to write to
   * @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition convert(DcmDataset &dataset) const =0;

  /** Do some completeness / validity checks. Should be called when
   *  dataset is completed and is about to be saved.
   *  @param dataset - [in] The dataset to check
   *  @return Error string if error occurs, empty string otherwise
   */
  virtual OFString isValid(DcmDataset& dataset) const = 0;

  /** Destructor
   *  @return none
   */
  virtual ~I2DOutputPlug() {};

  /** Sets the log stream
   *  The log stream is used to report any warnings and error messages.
   *  @param stream - [out] pointer to the log stream (might be NULL = no messages)
   *  @return none
   */
  void setLogStream(OFConsole *stream)
  {
    m_logStream = stream;
  }

  /** Sets the debug mode
   *  @param debugMode - [in] New status for debug mode
   *  @return none
   */
  virtual void setDebugMode(const OFBool& debugMode) { m_debug = debugMode; };

  /** Prints a message to the given stream.
   ** @param  stream - [out] output stream to which the message is printed
   *  @param  message1 - [in] first part of message to be printed
   *  @param  message2 - [in] second part of message to be printed
   *  @return none
   */
  static void printMessage(OFConsole *stream,
                           const OFString& message1,
                           const OFString& message2 = "")
  {
    if (stream != NULL)
    {
        stream->lockCerr() << message1 << message2 << OFendl;
        stream->unlockCerr();
    }
  }

  /** Enable/Disable basic validity checks for output dataset
   *  @param doChecks - [in] OFTrue enables checking, OFFalse turns it off.
   *  @param insertMissingType2 - [in] If true (default), missing type 2
   *         attributes are inserted automatically
   *  @param inventMissingType1 - [in] If true (default), missing type 1
   *         attributes are inserted automatically with a predefined
   *         value (if possible). An existing empty type 1 attribute is
   *         assigned a value, too.
   *  @return none
   */
  virtual void setValidityChecking(OFBool doChecks,
                                   OFBool insertMissingType2 = OFTrue,
                                   OFBool inventMissingType1 = OFTrue)
  {
    m_doAttribChecking = doChecks;
    m_inventMissingType2Attribs = insertMissingType2;
    m_inventMissingType1Attribs = inventMissingType1;
  };

protected:

  /** Checks whether a given tag exists in a dataset and provides a non-empty
   *  value. If not, the tag is inserted (if enabled) and a default value is
   *  inserted.
   *  @param key - [in] The tag to be checked/inserted
   *  @param targetDset - [in/out] The dataset to search (and insert) in
   *  @param defaultValue - [in] The default value to set
   *  @return Error string, which is empty if no error occurs.
   */
  virtual OFString checkAndInventType1Attrib(const DcmTagKey& key,
                                             DcmDataset* targetDset,
                                             const OFString& defaultValue ="") const
  {
    OFString err;
    OFBool exists = targetDset->tagExists(key);
    if (!exists && !m_inventMissingType1Attribs)
    {
      OFString err = "I2DOutputPlug: Missing type 1 attribute: "; err += DcmTag(key).getTagName(); err += "\n";
      return err;
    }
    DcmElement *elem;
    OFCondition cond = targetDset->findAndGetElement(key, elem);
    if (cond.bad() || !elem || (elem->getLength() == 0))
    {
      if (!m_inventMissingType1Attribs)
      {
        err += "I2DOutputPlug: Empty value for type 1 attribute: ";
        err += DcmTag(key).getTagName();
        err += "\n";
        return err;
      }
      //holds element to insert in item
      DcmElement *elem = NULL;
      DcmTag tag(key); OFBool wasError = OFFalse;
      //if dicom element could be created, insert in to item and modify to value
      if ( newDicomElement(elem, tag).good())
      {
          if (targetDset->insert(elem, OFTrue).good())
          {
            if (elem->putString(defaultValue.c_str()).good())
            {
              if (m_debug)
              {
                COUT << "I2DOutputPlug: Inserting missing type 1 attribute: " << tag.getTagName() << " with value " << defaultValue << OFendl;
                return err;
              }
            } else wasError = OFTrue;
          } else wasError = OFTrue;
      } else wasError = OFTrue;
      if (wasError)
      {
        err += "Unable to insert type 1 attribute "; err += tag.getTagName(); err += " with value "; err += defaultValue; err += "\n";
      }
    }
    return err;
  };


  /** Checks whether a given tag exists in a dataset (can be empty)
   *  If not, the tag is inserted (if enabled) with empty value.
   *  @param key - [in] The tag to be checked/inserted
   *  @param targetDset - [in/out] The dataset to search (and insert) in
   *  @return Error string, which is empty if no error occurs.
   */
  virtual OFString checkAndInventType2Attrib(const DcmTagKey& key,
                                             DcmDataset* targetDset,
                                             const OFString& defaultValue ="") const
  {
    OFString err;
    OFBool exists = targetDset->tagExists(key);
    if (!exists)
    {
      if (m_inventMissingType2Attribs)
      {
        //holds element to insert in item
        DcmElement *elem = NULL;
        DcmTag tag(key); OFBool wasError = OFFalse;
        //if dicom element could be created, insert in to item and modify to value
        if ( newDicomElement(elem, tag).good())
        {
          if (targetDset->insert(elem, OFTrue).good())
          {
            OFCondition result;
            if (!defaultValue.empty()) // only insert value if not empty(e. g. empty type 2 sequences)
            {
              result = elem->putString(defaultValue.c_str());
            }
            if (result.good())
            {
              if (m_debug)
              {
                COUT << "I2DOutputPlug: Inserting missing type 2 attribute: " << tag.getTagName() << " with value " << (defaultValue.empty() ? "<empty>" : defaultValue) << OFendl;
                return err;
              }
            } else wasError = OFTrue;
          } else wasError = OFTrue;
        } else wasError = OFTrue;
        if (wasError)
        {
          err += "Unable to insert type 2 attribute "; err += tag.getTagName(); err += " with value "; err += defaultValue; err += "\n";
        }        
      }
      else
      {
        err = "Image2Dcm: Missing type 2 attribute: "; err += DcmTag(key).getTagName(); err += "\n";
        return err;
      }
    }
    return err;
  };

  /// if enabled, some simple attribute checking is performed
  /// default: enabled (OFTrue)
  OFBool m_doAttribChecking;

  /// if enabled, missing type 2 attributes in the dataset are added automatically.
  /// default: enabled (OFTrue)
  OFBool m_inventMissingType2Attribs;

  /// if enbled, missing type 1 attributes are inserted and filled with a
  /// predefined value. Default: disabled (OFFalse)
  OFBool m_inventMissingType1Attribs;

  /// debug mode status
  OFBool m_debug;

  /// stream where warning/error message are sent to.
  /// can be NULL (default, no output).
  OFConsole *m_logStream;

};

#endif // #ifndef I2DOUTPL_H

/*
 * CVS/RCS Log:
 * $Log: i2doutpl.h,v $
 * Revision 1.6  2009-03-31 11:31:05  onken
 * Corrected commit message.
 *
 * Revision 1.5  2009-03-31 11:29:54  onken
 * Added possibility to also insert type 2 elements with a default value
 * when automatically inserting missing values (feature currently not in use).
 *
 * Revision 1.4  2009-01-16 09:51:55  onken
 * Completed doxygen documentation for libi2d.
 *
 * Revision 1.3  2008-01-16 16:32:23  onken
 * Fixed some empty or doubled log messages in libi2d files.
 *
 * Revision 1.2  2008-01-16 15:10:20  onken
 * Moved library "i2dlib" from /dcmdata/libsrc/i2dlib to /dcmdata/libi2d
 *
 * Revision 1.2  2008-01-11 14:17:53  onken
 * Added various options to i2dlib. Changed logging to use a configurable
 * logstream. Added output plugin for the new Multiframe Secondary Capture SOP
 * Classes. Added mode for JPEG plugin to copy exsiting APPn markers (except
 * JFIF). Changed img2dcm default behaviour to invent type1/type2 attributes (no
 * need for templates any more). Added some bug fixes.
 *
 * Revision 1.1  2007/11/08 15:58:55  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */

