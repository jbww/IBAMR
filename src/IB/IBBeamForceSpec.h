#ifndef included_IBBeamForceSpec
#define included_IBBeamForceSpec

// Filename: IBBeamForceSpec.h
// Last modified: <15.Jun.2010 15:40:32 griffith@boyce-griffiths-mac-pro.local>
// Created on 22 Mar 2007 by Boyce Griffith (griffith@box221.cims.nyu.edu)

/////////////////////////////// INCLUDES /////////////////////////////////////

// IBTK INCLUDES
#include <ibtk/Stashable.h>

// SAMRAI INCLUDES
#include <tbox/AbstractStream.h>

// C++ STDLIB INCLUDES
#include <utility>
#include <vector>

/////////////////////////////// CLASS DEFINITION /////////////////////////////

namespace IBAMR
{
/*!
 * \brief Class IBBeamForceSpec encapsulates the data necessary to compute the
 * forces generated by a collection of linear beams (i.e., structures that
 * resist bending) at a single node of the Lagrangian mesh.
 *
 * Beams are connections between three particular nodes of the Lagrangian mesh.
 * IBBeamForceSpec objects are stored as IBTK::Stashable data associated with only the
 * master beam nodes in the mesh.
 */
class IBBeamForceSpec
    : public IBTK::Stashable
{
public:
    /*!
     * \note Use of this typedef appears to be needed to get g++ to parse the
     * default parameters in the class constructor.
     */
    typedef std::pair<int,int> NeighborIdxs;

    /*!
     * \brief Register this class and its factory class with the singleton
     * IBTK::StashableManager object.  This method must be called before any
     * IBBeamForceSpec objects are created.
     *
     * \note This method is collective on all MPI processes.  This is done to
     * ensure that all processes employ the same stashable ID for the
     * IBBeamForceSpec class.
     */
    static void
    registerWithStashableManager();

    /*!
     * \brief Returns a boolean indicating whether the class has been registered
     * with the singleton IBTK::StashableManager object.
     */
    static bool
    getIsRegisteredWithStashableManager();

    /*!
     * \brief Default constructor.
     */
    IBBeamForceSpec(
        const int master_idx=-1,
        const std::vector<NeighborIdxs>& neighbor_idxs=std::vector<NeighborIdxs>(),
        const std::vector<double>& bend_rigidities=std::vector<double>(),
        const std::vector<std::vector<double> >& mesh_dependent_curvatures=std::vector<std::vector<double> >());

    /*!
     * \brief Virtual destructor.
     */
    virtual
    ~IBBeamForceSpec();

    /*!
     * \return The number of beams attached to the master node.
     */
    unsigned
    getNumberOfBeams() const;

    /*!
     * \return A const reference to the master node index.
     */
    const int&
    getMasterNodeIndex() const;

    /*!
     * \return A non-const reference to the master node index.
     */
    int&
    getMasterNodeIndex();

    /*!
     * \return A const reference to the neighbor node indices for the beams
     * attached to the master node.
     */
    const std::vector<NeighborIdxs>&
    getNeighborNodeIndices() const;

    /*!
     * \return A non-const reference to the neighbor node indices for the beams
     * attached to the master node.
     */
    std::vector<NeighborIdxs>&
    getNeighborNodeIndices();

    /*!
     * \return A const reference to the bending rigidities of the beams attached
     * to the master node.
     */
    const std::vector<double>&
    getBendingRigidities() const;

    /*!
     * \return A non-const reference to the bending rigidities of the beams
     * attached to the master node.
     */
    std::vector<double>&
    getBendingRigidities();

    /*!
     * \return A const reference to the mesh-dependent curvatures of the beams
     * attached to the master node.
     */
    const std::vector<std::vector<double> >&
    getMeshDependentCurvatures() const;

    /*!
     * \return A non-const reference to the mesh-dependent curvatures of the
     * beams attached to the master node.
     */
    std::vector<std::vector<double> >&
    getMeshDependentCurvatures();

    /*!
     * \brief Return the unique identifier used to specify the IBTK::StashableFactory
     * object used by the IBTK::StashableManager to extract Stashable objects from
     * data streams.
     */
    virtual int
    getStashableID() const;

    /*!
     * \brief Return an upper bound on the amount of space required to pack the
     * object to a buffer.
     */
    virtual size_t
    getDataStreamSize() const;

    /*!
     * \brief Pack data into the output stream.
     */
    virtual void
    packStream(
        SAMRAI::tbox::AbstractStream& stream);

private:
    /*!
     * \brief Copy constructor.
     *
     * \note This constructor is not implemented and should not be used.
     *
     * \param from The value to copy to this object.
     */
    IBBeamForceSpec(
        const IBBeamForceSpec& from);

    /*!
     * \brief Assignment operator.
     *
     * \note This operator is not implemented and should not be used.
     *
     * \param that The value to assign to this object.
     *
     * \return A reference to this object.
     */
    IBBeamForceSpec&
    operator=(
        const IBBeamForceSpec& that);

    /*!
     * Indicates whether the factory has been registered with the
     * IBTK::StashableManager.
     */
    static bool s_registered_factory;

    /*!
     * The stashable ID for this object type.
     */
    static int s_stashable_id;

    /*!
     * Data required to define the beam forces.
     */
    int d_master_idx;
    std::vector<NeighborIdxs> d_neighbor_idxs;
    std::vector<double> d_bend_rigidities;
    std::vector<std::vector<double> > d_mesh_dependent_curvatures;
};
}// namespace IBAMR

/////////////////////////////// INLINE ///////////////////////////////////////

#include <ibamr/IBBeamForceSpec.I>

//////////////////////////////////////////////////////////////////////////////

#endif //#ifndef included_IBBeamForceSpec
