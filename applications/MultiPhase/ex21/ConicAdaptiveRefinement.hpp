
#ifndef __Conic_Adaptive_Refinement_hpp__
#define __Conic_Adaptive_Refinement_hpp__

#include <iostream>
#include <cmath>
#include <vector>

#include "Fem.hpp"

using namespace std;
using namespace femus;

class ConicAdaptiveRefinement {
  public:
    ConicAdaptiveRefinement() {
      _xr = {{-1., -1.}, {1., 1.}, {1., -1.}, {-1., 1.},
        {-1., 0.}, {1., 0.}, {0., -1.}, { 0., 1.}, {0., 0.}
      };
      _wr = {0.0625, 0.0625, 0.0625, 0.0625, 0.125, 0.125, 0.125, 0.125, 0.25};

      _yr = {
        {{-1, -1}, {0, -1}, {0, 0}, {-1, 0}},
        {{0, -1}, {1, -1}, {1, 0}, {0, 0}},
        {{0, 0}, {1, 0}, {1, 1}, {0, 1}},
        {{-1, 0}, {0, 0}, {0, 1}, {-1, 1}}
      };
      // coordinates in the reference element of the nodes of the target elements

      _fem1 = new Fem(3, 2);
      _fem2 = new Fem(6, 2);
      _quad1 = _fem1->GetFiniteElement(3, 0); //quad linear fem for coarse integration
      _quad2 = _fem2->GetFiniteElement(3, 0); //quad linear fem for fine integration

    }

    ~ConicAdaptiveRefinement() {
      delete _fem1;
      delete _fem2;
    };

    void CalculateConicsInTargetElement(const std::vector<std::vector<double>> &x, const std::vector<double> &A, std::vector<double> &A1);
    void ComputeJacobian(const std::vector<std::vector<double>> &x, std::vector<std::vector<double>> &J);
    void ComputeInverseJacobian(std::vector<std::vector<double>> &J, std::vector<std::vector<double>> &IJ);
    double EvaluateConic(const std::vector<double>&x, const std::vector<double>&a) {
      return a[0] * x[0] * x[0] + a[1] * x[0] * x[1] + a[2] * x[1] * x[1] + a[3] * x[0] + a[4] * x[1] + a[5];
    }
    int TestIfIntesection(const std::vector<std::vector<double>>&x, const std::vector<double>&a);
    int TestIfIntesectionWithReferenceQuad(const std::vector<double>&A);

    void BestFitLinearInterpolation(const std::vector<double> &A, std::vector<double> &B);


    double GetVolumeFraction(const std::vector<double>&a);

    double AdaptiveRefinement(const unsigned &level,  const unsigned &j, const unsigned &levelMax,
                              const std::vector<std::vector<double>>&x, const std::vector<double>&Ar);

    void PrintElement(const unsigned &level, const unsigned &jp, const std::vector<std::vector<double>>&x) {
      const unsigned &n = x.size();
      const unsigned &dim = x[0].size();
      for(unsigned i = 0; i < n ; i++) {
        std::cout << level << " " << jp << " ";
        for(unsigned j = 0; j < dim; j++) {
          std::cout << x[i][j] << " ";
        }
        std::cout << 0 << " ";
        std::cout << std::endl;
      }
      std::cout << level << " " << jp << " ";
      for(unsigned j = 0; j < dim; j++) {
        std::cout << x[n - 1][j] << " ";
      }
      std::cout << 0 << " ";
      std::cout << std::endl << std::endl;
    }

    bool CheckIfRootsAreInBetweenM1andP1(const double &A, const double &B, const double &C) {
      double det = sqrt(B * B - 4. * A * C);
      if(det >= 0.) {
        double t = (-B - det) / (2. * A);
        if(t >= -1. && t <= 1.) return true;
        t = (-B + det) / (2. * A);
        if(t >= -1. && t <= 1.) return true;
      }
      return false;
    }

