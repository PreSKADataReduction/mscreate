//#  MSCreate.h: Class to create a MeasurementSet
//#
//#  Copyright (C) 2005
//#  ASTRON (Netherlands Foundation for Research in Astronomy)
//#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
//#
//#  This program is free software; you can redistribute it and/or modify
//#  it under the terms of the GNU General Public License as published by
//#  the Free Software Foundation; either version 2 of the License, or
//#  (at your option) any later version.
//#
//#  This program is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#  GNU General Public License for more details.
//#
//#  You should have received a copy of the GNU General Public License
//#  along with this program; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//#
//#  $Id: MSCreate.h 12842 2009-03-09 08:11:29Z diepen $

#ifndef BB_MS_MSCREATE_H
#define BB_MS_MSCREATE_H

// @file
// Class to create a MeasurementSet
// @author Ger van Diepen (diepen AT astron nl)

//# Includes
//#include <Common/LofarTypes.h>
#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
//#include <Common/lofar_vector.h>

#include <string>

//# Forward Declarations
namespace casa
{
  class String;
  class MPosition;
  class MBaseline;
  class MDirection;
  class MeasFrame;
  class MeasurementSet;
  class MSMainColumns;
  template<class T> class Block;
  template<class T> class Vector;
  template<class T> class Matrix;
  template<class T> class Cube;
}

namespace ulastai
{

  class raw_data_source
  {
  private:
    const int nbaselines;
  public:
    raw_data_source(int nb);
  public:
    int num_of_baselines()const;
    bool fetch_one();
    std::pair<int,int> antenna_pair(int bl)const;
    casa::Array<casa::Complex> data(int field,int band,int bl)const;
    casa::Array<casa::Float> sigma(int field,int band,int bl)const;
    casa::Array<casa::Bool> flags(int field,int band,int bl)const;
    double time()const;

  private:
    virtual bool do_fetch_one()=0;
    virtual std::pair<int,int> do_antenna_pair(int bl)const=0;
    virtual casa::Array<casa::Complex> do_data(int field,int band,int bl)const=0;
    virtual casa::Array<casa::Float> do_sigma(int field,int band,int bl)const=0;
    virtual casa::Array<casa::Bool> do_flags(int field,int band,int bl)const=0;
    virtual double do_time()const=0;
  };
  

  

  // @ingroup MS
  // @brief Class to create a MeasurementSet
  // @{

  // Class for creating a MeasurementSet (part).
  // It creates an MS with all required meta info (like UVW) and the correct
  // shapes of the DATA and FLAG columns. The data in these columns is set to 0.
  // Simulator software (like BBS or MeqTrees) should fill in the data.

  

  
  class mscreate
  {
  public:
    // Construct the MS with a given name.
    // The timeStep (in sec) is used by the write function
    // to calculate the time from the starting time and the timeCounter.
    // The antenna positions have to be given in ITRF coordinates as XYZ.
    // So antPos must have shape [3,nantennas].
    // If flagColumn is given and nFlagBits>0, an integer flag column is
    // created and column FLAG is mapped to it.
    mscreate (const std::string& ms_name,
	      double start_time, int npol,
	      const casa::Table& ant_tab,
	      const casa::MPosition& array_pos,
	      bool write_auto_corr);

    // Destructor
    ~mscreate();

    
  public:
    // Add the definition of the next frequency band.
    // 1, 2 or 4 polarizations can be given.
    // 1 is always XX; 2 is XX,YY; 4 is XX,XY,YX,YY.
    // The frequencies have to be given in Hz.
    // The first version assumes that all channels are adjacent and that the
    // the reference frequency is the center of the entire band. From that
    // it calculates the center frequency for each channel.
    // The second version can be used to specify the center frequency and width
    // for each channel. So both arrays must have nchannels entries.
    // Note that the total bandwidth is calculated from the minimum and
    // maximum channel frequency. Thus it is not the sum of all widths.
    // <br>It returns the id (0-relative seqnr) of the band.
    // <group>
    int add_band (int nchannels,
		 double refFreq, double chanWidth);
    int add_band (int nchannels,
		 double refFreq, const double* chanFreqs,
		 const double* chanWidths);
    // </group>

    // Add the definition of the next field (i.e. beam).
    // The angles have to be given in radians.
    // <br>It returns the id (0-relative seqnr) of the field.
    int add_field (double azimuth, double elevation);

    // Write the rows for a single time step.
    // It sets the shape of the data array.
    // All flags are set to False.
    void write_time_step(raw_data_source& rds);

    // Get the number of antennas.
    int num_of_antennas() const
    { return its_nantennas; }

    // Get the number of different polarization setups.
    int num_of_polarizations() const;

    // Get the number of exposures.
    //int nrTimes() const
    //  { return itsNrTimes; }

  private:
    // Forbid copy constructor and assignment by making them private.
    // <group>
    mscreate (const mscreate&);
    mscreate& operator= (const mscreate&);
    // </group>

    // Create the MS and fill its subtables as much as possible.
    void create_ms (const casa::String& ms_name, 
		   //const casa::Block<casa::MPosition>& antPos,
		   const casa::Table& ant_tab);

    // Set the band.
    int add_band (int nchannels,
		 double refFreq, const casa::Vector<double>& chanFreqs,
		 const casa::Vector<double>& chanWidths);

    // Add a polarization to the subtable.
    // Return the row number where it is added.
    int add_polarization (int npolarizations);

    // Fill the various subtables (at the end).
    // <group>
    void fill_antenna (const casa::Table& tab);
    void fill_feed();
    void fill_observation();
    void fill_processor();
    void fill_state();
    // </group>

    // Fill the vector of baselines itsAntBL.
    void fill_baselines();

    // Update the times in various subtables at the end of the observation.
    void update_times();

    //# Define the data.
    bool its_write_auto_corr;             //# write autocorrelations?
    int its_nbands;                     //# nr of bands
    int its_nfields;                    //# nr of fields (beams)
    int its_nantennas;                      //# nr of antennas (stations)
    //int itsNrFreq;                     //# Fixed nr of frequencies (channels)
    int its_npol_per_ant;                     //# Fixed nr of correlations (polar.)
    double its_start_time;               //# start time of observation (sec)
    double its_end_time;
  
  
  
    casa::Block<casa::Int>* its_npol;  //# nr of polarizations for each band
    casa::Block<casa::Int>* its_nchan; //# nr of channels for each band
    casa::Block<casa::Int>* its_poln;  //# rownr in POL subtable for each band
    casa::Block<casa::MBaseline>* its_ant_bl; //# Baseline vector for each antenna
    casa::MPosition*      its_array_pos; //# Position of array center
    casa::MeasFrame*      its_frame;    //# Frame to convert to apparent coordinates
    casa::Block<casa::MDirection>* its_phase_dir;   //# Phase directions of fields
    casa::MeasurementSet* its_ms;
    casa::MSMainColumns*  its_ms_col;
  };

  // @}

} // end namespace

#endif
