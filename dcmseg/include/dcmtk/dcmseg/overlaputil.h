/*
 *
 *  Copyright (C) 2023-2025, Open Connections GmbH
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
 *  Module:  dcmqi
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Interface of class OverlapUtil
 *
 */

#ifndef OVERLAPUTIL_H
#define OVERLAPUTIL_H

#include "dcmtk/dcmiod/iodtypes.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofvector.h"

class DcmSegmentation;

/** Class that analyzes the frame-based segment structures of a given segmentation object.
 *  It provides the following main functionality:
 *  - Grouping of "physical" frames by their position in space (called "logical frames"), i.e.
 *    frames that are at the same position in space are grouped together.
 *  - For every logical frame (i.e. position in space), it lists the segments found at this
 *    position together with their respective physical frame number
 *  - Return physical frame numbers for a given segment number
 *  - Building an overlap matrix that stores for each arbitrary segment pair whether
 *    they overlap or not.
 *  - Return groups of segments, that no two overlapping segments will be in the same group.
 *    This will not necessarily return the optimal solution, but a solution that should be good enough.
 *    For the method used see getNonOverlappingSegments().
 */
class OverlapUtil
{
public:
    /// Image Position Patient tuple (x,y,z)
    typedef OFVector<Float64> ImagePosition; // might be defined in respective functional group

    /// Physical Frame number with its respective position
    struct FramePositionAndNumber
    {
        /** Default constructor required for vector initialization
         */
        FramePositionAndNumber()
            : m_position()
            , m_frameNumber(0)
        {
        }
        /** Constructor
         *  @param  pos Image position
         *  @param  num Physical frame number
         */
        FramePositionAndNumber(const ImagePosition& pos, const Uint32& num)
            : m_position(pos)
            , m_frameNumber(num)
        {
        }
        /// Frame position in space
        ImagePosition m_position;
        /// Physical frame number (number of frame in DICOM object)
        Uint32 m_frameNumber;
    };

    /// Physical Frame number with its respective position
    typedef OFVector<FramePositionAndNumber> FramePositions;

    /// Logical Frame, represented and defined by various physical frames (numbers) at the same position
    typedef OFVector<Uint32> LogicalFrame;

    /// All distinct positions and for each position the physical frame numbers that can be found at it
    typedef OFVector<LogicalFrame> DistinctFramePositions;

    /// Lists frames for each segment where segment with index i is represented by the vector at index i,
    /// and index 0 is unused. I.e. index i is segment number, value is vector of physical frame numbers.
    typedef OFVector<OFVector<Uint32> > FramesForSegment;

    /// Implements comparison operator to be used for sorting of frame positions,
    /// making the sorting order depend on the coordinate given in the constructor
    struct ComparePositions
    {
        /** Constructor, used to configure coordinate position to be used for sorting
         *  @param  c Coordinate position to be used for sorting
         */
        ComparePositions(size_t c)
            : m_coordinate(c)
        {
        }

        /** Compare two frames
         *  @param  a First frame to compare
         *  @param  b Second frame to compare
         *  @return Returns true if a is less than b based on the coordinate used for sorting
         */
        bool operator()(const FramePositionAndNumber& a, const FramePositionAndNumber& b) const
        {
            return a.m_position[m_coordinate] < b.m_position[m_coordinate];
        }
        /// Coordinate position (0-2, i.e. x,x,z) to be used for sorting
        size_t m_coordinate;
    };

    /// Matrix of N x N segment numbers, where N is the number of segments.
    /// Value is 1 at x,y if x and y overlap, 0 if they don't overlap, and -1 if not initialized.
    typedef OFVector<OFVector<Sint8> > OverlapMatrix;

    /// Group of non-overlapping segments (each represented by its segment number)
    typedef OFVector<OFVector<Uint32> > SegmentGroups;

    /** Represents a segment number and a logical frame number it is found at
     */
    struct SegNumAndFrameNum
    {
        /** Constructor
         *  @param  s Segment number
         *  @param  f Logical frame number
         */
        SegNumAndFrameNum(const Uint16 s, const Uint32 f)
            : m_segmentNumber(s)
            , m_frameNumber(f)
        {
        }
        /** Default constructor
         */
        SegNumAndFrameNum()
            : m_segmentNumber(0)
            , m_frameNumber(0)
        {
        }

        /// Segment number as used in DICOM segmentation object (1-n)
        Uint16 m_segmentNumber;
        /// Logical frame number (number of frame in DistinctFramePositions vector)
        Uint32 m_frameNumber;
    };

    /// Segments and their physical frame number (inner set), grouped by their
    /// respective logical frame number (outer vector). The inner vector is not
    /// sorted by segment number but will uniquely contain each segment only once.
    /// A std::set would be more appropriate, but since this is not supported by all
    /// compilers used for DCMTK, we use a vector and check for duplicates manually.
    typedef OFVector<OFVector<SegNumAndFrameNum> > SegmentsByPosition;

