#ifndef DCPRIVATEOB_H
#define DCPRIVATEOB_H

#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/ofstd/oflist.h"
#include <dcmtk/dcmdata/dcistrma.h>
#include <dcmtk/dcmdata/dcostrma.h>

/** Class DcmPrivateOBTag represents a private tag with OB value representation
* and undefined length. While it is not supported by DICOM standard, there are
* systems, producing images with such tags
*/
class DCMTK_DCMDATA_EXPORT DcmPrivateOBTag : public DcmElement
{
public:
    DcmPrivateOBTag(DcmTag& tag);

    OFCondition read(DcmInputStream& inStream, const E_TransferSyntax xfer, const E_GrpLenEncoding glenc = EGL_noChange, const Uint32 maxReadLength = DCM_MaxReadLength) override;
    OFCondition write(DcmOutputStream& outStream, const E_TransferSyntax oxfer, const E_EncodingType enctype, DcmWriteCache* wcache) override;

    Uint32 getLength(const E_TransferSyntax xfer = EXS_LittleEndianImplicit, const E_EncodingType enctype = EET_UndefinedLength) override;
    int compare(const DcmElement& rhs) const override;
    DcmObject* clone() const override;
    DcmEVR ident() const override;
    void print(STD_NAMESPACE ostream& out, const size_t flags = 0, const int level = 0, const char* pixelFileName = NULL, size_t* pixelCounter = NULL) override;
    unsigned long getVM() override;
    unsigned long getNumberOfValues() override;
    OFCondition verify(const OFBool autocorrect) override;

    static void setupDelimiter(OFVector<uint8_t> delimiter);

private:
    OFVector<uint8_t> bytes;
    static OFVector<uint8_t> sequenceDelimiter;
    bool reachedEnd();
    int compareValues(DcmPrivateOBTag* lhs, DcmPrivateOBTag* rhs) const;
    void printValue(STD_NAMESPACE ostream& out, const size_t flags);
};

#endif //DCPRIVATEOB_H
