/*! \file unitTestKeplerPropagator.cpp
 *    Source file that defines a unit test that tests the Kepler propagator
 *    included in Tudat.
 *
 *    Path              : /Astrodynamics/Propagators/
 *    Version           : 2
 *    Check status      : Checked
 *
 *    Author            : K. Kumar
 *    Affiliation       : Delft University of Technology
 *    E-mail address    : K.Kumar@tudelft.nl
 *
 *    Checker           : E. Iorfida
 *    Affiliation       : Delft University of Technology
 *    E-mail address    : elisabetta_iorfida@yahoo.it
 *
 *    Date created      : 16 February, 2011
 *    Last modified     : 17 Feburary, 2011
 *
 *    References
 *      Melman, J. Propagate software, J.C.P.Melman@tudelft.nl, 2010.
 *
 *    Notes
 *      Test runs code and verifies result against expected value.
 *      If the tested code is erroneous, the test function returns a boolean
 *      true; if the code is correct, the function returns a boolean false.
 *
 *      Currently, this file makes use of benchmark data provided by J. Melman.
 *      In future, it is desirable that the benchmark data is the direct output
 *      of a commercial package such as STK, where are initial conditions of
 *      the simulation are known.
 *
 *      The basic_functions::ROOT_PATH variable needs to be set for this unit
 *      test to work. It must be set to the absolute path to the root directory
 *      for Tudat.
 *
 *    Copyright (c) 2010 Delft University of Technology.
 *
 *    This software is protected by national and international copyright.
 *    Any unauthorized use, reproduction or modification is unlawful and
 *    will be prosecuted. Commercial and non-private application of the
 *    software in any form is strictly prohibited unless otherwise granted
 *    by the authors.
 *
 *    The code is provided without any warranty; without even the implied
 *    warranty of merchantibility or fitness for a particular purpose.
 *
 *    Changelog
 *      YYMMDD    Author            Comment
 *      110216    K. Kumar          First creation of code.
 *      110217    E. Iorfida        Minor changes made.
 */

// Include statements.
#include "unitTestKeplerPropagator.h"

// Using declarations.
using std::cerr;
using std::endl;
using std::string;