  private:
    std::vector<double> _wr;
    std::vector<std::vector<double>> _xr;
    std::vector<std::vector<std::vector<double>>> _yr;
    std::vector<std::vector<std::vector<std::vector<double>>>> _y;
    Fem* _fem1, *_fem2;
    const elem_type *_quad1, *_quad2;
    double _weight;
    std::vector <double> _phi;
    std::vector <double> _phix;
    std::vector<double> _xg;

};


//coefficients of conic in the reference system
void ConicAdaptiveRefinement::CalculateConicsInTargetElement(const std::vector<std::vector<double>> &x, const std::vector<double> &A, std::vector<double> &A1) {

  const double &x1 = x[0][0];
  const double &x4 = x[3][0];

  const double &y1 = x[0][1];
  const double &y2 = x[1][1];

  double Lx = x[1][0] - x1;
  double Ly = x[3][1] - y1;

  const double &a = A[0];
  const double &b = A[1];
  const double &c = A[2];
  const double &d = A[3];
  const double &e = A[4];
  const double &f = A[5];

  //compact
  double Lx14 = Lx + x1 + x4;
  double Ly12 = Ly + y1 + y2;
  double y12 = y1 - y2;
  double x14 = x1 - x4;
  double Lxy = Lx * Ly;

  A1.resize(A.size());

  A1[0] = 0.25 * (a * Lx * Lx + (-b * Lx + c * y12) * y12);
  A1[1] = 0.25 * (b * Lxy - 2 * a * Lx * x14 + b * x14 * y12 - 2 * c * Ly * y12);
  A1[2] = 0.25 * (c * Ly * Ly + (-b * Ly + a * x14) * x14);
  A1[3] = 0.25 * (2 * d * Lx + b * Lxy + 2 * a * Lx * Lx14 - 2 * e * y1 - 2 * c * Ly * y1 - b * x1 * y1 - b * x4 * y1 - 2 * c * y1 * y1 + (2 * e + 2 * c * Ly + b * (2 * Lx + x1 + x4)) * y2 + 2 * c * y2 * y2);
  A1[4] = 0.25 * (2 * e * Ly - 2 * x14 * (d + a * Lx14) + 2 * c * Ly * Ly12 + b * (Lxy - x1 * (y1 + y2) + x4 * (2 * Ly + y1 + y2)));
  A1[5] = 0.25 * (4 * f + 2 * d * Lx14 + a * Lx14 * Lx14 + Ly12 * (2 * e + b * Lx14 + c * Ly12));
}
//Jacobian of trasformation
void ConicAdaptiveRefinement::ComputeJacobian(const std::vector<std::vector<double>> &x, std::vector<std::vector<double>> &J) {
  const double &x1 = x[0][0];
  const double &x2 = x[1][0];
  const double &x4 = x[3][0];

  const double &y1 = x[0][1];
  const double &y2 = x[1][1];
  const double &y4 = x[3][1];

  J[0][0] = 0.5 * (x2 - x1);                                //partial derivative of x wrt xi
  J[0][1] = 0.5 * (x4 - x1);                                //partial derivative of x wrt eta
  J[1][0] = 0.5 * (y2 - y1);                                //partial derivative of y wrt xi
  J[1][1] = 0.5 * (y4 - y1);                                //partial derivative of y wrt eta
}

//Inverse of Jacobian calculation
void ConicAdaptiveRefinement::ComputeInverseJacobian(std::vector<std::vector<double>> &J, std::vector<std::vector<double>> &IJ) {
  double detJ = J[0][0] * J[1][1] - J[0][1] * J[1][0];

  IJ.resize(2, std::vector<double>(2));

  if(std::abs(detJ) > 1e-10) {
    IJ[0][0] = J[1][1] / detJ;
    IJ[0][1] = -J[0][1] / detJ;
    IJ[1][0] = -J[1][0] / detJ;
    IJ[1][1] = J[0][0] / detJ;
  }
  else {
    std::cout << " Error: Jacobian Matrix is singular." << std::endl;
  }


}

int ConicAdaptiveRefinement::TestIfIntesection(const std::vector<std::vector<double>>&x, const std::vector<double>&a) {

  double value0 = EvaluateConic(x[0], a);
  for(unsigned i = 1; i < x.size(); i++) {
    if(EvaluateConic(x[i], a) * value0 <= 0) return 0;
  }
  return (value0 > 0) ? 1 : -1;
}


