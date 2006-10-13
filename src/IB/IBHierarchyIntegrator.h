#ifndef included_IBHierarchyIntegrator
#define included_IBHierarchyIntegrator

// Filename: IBHierarchyIntegrator.h
// Created on 12 Jul 2004 by Boyce Griffith (boyce@trasnaform.speakeasy.net)
// Last modified: <04.Oct.2006 19:52:35 boyce@boyce-griffiths-powerbook-g4-15.local>

/////////////////////////////// INCLUDES /////////////////////////////////////

// IBAMR INCLUDES
#include <ibamr/IBEulerianForceSetter.h>
#include <ibamr/IBEulerianSourceSetter.h>
#include <ibamr/IBLagrangianForceStrategy.h>
#include <ibamr/IBLagrangianSourceStrategy.h>
#include <ibamr/INSHierarchyIntegrator.h>
#include <ibamr/LDataManager.h>
#include <ibamr/LNodePosnInitStrategy.h>
#include <ibamr/LagSiloDataWriter.h>
#include <ibamr/SetDataStrategy.h>

// SAMRAI INCLUDES
#include <CellVariable.h>
#include <GriddingAlgorithm.h>
#include <IntVector.h>
#include <LoadBalancer.h>
#include <PatchHierarchy.h>
#include <PatchLevel.h>
#include <RefineAlgorithm.h>
#include <RefineSchedule.h>
#include <VariableContext.h>
#include <VisItDataWriter.h>
#include <tbox/Database.h>
#include <tbox/Pointer.h>

// STL INCLUDES
#include <map>
#include <vector>

/////////////////////////////// CLASS DEFINITION /////////////////////////////