//! Namespace for all unit tests.
namespace unit_tests
{

//! Test of implementation of Kepler propagator class.
bool testKeplerPropagator( )
{
    // Test to see if the orbit of a satellite around the Earth is correctly
    // reproduced with respect to benchmark reference data.

    // Test result initialised to false.
    bool isKeplerPropagatorErroneous = false;

    // Load benchmark data.
    // This data originates from J. Melman and is generated by the software
    // package Propagate. The benchmark data was verified against output from
    // Satellite Toolkit (STK).

    // Load file with benchmark data.
    string relativePathToBenchmarkData
            = "Astrodynamics/Propagators/twoBodyKeplerData.dat";

    string absolutePathToBenchmarkData = basic_functions::ROOT_PATH
                                         + relativePathToBenchmarkData;

    std::ifstream twoBodyKeplerData( absolutePathToBenchmarkData.c_str( ) );

    // Check if file could be opened.
    if ( !twoBodyKeplerData )
    {
          cerr << "Error: file could not be opened" << endl;
          cerr << absolutePathToBenchmarkData << endl;
    }

    // Create propagation history map for benchmark data to be stored in.
    std::map < double, State* > benchmarkKeplerPropagationHistory;

    // Declare elapsed time.
    double elapsedTime = 0.0;

    // Initialize counter.
    unsigned int twoBodyKeplerDataCounter = 0;

    // Populate propagation history map with benchmark data from file.
    while ( !twoBodyKeplerData.eof( ) )
    {
        // Store elapsed time which is given in first coloumn.
        twoBodyKeplerData >> elapsedTime;

        // Create new state in map.
        benchmarkKeplerPropagationHistory[ twoBodyKeplerDataCounter * 3600.0 ]
                = new CartesianElements;

        // Store state date from file.
        for ( unsigned int i = 0; i < 6; i++ )
        {
            twoBodyKeplerData >>
                    benchmarkKeplerPropagationHistory[
                            twoBodyKeplerDataCounter * 3600.0 ]->state( i );
        }

        // Increment counter.
        twoBodyKeplerDataCounter++;
    }

    // Close benchmark data file.
    twoBodyKeplerData.close( );

    // Run Kepler propagator simulation.

    // Create pointer to the state of satellite Asterix given in Cartesian
    // elements.
    CartesianElements* pointerToStateOfAsterix = new CartesianElements;

    // Fill initial state vector with position and
    // velocity given for Asterix.
    // Position is given in kilometers and
    // velocity is given in kilometers per second.
    pointerToStateOfAsterix->setCartesianElementX( 6.75e3 );
    pointerToStateOfAsterix->setCartesianElementY( 0.0 );
    pointerToStateOfAsterix->setCartesianElementZ( 0.0 );
    pointerToStateOfAsterix->setCartesianElementXDot( 0.0 );
    pointerToStateOfAsterix->setCartesianElementYDot( 8.0595973215 );
    pointerToStateOfAsterix->setCartesianElementZDot( 0.0 );

    // Convert initial state vector to meters from
    // kilometers.
    pointerToStateOfAsterix->state =
            unit_conversions::convertKilometersToMeters(
                    pointerToStateOfAsterix->state );

    // Create map of propagation history of Asterix using a Kepler propagator
    // and a map iterator.
    std::map < double, State* > asterixKeplerPropagationHistory;
    std::map < double, State* >::iterator
            iteratorAsterixKeplerPropagationHistory;

    // Create a pointer to new vehicle for Asterix.
    Vehicle* pointerToAsterix = new Vehicle;

    // Create pre-defined Earth object.
    CelestialBody* pointerToEarth = predefined_planets::
                                    createPredefinedPlanet(
                                            predefined_planets::earth );

    // Create Newton-Raphson object.
    NewtonRaphson* pointerToNewtonRaphson = new NewtonRaphson;

    // Create Kepler propagator object.
    KeplerPropagator keplerPropagator;

    // Set fixed output interval for output in Kepler
    // propagator object.
    keplerPropagator.setFixedOutputInterval( 3600.0 );

    // Set the propagation start time.
    keplerPropagator.setPropagationIntervalStart( 0.0 );

    // Set the propagation end time.
    keplerPropagator.setPropagationIntervalEnd( 86400.0 );

    // Set Newton-Raphson method.
    keplerPropagator.setNewtonRaphson( pointerToNewtonRaphson );

    // Add Asterix as the body that has to be propagated.
    keplerPropagator.addBody( pointerToAsterix );

    // Set the central body for Asterix
    keplerPropagator.setCentralBody( pointerToAsterix, pointerToEarth );

    // Set initial state of Asterix.
    keplerPropagator.setInitialState( pointerToAsterix,
                                      pointerToStateOfAsterix );

    // Run simulation.
    keplerPropagator.propagate( );

    // Get propagation history of Asterix.
    asterixKeplerPropagationHistory
            = keplerPropagator
              .getPropagationHistoryAtFixedOutputIntervals( pointerToAsterix );

    // Compute propagation history state data from meters to kilometers.
    for ( iteratorAsterixKeplerPropagationHistory
          = asterixKeplerPropagationHistory.begin( );
          iteratorAsterixKeplerPropagationHistory
          != asterixKeplerPropagationHistory.end( );
          iteratorAsterixKeplerPropagationHistory++ )
    {
        iteratorAsterixKeplerPropagationHistory->second->state
                = unit_conversions::convertMetersToKilometers(
                        iteratorAsterixKeplerPropagationHistory->second->state );
    }

    // Declare tolerance between benchmark data and simulation data.
    double toleranceBetweenBenchmarkAndSimulationData = 1e-6;

    // Declare difference between benchmark data and simulation data.
    double differenceKeplerData;

    // Check if results match benchmark data.
    for ( unsigned int i = 0;
          i < keplerPropagator.getPropagationIntervalStart( )
          / keplerPropagator.getFixedOutputInterval( ); i++ )
    {
        // Initialize difference in data.
        differenceKeplerData = 0.0;

        for ( unsigned int i = 0; i < 6; i++ )
        {
            differenceKeplerData
                    += mathematics::computeAbsoluteValue(
                            asterixKeplerPropagationHistory[
                                    i * keplerPropagator
                                    .getFixedOutputInterval( ) ]
                            ->state( i )
                            - benchmarkKeplerPropagationHistory[
                                    i * keplerPropagator
                                    .getFixedOutputInterval( ) ]
                            ->state( i ) );
        }

        if ( differenceKeplerData
             > toleranceBetweenBenchmarkAndSimulationData )
        {
            isKeplerPropagatorErroneous = true;

            std::cerr << "The Kepler propagator does not produce "
                      << "consistent results, as running a simulation with "
                      << "does not yield the same results as the benchmark data "
                      << "given the same initial condition." << std::endl;
        }
    }

    // Return test result.
    // If test is successful return false; if test fails, return true.
    return isKeplerPropagatorErroneous;
}

}

// End of file.