    // ------------------------------------------ Methods ------------------------------------------

    /** Constructor. Use setSegmentationObject() to set the segmentation object to work with.
     */
    OverlapUtil();

    /** Destructor */
    ~OverlapUtil();

    /** Set the segmentation object to work with and clears all old data.
     *  @param  seg The segmentation object to work with (not owned by this class)
     */
    void setSegmentationObject(DcmSegmentation* seg);

    /** Clears all internal data (except segmentation object reference).
     *  This should be called whenever the input data (i.e. the underlying)
     *  DICOM segmentation object changes, before calling any other method.
     */
    void clear();

    /** Get all distinct frame positions and the physical frame numbers at this position
     *  @param  result Resulting vector of distinct frame positions
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition getFramesByPosition(DistinctFramePositions& result);

    /** Get all segments and their physical frame number, grouped by their respective logical frame number
     *  @param  result Resulting vector of segments grouped by logical frame number
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition getSegmentsByPosition(SegmentsByPosition& result);

    /** Get physical frames for a specific segment by its segment number
     *  @param  segmentNumber Segment number to get frames for (1..n)
     *  @param  frames Resulting vector of physical frame numbers (first frame is frame 0)
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition getFramesForSegment(const Uint32 segmentNumber, OFVector<Uint32>& frames);

    /** Returns computed overlap matrix
     *  @param  matrix Resulting overlap matrix
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition getOverlapMatrix(OverlapMatrix& matrix);

    /** Returns segments grouped together in a way, that no two overlapping
     *  segments will be in the same group. This method does not necessarily
     *  returns the optimal solution, but a solution that should be good enough.
     *  It is guaranteed, that segments in the same group don't overlap.
     *
     *  It is based on the idea of a greedy algorithm  that creates a first group
     *  containing the first segment. Then it goes to the next segment, checks whether
     *  it fits into the first group with no overlaps (easily checked in overlap matrix)
     *  and inserts it into that group if no overlaps exists. Otherwise, it creates a
     *  new group and continues with the next segment (trying to insert it into
     *  the first group, then second group, otherwise creates third group, and so on).
     *  @param  segmentGroups Resulting vector of segment groups, each listing the
     *          segment numbers that are in that group
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition getNonOverlappingSegments(SegmentGroups& segmentGroups);

    /** Prints segments by their position in space
     * @param  ss The stream to dump to
     */
    void printSegmentsByPosition(OFStringStream& ss);

    /** Prints segment overlap matrix to given stream
     *  @param  ss The stream to dump to
     */
    void printOverlapMatrix(OFStringStream& ss);

    /** Prints groups of non-overlapping segments (identified by their numbers)
     *  to given stream
     *  @param  ss The stream to dump to
     */
    void printNonOverlappingSegments(OFStringStream& ss);

    /** Return whether there are at least two segments, that overlap each other.
     *  The overlap matrix is used (and computed if not already done) for this purpose.
     *  @return True if overlapping segments exist, false otherwise
     */
    OFBool hasOverlappingSegments();

    /** Returns the absolute value of a floating-point number
     *  @param value The input value
     *  @return The absolute value of the input
     */
    static Float64 fabs(const Float64 value);

protected:


    /** Group physical frame positions into logical positions. This is done by sorting
     *  frames after *that* position coordinate that in its mean position difference is
     *  larger than slice thickness * 0.9. Then those frames  that are close enough to
     *  each other (i.e. distance is smaller than slice thickness * 0.01), end up at the
     *  same logical position (considered a "logical frame")
     *  TODO: This should probably not use mean values for the coordinates
     *  since in some cases, the mean difference in a slice coordinate might be close to 0
     *  if many frames are at the same position. Instead, the maximum difference, variance or
     *  something else could be used?
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition groupFramesByLogicalPosition();

    /** Builds the overlap matrix, if not already done.
     *  @return EC_Normal if successful or already existent, error otherwise
     */
    OFCondition buildOverlapMatrix();

    /** Checks if frames are parallel, i.e. if DICOM Image Position Patient is present and
     *  all frames are parallel to each other (i.e. found in the shared functional group)
     *  @return EC_Normal if parallel, SG_EC_FramesNotParallel if image orientation is not shared,
     *          error otherwise
     */
    OFCondition ensureFramesAreParallel();

