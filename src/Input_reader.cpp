/*

###############################################################################
#                                                                             #
#                 LICHEM: Layered Interacting CHEmical Models                 #
#                              By: Eric G. Kratz                              #
#                                                                             #
#                      Symbiotic Computational Chemistry                      #
#                                                                             #
###############################################################################

 Routines for reading and checking the input for LICHEM.

*/

/*!
  \ingroup Misc
*/
///@{

//Various input and error checking functions
void ReadArgs(int& argc, char**& argv, fstream& xyzFile,
              fstream& connectFile, fstream& regionFile, fstream& outFile)
{
  //Function to read arguments
  string dummy; //Generic string
  stringstream call; //Stream for system calls and reading/writing files
  //Read command line arguments
  if (argc == 1)
  {
    //Escape if there are no arguments
    cout << '\n';
    cout << "Missing arguments..." << '\n' << '\n';
    cout << "Usage: lichem -n Ncpus -x Input.xyz -c Connectivity.inp ";
    cout << "-r Regions.inp -o Output.xyz" << '\n';
    cout << '\n';
    cout << "Use -h or --help for detailed instructions.";
    cout << '\n' << '\n';
    cout.flush();
    exit(0);
  }
  dummy = string(argv[1]);
  if (dummy == "-GauExtern")
  {
    //Escape to GauExternal
    ExternalGaussian(argc,argv);
  }
  if (dummy == "-convert")
  {
    //Attempt to create LICHEM input from other formats
    dummy = string(argv[2]);
    if (dummy == "-t")
    {
      //Create LICHEM input from Gaussian input
      TINK2LICHEM(argc,argv);
    }
    if (dummy == "-b")
    {
      //Create BASIS files
      LICHEM2BASIS(argc,argv);
    }
    if (dummy == "-q")
    {
      //Create a QM connectivity file
      WriteQMConnect(argc,argv);
    }
    else
    {
      //Bad arguments
      cout << '\n';
      cout << "Unrecognized file format.";
      cout << '\n';
      cout << '\n';
      cout.flush();
    }
  }
  if (dummy == "-tinker")
  {
    //Attempt to create a TINKER XYZ file from LICHEM input
    LICHEM2TINK(argc,argv);
  }
  if (dummy == "-GlobalPoles")
  {
    //Print multipole moments in the global frame of reference
    ExtractGlobalPoles(argc,argv);
  }
  if (dummy == "-path")
  {
    //Create an initial reaction path called BeadStartStruct.xyz
    PathLinInterpolate(argc,argv);
  }
  if (dummy == "-splitpath")
  {
    //Separate a reaction path frame into a trajectory file
    SplitPathTraj(argc,argv);
  }
  if ((argc % 2) != 1)
  {
    //Check for help or missing arguments
    dummy = string(argv[1]);
    if ((dummy != "-h") && (dummy != "--help"))
    {
      //Escape if there are missing arguments
      cout << '\n';
      cout << "Odd number of arguments..." << '\n' << '\n';
      cout << "Usage: lichem -n Ncpus -x Input.xyz -c Connectivity.inp ";
      cout << "-r Regions.inp -o Output.xyz" << '\n';
      cout << '\n';
      cout << "Use -h or --help for detailed instructions.";
      cout << '\n' << '\n';
      cout.flush();
      exit(0);
    }
  }
  for (int i=0;i<argc;i++)
  {
    //Read file names and CPUs
    dummy = string(argv[i]);
    if ((dummy == "-h") || (dummy == "--help"))
    {
      //Print helpful information and exit
      cout << '\n';
      cout << "Usage: lichem -n Ncpus -x Input.xyz -c Connectivity.inp ";
      cout << "-r Regions.inp -o Output.xyz" << '\n';
      cout << '\n';
      cout << "Command line arguments:" << '\n' << '\n';
      cout << "  -n    Number of CPUs used for the QM calculation." << '\n';
      cout << '\n';
      cout << "  -x    Input xyz file." << '\n' << '\n';
      cout << "  -c    Connectivity and force field input file." << '\n';
      cout << '\n';
      cout << "  -r    Information about how the system is subdivided" << '\n';
      cout << "        into QM, MM, and pseudo-atom regions." << '\n' << '\n';
      cout << "  -o    Output xyz file for the optimized structures.";
      cout << '\n' << '\n';
      cout.flush();
      exit(0);
    }
    if (dummy == "-n")
    {
      //Read the number of CPUs
      Ncpus = atoi(argv[i+1]);
    }
    if (dummy == "-x")
    {
      //Read the XYZ filename
      xyzFilename = string(argv[i+1]);
      xyzFile.open(argv[i+1],ios_base::in);
    }
    if (dummy == "-c")
    {
      //Read the connectivity filename
      conFilename = string(argv[i+1]);
      connectFile.open(argv[i+1],ios_base::in);
    }
    if (dummy == "-r")
    {
      //Read the regions filename
      regFilename = string(argv[i+1]);
      regionFile.open(argv[i+1],ios_base::in);
    }
    if (dummy == "-o")
    {
      //Read the output XYZ filename
      outFile.open(argv[i+1],ios_base::out);
    }
  }
  for (int i=0;i<argc;i++)
  {
    //Detect bad arguments
    dummy = string(argv[i]);
    if (dummy[0] == '-')
    {
      bool badArgs = 0; //Bad argument found
      if ((dummy != "-n") && (dummy != "-x") &&
      (dummy != "-c") && (dummy != "-r") &&
      (dummy != "-o"))
      {
        badArgs = 1;
      }
      if (badArgs)
      {
        cout << '\n';
        cout << "Unrecognized flag..." << '\n' << '\n';
        cout << "Usage: lichem -n Ncpus -x Input.xyz -c Connectivity.inp ";
        cout << "-r Regions.inp -o Output.xyz" << '\n';
        cout << '\n';
        cout << "Use -h or --help for detailed instructions.";
        cout << '\n' << '\n';
        cout.flush();
        exit(0);
      }
    }
  }
  if (argc != 11)
  {
    //Escape if there are too few arguments
    cout << '\n';
    cout << "Missing arguments..." << '\n' << '\n';
    cout << "Usage: lichem -n Ncpus -x Input.xyz -c Connectivity.inp ";
    cout << "-r Regions.inp -o Output.xyz" << '\n';
    cout << '\n';
    cout << "Use -h or --help for detailed instructions.";
    cout << '\n' << '\n';
    cout.flush();
    exit(0);
  }
  //Make sure input files can be read
  bool doQuit = 0;
  if (!xyzFile.good())
  {
    //Coordinate file does not exist
    cout << "Error: Could not open xyz file.";
    cout << '\n';
    doQuit = 1;
  }
  if (!connectFile.good())
  {
    //Connectivity file does not exist
    cout << "Error: Could not open connectivity file.";
    cout << '\n';
    doQuit = 1;
  }
  if (!regionFile.good())
  {
    //Regions file does not exist
    cout << "Error: Could not open region file.";
    cout << '\n';
    doQuit = 1;
  }
  if (!outFile.good())
  {
    //No write permissions
    cout << "Error: Could not create output file.";
    cout << '\n';
    doQuit = 1;
  }
  if (doQuit)
  {
    //Quit with an error
    cout.flush(); //Print errors
    exit(0);
  }
  return;
};