int ConicAdaptiveRefinement::TestIfIntesectionWithReferenceQuad(const std::vector<double>&Ar) {

  const double& a = Ar[0];
  const double& b = Ar[1];
  const double& c = Ar[2];
  const double& d = Ar[3];
  const double& e = Ar[4];
  const double& f = Ar[5];


  double det;
  double A, B, C;

  //main diagonal
  A = a + b + c;
  B =  d + e;
  C = f;
  if(CheckIfRootsAreInBetweenM1andP1(A, B, C)) return 0;

  //other diagonal
  A = a - b + c;
  B = -d + e;
  C = f;
  if(CheckIfRootsAreInBetweenM1andP1(A, B, C)) return 0;

  //bottom edge
  A = a;
  B = -b + d;
  C = c - e + f;
  if(CheckIfRootsAreInBetweenM1andP1(A, B, C)) return 0;

  //top edge
  A = a;
  B = b + d;
  C = c + e + f;
  if(CheckIfRootsAreInBetweenM1andP1(A, B, C)) return 0;

  //left edge
  A = c;
  B = -b + e;
  C = a - d + f;
  if(CheckIfRootsAreInBetweenM1andP1(A, B, C)) return 0;

  //right edge
  A = c;
  B =  b + e;
  C = a + d + f;
  if(CheckIfRootsAreInBetweenM1andP1(A, B, C)) return 0;

  return (f > 0) ? 1 : -1;
}





double ConicAdaptiveRefinement::GetVolumeFraction(const std::vector<double>&a) {

  double C = 0.;
  for(unsigned i = 0; i < _xr.size(); i++) {
    if(EvaluateConic(_xr[i], a) < 0) C += _wr[i];
  }
  return C;
}


double ConicAdaptiveRefinement::AdaptiveRefinement(
  const unsigned &level, // mylevel, with initial level = 1
  const unsigned &j, // son number with respect to the father, for level = 1, is only 1
  const unsigned &levelMax,
  const std::vector<std::vector<double>>&x, // myphysical coordinates
  const std::vector<double>&Ar) { // myconic

  double area = 0.;

  const double &x1 = x[0][0];
  const double &x2 = x[1][0];
  const double &x3 = x[2][0];
  const double &x4 = x[3][0];

  const double &y1 = x[0][1];
  const double &y2 = x[1][1];
  const double &y3 = x[2][1];
  const double &y4 = x[3][1];

  if(level == 1) _y.resize(levelMax);
  _y[level - 1] = {
    {{x1, y1}, {0.5 * (x1 + x2), 0.5 * (y1 + y2)}, {0.25 * (x1 + x2 + x3 + x4), 0.25 * (y1 + y2 + y3 + y4)}, {0.5 * (x1 + x4), 0.5 * (y1 + y4)}},
    {{0.5 * (x1 + x2), 0.5 * (y1 + y2)}, {x2, y2}, {0.5 * (x2 + x3), 0.5 * (y2 + y3)}, {0.25 * (x1 + x2 + x3 + x4), 0.25 * (y1 + y2 + y3 + y4)}},
    {{0.25 * (x1 + x2 + x3 + x4), 0.25 * (y1 + y2 + y3 + y4) }, {0.5 * (x2 + x3), 0.5 * (y2 + y3)}, {x3, y3}, {0.5 * (x3 + x4), 0.5 * (y3 + y4)}},
    {{0.5 * (x1 + x4), 0.5 * (y1 + y4)}, {0.25 * (x1 + x2 + x3 + x4), 0.25 * (y1 + y2 + y3 + y4)}, {0.5 * (x3 + x4), 0.5 * (y3 + y4)}, {x4, y4}}
  }; // physical coordinates of my children

  if(level < levelMax) {
    int test = TestIfIntesectionWithReferenceQuad(Ar);
    if(test == 0) { // it means there is an intersection
      for(unsigned i = 0; i < 4; i++) {
        std::vector<double> At;                             // conic cofficient in the target element
        CalculateConicsInTargetElement(_yr[i], Ar, At);
        area += AdaptiveRefinement(level + 1, i + 1, levelMax, _y[level - 1][i], At);
      }
    }
    else {
      //PrintElement(level, j, x);

      if(test == -1) { // it means it is a full element
        for(unsigned ig = 0; ig < _quad1->GetGaussPointNumber(); ig++) {
          _quad1->Jacobian({{x1, x2, x3, x4}, {y1, y2, y3, y4}}, ig, _weight, _phi, _phix);
          area += _weight;
        }
      }
    }
  }
  else {
    //PrintElement(level, j, x);
    //double VF = GetVolumeFraction(Ar);

    //std::cout<< _quad2->GetGaussPointNumber() <<" ";

    for(unsigned ig = 0; ig < _quad2->GetGaussPointNumber(); ig++) {
      _quad2->Jacobian({{x1, x2, x3, x4}, {y1, y2, y3, y4}}, ig, _weight, _phi, _phix);
      _xg.assign(x[0].size(), 0.);
      for(unsigned i = 0; i < x.size(); i++) {
        for(unsigned k = 0; k < x[i].size(); k++) {
          _xg[k] += _phi[i] * _xr[i][k];
        }
      }
      if(EvaluateConic(_xg, Ar) < 0) {
        area += _weight;
      }
    }

  }

  return area;
}