    /** Groups all physical frames by their position. This also works if the physical frames
     *  have slightly different positions, i.e. if they are not exactly the same and are only
     *  "close enough" to be considered the same. Right now, the maximum distance treated equal
     *  is if distance is smaller than slice thickness * 0.01 (i.e. 1% of slice thickness).
     *  Only performs the computation, if not done before.
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition groupFramesByPosition();

    /** Checks whether the given two frames overlap
     *  @param  f1 Frame 1, provided by its physical frame number
     *  @param  f2 Frame 2, provided by its physical frame number
     *  @param  overlap Resulting overlap (overlaps if OFTrue, otherwise not)
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition checkFramesOverlap(const Uint32& f1, const Uint32& f2, OFBool& overlap);

    /** Checks whether the given two frames overlap by using comparing their pixel data
     *  by bitwise "and". This is very efficient, however, only works and is called (right now),
     *  if row*cols % 8 = 0, so we can easily extract frames as binary bitsets without unpacking them.
     *  TODO: Check whether this can be easily extended to other cases as well.
     *  @param  f1 Frame 1, provided by its physical frame number
     *  @param  f2 Frame 2, provided by its physical frame number
     *  @param  f1_data Pixel data of frame 1
     *  @param  f2_data Pixel data of frame 2
     *  @param  rows Number of rows of the frame(s), not used right now
     *  @param  cols Number of columns of the frame(s), not used right now
     *  @param  overlap Resulting overlap (overlaps if OFTrue, otherwise not)
     *  @return EC_Normal if successful, error otherwise
     */
    static OFCondition checkFramesOverlapBinary(const Uint32& f1,
                                         const Uint32& f2,
                                         const DcmIODTypes::Frame<Uint8>* f1_data,
                                         const DcmIODTypes::Frame<Uint8>* f2_data,
                                         const Uint16& rows,
                                         const Uint16& cols,
                                         OFBool& overlap);

    /** Checks whether the given two frames overlap by using comparing their pixel data
     *  after unpacking, i.e. expanding every bit to a byte, and then comparing whether the two
     *  related bytes of each frame are both non-zero. This is less efficient than checkFramesOverlapBinary(),
     *  @param  f1 Frame 1, provided by its physical frame number
     *  @param  f2 Frame 2, provided by its physical frame number
     *  @param  f1_data Pixel data of frame 1
     *  @param  f2_data Pixel data of frame 2
     *  @param  rows Number of rows of the frame(s)
     *  @param  cols Number of columns of the frame(s)
     *  @param  overlap Resulting overlap (overlaps if OFTrue, otherwise not)
     *  @return EC_Normal if successful, error otherwise
     */
    static OFCondition checkFramesOverlapUnpacked(const Uint32& f1,
                                           const Uint32& f2,
                                           const DcmIODTypes::Frame<Uint8>* f1_data,
                                           const DcmIODTypes::Frame<Uint8>* f2_data,
                                           const Uint16& rows,
                                           const Uint16 cols,
                                           OFBool& overlap);

    /** Return the most relevant (changing) coordinate, computed by multiplying
     *  x and y vectors of the image orientation and selecting the coordinate
     *  with the largest absolute value.
     *  @param  imageOrientation Image orientation patient (3 coordinates for x vector,
     *  3 coordinates for y vector )
     *  @return 0 if x, 1 if y, 2 if z, 3 if not determinable
     */
    static Uint8 identifyChangingCoordinate(const OFVector<Float64>& imageOrientation);

private:
    /// Image Orientation Patient
    OFVector<Float64> m_imageOrientation;

    /// Physical frames with their respective positions (IPP)
    FramePositions m_framePositions;

    /// Outer vector with one entry per segment. Index is the DICOM segment
    /// number where segment 1 goes to index 0, segment 2 to index 1, and so on.
    /// Inner vector contains the physical frame numbers that represent the
    /// segment.
    FramesForSegment m_framesForSegment;

    /// Logical frames, ie. physical frames with the same position are
    /// grouped together to a logical frame. For every logical frame, we
    /// store the related physical frame numbers. The logical frame number
    /// is implicitly given by the index in the vector.
    DistinctFramePositions m_logicalFramePositions;

    /// Stores for each logical frame a collection of (paired) segment and
    /// physical frame number, that exists at that position.
    SegmentsByPosition m_segmentsByPosition;

    /// Matrix that stores for each segment pair whether they overlap or not.
    /// I.e. Matrix has size N x N, where N is the number of segments.
    /// The diagonal is always 0 (no overlap), i.e. a segment never overlaps with itself.
    /// If there is an overlap, the value is 1.
    /// If the field is not initialized, the value is -1.
    OverlapMatrix m_segmentOverlapMatrix;

    //// Groups of segments that do not overlap with each other
    SegmentGroups m_nonOverlappingSegments;

    /// Reference to segmentation object to work with
    /// Must be freed outside this class.
    DcmSegmentation* m_seg;
};

#endif // OVERLAPUTIL_H

