#include "dcmtk/dcmdata/dcprivateob.h"

OFVector<uint8_t> DcmPrivateOBTag::sequenceDelimiter;

DcmPrivateOBTag::DcmPrivateOBTag(DcmTag& tag)
    : DcmElement(tag),
    bytes()
{
}

OFCondition DcmPrivateOBTag::read(DcmInputStream& inStream, const E_TransferSyntax xfer, const E_GrpLenEncoding glenc, const Uint32 maxReadLength)
{
    if (getTransferState() == ERW_notInitialized)
    {
        errorFlag = EC_IllegalCall;
        return errorFlag;
    }
    // container to read 1 byte from stream
    uint8_t byteBuffer[1];
    // Bytes read per one read operation
    offile_off_t read;

    // Setup default delimiter, if one was not provided with setupDelimiter
    if (sequenceDelimiter.size() == 0)
    {
        sequenceDelimiter = { 0xFE, 0xFF, 0xDD, 0xE0, 0x00, 0x00, 0x00, 0x00 };
    }

    bytes.clear();

    errorFlag = inStream.status();
    if (!errorFlag.good())
    {
        return errorFlag;
    }

    if (inStream.eos())
    {
        errorFlag = EC_EndOfStream;
        return errorFlag;
    }
    else if (getTransferState() != ERW_ready)
    {
        setTransferState(ERW_inWork);

        // need at least as many bytes as delimiter, otherwise return error
        if (inStream.avail() < sequenceDelimiter.size())
        {
            errorFlag = EC_StreamNotifyClient;
            return errorFlag;
        }

        for (int i = 0; i < sequenceDelimiter.size(); i++)
        {
            read = inStream.read(byteBuffer, 1);

            if (read != 1)
            {
                errorFlag = EC_InvalidStream; // What is the error here?
                return errorFlag;
            }
            bytes.push_back(byteBuffer[0]);
        }

        // Read bytes until delimiter is reached or stream ends. Since field length is not known in advance, maxReadLength
        // has to be ignored, as it's impossible to determine how many bytes have to be skipped to reach the next tag
        while (inStream.good())
        {
            if (reachedEnd())
            {
                setLengthField(bytes.size());
                setTransferState(ERW_ready);
                return errorFlag;
            }
            else
            {
                if (inStream.eos())
                {
                    errorFlag = EC_EndOfStream;
                    return errorFlag;
                }

                read = inStream.read(byteBuffer, 1);
                if (read != 1)
                {
                    errorFlag = EC_InvalidStream;
                    return errorFlag;
                }

                bytes.push_back(byteBuffer[0]);
            }
        }

        if (inStream.eos())
        {
            errorFlag = EC_EndOfStream;
        }
        else
        {
            errorFlag = inStream.status();
        }
    }

    return errorFlag;
}

OFCondition DcmPrivateOBTag::write(DcmOutputStream& outStream, const E_TransferSyntax oxfer, const E_EncodingType enctype, DcmWriteCache* wcache)
{
    if (getTransferState() == ERW_init)
    {
        setTransferredBytes(0);
        setTransferState(ERW_inWork);
    }

    Uint32 written;
    Uint32 tagLength = getTagAndLengthSize(oxfer);
    DcmTagKey tagKey = getTag().getXTag();
    DcmTag tag(tagKey.getGroup(), tagKey.getElement(), EVR_OB);

    if (outStream.good())
    {
        // Write tag and length. If there's not enough space in the buffer, return EC_StreamNotifyClient to attempt next time
        if (outStream.avail() > tagLength)
        {
            if (getTransferredBytes() < tagLength)
            {
                errorFlag = writeTagAndLength(outStream, oxfer, written);
                setTransferredBytes(written);

                if (!errorFlag.good())
                {
                    return errorFlag;
                }
            }
        }
        else
        {
            errorFlag = EC_StreamNotifyClient;
            return errorFlag;
        }
    }
    else
    {
        errorFlag = outStream.status();
        return errorFlag;
    }

    // Write data, keep the actual bytes unchanges regardless of compression, byte order or other things. Stop when buffer is exhausted
    // (in that case return EC_StreamNotifyClient to pick up from here next time) or everything is written out.
    uint8_t buffer[1];
    int i = getTransferredBytes() - tagLength;
    while (outStream.avail() > 0 && i < bytes.size())
    {
        buffer[0] = bytes[i];
        outStream.write(buffer, 1);
        i++;
        setTransferredBytes(i + tagLength);
    }

    if (i == bytes.size())
    {
        setTransferState(ERW_ready);

        errorFlag = EC_Normal;
        return errorFlag;
    }
    else
    {
        errorFlag = EC_StreamNotifyClient;
        return errorFlag;
    }

}

Uint32 DcmPrivateOBTag::getLength(const E_TransferSyntax xfer, const E_EncodingType enctype)
{
    return bytes.size();
}