namespace IBAMR
{
/*!
 * XXXX
 */
class IBHierarchyIntegrator
    : public SAMRAI::mesh::StandardTagAndInitStrategy<NDIM>,
      public SAMRAI::tbox::Serializable
{
public:
    typedef std::map<std::string,SAMRAI::tbox::Pointer<SAMRAI::xfer::RefineAlgorithm<NDIM> > >              RefineAlgMap;
    typedef std::map<std::string,std::vector<SAMRAI::tbox::Pointer<SAMRAI::xfer::RefineSchedule<NDIM> > > > RefineSchedMap;

    /*!
     * XXXX
     *
     * When assertion checking is active, passing any null pointer or
     * an empty string as an argument will result in an assertion
     * failure.
     */
    IBHierarchyIntegrator(
        const std::string& object_name,
        SAMRAI::tbox::Pointer<SAMRAI::tbox::Database> input_db,
        SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > hierarchy,
        SAMRAI::tbox::Pointer<INSHierarchyIntegrator> ins_hier_integrator,
        SAMRAI::tbox::Pointer<IBLagrangianForceStrategy> force_strategy,
        SAMRAI::tbox::Pointer<IBLagrangianSourceStrategy> source_strategy=NULL,
        bool register_for_restart=true);

    /*!
     * The destructor for IBHierarchyIntegrator unregisters the
     * integrator object with the restart manager when so registered.
     */
    virtual ~IBHierarchyIntegrator();

    /*!
     * Supply an optional cell centered body forcing term.
     *
     * NOTE: This forcing term will be added to the Eulerian force
     * density.
     */
    void registerBodyForceSpecification(
        SAMRAI::tbox::Pointer<SetDataStrategy> body_force_set);

    /*!
     * Register a concrete strategy object with the integrator that
     * specifies the initial configuration of the curvilinear mesh
     * nodes.
     */
    void registerLNodePosnInitStrategy(
        SAMRAI::tbox::Pointer<LNodePosnInitStrategy> lag_posn_init);

    /*!
     * Free the concrete initialization strategy object.
     *
     * NOTE: Be sure to call this method only once the initialization
     * object is no longer needed.
     */
    void freeLNodePosnInitStrategy();

    /*!
     * Register a VisIt data writer so this class will write plot
     * files that may be postprocessed with the VisIt visualization
     * tool.
     */
    void registerVisItDataWriter(
        SAMRAI::tbox::Pointer<SAMRAI::appu::VisItDataWriter<NDIM> > visit_writer);

    /*!
     * Register a Lagrangian Silo data writer so this class will write
     * plot files that may be postprocessed with the VisIt
     * visualization tool.
     */
    void registerLagSiloDataWriter(
        SAMRAI::tbox::Pointer<LagSiloDataWriter> silo_writer);

    /*!
     * @brief Register a load balancer for non-uniform load balancing.
     */
    void registerLoadBalancer(
        SAMRAI::tbox::Pointer<SAMRAI::mesh::LoadBalancer<NDIM> > load_balancer);

    /*!
     * @brief Gather all data to the specified root MPI process,
     * assuming that markers have larger Lagrangian indices than
     * material points.
     */
    void gatherAllData(
        const int mpi_root,
        double* const X_structure,
        const int struct_sz,
        double* const X_marker,
        const int marker_sz,
        const int level_num=-1);

    ///
    ///  The following routines:
    ///
    ///      initializeHierarchyIntegrator(),
    ///      initializeHierarchy(),
    ///      advanceHierarchy(),
    ///      atRegridPoint(),
    ///      getIntegratorTime(),
    ///      getStartTime(),
    ///      getEndTime(),
    ///      getIntegratorStep(),
    ///      getMaxIntegratorSteps(),
    ///      stepsRemaining(),
    ///      getPatchHierarchy(),
    ///      getGriddingAlgorithm()
    ///
    ///  allow the IBHierarchyIntegrator to be used as a hierarchy
    ///  integrator.
    ///

    /*!
     * XXXX.
     */
    virtual void initializeHierarchyIntegrator(
        SAMRAI::tbox::Pointer<SAMRAI::mesh::GriddingAlgorithm<NDIM> > gridding_alg);

    /*!
     * XXXX.
     */
    virtual double initializeHierarchy();

    /*!
     * Integrate data on all patches on all levels of the patch
     * hierarchy from current time (current_time) to new time
     * (new_time).
     *
     * When assertion checking is active, an unrecoverable exception
     * will result if the new time is not greater than the given time.
     */
    virtual double advanceHierarchy(
        const double dt,
        const bool rebalance_coarsest=false);

    /*!
     * Return true if the current step count indicates that regridding
     * should occur.
     */
    bool atRegridPoint() const;

    /*!
     * Return the current integration time for coarsest hierarchy
     * level.
     */
    double getIntegratorTime() const;

    /*!
     * Return the initial integration time.
     */
    double getStartTime() const;

    /*!
     * Return the final integration time.
     */
    double getEndTime() const;

    /*!
     * Return the integration step count for entire hierarchy (i.e.,
     * number of steps taken on the coarsest level).
     */
    int getIntegratorStep() const;

    /*!
     * Return the maximum number of integration steps allowed for
     * entire hierarchy (i.e., steps allowed on coarsest level).
     */
    int getMaxIntegratorSteps() const;

    /*!
     * Return true if any integration steps remain, false otherwise.
     */
    bool stepsRemaining() const;

    /*!
     * Return a const pointer to the patch hierarchy managed by integrator.
     */
    const SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > getPatchHierarchy() const;

    /*!
     * Return a pointer to the gridding algorithm object.
     */
    SAMRAI::tbox::Pointer<SAMRAI::mesh::GriddingAlgorithm<NDIM> > getGriddingAlgorithm() const;

    ///
    ///  The following routines:
    ///
    ///      rebalanceCoarsestLevel(),
    ///      regridHierarchy(),
    ///      synchronizeHierarchy(),
    ///      synchronizeNewLevels(),
    ///      resetTimeDependentHierData(),
    ///      resetHierDataToPreadvanceState()
    ///
    ///  allow the IBHierarchyIntegrator to provide data management
    ///  for a time integrator which making use of this class.
    ///

    /*!
     * Rebalance the hierarchy.
     */
    virtual void rebalanceCoarsestLevel();

    /*!
     * Regrid the hierarchy.
     */
    virtual void regridHierarchy();

    /*!
     * Synchronize the hierarchy.
     */
    virtual void synchronizeHierarchy();

    /*!
     * Coarsen current solution data from finest hierarchy level specified
     * down through the coarsest hierarchy level specified, if initial_time
     * is true.  In this case, the hierarchy is being constructed at the
     * initial simulation time,  After data is coarsened, the application-
     * specific initialization routine is called to set data before that
     * solution is further coarsened to the next coarser level in the
     * hierarchy.  This operation makes the solution consistent between
     * coarser levels and finer levels that did not exist when the coarse
     * levels where created and initialized originally.
     *
     * When initial_time is false, this routine does nothing since the
     * standard hyperbolic AMR algorithm for conservation laws requires
     * no data synchronization after regridding beyond interpolation of
     * data from coarser levels in the hierarchy in some conservative fashion.
     *
     * When assertion checking is active, an unrecoverable exception will
     * result if the hierarchy pointer is null, the level numbers do
     * not properly match existing levels in the hierarchy (either
     * coarsest_level > finest_level or some level is null).
     */
    virtual void synchronizeNewLevels(
        const SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > hierarchy,
        const int coarsest_level,
        const int finest_level,
        const double sync_time,
        const bool initial_time);

    /*!
     * Reset time dependent data.
     */
    virtual void resetTimeDependentHierData(
        const double new_time);

    /*!
     * Deallocate all new simulation data.
     */
    virtual void resetHierDataToPreadvanceState();

    ///
    ///  The following routines:
    ///
    ///      initializeLevelData(),
    ///      resetHierarchyConfiguration(),
    ///      applyGradientDetector()
    ///
    ///  are concrete implementations of functions declared in the
    ///  SAMRAI::mesh::StandardTagAndInitStrategy<NDIM> abstract base class.
    ///

    /*!
     * Initialize data on a new level after it is inserted into an AMR
     * patch hierarchy by the gridding algorithm.  The level number
     * indicates that of the new level.  The old_level pointer
     * corresponds to the level that resided in the hierarchy before
     * the level with the specified number was introduced.  If the
     * pointer is null, there was no level in the hierarchy prior to
     * the call and the level data is set based on the user routines
     * and the simulation time.  Otherwise, the specified level
     * replaces the old level and the new level receives data from the
     * old level appropriately before it is destroyed.
     *
     * Typically, when data is set, it is interpolated from coarser
     * levels in the hierarchy.  If the data is to be set, the level
     * number must match that of the old level, if non-null.  If the
     * old level is non-null, then data is copied from the old level
     * to the new level on regions of intersection between those
     * levels before interpolation occurs.  Then, user-supplied patch
     * routines are called to further initialize the data if needed.
     * The boolean argument initial_time is passed into the user's
     * routines.
     *
     * The boolean argument initial_time indicates whether the level
     * is being introduced for the first time (i.e., at initialization
     * time), or after some regrid process during the calculation
     * beyond the initial hierarchy construction.  This information is
     * provided since the initialization of the data on a patch may be
     * different in each of those circumstances.  The can_be_refined
     * boolean argument indicates whether the level is the finest
     * level allowed in the hierarchy.  This may or may not affect the
     * data initialization process depending on the problem.
     *
     * When assertion checking is active, an unrecoverable exception
     * will result if the hierarchy pointer is null, the level number
     * does not match any level in the hierarchy, or the old level
     * number does not match the level number (if the old level
     * pointer is non-null).
     */
    virtual void initializeLevelData(
        const SAMRAI::tbox::Pointer<SAMRAI::hier::BasePatchHierarchy<NDIM> > hierarchy,
        const int level_number,
        const double init_data_time,
        const bool can_be_refined,
        const bool initial_time,
        const SAMRAI::tbox::Pointer<SAMRAI::hier::BasePatchLevel<NDIM> > old_level=SAMRAI::tbox::Pointer<SAMRAI::hier::BasePatchLevel<NDIM> >(NULL),
        const bool allocate_data=true);

    /*!
     * Reset cached communication schedules after the hierarchy has
     * changed (for example, due to regidding) and the data has been
     * initialized on the new levels.  The intent is that the cost of
     * data movement on the hierarchy will be amortized across
     * multiple communication cycles, if possible.  The level numbers
     * indicate the range of levels in the hierarchy that have
     * changed.  However, this routine updates communication schedules
     * every level finer than and including that indexed by the
     * coarsest level number given.
     *
     * When assertion checking is active, an unrecoverable exception
     * will result if the hierarchy pointer is null, any pointer to a
     * level in the hierarchy that is coarser than the finest level is
     * null, or the given level numbers not specified properly; e.g.,
     * coarsest_level > finest_level.
     */
    virtual void resetHierarchyConfiguration(
        const SAMRAI::tbox::Pointer<SAMRAI::hier::BasePatchHierarchy<NDIM> > hierarchy,
        const int coarsest_level,
        const int finest_level);

    /*!
     * Set integer tags to "one" in cells where refinement of the
     * given level should occur according to some user-supplied
     * gradient criteria.  The double time argument is the regrid
     * time.  The integer "tag_index" argument is the patch descriptor
     * index of the cell centered integer tag array on each patch in
     * the hierarchy.  The boolean argument initial_time indicates
     * whether the level is being subject to refinement at the initial
     * simulation time.  If it is false, then the error estimation
     * process is being invoked at some later time after the AMR
     * hierarchy was initially constructed.  The boolean argument
     * uses_richardson_extrapolation_too is true when Richardson
     * extrapolation error estimation is used in addition to the
     * gradient detector, and false otherwise.  This argument helps
     * the user to manage multiple regridding criteria.  This
     * information is passed along to the user's patch tagging
     * routines since the application of the gradient detector may be
     * different in each case.
     *
     * When assertion checking is active, an unrecoverable exception
     * will result if the hierarchy pointer is null or the level
     * number does not match any existing level in the hierarchy.
     */
    virtual void applyGradientDetector(
        const SAMRAI::tbox::Pointer<SAMRAI::hier::BasePatchHierarchy<NDIM> > hierarchy,
        const int level_number,
        const double error_data_time,
        const int tag_index,
        const bool initial_time,
        const bool uses_richardson_extrapolation_too);

    ///
    ///  The following routines:
    ///
    ///      getCurrentContext(),
    ///      getNewContext(),
    ///      getOldContext(),
    ///      getScratchContext(),
    ///      getPlotContext()
    ///
    ///  allow access to the various variable contexts maintained by
    ///  the integrator.
    ///

    /*!
     * Return pointer to "current" variable context used by
     * integrator.  Current data corresponds to state data at the
     * beginning of a timestep, or when a new level is initialized.
     */
    SAMRAI::tbox::Pointer<SAMRAI::hier::VariableContext> getCurrentContext() const;

    /*!
     * Return pointer to "new" variable context used by integrator.
     * New data corresponds to advanced state data at the end of a
     * timestep.  The data is one timestep later than the "current"
     * data.
     */
    SAMRAI::tbox::Pointer<SAMRAI::hier::VariableContext> getNewContext() const;

    /*!
     * Return pointer to "old" variable context used by integrator.
     * Old data corresponds to an extra time level of state data used
     * for Richardson extrapolation error estimation.  The data is one
     * timestep earlier than the "current" data.
     *
     * Note that only in certain cases when using time-dependent error
     * estimation, such as Richardson extrapolation, is the returned
     * pointer will non-null.  See contructor for more information.
     */
    SAMRAI::tbox::Pointer<SAMRAI::hier::VariableContext> getOldContext() const;

    /*!
     * Return pointer to "scratch" variable context used by
     * integrator.  Scratch data typically corresponds to storage that
     * user-routines in the concrete GodunovAdvector object
     * manipulate; in particular, scratch data contains ghost cells.
     */
    SAMRAI::tbox::Pointer<SAMRAI::hier::VariableContext> getScratchContext() const;

    /*!
     * Return pointer to variable context used for plotting.  This
     * context corresponds to the data storage that should be written
     * to plot files.  Typically, this is the same as the "current"
     * context.
     */
    SAMRAI::tbox::Pointer<SAMRAI::hier::VariableContext> getPlotContext() const;

    ///
    ///  The following routines:
    ///
    ///      putToDatabase()
    ///
    ///  are concrete implementations of functions declared in the
    ///  SAMRAI::tbox::Serializable abstract base class.
    ///

    /*!
     * Write out object state to the given database.
     *
     * When assertion checking is active, database pointer must be
     * non-null.
     */
    virtual void putToDatabase(
        SAMRAI::tbox::Pointer<SAMRAI::tbox::Database> db);

    ///
    ///  The following routines:
    ///
    ///      printClassData()
    ///
    ///  are provided for your viewing pleasure.
    ///

    /*!
     * Print all data members for IBHierarchyIntegrator
     * class.
     */
    virtual void printClassData(
        ostream& os) const;

private:
    /*!
     * @brief Default constructor.
     *
     * NOTE: This constructor is not implemented and should not be
     * used.
     */
    IBHierarchyIntegrator();

    /*!
     * @brief Copy constructor.
     *
     * NOTE: This constructor is not implemented and should not be
     * used.
     *
     * @param from The value to copy to this object.
     */
    IBHierarchyIntegrator(
        const IBHierarchyIntegrator& from);

    /*!
     * @brief Assignment operator.
     *
     * NOTE: This operator is not implemented and should not be used.
     *
     * @param that The value to assign to this object.
     *
     * @return A reference to this object.
     */
    IBHierarchyIntegrator& operator=(
        const IBHierarchyIntegrator& that);

    /*!
     * Initialize the IBLagrangianForceStrategy object for the current
     * configuration of the curvilinear mesh.
     */
    void resetLagrangianForceStrategy(
        const double init_data_time,
        const bool initial_time);

    /*!
     * Initialize the IBLagrangianSourceStrategy object for the
     * current configuration of the curvilinear mesh.
     */
    void resetLagrangianSourceStrategy(
        const double init_data_time,
        const bool initial_time);

    /*!
     * Read input values, indicated above, from given database.  The
     * boolean argument is_from_restart should be set to true if the
     * simulation is beginning from restart.  Otherwise it should be
     * set to false.
     *
     * When assertion checking is active, the database pointer must be
     * non-null.  Otherwise, all your base are belong to us.
     */
    void getFromInput(
        SAMRAI::tbox::Pointer<SAMRAI::tbox::Database> db,
        bool is_from_restart);

    /*!
     * Read object state from the restart file and initialize class
     * data members.  The database from which the restart data is read
     * is determined by the object_name specified in the constructor.
     *
     * Unrecoverable Errors:
     *
     *    -   The database corresponding to object_name is not found
     *        in the restart file.
     *
     *    -   The class version number and restart version number do not
     *        match.
     *
     */
    void getFromRestart();

    /*
     * The object name is used as a handle to databases stored in
     * restart files and for error reporting purposes.  The boolean is
     * used to control restart file writing operations.
     */
    std::string d_object_name;
    bool d_registered_for_restart;

    /*
     * Pointers to the patch hierarchy and gridding algorithm objects
     * associated with this time integration object.
     *
     * The gridding algorithm provides grid generation and regridding
     * routines for the AMR hierarchy.
     */
    SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > d_hierarchy;
    SAMRAI::tbox::Pointer<SAMRAI::mesh::GriddingAlgorithm<NDIM> > d_gridding_alg;

    /*
     * We cache a pointer to the VisIt and Silo data writers to
     * register plot variables.
     */
    SAMRAI::tbox::Pointer<SAMRAI::appu::VisItDataWriter<NDIM> > d_visit_writer;
    SAMRAI::tbox::Pointer<LagSiloDataWriter> d_silo_writer;

    /*
     * We cache a pointer to the load balancer.
     */
    SAMRAI::tbox::Pointer<SAMRAI::mesh::LoadBalancer<NDIM> > d_load_balancer;

    /*
     * The INSHierarchyIntegrator is used to provide time integration
     * capabilitiy for the incompressible Navier-Stokes equations.
     */
    SAMRAI::tbox::Pointer<INSHierarchyIntegrator> d_ins_hier_integrator;

    /*
     * The LDataManager is used to coordinate the distribution of
     * Lagrangian data on the patch hierarchy.
     */
    LDataManager* d_lag_data_manager;

    /*
     * The specification and initialization information for the
     * Lagrangian data used by the integrator.
     */
    SAMRAI::tbox::Pointer<LNodePosnInitStrategy> d_lag_posn_init;

    /*
     * The force generators.
     */
    SAMRAI::tbox::Pointer<SetDataStrategy> d_body_force_set;
    SAMRAI::tbox::Pointer<IBEulerianForceSetter> d_eulerian_force_set;
    SAMRAI::tbox::Pointer<IBLagrangianForceStrategy> d_force_strategy;
    bool d_force_strategy_needs_init;

    /*
     * The source/sink generators.
     */
    SAMRAI::tbox::Pointer<IBEulerianSourceSetter> d_eulerian_source_set;
    SAMRAI::tbox::Pointer<IBLagrangianSourceStrategy> d_source_strategy;
    bool d_source_strategy_needs_init;

    /*
     * The name of the discrete delta function to employ for
     * interpolation and spreading.
     */
    std::string d_delta_fcn;
    SAMRAI::hier::IntVector<NDIM> d_ghosts, d_pres_ghosts, d_source_ghosts;

    /*
     * Integrator data read from input or set at initialization.
     */
    double d_start_time;
    double d_end_time;
    double d_grow_dt;
    int d_max_integrator_steps;

    /*
     * The number of initial cycles to perform each timestep.
     */
    int d_num_cycles;

    /*
     * The number of initial cycles to perform in order to obtain a
     * sufficiently accurate guess for P(n=1/2).
     */
    int d_num_init_cycles;

    /*
     * The regrid interval indicates the number of integration steps
     * taken between invocations of the regridding process.
     */
    int d_regrid_interval;

    /*
     * The order of accuracy of the SSP Runge-Kutta method used to
     * advance the configuration of the Lagrangian mesh.
     */
    int d_timestepping_order;

    /*
     * Integrator data that evolves during time integration and
     * maintains the state of the timestep sequence over the levels in
     * the AMR hierarchy.
     */
    double d_old_dt;
    double d_integrator_time;
    int    d_integrator_step;

    /*
     * A maximum timestep constraint over the specified time interval.
     */
    double d_dt_max;
    double d_dt_max_time_max;
    double d_dt_max_time_min;

    /*
     * Indicates whether the integrator has been initialized.
     */
    bool d_is_initialized;

    /*
     * Indicates whether the integrator should output logging
     * messages.
     */
    bool d_do_log;

    /*
     * Communications algorithms and schedules.
     */
    RefineAlgMap    d_ralgs;
    RefineSchedMap  d_rscheds;

    /*
     * Variables and variable contexts.
     */
    SAMRAI::tbox::Pointer<SAMRAI::pdat::CellVariable<NDIM,double> > d_V_var, d_W_var;
    SAMRAI::tbox::Pointer<SAMRAI::pdat::CellVariable<NDIM,double> > d_F_var;
    SAMRAI::tbox::Pointer<SAMRAI::pdat::CellVariable<NDIM,double> > d_P_var, d_Q_var;
    SAMRAI::tbox::Pointer<SAMRAI::hier::VariableContext> d_context;
    int d_V_idx, d_W_idx, d_F_idx, d_P_idx, d_Q_idx;
};
}// namespace IBAMR

/////////////////////////////// INLINE ///////////////////////////////////////

//#include <ibamr/IBHierarchyIntegrator.I>

//////////////////////////////////////////////////////////////////////////////

#endif //#ifndef included_IBHierarchyIntegrator