void ConicAdaptiveRefinement::BestFitLinearInterpolation(const std::vector<double> &A, std::vector<double> &B) {



  std::vector<std::vector <double>> M(3, std::vector<double>(3, 0)) ;
  std::vector <double> F(3, 0);

  double s2 = 0;

  unsigned n = 20;
  double h = 2. / n;
  for(unsigned i = 0; i <= n ; i++) {
    for(unsigned j = 0; j <= n ; j++) {
      double z = EvaluateConic({-1. + i * h, -1 + j * h}, A);
      s2 += z * z;
    }
  }
  s2 /= n^2;

  std::cout << s2 << std::endl;

  for(unsigned i = 0; i <= n ; i++) {
    for(unsigned j = 0; j <= n ; j++) {
      const double& x = -1. + h * i;
      const double& y = -1. + h * j;

      double z = EvaluateConic({x, y}, A);

      double w = 1.;//exp(- z * z /s2);
      //double w = exp(- sqrt(z * z) / sqrt(z2max));
      //double w = 1;
      std::cout << z << " " << w << std::endl;


      M[0][0] += w * x * x;
      M[0][1] += w * x * y;
      M[0][2] += w * x;

      M[1][1] += w * y * y;
      M[1][2] += w * y;

      M[2][2] += w;

      F[0] += w * z * x;
      F[1] += w * z * y;
      F[2] += w * z;
    }
  }

  M[1][0] = M[0][1];
  M[2][0] = M[0][2];
  M[2][1] = M[1][2];

  double det = M[0][0] * (M[1][2] * M[1][2] - M[1][1] * M[2][2]) +
               M[0][1] * (M[1][0] * M[2][2] - M[2][1] * M[0][2]) +
               M[0][2] * (M[1][1] * M[0][2] - M[0][1] * M[1][2]) ;



  std::vector<std::vector <double>> Mi = {
    {(M[1][2] * M[1][2] - M[1][1] * M[2][2]) / det},
    {(M[0][1] * M[2][2] - M[0][2] * M[1][2]) / det, (M[0][2] * M[0][2] - M[0][0] * M[2][2]) / det},
    {(M[0][2] * M[1][1] - M[0][1] * M[1][2]) / det, (M[0][0] * M[1][2] - M[0][1] * M[0][2]) / det, (M[0][1] * M[0][1] - M[0][0] * M[1][1]) / det}
  };
  Mi[0][1] = Mi[1][0];
  Mi[0][2] = Mi[2][0];
  Mi[1][2] = Mi[2][1];

  B.assign(3, 0);
  for(unsigned i = 0; i < 3; i++) {
    for(unsigned j = 0; j < 3; j++) {
      B[i] += Mi[i][j] * F[j];
    }
  }


}





#endif