void ReadLICHEMInput(fstream& xyzFile, fstream& connectFile,
                     fstream& regionFile, vector<QMMMAtom>& QMMMData,
                     QMMMSettings& QMMMOpts)
{
  //Read input
  string dummy; //Generic string
  if (!GauExternal)
  {
    xyzFile >> Natoms;
    for (int i=0;i<Natoms;i++)
    {
      //Save atom information
      QMMMAtom tmp;
      //Set coordinates
      xyzFile >> tmp.QMTyp;
      Coord tmp2;
      xyzFile >> tmp2.x >> tmp2.y >> tmp2.z;
      tmp.P.push_back(tmp2); //Set up zeroth replica
      //Set ID and regions
      tmp.id = i;
      tmp.NEBActive = 1;
      tmp.QMRegion = 0;
      tmp.MMRegion = 1;
      tmp.PBRegion = 0;
      tmp.BARegion = 0;
      tmp.frozen = 0;
      //Set electrostatic field
      MPole tmp3; //Initialize charges and multipoles
      OctCharges tmp4; //Initialize charges and multipoles
      //Add to arrays
      tmp.MP.push_back(tmp3);
      tmp.PC.push_back(tmp4);
      //Save atomic properties
      QMMMData.push_back(tmp);
    }
  }
  for (int i=0;i<Natoms;i++)
  {
    //Save connectivity information
    int tmp;
    //id MMTyp numTyp q Nbonds [connectivity]
    connectFile >> tmp; //Atom ID
    if (tmp != QMMMData[i].id)
    {
      //Escape if connectivity errors are found
      cout << "Error: Atoms in the connectivity file are out of order.";
      cout << '\n';
      cout.flush();
      exit(0); //Escape
    }
    connectFile >> QMMMData[i].MMTyp >> QMMMData[i].numTyp;
    connectFile >> QMMMData[i].m >> QMMMData[i].MP[0].q;
    connectFile >> tmp; //Number of bonds
    for (int j=0;j<tmp;j++)
    {
      //Save each bond to the atom's connectivity list
      int atomID;
      connectFile >> atomID;
      if (atomID >= Natoms)
      {
        //Search for more connectivity errors
        cout << "Error: Atom index out of range in connectivity.";
        cout << '\n';
        cout << "Atom " << i << " bonded to non-existant atom ";
        cout << atomID << '\n';
        cout.flush();
        exit(0); //Escape
      }
      QMMMData[i].bonds.push_back(atomID); //Add bond
    }
  }
  //Read simulation keywords
  while (regionFile.good() && (!regionFile.eof()))
  {
    string keyword;
    regionFile >> keyword;
    LICHEMLowerText(keyword);
    //Check for comments
    if ((keyword[0] == '#') || (keyword[0] == '!'))
    {
      //Skip comment
    }
    //Check for simulation keywords (alphabetical)
    else if (keyword == "acceptance_ratio:")
    {
      //Read the Monte Carlo acceptance ratio
      regionFile >> QMMMOpts.accRatio;
    }
    else if (keyword == "beads:")
    {
      //Read the number of replica beads
      regionFile >> QMMMOpts.NBeads;
    }
    else if (keyword == "box_size:")
    {
      //Read the box size
      regionFile >> Lx >> Ly >> Lz;
    }
    else if (keyword == "calculation_type:")
    {
      //Set the type of calculation
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      //Single-point calculations
      if ((dummy == "single-point") || (dummy == "sp") ||
         (dummy == "energy"))
      {
        //Read energy minimization options
        SinglePoint = 1;
      }
      if ((dummy == "freq") || (dummy == "frequency"))
      {
        //Read energy minimization options
        FreqCalc = 1;
      }
      //Optimizations
      if ((dummy == "opt") || (dummy == "optimize"))
      {
        //Optimize with native QM and MM optimizers
        OptSim = 1;
      }
      if ((dummy == "steep") || (dummy == "sd"))
      {
        //Optimize with the LICHEM steepest descent method
        SteepSim = 1;
      }
      if ((dummy == "dfp") || (dummy == "bfgs"))
      {
        //Optimize with the DFP optimizer
        DFPSim = 1;
        if (dummy == "bfgs")
        {
          //Print BFGS error
          cerr << "Warning: A BFGS optimizer is not implemented.";
          cerr << '\n';
          cerr << " The DFP algorithm will be used instead of BFGS.";
          cerr << '\n' << '\n';
          cerr.flush(); //Print error immediately
        }
      }
      //Reaction pathways
      if ((dummy == "neb") || (dummy == "ci-neb") || (dummy == "cineb"))
      {
        //Optimize a path with climbing image NEB
        NEBSim = 1;
      }
      //Ensemble sampling
      if (dummy == "pimc")
      {
        //Path-integral Monte Carlo
        PIMCSim = 1;
      }
      if (dummy == "fbneb")
      {
        //Force-bias Monte Carlo
        FBNEBSim = 1;
      }
    }
    else if (keyword == "electrostatics:")
    {
      //Check the type of force field
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if ((dummy == "charges") || (dummy == "charge") ||
         (dummy == "point-charge"))
      {
        //Point-charge force fields
        CHRG = 1;
      }
      if (dummy == "amoeba")
      {
        //AMOEBA polarizable force field
        AMOEBA = 1;
        if (TINKER)
        {
          ExtractTINKpoles(QMMMData,0);
        }
      }
      if (dummy == "gem")
      {
        //Frozen density
        GEM = 1;
        if (TINKER)
        {
          //Collect TINKER multipoles or GEM-DM
          ExtractTINKpoles(QMMMData,0);
        }
      }
    }
    else if (keyword == "ensemble:")
    {
      //Set the thermodynamic ensemble
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if (dummy == "nvt")
      {
        //Set a consistent name for the ensemble
        QMMMOpts.ensemble = "NVT";
      }
      if (dummy == "npt")
      {
        //Set a consistent name for the ensemble
        QMMMOpts.ensemble = "NPT";
      }
    }
    else if (keyword == "eq_steps:")
    {
      //Read the number of equilibration steps
      regionFile >> QMMMOpts.NEq;
    }
    else if (keyword == "frozen_ends:")
    {
      //Check for inactive NEB end-points
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if ((dummy == "yes") || (dummy == "true"))
      {
        QMMMOpts.frznEnds = 1;
      }
    }
    else if (keyword == "init_path_chk:")
    {
      //Check for inactive NEB end-points
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if ((dummy == "no") || (dummy == "false"))
      {
        QMMMOpts.startPathChk = 0;
      }
    }
    else if (keyword == "lrec_cut:")
    {
      //Read the QMMM electrostatic cutoff for LREC
      regionFile >> QMMMOpts.LRECCut;
    }
    else if (keyword == "lrec_exponent:")
    {
      //Read the exponent for the LREC smoothing function
      regionFile >> QMMMOpts.LRECPow;
    }
    else if (keyword == "max_opt_steps:")
    {
      //Read maximum number of optimization steps
      regionFile >> QMMMOpts.maxOptSteps;
    }
    else if (keyword == "max_stepsize:")
    {
      //Read the maximum displacement during optimizations
      regionFile >> QMMMOpts.maxStep;
    }
    else if (keyword == "mm_opt_cut:")
    {
      //Read MM optimization cutoff
      regionFile >> QMMMOpts.MMOptCut;
    }
    else if (keyword == "mm_opt_tolerance:")
    {
      //Read MM optimization tolerance (RMSD value)
      regionFile >> QMMMOpts.MMOptTol;
    }
    else if (keyword == "mm_type:")
    {
      //Set MM wrapper
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if (dummy == "tinker")
      {
        TINKER = 1;
      }
      if (dummy == "lammps")
      {
        LAMMPS = 1;
      }
    }
    else if (keyword == "neb_atoms:")
    {
      //Read the list of atoms to include in NEB tangents
      int numActive;
      regionFile >> numActive;
      //Temporarily mark active atoms as inactive
      for (int i=0;i<numActive;i++)
      {
        //Change flag
        int atomID;
        regionFile >> atomID;
        QMMMData[atomID].NEBActive = 0;
      }
      //Switch active and inactive groups
      #pragma omp parallel for schedule(dynamic)
      for (int i=0;i<Natoms;i++)
      {
        if (QMMMData[i].NEBActive)
        {
          QMMMData[i].NEBActive = 0;
        }
        else
        {
          QMMMData[i].NEBActive = 1;
        }
      }
    }
    else if (keyword == "opt_stepsize:")
    {
      //Read the optimization stepsize
      regionFile >> QMMMOpts.stepScale;
    }
    else if (keyword == "pbc:")
    {
      //Check for periodic boundaries
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if ((dummy == "yes") || (dummy == "true"))
      {
        PBCon = 1;
      }
    }
    else if (keyword == "potential_type:")
    {
      //Set QM, MM, and QMMM options
      regionFile >> dummy; //Potential type
      LICHEMLowerText(dummy);
      if (dummy == "qm")
      {
        //Pure QM simulation
        QMonly = 1;
        Nqm = Natoms; //Save number of QM atoms
      }
      if (dummy == "mm")
      {
        //Pure MM simulation
        MMonly = 1;
        Nmm = Natoms; //Save number of QM atoms
      }
      if (dummy == "qmmm")
      {
        //QMMM simulation
        QMMM = 1;
      }
    }
    else if (keyword == "pressure:")
    {
      //Read the pressure
      regionFile >> QMMMOpts.press;
    }
    else if (keyword == "print_normal_modes:")
    {
      //Check for inactive NEB end-points
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if ((dummy == "yes") || (dummy == "true"))
      {
        QMMMOpts.printNormModes = 1;
      }
    }
    else if (keyword == "print_steps:")
    {
      //Read the number of steps between MD and MC output
      regionFile >> QMMMOpts.NPrint;
    }
    else if (keyword == "prod_steps:")
    {
      //Read the number of production (MD or MC) steps
      regionFile >> QMMMOpts.NSteps;
    }
    else if (keyword == "qm_basis:")
    {
      //Set the basis set or semi-empirical Hamiltonian
      regionFile >> QMMMOpts.basis;
    }
    else if (keyword == "qm_charge:")
    {
      //Set the total charge on the QM region
      regionFile >> QMMMOpts.charge;
    }
    else if (keyword == "qm_memory:")
    {
      //Set the amount of memory for the QM calculations
      regionFile >> QMMMOpts.RAM;
      //Check units
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if (dummy == "mb")
      {
        //RAM is in MB
        QMMMOpts.memMB = 1;
      }
      else
      {
        //RAM is in GB
        QMMMOpts.memMB = 0;
      }
    }
    else if (keyword == "qm_method:")
    {
      //Set QM functional or method
      regionFile >> dummy;
      QMMMOpts.func = dummy; //Save name with correct case
      //Check for special methods
      LICHEMLowerText(dummy);
      if ((dummy == "semiempirical") || (dummy == "se-scf") ||
         (dummy == "semi-empirical") || (dummy == "sescf") ||
         (dummy == "semiemp"))
      {
        //Flag the method as a semi-empirical Hamiltonian
        QMMMOpts.func = "SemiEmp";
      }
    }
    else if (keyword == "qm_opt_tolerance:")
    {
      //Read QM optimization tolerance (RMSD value)
      regionFile >> QMMMOpts.QMOptTol;
    }
    else if (keyword == "qm_spin:")
    {
      //Set the multiplicity
      regionFile >> QMMMOpts.spin;
    }
    else if (keyword == "qm_type:")
    {
      //Set QM wrapper
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if (dummy == "psi4")
      {
        PSI4 = 1;
      }
      if (dummy == "nwchem")
      {
        NWChem = 1;
      }
      if ((dummy == "gaussian") || (dummy == "g09"))
      {
        Gaussian = 1;
      }
    }
    else if (keyword == "qm_units:")
    {
      //Read distance units for QM calculations
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if ((dummy == "bohr") || (dummy == "a.u."))
      {
        //Change distance units to a.u.
        QMMMOpts.unitsQM = "Bohr";
      }
    }
    else if (keyword == "solv_model:")
    {
      //Read MM implicit solvent model
      regionFile >> QMMMOpts.solvModel;
    }
    else if (keyword == "spring_constant:")
    {
      //Read the NEB spring constant
      regionFile >> QMMMOpts.kSpring;
    }
    else if (keyword == "temperature:")
    {
      //Read the temperature
      regionFile >> QMMMOpts.temp;
      //Save the inverse temperature
      QMMMOpts.beta = 1/(kBoltz*QMMMOpts.temp);
    }
    else if (keyword == "ts_freq:")
    {
      //Check for inactive NEB end-points
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if ((dummy == "yes") || (dummy == "true"))
      {
        QMMMOpts.NEBFreq = 1;
      }
    }
    else if (keyword == "use_ewald:")
    {
      //Check for MM Ewald summation
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if ((dummy == "yes") || (dummy == "true"))
      {
        //Turn on Ewald or PME
        QMMMOpts.useEwald = 1;
      }
    }
    else if (keyword == "use_lrec:")
    {
      //Turn on long-range corrections
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if ((dummy == "yes") || (dummy == "true"))
      {
        //Turn on long-range corrections
        QMMMOpts.useLREC = 1;
      }
    }
    else if (keyword == "use_mm_cutoff:")
    {
      //Check for the MM optimization cutoff
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if ((dummy == "yes") || (dummy == "true"))
      {
        //Turn on the optimization cutoff
        QMMMOpts.useMMCut = 1;
      }
    }
    else if (keyword == "use_solvent:")
    {
      //Check for MM implicit solvation
      regionFile >> dummy;
      LICHEMLowerText(dummy);
      if ((dummy == "yes") || (dummy == "true"))
      {
        //Turn on the implicit solvent
        QMMMOpts.useImpSolv = 1;
      }
    }
    //Check for region keywords
    else if (keyword == "qm_atoms:")
    {
      //Read the list of QM atoms
      regionFile >> Nqm;
      for (int i=0;i<Nqm;i++)
      {
        int atomID;
        regionFile >> atomID;
        QMMMData[atomID].QMRegion = 1;
        QMMMData[atomID].PBRegion = 0;
        QMMMData[atomID].BARegion = 0;
        QMMMData[atomID].MMRegion = 0;
      }
    }
    else if (keyword == "pseudobond_atoms:")
    {
      //Read the list of pseudobond atoms
      regionFile >> Npseudo;
      for (int i=0;i<Npseudo;i++)
      {
        int atomID;
        regionFile >> atomID;
        QMMMData[atomID].QMRegion = 0;
        QMMMData[atomID].PBRegion = 1;
        QMMMData[atomID].BARegion = 0;
        QMMMData[atomID].MMRegion = 0;
      }
    }
    else if (keyword == "boundary_atoms:")
    {
      //Read the list of boundary atoms
      regionFile >> Nbound;
      for (int i=0;i<Nbound;i++)
      {
        int atomID;
        regionFile >> atomID;
        QMMMData[atomID].QMRegion = 0;
        QMMMData[atomID].PBRegion = 0;
        QMMMData[atomID].BARegion = 1;
        QMMMData[atomID].MMRegion = 0;
      }
    }
    else if (keyword == "frozen_atoms:")
    {
      //Read the list of frozen atoms
      regionFile >> Nfreeze;
      for (int i=0;i<Nfreeze;i++)
      {
        int atomID;
        regionFile >> atomID;
        QMMMData[atomID].frozen = 1;
      }
    }
    //Check for bad keywords
    else if (regionFile.good() && (!regionFile.eof()))
    {
      //Inform the user about the bad keyword
      cout << "Error: Unrecognized keyword: ";
      cout << keyword << '\n';
      cout.flush();
      //Quit
      exit(0);
    }
  }
  //Reset regions for pure QM and MM
  if (QMonly)
  {
    //Reset the numbers if regions were specified in the input
    Nqm = Natoms;
    Npseudo = 0;
    Nbound = 0;
    //Redundant, but safe
    for (int i=0;i<Natoms;i++)
    {
      QMMMData[i].QMRegion = 1;
      QMMMData[i].MMRegion = 0;
      QMMMData[i].PBRegion = 0;
      QMMMData[i].BARegion = 0;
    }
    //Adjust optimization settings
    QMMMOpts.MMOptTol = QMMMOpts.QMOptTol; //Prevents early termination
  }
  if (MMonly)
  {
    //Reset the numbers if regions were specified in the input
    Nqm = 0;
    Npseudo = 0;
    Nbound = 0;
    //Redundant, but safe
    for (int i=0;i<Natoms;i++)
    {
      QMMMData[i].QMRegion = 0;
      QMMMData[i].MMRegion = 1;
      QMMMData[i].PBRegion = 0;
      QMMMData[i].BARegion = 0;
    }
  }
  Nmm = Natoms-Nqm-Npseudo-Nbound; //Set number of MM atoms
  //Replicate atoms
  if (QMMMOpts.NBeads > 1)
  {
    //Duplicate data
    for (int i=0;i<Natoms;i++)
    {
      //Create reaction-path beads
      for (int j=0;j<(QMMMOpts.NBeads-1);j++)
      {
        //Create replicas
        Coord temp = QMMMData[i].P[0];
        QMMMData[i].P.push_back(temp);
        MPole temp2 = QMMMData[i].MP[0];
        QMMMData[i].MP.push_back(temp2);
        OctCharges temp3 = QMMMData[i].PC[0];
        QMMMData[i].PC.push_back(temp3);
      }
    }
    //Set initial transition state for reaction pathways
    if (NEBSim)
    {
      if ((QMMMOpts.NBeads%2) == 0)
      {
        //Even number of beads
        QMMMOpts.TSBead = (QMMMOpts.NBeads/2); //Slightly on the product side
      }
      else
      {
        //Odd number of beads
        QMMMOpts.TSBead = ((QMMMOpts.NBeads-1)/2); //Middle bead
      }
    }
    //Add random displacements for PIMC simulations
    if (PIMCSim)
    {
      for (int i=0;i<Natoms;i++)
      {
        //Shift path-integral beads
        double massScale = sqrt(12.0/QMMMData[i].m); //Relative to carbon
        massScale *= 2*stepMin*centRatio; //Scale based on settings
        //Update all beads
        for (int j=0;j<(QMMMOpts.NBeads-1);j++)
        {
          //Pick random displacements
          double randX = (((double)rand())/((double)RAND_MAX));
          double randY = (((double)rand())/((double)RAND_MAX));
          double randZ = (((double)rand())/((double)RAND_MAX));
          //Place the first bead at the initial position
          if (j == 0)
          {
            randX = 0.5;
            randY = 0.5;
            randZ = 0.5;
          }
          //Update positions of active atoms
          if (!QMMMData[i].frozen)
          {
            QMMMData[i].P[j].x += (2*(randX-0.5)*massScale);
            QMMMData[i].P[j].y += (2*(randY-0.5)*massScale);
            QMMMData[i].P[j].z += (2*(randZ-0.5)*massScale);
          }
        }
      }
    }
  }
  //Read initial structures for all beads or create new ones
  if (CheckFile("BeadStartStruct.xyz") && (!GauExternal))
  {
    //Print output
    cout << "Reading restart information...";
    cout << '\n' << '\n';;
    //Open file
    fstream beadFile;
    beadFile.open("BeadStartStruct.xyz",ios_base::in);
    //Read and discard number of atoms
    int atTest = 0;
    beadFile >> atTest;
    if (atTest != (Natoms*QMMMOpts.NBeads))
    {
      //Print warning if the XYZ file has incorrect dimensions
      cout << "Error: Restart file does not have the correct format!";
      cout << '\n' << '\n';
      cout.flush();
      //Quit
      exit(0);
    }
    //Read atom/bead positions
    for (int i=0;i<Natoms;i++)
    {
      for (int j=0;j<QMMMOpts.NBeads;j++)
      {
        //Read atom type and discard
        beadFile >> dummy;
        //Read XYZ coordinates
        beadFile >> QMMMData[i].P[j].x;
        beadFile >> QMMMData[i].P[j].y;
        beadFile >> QMMMData[i].P[j].z;
      }
    }
  }
  else if (NEBSim)
  {
    //Exit with an error
    cout << "Error: No initial reaction path found in the restart file!!!";
    cout << '\n' << '\n';
    cout.flush();
    //Quit
    exit(0);
  }
  //Collect additonal TINKER input
  if (TINKER && (!GauExternal))
  {
    //NB: Classes are not used in the QMMM
    FindTINKERClasses(QMMMData); //Finds errors
  }
  //Check if QM log files should be saved
  if (CheckFile("BACKUPQM"))
  {
    //Read backup directory
    fstream backFile;
    //Set to default value
    QMMMOpts.backDir = "Old_files";
    //Check directory
    backFile.open("BACKUPQM",ios_base::in);
    if (backFile.good())
    {
      string newName;
      backFile >> newName;
      if (!backFile.eof())
      {
        QMMMOpts.backDir = newName;
      }
    }
  }
  //Set threads based on QM CPUs and total CPUs
  if (!GauExternal)
  {
    //NB: Sanity checks and error checking are only enabled with OpenMP
    //Set default number of threads for serial builds
    Nthreads = 1;
    //Set a better more realistic number of threads for OpenMP
    #ifdef _OPENMP
      //OpenMP settings
      double Procs = ((double)FindMaxThreads());
      Nthreads = FindMaxThreads();
      omp_set_num_threads(Nthreads);
      //Sanity check
      if (Ncpus > Nthreads)
      {
        //Assuming only one node is used for QM
        Ncpus = Nthreads;
      }
      //Modify threads for certain multi-replica simulations
      if ((QMMMOpts.NBeads > 1) && (PIMCSim || FBNEBSim))
      {
        //Divide threads between the beads
        Nthreads = ((int)floor(Procs/Ncpus));
        //Set number of threads for wrappers
        omp_set_num_threads(Nthreads);
      }
    #endif
    //Set eigen threads
    setNbThreads(Nthreads);
  }
  return;
};

