/*

##############################################################################
#                                                                            #
#              LICHEM: Fields Layered Under Kohn-sham Electrons               #
#                             By: Eric G. Kratz                              #
#                                                                            #
##############################################################################

 Headers and globals for LICHEM.

*/

//Make including safe
#ifndef LICHEM_HEADERS
#define LICHEM_HEADERS

//Header Files
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <complex>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <sys/stat.h>
#include <algorithm>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <Eigen/QR>
#include <Eigen/SVD>
#include <Eigen/Eigenvalues>
#include <Eigen/StdList>
#include <Eigen/Eigen>
#include <Eigen/StdVector>
using namespace Eigen;
using namespace std;

//Compile options
const bool Jokes = 0; //Print humorous comments
const bool Isotrop = 1; //Force isotropic expansion in NPT Monte Carlo
const double StepMin = 0.01; //Minimum Monte Carlo step size
const double StepMax = 1.0; //Maximum Monte Carlo step size
const double Centratio= 5.0; //Scales step size for path-integral centroids
const int Acc_Check = 2000; //Eq Monte Carlo steps before checking accratio

//Move Probabilities for PIMC
//Note: These probabilities allow for multi-particle moves
double BeadProb = 0.55; //Probability to move a single bead
double CentProb = 0.55; //Probability to move a centroid
double VolProb = 0.10; //Volume change probability

//Global exact constants
const double pi = 4*atan(1); //Pi
const double sqrt2 = pow(2,0.5); //Square root of 2
const double HugeNum = 1e50; //Large number to reject step
const double fs2s = 1e-12; //Convert fs to s
const double m2Ang = 1.0e10; //Angstroms to meters
const double atm2Pa = 1.01325e5; //Atmospheres to Pascal

//Global measured constants (NIST, CODATA 2010)
const double EpsZero = 8.54187817e-12; //Electric constant
const double hbar = 6.58211928e-16; //Reduced Planck Constant (eV)
const double k = 8.6173324e-5; //Boltzmann constant (eV)
const double kSI = 1.3806488e-23; //Boltzmann constant (SI)
const double amu2kg = 1.660538921e-27; //Atomic mass units to kg
const double SI2eV = 1/(1.602176565e-19); //Convert SI to eV
const double Masse = 9.10938291e-31; //Mass of an electron (kg)
const double BohrRad = 0.52917721092; //Bohr radius (Ang)
const double Har2eV = 27.21138505; //Hartrees to eV
const double Na = 6.02214129e23; //Avogadro's number
const double Debye2au = 0.393430307; //Convert from Debye to au

//Global derived constants
const double atm2eV = SI2eV*atm2Pa/(m2Ang*m2Ang*m2Ang); //atmA^3 to eV
const double C2eV = m2Ang/(4*pi*SI2eV*EpsZero); //Coulomb to eV
const double ElecMass = Masse/amu2kg; //Mass of an electron (amu)
const double h = 2*pi*hbar; //Planck Constant (eV)
const double ToeV = amu2kg*SI2eV/(m2Ang*m2Ang); //Convert to eV units (PIMC)
const double kcal2eV = 4184*SI2eV/Na; //kcal/mol to eV

//Globals
int GlobalSys; //Global dummy return for all system calls
string xyzfilename; //Saves a filename given in the arguments
string confilename; //Saves a filename given in the arguments
string regfilename; //Saves a filename given in the arguments
int Nthreads = 1;
int Ncpus = 1; //Number of processors for QM calculations
int Nfreeze = 0; //Number of frozen atoms
int Npseudo = 0; //Number of pseudo-bonds
int Nbound = 0; //Number of boundary-atoms
int Natoms = 0; //Total number of atoms
int Nqm = 0; //Number of QM atoms
int Nmm = 0; //Number of MM atoms
double step = StepMin; //PIMC step size
double Lx = 10000.0; //Box length
double Ly = 10000.0; //Box length
double Lz = 10000.0; //Box length

