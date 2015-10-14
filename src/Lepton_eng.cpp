/*

###############################################################################
#                                                                             #
#                 LICHEM: Layered Interacting CHEmical Models                 #
#                              By: Eric G. Kratz                              #
#                                                                             #
#                      Symbiotic Computational Chemistry                      #
#                                                                             #
###############################################################################

 Functions and options for the eFF semi-classical electron model.

 Reference for eFF:
 Su et al., Phys. Rev. Lett., 18, 99, 185002, (2007)

*/

//Compile time eFF options
const double Rad = 0.10; //Initial electron radius
const double rho = 1.0; //Parameter for eFF VB mixing
const double sbar = 1.0; //Parameter for eFF radius scaling
const double rbar = 1.0; //Parameter for eFF distance scaling
const double Charm = 0; //Force constant for eFF harmonic constraint
const bool Scale_eFF = 0; //Scale the eFF kinetic energy by ElrtNbeads
const double Scale_POW = 0.5; //ElrtNbeads = pow(P,Scale_POW)
const bool qGrid = 0; //Restrict point-charge movements
const double RadMin = 0.01; //Minimum electron radius
const double RadMax = 25.0; //Maximum electron radius
const double ElecCutoff = 15.0; //Cutoff for electron electrostatics

//PIMC move probabilities
double ElBeadProb = 0.25; //Probability to move an electron bead
double ElCentProb = 0.25; //Probability to move an electron centroid
double RadProb = 0; //Probability to change electron radius
double SwapProb = 0.05; //Probability to swap spins
double FlipProb = 0.05; //Probability to flip a single spin

//Global variables
double ElrtNbeads; //Scale factor for eFF kinetic energy

//Functions
double EFFEnergy(QMMMAtom& atom, QMMMElec& elec, int Bead)
{
  //Atom-lepton interactions
  double E = 0.0;
  double r = CoordDist2(atom.P[Bead],elec.P[Bead]);
  if (r <= ElecCutoff*ElecCutoff)
  {
    if (r == 0.0)
    {
      E = C2eV*atom.MP[Bead].q*elec.q*sqrt(8/pi)/elec.rad[Bead];
    }
    else
    {
      r = sqrt(r);
      E = (C2eV*atom.MP[Bead].q*elec.q/r)*erf(sqrt2*r/elec.rad[Bead]);
    }
  }
  return E;
};

double EFFCorr(QMMMElec& elec1, QMMMElec& elec2, int Bead)
{
  //Lepton-lepton interactions
  double E = 0.0;
  double r = CoordDist2(elec1.P[Bead],elec2.P[Bead]);
  //Electrostatic energy
  if (r <= (ElecCutoff*ElecCutoff))
  {
    r = sqrt(r);
    if (r == 0.0)
    {
      E = C2eV*elec1.q*elec2.q*sqrt(8/pi);
      E /= sqrt(elec1.rad[Bead]*elec1.rad[Bead]
           +elec2.rad[Bead]*elec2.rad[Bead]);
      return HugeNum; //Escape to avoid singularities later
    }
    else
    {
      double radij = elec1.rad[Bead]*elec1.rad[Bead];
      radij += elec2.rad[Bead]*elec2.rad[Bead];
      radij = sqrt(radij);
      E = (C2eV*elec1.q*elec2.q/r);
      E *= erf(sqrt2*r/radij);
    }
    //Pauli repulsion
    if (elec1.typ == elec2.typ)
    {
      //Overlap
      double Sij = 2/((elec1.rad[Bead]/elec2.rad[Bead])
             +(elec2.rad[Bead]/elec1.rad[Bead]));
      Sij *= Sij*Sij;
      Sij = sqrt(Sij);
      double tmp = -1*rbar*rbar*r*r;
      tmp /= (elec1.rad[Bead]*elec1.rad[Bead]+elec2.rad[Bead]*elec2.rad[Bead]);
      tmp /= sbar*sbar;
      Sij *= exp(tmp);
      //Kinetic energy difference
      double Tij = 1/(elec1.rad[Bead]*elec1.rad[Bead]);
      Tij += 1/(elec2.rad[Bead]*elec2.rad[Bead]);
      Tij *= 3/(2*sbar*sbar);
      tmp = 6*sbar*sbar*(elec1.rad[Bead]*elec1.rad[Bead]
            +elec2.rad[Bead]*elec2.rad[Bead]);
      tmp -= 4*rbar*rbar*r*r;
      tmp /= sbar*sbar*(elec1.rad[Bead]*elec1.rad[Bead]
             +elec2.rad[Bead]*elec2.rad[Bead]);
      tmp /= sbar*sbar*(elec1.rad[Bead]*elec1.rad[Bead]
             +elec2.rad[Bead]*elec2.rad[Bead]);
      Tij -= tmp;
      Tij *= Har2eV*BohrRad*BohrRad;
      if (elec1.spin == elec2.spin)
      {
        //Symmetric VB spin-orbital
        double Etmp = Sij*Sij/(1-(Sij*Sij));
        Etmp += (1-rho)*Sij*Sij/(1+(Sij*Sij));
        E += Etmp*Tij;
      }
      else
      {
        //Antisymmetric VB spin orbital
        E += -1*rho*Sij*Sij*Tij/(1+(Sij*Sij));
      }
    }
  }
  return E;
};

double KineticE_eFF(vector<QMMMElec>& elecs, QMMMSettings& QMMMOpts)
{
  //Total electron kinetic energy
  double E = 0;
  for (unsigned int i=0;i<elecs.size();i++)
  {
    elecs[i].Ep = 0.0;
    for (int k=0;k<QMMMOpts.Nbeads;k++)
    {
      //Lepton kinetic energy
      double Etmp = 3/(2*elecs[i].rad[k]*elecs[i].rad[k]);
      Etmp *= ElecMass/elecs[i].m; //Scale by mass
      Etmp *= Har2eV*BohrRad*BohrRad;
      if (Scale_eFF)
      {
        //Reduce kinetic energy as the beads increase
        Etmp /= ElrtNbeads;
      }
      elecs[i].Ep += Etmp;
    }
  }
  return E;
};

double Get_EeFF(vector<QMMMAtom>& Struct, vector<QMMMElec>& elecs,
       QMMMSettings& QMMMOpts)
{
  //Total eFF interaction energy
  double E = 0;
  #pragma omp parallel for
  for (int i=0;i<Natoms;i++)
  {
    Struct[i].Ep = 0;
    for (unsigned int j=0;j<elecs.size();j++)
    {
      for (int k=0;k<QMMMOpts.Nbeads;k++)
      {
        Struct[i].Ep += EFFEnergy(Struct[i],elecs[j],k);
      }
    }
  }
  #pragma omp barrier
  #pragma omp parallel for
  for (unsigned int i=0;i<elecs.size();i++)
  {
    elecs[i].Ep = 0.0;
    for (int k=0;k<QMMMOpts.Nbeads;k++)
    {
      //Lepton interaction energy
      for (unsigned int j=0;j<i;j++)
      {
        //Lepton-lepton correlation
        elecs[i].Ep += EFFCorr(elecs[i],elecs[j],k);
      }
    }
  }
  #pragma omp barrier
  for (int i=0;i<Natoms;i++)
  {
    E += Struct[i].Ep;
  }
  for (unsigned int i=0;i<elecs.size();i++)
  {
    E += elecs[i].Ep;
  }
  E /= QMMMOpts.Nbeads; //Removes double counting
  return E;
};