void LICHEMErrorChecker(QMMMSettings& QMMMOpts)
{
  //Checks for basic errors and conflicts
  bool doQuit = 0; //Bool, quit with error
  //General errors
  if (QMMM)
  {
    //Check number of QM and MM atoms
    if ((Nqm+Npseudo) < 1)
    {
      //Make sure there are some atoms in the QM calculation
      cout << " Error: No QM or PB atoms defined for the QMMM calculations.";
      cout << '\n';
      doQuit = 1;
    }
    if ((Nmm+Nbound) < 1)
    {
      //Make sure there are some atoms in the MM calculations
      cout << " Error: No MM or BA atoms defined for the QMMM calculations.";
      cout << '\n';
      doQuit = 1;
    }
  }
  //Check LREC settings
  if (QMMMOpts.useLREC || PBCon)
  {
    //Check LREC cutoff
    if (PBCon)
    {
      //Find maximum box length
      double minLen = Lx;
      if (Ly < minLen)
      {
        minLen = Ly;
      }
      if (Lz < minLen)
      {
        minLen = Lz;
      }
      //Check cutoff
      if (QMMMOpts.useLREC && (QMMMOpts.LRECCut > (0.5*minLen)))
      {
        //Needed to make the minimum image convention safe
        QMMMOpts.LRECCut = 0.5*minLen;
        cout << "Warning: Reducing LREC cutoff (";
        cout << LICHEMFormFloat(QMMMOpts.LRECCut,6);
        cout << ") due to the minimum image convention.";
        cout << '\n' << '\n';
      }
    }
    if (QMMMOpts.useLREC && (QMMMOpts.LRECCut <= 0.10))
    {
      //Adjust cutoff to avoid divide by zero errors
      QMMMOpts.LRECCut = 0.10; //Minimum value, effectively zero
      cout << "Warning: LREC cutoffs less than 0.1 are not allowed.";
      cout << '\n' << '\n';
    }
    //Check LREC exponent
    if (QMMMOpts.LRECPow < 1)
    {
      //Needed to make the minimum image convention safe
      QMMMOpts.LRECPow = 3;
      cout << "Warning: Invalid LREC exponent.";
      cout << " LREC exponent set to 3.";
      cout << '\n' << '\n';
    }
  }
  //Check Ewald and implicit solvation settings
  if (QMMMOpts.useEwald && (!PBCon))
  {
    //Check Ewald settings
    cout << " Error: Ewald summation cannot be used without PBC.";
    cout << '\n';
    doQuit = 1;
  }
  if (QMMMOpts.useImpSolv && PBCon)
  {
    //Check Ewald settings
    cout << " Error: Implicit solvation models cannot be used with PBC.";
    cout << '\n';
    doQuit = 1;
  }
  //Check threading
  if (Ncpus < 1)
  {
    //Checks the number of threads and continue
    cout << " Warning: Calculations cannot run with ";
    cout << Ncpus << " CPUs.";
    cout << '\n';
    if (JOKES)
    {
      cout << " Do you know how computers work?";
    }
    cout << " Ncpus set to 1";
    cout << '\n' << '\n';
    Ncpus = 1;
    cout.flush(); //Print warning
  }
  //Wrapper errors
  if ((!TINKER) && (!LAMMPS) && (!QMonly))
  {
    //Check the MM wrappers
    cout << " Error: No valid MM wrapper selected.";
    cout << '\n';
    cout << "  Select a wrapper if you want to run this type ";
    cout << "of calculation.";
    cout << '\n';
    doQuit = 1;
  }
  if ((!Gaussian) && (!PSI4) && (!NWChem) && (!MMonly))
  {
    //Check the QM wrappers
    cout << " Error: No valid QM wrapper selected.";
    cout << '\n';
    cout << "  Select a wrapper if you want to run this type ";
    cout << "of calculation.";
    cout << '\n';
    doQuit = 1;
  }
  if (Gaussian && QMMM)
  {
    //Avoid options that conflict with NWChem capabilities
    if (OptSim)
    {
      //The NWChem optimizer cannot incorporate MM forces
      cout << " Error: QMMM Gaussian optimizations can only be performed";
      cout << '\n';
      cout << " with steepest descent or Davidon-Fletcher-Powell.";
      cout << '\n';
      doQuit = 1;
    }
  }
  if (PSI4 && QMMM)
  {
    //Avoid options that conflict with PSI4 capabilities
    if (OptSim)
    {
      //The PSI4 optimizer cannot incorporate MM forces
      cout << " Error: QMMM PSI4 optimizations can only be performed";
      cout << '\n';
      cout << " with steepest descent or Davidon-Fletcher-Powell.";
      cout << '\n';
      doQuit = 1;
    }
    if ((Npseudo != 0) || (Nbound != 0))
    {
      //PSI4 does not currently have pseudopotentials
      cout << " Error: The PSI4 wrapper can only use QM and MM atoms.";
      cout << '\n';
      cout << " Remove the pseudo-bonds and boundary-atoms.";
      cout << '\n';
      doQuit = 1;
    }
  }
  if (NWChem && QMMM)
  {
    //Avoid options that conflict with NWChem capabilities
    if (OptSim)
    {
      //The NWChem optimizer cannot incorporate MM forces
      cout << " Error: QMMM NWChem optimizations can only be performed";
      cout << '\n';
      cout << " with steepest descent or Davidon-Fletcher-Powell.";
      cout << '\n';
      doQuit = 1;
    }
  }
  if (LAMMPS && AMOEBA)
  {
    //Avoid options that conflict with LAMMPS capabilities
    cout << " Error: LAMMPS calculations cannot be performed with";
    cout << '\n';
    cout << " polarizable force fields.";
    cout << '\n';
    doQuit = 1;
  }
  //Simulation errors
  if ((QMMMOpts.ensemble == "NPT") && (!PBCon))
  {
    //Check the PBC options
    cout << " Error: NPT simulation without PBC.";
    cout << '\n';
    cout << "  Turn PBC on if you want to run this type ";
    cout << "of calculation.";
    cout << '\n';
    doQuit = 1;
  }
  if (QMMMOpts.stepScale > 1)
  {
    //Checks the number of threads and continue
    cout << " Warning: The optimization step scale cannot be greater";
    cout << " than 1.";
    cout << '\n';
    cout << " Step scale set to 1.";
    cout << '\n';
    QMMMOpts.stepScale = 1; //Reset step size
    cout.flush(); //Print warning
  }
  if (doQuit)
  {
    //Quits
    cout << '\n';
    cout.flush();
    exit(0);
  }
  //Sarcastically continue
  cout << "No fatal errors detected.";
  cout << '\n';
  if (JOKES)
  {
    cout << " And there was much rejoicing. Yay...";
    cout << '\n';
    cout << '\n';
    cout.flush();
    if (CheckFile("EASTEREGG"))
    {
      PrintLapin();
    }
  }
  return;
};

