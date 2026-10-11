/*
 * Test for DcmFileHandleLoader with single file handle strategy.
 *
 * The tests use private elements in sequence items. Each thread operates on a single item.
 * This is because navigating items and accessing item elements is not thread-safe,
 * but if each thread operates on it's own item, then the operations are thread-safe.
 *
 * All the private elements are LO string values.
 * The tests ensure that these values are correctly read back from the DICOM file.
 */
#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofrand.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcistrmh.h"


static OFLogger tfhstrmLogger = OFLog::getLogger("dcmtk.test.tfhstrm");

/// The sequence tag used for the private sequence items.
static DcmTag sequenceTag(0x8477, 0x1001, EVR_SQ);

/// Base for generating private groups for the private elements.
static Uint16 privateGroupBase = 0x8500;

#define CHECK(cond) if (cond.bad()) { COUT << "Check failed: " << cond.text() << OFendl; return cond; }

/** Gets the private group for a given group index.
 *  @param groupIndex The index of the private group.
 *  @return The private group number to be used in DICOM tags.
 */
static Uint16 getGroup(Uint16 groupIndex)
{
  return privateGroupBase + 1 + (groupIndex * 2);
}

/// generates a random string based on the given seed.
/// the same seed will generate the same string.
static OFString generateString(uint32_t seed)
{
  static constexpr char alphabet[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

  OFRandom rnd;
  rnd.seed(seed);

  Uint16 len = rnd.getRND16() % 25 + 40; // Generate a length between 40 and 64

  OFString result;
  result.reserve(len);

  for (int i = 0; i < len; ++i)
    result.push_back(alphabet[rnd.getRND16() % (sizeof(alphabet) - 1)]);

  return result;
}

/// A helper class to create a DICOM file for the tests.
class TestDataset
{
public:
  /** constructs a TestDataset object with the given file name.
   *  @param fileName The name of the DICOM file to create.
   */
  TestDataset(const OFString &fileName)
    : fileName_(fileName) {}

  /** Destructor that deletes the DICOM file created by this object.
   */
  ~TestDataset()
  {
    unlink(fileName_.c_str());
  }

  /** Creates the DICOM file with the specified number of private groups and elements per group.
   *  Each group has its own item places to the sequence.
   *  @param numGroups The number of private groups to create. (max 255)
   *  @param numElementsPerGroups The number of private elements per group. (max 255)
   *  @return EC_Normal if the file was created successfully, an error code otherwise.
   */
  OFCondition create(Uint8 numGroups, Uint8 numElementsPerGroups);

private:
  OFString fileName_;
};


OFCondition TestDataset::create(Uint8 numGroups, Uint8 numElementsPerGroups)
{
  OFCondition cond = EC_Normal;
  DcmFileFormat dcmff;
  DcmItem *dset = dcmff.getDataset();

  DcmSequenceOfItems *seq = new DcmSequenceOfItems(sequenceTag);
  cond = dset->insert(seq);
  CHECK(cond);

  for (Uint8 g = 0; g < numGroups; ++g)
  {
    DcmItem *item = new DcmItem();
    cond = seq->insert(item);
    CHECK(cond);

    Uint16 group = getGroup(g);
    Uint16 privateCreatorElement = 0x10;
    OFString privateCreatorString = "TEST_" + std::to_string(g);

    cond = item->putAndInsertOFStringArray(DcmTag(group, privateCreatorElement), privateCreatorString);
    CHECK(cond);

    for (Uint8 element = 0; element < numElementsPerGroups ; ++element)
    {
      Uint16 ndx =  group << 8 | element;
      Uint16 elementTag = 0x1000 | element;
      OFString value = generateString(ndx);

      cond = item->putAndInsertOFStringArray(DcmTag(DcmTagKey(group,elementTag), EVR_LO), value);
    }
  }

  return dcmff.saveFile(fileName_.c_str(), EXS_LittleEndianExplicit);
}

/**
 * Checks the elements of a DICOM item against expected values.
 * @param ditem The DICOM item to check.
 * @param group The group number of the private elements.
 * @param numElements The number of private elements to check.
 * @return A pair where the first element is true if all elements match the expected values, false otherwise,
 *         and the second element is an error message if the check failed.
 */
static OFPair<bool, OFString> checkItem(DcmItem* ditem, Uint16 group, Uint8 numElements)
{
  OFCondition cond = EC_Normal;
  for (Uint8 element = 0; element < numElements ; ++element)
  {
    Uint16 ndx =  group << 8 | element;
    Uint16 elementTag = 0x1000 | element;
    OFString expectedValue = generateString(ndx);
    OFString value;

    cond = ditem->findAndGetOFStringArray(DcmTag(DcmTagKey(group, elementTag), EVR_LO), value);
    if (cond.bad()) {
      return OFMake_pair(false, "Failed to find element: " + std::to_string(group) + "," + std::to_string(elementTag) + ": " + cond.text());
    }
    if (value != expectedValue) {
      return OFMake_pair(false, "Value mismatch for element: " + std::to_string(group) + "," + std::to_string(elementTag) + ": expected '" + expectedValue + "', got '" + value + "'");
    }
  }
  return OFMake_pair(true, "");
}


/// Test case for intial and lazy loading on a single thread - without locking involved.
OFTEST(dcmdata_fhstrm_single) {

  Uint8 numGroups = 32;
  Uint8 numElements = 255;

  OFString filename = "testfhs_single.dcm";

  OFCondition cond = EC_Normal;
  TestDataset testdata(filename);

  cond = testdata.create(numGroups, numElements);
  if (cond.bad()) {
    OFCHECK_FAIL("Failed to create test dataset: " << cond.text());
    return;
  }

  DcmFileFormat dcmff;
  DcmFileHandleLoader loader(filename, EDFH_Single);

  cond = loader.load(dcmff, EXS_Unknown, EGL_noChange, 16);
  if (cond.bad()) {
    OFCHECK_FAIL("Failed to load test dataset: " << cond.text());
    return;
  }

  DcmSequenceOfItems *seq = nullptr;
  cond = dcmff.getDataset()->findAndGetSequence(sequenceTag, seq);
  if (cond.bad()) {
    OFCHECK_FAIL("Failed to find sequence: " << cond.text());
    return;
  }

  if (seq->card() != numGroups) {
    OFCHECK_FAIL("Number of groups in sequence does not match expected: " << seq->card() << " != " << (int)numGroups << OFendl);
    return;
  }

  for (Uint8 g = 0; g < numGroups; ++g) {
    DcmItem* ditem = seq->getItem(g);
    Uint16 group = getGroup(g);

    OFPair<bool, OFString> checkItemResult = checkItem(ditem, group, numElements);
    if (!checkItemResult.first) {
      OFCHECK_FAIL("Error in group " << (int)g << ": " << checkItemResult.second << OFendl);
      return;
    }
  }
}

/// Thread for lazy loading elements from a DICOM sequence item.
class LazyReader : public OFThread
{
public:
  /** Constructor for the LazyReader thread.
   *  @param item the DICOM sequence item to read from.
   *  @param groupIndex group index (item index within the sequence)
   *  @param numElements number of elements in the group (item).
   */
  LazyReader(DcmItem* item, Uint8 groupIndex, Uint8 numElements)
  : ditem_(item)
  , group_(getGroup(groupIndex))
  , numElements_(numElements)
  , good_(true)
  , errMsg_("")
  {
  }

  ~LazyReader() {}

  bool isGood() const { return good_; }

  const OFString& getErrMsg() const { return errMsg_; }

  virtual void run()
  {
    OFPair<bool, OFString> checkItemResult = checkItem(ditem_, group_, numElements_);
    if (!checkItemResult.first) {
      good_ = false;
      errMsg_ = checkItemResult.second;
    }
  }

private:
  DcmItem *ditem_;
  Uint16 group_;
  Uint8 numElements_;
  bool good_;
  OFString errMsg_;
};


/// Test for lazy loading of DICOM sequence items on multiple threads after initial load
OFTEST(dcmdata_fhstrm_lazy) {

  Uint8 numGroups = 32;
  Uint8 numElements = 255;

  OFString filename = "testfhs_lazy.dcm";

  OFCondition cond = EC_Normal;
  TestDataset testdata(filename);

  cond = testdata.create(numGroups, numElements);
  if (cond.bad()) {
    OFCHECK_FAIL("Failed to create test dataset: " << cond.text());
    return;
  }

  DcmFileFormat dcmff;

  {
    DcmFileHandleLoader loader(filename, EDFH_Lazy);
    cond = loader.load(dcmff, EXS_Unknown, EGL_noChange, 16);
  }

  DcmSequenceOfItems *seq = nullptr;
  cond = dcmff.getDataset()->findAndGetSequence(sequenceTag, seq);
  if (cond.bad()) {
    OFCHECK_FAIL("Failed to find sequence: " << cond.text());
    return;
  }

  if (seq->card() != numGroups) {
    OFCHECK_FAIL("Number of groups in sequence does not match expected: " << seq->card() << " != " << (int)numGroups << OFendl);
    return;
  }

  OFVector<LazyReader*> readers;
  for (Uint8 g = 0; g < numGroups; ++g) {
    LazyReader* reader = new LazyReader(seq->getItem(g), g, numElements);
    readers.push_back( reader );
    reader->start();
  }

  for (Uint8 g = 0; g < numGroups; ++g) {
    readers[g]->join();
    if (!readers[g]->isGood()) {
      OFCHECK_FAIL("Error in group " << (int)g << ": " << readers[g]->getErrMsg() << OFendl);
    }
    delete readers[g];
  }
}


/**
 * Thread to load a particular group (item) from a DICOM file.
 * Multiple threads expected to read the same file and the same file handle concurrently.
 */
class AlwaysReader : public OFThread
{
public:
  /**
   * @brief Constructor for the AlwaysReader thread.
   * @param loader Reference to the DcmFileHandleLoader used to load the DICOM file.
   * @param groupIndex Index of the group (item) to be read from the sequence.
   * @param numElementsPerGroups Number of elements expected per group.
   */
  AlwaysReader(DcmFileHandleLoader& loader, Uint8 groupIndex, Uint8 numElements)
  : loader_(loader)
  , groupIndex_(groupIndex)
  , numElements_(numElements)
  , good_(true)
  , errMsg_("")
  {
  }

  ~AlwaysReader() {}

  bool isGood() const { return good_; }

  const OFString& getErrMsg() const { return errMsg_; }

  virtual void run()
  {
    OFCondition cond = EC_Normal;

    DcmFileFormat dcmff;
    cond = loader_.load(dcmff, EXS_Unknown, EGL_noChange, 16);

    if (cond.bad()) {
      good_ = false;
      errMsg_ = OFString("Failed to load file: ") + cond.text();
      return;
    }

    DcmSequenceOfItems *seq = nullptr;
    cond = dcmff.getDataset()->findAndGetSequence(sequenceTag, seq);
    if (cond.bad()) {
      good_ = false;
      errMsg_ = OFString("Failed to find sequence: ") + cond.text();
      return;
    }

    if (seq->card() <= groupIndex_) {
      good_ = false;
      errMsg_ = OFString("Group index is larger than the number of items in the sequence: ") + std::to_string(seq->card()) + " < " + std::to_string(groupIndex_);
      return;
    }

    DcmItem* ditem = seq->getItem(groupIndex_);
    Uint16 group = getGroup(groupIndex_);

    OFPair<bool, OFString> checkItemResult = checkItem(ditem, group, numElements_);
    if (!checkItemResult.first) {
      good_ = false;
      errMsg_ = checkItemResult.second;
    }
  }

private:
  DcmFileHandleLoader& loader_;
  Uint16 groupIndex_;
  Uint8 numElements_;
  bool good_;
  OFString errMsg_;
};


/// Test case for reading from the same DICOM file and file handle simultaneously on multiple threads
OFTEST(dcmdata_fhstrm_multi) {

  Uint8 numGroups = 32;
  Uint8 numElements = 255;

  OFString filename = "testfhs_multi.dcm";

  OFCondition cond = EC_Normal;
  TestDataset testdata(filename);

  cond = testdata.create(numGroups, numElements);
  if (cond.bad()) {
    OFCHECK_FAIL("Failed to create test dataset: " << cond.text());
    return;
  }

  DcmFileHandleLoader loader(filename, EDFH_Multi);

  OFVector<AlwaysReader*> readers;
  for (Uint8 g = 0; g < numGroups; ++g) {
    AlwaysReader* reader = new AlwaysReader(loader, g, numElements);
    readers.push_back(reader);
    reader->start();
  }

  for (Uint8 g = 0; g < numGroups; ++g) {
    readers[g]->join();
    if (!readers[g]->isGood()) {
      OFCHECK_FAIL("Error in group " << (int)g << ": " << readers[g]->getErrMsg() << OFendl);
    }
    delete readers[g];
  }
}