//Flags for simulation options
int GEM = 0; //Flag for frozen density QMMM potential
int AMOEBA = 0; //Flag for polarizable QMMM potential
int CHRG = 0; //Flag for point-charge QMMM potential
int PSI4 = 0; //Wrapper flag
int NWChem = 0; //Wrapper flag
int Gaussian = 0; //Wrapper flag
int TINKER = 0; //Wrapper flag
int LAMMPS = 0; //Wrapper flag
int AMBER = 0; //Wrapper flag
bool PBCon = 0; //Flag for the boundary conditions
bool QMMM = 0; //Flag for the type of wrapper
bool MMonly = 0; //Flag for the type of wrapper
bool QMonly = 0; //Flag for the type of wrapper
bool OptSim = 0; //Flag for energy minimization with QM packages
bool SteepSim = 0; //Flag for steepest descent minimization in LICHEM
bool DFPSim = 0; //Flag for DFP minimization in LICHEM
bool NEBSim = 0; //Flag for NEB path optimization in LICHEM
bool ESDSim = 0; //Flag for ensemble steepest descent
bool PIMCSim = 0; //Flag for Monte Carlo
bool ENEBSim = 0; //Flag for ensemble NEB reaction paths
bool SinglePoint = 0; //Flag for energy calculation
bool GauExternal = 0; //Runs Gaussian with External

//Timers
int StartTime = 0; //Time the calculation starts
int EndTime = 0; //Time the calculation ends
int QMTime = 0; //Sum of QM wrapper times
int MMTime = 0; //Sum of MM wrapper times

//Custom data types
struct Coord
{
  double x; //x position
  double y; //y position
  double z; //z position
};

struct Mpole
{
  //Cartesian multipoles
  //Atoms for the local frame of reference
  bool ChiralFlip; //Flip y axis
  string Type; //Bisector, Z-then-X, Z-Only, 3-Fold, or Z-Bisect
  int Atom1; //Atom which defines the z axis
  int Atom2; //Atom which defines the x axis
  int Atom3; //Atom which defines the y axis (chiral only)
  //Monopole moment
  double q;
  //Cartesian dipole moments
  double Dx;
  double Dy;
  double Dz;
  //Cartesian induced dipole moments (global frame)
  double IDx;
  double IDy;
  double IDz;
  //Cartesian quadrupole moments (Q_ij = Q_ji)
  double Qxx;
  double Qxy;
  double Qxz;
  double Qyy;
  double Qyz;
  double Qzz;
};

struct RedMpole
{
  //Reduced multipole from sph. harm. and diagonalization
  //Monopole
  double Q00;
  //Dipole moments
  double Q10; //Z component
  double Q11c; //X component
  double Q11s; //Y component
  //Quadrupole moments
  double Q20; //Z^2 component
  double Q22c; //X^2-Y^2 component
  //Spherical harmonic vectors
  Vector3d Vecx; //X direction in quadrupole frame
  Vector3d Vecy; //Y direction in quadrupole frame
  Vector3d Vecz; //Z direction in quadrupole frame
};

struct OctCharges
{
  //A grid of point-charges which replaces multipoles
  double q1; //Charge in the +x direction
  double q2; //Charge in the +y direction
  double q3; //Charge in the +z direction
  double q4; //Charge in the -x direction
  double q5; //Charge in the -y direction
  double q6; //Charge in the -z direction
  //Positions of the charges in the global frame
  double x1;
  double y1;
  double z1;
  double x2;
  double y2;
  double z2;
  double x3;
  double y3;
  double z3;
  double x4;
  double y4;
  double z4;
  double x5;
  double y5;
  double z5;
  double x6;
  double y6;
  double z6;
};

class GauDen1s
{
  //Simple 1s Gaussian class
  private:
    //Properties in a.u.
    double mag; //Magnitude/population (prefactor)
    double wid; //Width
    double q; //Nuclear charge
    double x; //X position
    double y; //Y position
    double z; //Z position
  public:
    //Constructor
    GauDen1s(double mag,double wid,double q,double x,double y,double z)
    {
      //Convert to a.u.
      wid /= BohrRad;
      x /= BohrRad;
      y /= BohrRad;
      z /= BohrRad;
      return;
    }
    //Electron density integrals
    double TwoOver(GauDen1s); //Density-density overlap
    double OneCoulPC(double,Coord,double); //Density-charge (MM)
    double OneCoulNuc(GauDen1s,double); //Density-nucleus
    double TwoCoul(GauDen1s,double); //Density-density Coulomb repulsion
};