void LICHEMPrintSettings(vector<QMMMAtom>& QMMMData, QMMMSettings& QMMMOpts)
{
  //Prints out the simulation details
  cout << "Setting up simulation..." << '\n';
  cout << '\n';
  cout << "Input files:" << '\n';
  cout << " Coordinate file: " << xyzFilename << '\n';
  cout << " Connectivity file: " << conFilename << '\n';
  cout << " Region file: " << regFilename << '\n';
  if (CheckFile("BeadStartStruct.xyz"))
  {
    cout << " Restart file: BeadStartStruct.xyz" << '\n';
  }
  cout << '\n';
  cout << "Atoms: " << Natoms << '\n';
  if (QMonly || QMMM)
  {
    //QM regions
    cout << " QM atoms: " << Nqm << '\n';
    cout << "  Charge: " << QMMMOpts.charge << '\n';
    cout << "  Spin: " << QMMMOpts.spin << '\n';
  }
  if (MMonly || QMMM)
  {
    //MM regions
    cout << " MM atoms: " << Nmm << '\n';
    if (QMMM)
    {
      cout << " Pseudo-atoms: " << Npseudo << '\n';
      cout << " Boundary-atoms: " << Nbound << '\n';
    }
    if (Nfreeze > 0)
    {
      cout << " Frozen atoms: " << Nfreeze << '\n';
    }
  }
  if (NEBSim)
  {
    //Print reaction path input for error checking
    cout << " RP beads: " << QMMMOpts.NBeads << '\n';
    cout << '\n';
    cout << "Simulation mode: ";
    if (QMMM)
    {
      cout << "QMMM";
    }
    if (QMonly)
    {
      cout << "Pure QM";
    }
    if (MMonly)
    {
      cout << "Pure MM";
    }
    cout << " NEB" << '\n';
  }
  if (PIMCSim)
  {
    //Print PIMC input for error checking
    if (QMMMOpts.NBeads > 1)
    {
      cout << " PI beads: " << QMMMOpts.NBeads << '\n';
    }
    cout << '\n';
    cout << "Simulation mode: ";
    if (QMMM)
    {
      cout << "QMMM";
    }
    if (QMonly)
    {
      cout << "Pure QM";
    }
    if (MMonly)
    {
      cout << "Pure MM";
    }
    cout << " " << QMMMOpts.ensemble;
    if (QMMMOpts.NBeads > 1)
    {
      cout << " path-integral";
    }
    cout << " Monte Carlo" << '\n';
    cout << " Equilibration MC steps: " << QMMMOpts.NEq << '\n';
    cout << " Production MC steps: " << QMMMOpts.NSteps << '\n';
  }
  if (FBNEBSim)
  {
    //Print FBNEB input for error checking
    if (QMMMOpts.NBeads > 1)
    {
      cout << " RP beads: " << QMMMOpts.NBeads << '\n';
    }
    cout << '\n';
    cout << "Simulation mode: ";
    if (QMMM)
    {
      cout << "QMMM";
    }
    if (QMonly)
    {
      cout << "Pure QM";
    }
    if (MMonly)
    {
      cout << "Pure MM";
    }
    cout << " NVT";
    if (QMMMOpts.NBeads > 1)
    {
      cout << " force-bias";
    }
    cout << " Monte Carlo" << '\n';
    cout << " Equilibration MC steps: " << QMMMOpts.NEq << '\n';
    cout << " Production MC steps: " << QMMMOpts.NSteps << '\n';
  }
  if (OptSim || SteepSim || DFPSim)
  {
    //Print optimization input for error checking
    cout << '\n';
    cout << "Simulation mode: ";
    if (QMMM)
    {
      cout << "QMMM";
    }
    if (QMonly)
    {
      cout << "Pure QM";
    }
    if (MMonly)
    {
      cout << "Pure MM";
    }
    cout << " energy minimization" << '\n';
    if (QMMM || QMonly)
    {
      cout << " QM";
      if (QMMM)
      {
        cout << "MM";
      }
      cout << " minimizer: ";
      if (OptSim)
      {
        cout << "Native QM optimizer" << '\n';
      }
      if (SteepSim)
      {
        cout << "LICHEM steepest descent" << '\n';
      }
      if (DFPSim)
      {
        cout << "LICHEM DFP" << '\n';
      }
    }
  }
  if (SinglePoint)
  {
    //Print single-point energy settings for error checking
    cout << '\n';
    cout << "Simulation mode: ";
    if (QMMM)
    {
      cout << "QMMM";
    }
    if (QMonly)
    {
      cout << "Pure QM";
    }
    if (MMonly)
    {
      cout << "Pure MM";
    }
    if (QMMMOpts.NBeads == 1)
    {
      cout << " single-point energy" << '\n';
    }
    else
    {
      cout << " multi-point energy" << '\n';
    }
  }
  if (FreqCalc)
  {
    //Print frequency settings for error checking
    cout << '\n';
    cout << "Simulation mode: ";
    if (QMMM)
    {
      cout << "QMMM";
    }
    if (QMonly)
    {
      cout << "Pure QM";
    }
    if (MMonly)
    {
      cout << "Pure MM";
    }
    if (QMMMOpts.NBeads == 1)
    {
      cout << " single-point frequencies" << '\n';
    }
    else
    {
      cout << " multi-point frequencies" << '\n';
    }
  }
  if (QMonly || QMMM)
  {
    //Print QM wrapper input for error checking
    cout << " QM wrapper: ";
    if (PSI4)
    {
      cout << "PSI4" << '\n';
    }
    if (Gaussian)
    {
      cout << "Gaussian" << '\n';
    }
    if (NWChem)
    {
      cout << "NWChem" << '\n';
    }
    cout << " QM method: ";
    if (QMMMOpts.func != "SemiEmp")
    {
      //Avoid printing method and basis for semi-empirical
      cout << QMMMOpts.func << "/";
    }
    cout << QMMMOpts.basis << '\n';
  }
  if (MMonly || QMMM)
  {
    //Print MM wrapper input for error checking
    cout << " MM wrapper: ";
    if (TINKER)
    {
      cout << "TINKER" << '\n';
    }
    if (LAMMPS)
    {
      cout << "LAMMPS" << '\n';
    }
    if (QMMM)
    {
      //Print QMMM wrapper input for error checking
      cout << " MM potential: ";
      if (CHRG)
      {
        cout << "Point-charge force field" << '\n';
      }
      if (AMOEBA)
      {
        cout << "Polarizable force field" << '\n';
      }
      if (GEM)
      {
        cout << "Diffuse-charge force field" << '\n';
      }
    }
    //Print PBC information
    if (PBCon || QMMMOpts.useLREC || QMMMOpts.useImpSolv)
    {
      cout << '\n';
      cout << "Simulation box settings:" << '\n';
      if (PBCon)
      {
        //Print box size and density
        double initDen = 0; //Initial density
        cout << " Boundaries: Periodic" << '\n';
        cout << " Box size (\u212B): ";
        cout << LICHEMFormFloat(Lx,10) << " ";
        cout << LICHEMFormFloat(Ly,10) << " ";
        cout << LICHEMFormFloat(Lz,10) << '\n';
        cout << " Density: ";
        initDen = LICHEMDensity(QMMMData,QMMMOpts);
        cout << LICHEMFormFloat(initDen,10);
        cout << " g/cm\u00B3" << '\n';
      }
      if (QMMMOpts.useLREC)
      {
        //Print LREC cutoff options
        cout << " QM LREC: Yes" << '\n';
        cout << " LREC cutoff: ";
        cout << LICHEMFormFloat(QMMMOpts.LRECCut,8);
        cout << " \u212B" << '\n';
        cout << " LREC exponent: " << QMMMOpts.LRECPow << '\n';
      }
      if (QMMMOpts.useEwald)
      {
        //Print Ewald summation options
        cout << " MM Ewald: Yes" << '\n';
      }
      if (QMMMOpts.useImpSolv)
      {
        //Print continuum solvation options
        cout << " Implicit solvent: " << QMMMOpts.solvModel;
        cout << '\n';
      }
    }
  }
  cout << '\n';
  //Print parallelization settings
  cout << "Parallelization and memory settings:" << '\n';
  cout << " OpenMP threads: " << Nthreads << '\n';
  if (QMonly || QMMM)
  {
    cout << " QM threads: " << Ncpus << '\n';
    cout << " QM memory: " << QMMMOpts.RAM << " ";
    if (QMMMOpts.memMB)
    {
      cout << "MB";
    }
    else
    {
      cout << "GB";
    }
    cout << '\n';
  }
  if (MMonly || QMMM)
  {
    cout << " MM threads: " << Ncpus << '\n';
  }
  //Print Monte Carlo settings
  if (PIMCSim || FBNEBSim)
  {
    cout << '\n';
    cout << "Monte Carlo settings:" << '\n';
    cout << " Temperature: " << QMMMOpts.temp;
    cout << " K" << '\n';
    if (QMMMOpts.ensemble == "NPT")
    {
      cout << " Pressure: " << QMMMOpts.press;
      cout << " atm" << '\n';
    }
    if (FBNEBSim && (QMMMOpts.NBeads > 1))
    {
      cout << " Spring constant: " << QMMMOpts.kSpring;
      cout << " eV/\u212B\u00B2" << '\n';
    }
    cout << " Acceptance ratio: ";
    cout << LICHEMFormFloat(QMMMOpts.accRatio,4);
    cout << '\n';
    cout << " Equilibration MC steps: " << QMMMOpts.NEq;
    cout << '\n';
    cout << " Production MC steps: " << QMMMOpts.NSteps;
    cout << '\n';
    cout << " Sample every " << QMMMOpts.NPrint;
    cout << " steps" << '\n';
  }
  //Print convergence criteria for optimizations
  if (OptSim || SteepSim || DFPSim || NEBSim)
  {
    cout << '\n';
    cout << "Optimization settings:" << '\n';
    if (!OptSim)
    {
      cout << " Step scale factor: ";
      cout << LICHEMFormFloat(QMMMOpts.stepScale,6);
      cout << '\n';
    }
    cout << " Max. step size: ";
    cout << LICHEMFormFloat(QMMMOpts.maxStep,6);
    cout << " \u212B" << '\n';
    cout << " Max. steps: " << QMMMOpts.maxOptSteps;
    if (QMMMOpts.useMMCut && (Nmm > 0))
    {
      //Print MM cutoff settings
      cout << '\n';
      cout << " MM cutoff: ";
      cout << LICHEMFormFloat(QMMMOpts.MMOptCut,8);
      cout << " \u212B";
    }
    if (NEBSim)
    {
      //Spring constant for the path
      cout << '\n';
      cout << " Spring constant: " << QMMMOpts.kSpring;
      cout << " eV/\u212B\u00B2" << '\n';
      cout << " End points: ";
      if (QMMMOpts.frznEnds)
      {
        cout << "Frozen";
      }
      else
      {
        cout << "Active";
      }
    }
    cout << '\n';
    if (SteepSim || DFPSim || NEBSim)
    {
      cout << '\n';
      cout << "QM convergence criteria:" << '\n';
      cout << " RMS deviation: " << QMMMOpts.QMOptTol;
      cout << " \u212B" << '\n';
      cout << " Max. force: " << (20*QMMMOpts.QMOptTol);
      cout << " eV/\u212B" << '\n';
      cout << " RMS force: " << (10*QMMMOpts.QMOptTol);
      cout << " eV/\u212B" << '\n';
    }
    if (Nmm > 0)
    {
      cout << '\n';
      cout << "MM convergence criteria:" << '\n';
      cout << " RMS deviation: " << QMMMOpts.MMOptTol;
      cout << " \u212B" << '\n';
      cout << " RMS force: ";
      cout << LICHEMFormFloat(QMMMOpts.MMOptTol*kcal2eV,12);
      cout << " eV/\u212B" << '\n';
    }
  }
  //Print frequency analysis settings
  if (FreqCalc || QMMMOpts.NEBFreq)
  {
    cout << '\n';
    cout << "Frequency settings:" << '\n';
    //Always removed
    cout << "  Remove low frequencies: Yes";
    cout << '\n';
    //Removed for QM calculations
    cout << "  Remove translations: ";
    if (QMMM)
    {
      cout << "No" << '\n';
    }
    else
    {
      cout << "Yes" << '\n';
    }
    //Removed for QM calculations
    cout << "  Remove rotations: ";
    if (QMMM)
    {
      cout << "No" << '\n';
    }
    else
    {
      cout << "Yes" << '\n';
    }
  }
  cout << '\n';
  cout.flush(); //Flush for output being redirected to a file
  return;
};

//End of file group
///@}