int DcmPrivateOBTag::compare(const DcmElement& rhs) const
{
    int result = DcmElement::compare(rhs);
    if (result != 0)
    {
        return result;
    }

    /* cast away constness (dcmdata is not const correct...) */
    DcmPrivateOBTag* myThis = NULL;
    DcmPrivateOBTag* myRhs = NULL;
    myThis = OFconst_cast(DcmPrivateOBTag*, this);
    myRhs = OFstatic_cast(DcmPrivateOBTag*, OFconst_cast(DcmElement*, &rhs));

    /* compare length */
    unsigned long thisLength = myThis->getLength();
    unsigned long rhsLength = myRhs->getLength();
    if (thisLength < rhsLength)
    {
        return -1;
    }
    else if (thisLength > rhsLength)
    {
        return 1;
    }
    return compareValues(myThis, myRhs);
}

DcmObject* DcmPrivateOBTag::clone() const
{
    DcmTag tag = getTag();
    DcmPrivateOBTag* newObj = new DcmPrivateOBTag(tag);

    for (auto start = bytes.begin(); start != bytes.end(); start++)
    {
        newObj->bytes.push_back(*start);
    }
    return newObj;
}

DcmEVR DcmPrivateOBTag::ident() const
{
    return DcmEVR::EVR_OB;
}

void DcmPrivateOBTag::print(STD_NAMESPACE ostream& out, const size_t flags, const int level, const char* pixelFileName, size_t* pixelCounter)
{
    // Print indentation
    DcmTag tag = getTag();
    OFString padding;
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        for (int i = 1; i < level; i++)
        {
            out << "| ";
        }
        out << "Unknown Tag & Data ";
        printValue(out, flags);
    }
    else
    {
        for (int i = 1; i < level; i++)
        {
            out << "  ";
        }

        // Print tag and VR
        out << std::hex;
        out << "(" << tag.getGroup() << "," << tag.getElement() << ")" << " OB ";

        printValue(out, flags);

        // Print length, VM and info
        out << " # " << std::dec << bytes.size() << ", 1 Unknown Tag & Data";
    }

    out << std::endl;
}

void DcmPrivateOBTag::printValue(STD_NAMESPACE ostream& out, const size_t flags)
{
    // Print bytes. First byte is separate to consistently print the '/' delimiter the following bytes. When printing, set formatting and upcast to short to print numerical value,
    // rather than treating them as characters by default
    if (bytes.empty())
    {
        return;
    }

    int i = 0, end = bytes.size();
    if (flags & DCMTypes::PF_shortenLongTagValues)
    {
        end = DCM_OptPrintLineLength / 3;
    }

    out << std::hex;
    out << (uint16_t)bytes[i];

    for (i = 1; i < end; i++)
    {
        out << "\\" << std::setw(2) << std::setfill('0') << (uint16_t)bytes[i];
    }
    if (end != bytes.size())
    {
        out << " ...";
    }
    out << std::dec;
}

unsigned long DcmPrivateOBTag::getVM()
{
    return 1;
}

unsigned long DcmPrivateOBTag::getNumberOfValues()
{
    return bytes.size();
}

OFCondition DcmPrivateOBTag::verify(const OFBool autocorrect)
{
    return OFCondition();
}

void DcmPrivateOBTag::setupDelimiter(OFVector<uint8_t> delimiter)
{
    DcmPrivateOBTag::sequenceDelimiter = delimiter;
}

bool DcmPrivateOBTag::reachedEnd()
{
    if (bytes.size() < sequenceDelimiter.size())
    {
        return false;
    }

    bool reached = true;

    for (int i = bytes.size() - sequenceDelimiter.size(), j = 0; i < bytes.size(); i++, j++)
    {
        if (bytes[i] != sequenceDelimiter[j])
        {
            reached = false;
            break;
        }
    }
    return reached;
}

int DcmPrivateOBTag::compareValues(DcmPrivateOBTag* myValue, DcmPrivateOBTag* rhsValue) const
{
    /* check for null pointers before comparing */
    if (myValue == OFnullptr || rhsValue == OFnullptr)
    {
        /* handle null pointers appropriately, e.g., treat null as less than non-null */
        if (myValue == OFnullptr && rhsValue == OFnullptr)
            return 0; // both are null, considered equal
        else if (myValue == OFnullptr)
            return -1; // null is less than non-null
        else
            return 1; // non-null is greater than null
    }
    else {
        /* Proceed with the comparison */
        for (int i = 0; i < myValue->bytes.size(); i++)
        {
            uint8_t lByte = myValue->bytes[i], rByte = rhsValue->bytes[i];
            if (lByte < rByte)
            {
                return -1;
            }
            if (lByte > rByte)
            {
                return 1;
            }
        }
        return 0;
    }
}