struct QMMMAtom
{
  //Data type for atomic information
  double m; //Mass of atom
  bool QMregion; //QM, MM, pseudo-bond, or boundary-atom
  bool MMregion; //QM, MM, pseudo-bond, or boundary-atom
  bool PBregion; //QM, MM, pseudo-bond, or boundary-atom
  bool BAregion; //QM, MM, pseudo-bond, or boundary-atom
  bool Frozen; //Part of a frozen shell
  string QMTyp; //Real atom type
  string MMTyp; //Force field atom type
  int NumTyp; //Numerical atom type (if used)
  int NumClass; //Numerical atom class (if used)
  int id; //Atom number, starts at zero
  vector<int> Bonds; //Connectivity
  double Ep; //Storage for PI energies
  vector<Coord> P; //Array of beads
  vector<Mpole> MP; //Multipoles
  vector<OctCharges> PC; //Point-charge multipoles
};

struct QMMMElec
{
  //Data type for electronic information (eFF model)
  string typ; //Lepton type
  double m; //mass (amu)
  double q; //Charge (au)
  int spin; //Spin
  double Ep; //Temporary energy for parallel
  vector<Coord> P; //Bead coordinates
  vector<double> rad; //Radius (Ang)
};

struct QMMMSettings
{
  //Input needed for QM wrappers
  string Func; //DFT functional
  string Basis; //Basis set for QM calculations
  string RAM; //Ram for QM calculations
  bool MemMB; //Is the RAM in mb or gb
  string Charge; //QM total charge
  string Spin; //QM total spin
  //Input needed for MC and MD functions
  string Ensemble; //NVT or NPT
  double Temp; //Temperature
  double Beta; //Inverse temperature
  double Press; //External pressure
  int Neq; //Number of equilibration run steps
  int Nsteps; //Number of production run steps
  int Nbeads; //Number of time-slices or beads
  double accratio; //Target acceptance ratio
  int Nprint; //Number of steps before printing
  double dt; //MD timestep
  double tautemp; //Thermostat time constant
  double taupress; //Barostat time constant
  //Input needed for optimizations
  int MaxOptSteps; //Maximum iterative optimization steps
  double MMOptTol; //Criteria to end the optimization
  double QMOptTol; //Criteria to end the optimization
  double StepScale; //Steepest descent step size (Ang)
  double MaxStep; //Maximum size of the optimization step
  //Input needed for reaction paths
  double Kspring; //Elastic band spring constant
  //Storage of energies (PIMC)
  double Eold;
};

//Function declarations (alphabetical)
double AMBERForces(vector<QMMMAtom>&,vector<Coord>&,QMMMSettings&,int);

double AMBEREnergy(vector<QMMMAtom>&,QMMMSettings&,int);

double AMBEROpt(vector<QMMMAtom>&,QMMMSettings&,int);

double Bohring(double);

void BurstTraj(vector<QMMMAtom>&,QMMMSettings&);

RedMpole Cart2SphHarm(Mpole&);

bool CheckFile(const string&);

double CoordDist2(Coord&,Coord&);

double EFFCorr(QMMMElec&,QMMMElec&,int);

double EFFEnergy(QMMMAtom&,QMMMElec&,int);

void EnsembleNEB(vector<QMMMAtom>&,fstream&,QMMMSettings&);

void EnsembleSD(vector<QMMMAtom>&,fstream&,QMMMSettings&,int);

VectorXd EnsembleTangent(vector<QMMMAtom>&,QMMMSettings&,int);

void ExternalGaussian(int&,char**&);

void ExtractGlobalPoles(int& argc, char**& argv);

void ExtractTINKpoles(vector<QMMMAtom>&,int);

void FindTINKERClasses(vector<QMMMAtom>&);

void GaussianCharges(vector<QMMMAtom>&,QMMMSettings&,int);

double GaussianEnergy(vector<QMMMAtom>&,QMMMSettings&,int);

double GaussianForces(vector<QMMMAtom>&,vector<Coord>&,QMMMSettings&,int);

double GaussianOpt(vector<QMMMAtom>&,QMMMSettings&,int);

double Get_EeFF(vector<QMMMAtom>&,vector<QMMMElec>&,QMMMSettings&);

double Get_PI_Espring(vector<QMMMAtom>&,QMMMSettings&);

double Get_PI_Epot(vector<QMMMAtom>&,QMMMSettings&);

void GetQuotes(vector<string>&);

VectorXd KabschDisplacement(MatrixXd&,MatrixXd&,int);

void KabschRotation(MatrixXd&,MatrixXd&,int);

double KineticE_eFF(vector<QMMMElec>&,QMMMSettings&);

double LAMMPSEnergy(vector<QMMMAtom>&,QMMMSettings&,int);

double LAMMPSForces(vector<QMMMAtom>&,vector<Coord>&,QMMMSettings&,int);

double LAMMPSOpt(vector<QMMMAtom>&,QMMMSettings&,int);

void LICHEMDFP(vector<QMMMAtom>&,QMMMSettings&,int);

void LICHEMErrorChecker(QMMMSettings&);

void LICHEMNEB(vector<QMMMAtom>&,QMMMSettings&);

void LICHEMPrintSettings(QMMMSettings&);

void LICHEMSteepest(vector<QMMMAtom>&,QMMMSettings&,int);

void LICHEM2TINK(int&,char**&);

bool MCMove(vector<QMMMAtom>&,QMMMSettings&,double&);

void NWChemCharges(vector<QMMMAtom>&,QMMMSettings&,int);

double NWChemEnergy(vector<QMMMAtom>&,QMMMSettings&,int);

double NWChemForces(vector<QMMMAtom>&,vector<Coord>&,QMMMSettings&,int);

double NWChemOpt(vector<QMMMAtom>&,QMMMSettings&,int);

bool OptConverged(vector<QMMMAtom>&,vector<QMMMAtom>&,vector<Coord>&,
     int,QMMMSettings& QMMMOpts,int,bool);

bool PathConverged(vector<QMMMAtom>&,vector<QMMMAtom>&,
     vector<vector<double> >&,int,QMMMSettings& QMMMOpts,bool);

void PrintFancyTitle();

void Print_traj(vector<QMMMAtom>&,fstream&,QMMMSettings&);

void PSICharges(vector<QMMMAtom>&,QMMMSettings&,int);

double PSIEnergy(vector<QMMMAtom>&,QMMMSettings&,int);

double PSIForces(vector<QMMMAtom>&,vector<Coord>&,QMMMSettings&,int);

double PSIOpt(vector<QMMMAtom>&,QMMMSettings&,int);

void ReadArgs(int&,char**&,fstream&,fstream&,fstream&,fstream&);

void ReadLICHEMInput(fstream&,fstream&,fstream&,
     vector<QMMMAtom>&,QMMMSettings&);

int RevTyping(string);

void RotateTINKCharges(vector<QMMMAtom>&,int);

OctCharges SphHarm2Charges(RedMpole);

void SetGauDen(vector<GauDen1s>&,vector<QMMMAtom>&,int);

double SetvdWRad(string);

double SpringEnergy(double,double);

double TINKEREnergy(vector<QMMMAtom>&,QMMMSettings&,int);

void TINKERDynamics(vector<QMMMAtom>&,QMMMSettings&,int);

double TINKEROpt(vector<QMMMAtom>&,QMMMSettings&,int);

double TINKERForces(vector<QMMMAtom>&,vector<Coord>&,QMMMSettings&,int);

double TINKERPolForces(vector<QMMMAtom>&,vector<Coord>&,QMMMSettings&,int);

double TINKERMMForces(vector<QMMMAtom>&,vector<Coord>&,QMMMSettings&,int);

void TINKERInduced(vector<QMMMAtom>&,QMMMSettings&,int);

double TINKERPolEnergy(vector<QMMMAtom>&,QMMMSettings&,int);

void TINK2LICHEM(int&,char**&);

vector<int> TraceBoundary(vector<QMMMAtom>&,int);

string Typing(int);

void WriteTINKMpole(vector<QMMMAtom>&,fstream&,int,int);

//Function definitions (alphabetical)
#include "Analysis.cpp"
#include "Core_funcs.cpp"
#include "Frozen_density.cpp"
#include "Input_Reader.cpp"
#include "Multipoles.cpp"
#include "Optimizers.cpp"
#include "PathIntegral.cpp"
#include "ReactionPath.cpp"
#include "TINK2LICHEM.cpp"

//Wrapper definitions (alphabetical)
#include "AMBER.cpp"
#include "Gaussian.cpp"
#include "LAMMPS.cpp"
#include "Lepton_eng.cpp"
#include "NWChem.cpp"
#include "PSI4.cpp"
#include "TINKER.cpp"

#endif

