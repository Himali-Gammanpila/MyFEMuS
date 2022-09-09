#ifndef __femus_Cloud_hpp__
#define __femus_Cloud_hpp__

#include <fstream>
#include <iostream>     // std::cout, std::ios
#include <sstream>      // std::ostringstream

#include "MultiLevelSolution.hpp"
#include "./MyMarker/MyMarker.hpp"
#include "MyEigenFunctions.hpp"

namespace femus {

  class Cloud {
    public:
      Cloud() {
        _mrk = MyMarker();
      };
      ~Cloud() {};
      void SetNumberOfMarker(const unsigned &nMax);
      void InitCircle(const std::vector<double> &xc, const double &R, const unsigned &nMax, Solution* sol);
      void InitEllipse(const std::vector<double> &xc, const std::vector<double> &a, const unsigned &nMax, Solution* sol);

      void PrintNoOrder(const unsigned &t);
      void PrintWithOrder(const unsigned &t);
      void PrintCSV(const unsigned &t);

      void ComputeQuadraticBestFit();
      
      void GetCellInt(const std::vector<std::vector<double>> &xv, const unsigned &iel);

      const std::map<unsigned, std::vector<double>> GetQuadraticBestFitCoefficients() {
        return _A;
      }

      const std::vector<double> GetQuadraticBestFitCoefficients(const unsigned &iel) {
        if(_A.find(iel) != _A.end()) {
          return _A.at(iel);
        }
        else {
          return {};
        }
      }

      unsigned GetNumberOfMarker(const unsigned &iel) {
        unsigned cnt = 0;
        for(unsigned i = 0; i < _elem.size(); i++) {
          if(_elem[_map[i]] == iel) cnt++;
        }
        return cnt;
      }

      double getCurvature(const unsigned &iel, const std::vector<double> &xp) {
        return (8 * _A[iel][0] * _A[iel][2] * _A[iel][2] * xp[1] * xp[1] + 2 * _A[iel][2] * ((_A[iel][3] + 2 * _A[iel][0] * xp[0]) * (_A[iel][3] + 2 * _A[iel][0] * xp[0]) + 4 * _A[iel][0] * (_A[iel][4] + _A[iel][1] * xp[0]) * xp[1] - _A[iel][1] * _A[iel][1] * xp[1] * xp[1]) - 2 * (_A[iel][4] + _A[iel][1] * xp[0]) * (-_A[iel][0] * _A[iel][4] + _A[iel][1] * (_A[iel][3] + _A[iel][0] * xp[0] + _A[iel][1] * xp[1]))) / pow(((_A[iel][4] + _A[iel][1] * xp[0] + 2 * _A[iel][2] * xp[1]) * (_A[iel][4] + _A[iel][1] * xp[0] + 2 * _A[iel][2] * xp[1]) + (_A[iel][3] + 2 * _A[iel][0] * xp[0] + _A[iel][1] * xp[1]) * (_A[iel][3] + 2 * _A[iel][0] * xp[0] + _A[iel][1] * xp[1])), 3. / 2.);
      }

      std::vector<double> getNormal(const unsigned &iel, const std::vector<double> &xp) {
        std::vector<double> N(xp.size());

        N[0] = ((_A[iel][3] + 2 * _A[iel][0] * xp[0] + _A[iel][1] * xp[1]) * (8 * _A[iel][0] * _A[iel][2] * _A[iel][2] * xp[1] * xp[1] + 2 * _A[iel][2] * ((_A[iel][3] + 2 * _A[iel][0] * xp[0]) * (_A[iel][3] + 2 * _A[iel][0] * xp[0]) + 4 * _A[iel][0] * (_A[iel][4] + _A[iel][1] * xp[0]) * xp[1] - _A[iel][1] * _A[iel][1] * xp[1] * xp[1]) - 2 * (_A[iel][4] + _A[iel][1] * xp[0]) * (-_A[iel][0] * _A[iel][4] + _A[iel][1] * (_A[iel][3] + _A[iel][0] * xp[0] + _A[iel][1] * xp[1])))) / (pow((_A[iel][4] + _A[iel][1] * xp[0] + 2 * _A[iel][2] * xp[1]) * (_A[iel][4] + _A[iel][1] * xp[0] + 2 * _A[iel][2] * xp[1]) + (_A[iel][3] + 2 * _A[iel][0] * xp[0] + _A[iel][1] * xp[1]) * (_A[iel][3] + 2 * _A[iel][0] * xp[0] + _A[iel][1] * xp[1]), 2));

        N[1] = ((_A[iel][4] + _A[iel][1] * xp[0] + 2 * _A[iel][2] * xp[1]) * (8 * _A[iel][0] * _A[iel][2] * _A[iel][2] * xp[1] * xp[1] + 2 * _A[iel][2] * ((_A[iel][3] + 2 * _A[iel][0] * xp[0]) * (_A[iel][3] + 2 * _A[iel][0] * xp[0]) + 4 * _A[iel][0] * (_A[iel][4] + _A[iel][1] * xp[0]) * xp[1] - _A[iel][1] * _A[iel][1] * xp[1] * xp[1]) - 2 * (_A[iel][4] + _A[iel][1] * xp[0]) * (-_A[iel][0] * _A[iel][4] + _A[iel][1] * (_A[iel][3] + _A[iel][0] * xp[0] + _A[iel][1] * xp[1])))) / (pow((_A[iel][4] + _A[iel][1] * xp[0] + 2 * _A[iel][2] * xp[1]) * (_A[iel][4] + _A[iel][1] * xp[0] + 2 * _A[iel][2] * xp[1]) + (_A[iel][3] + 2 * _A[iel][0] * xp[0] + _A[iel][1] * xp[1]) * (_A[iel][3] + 2 * _A[iel][0] * xp[0] + _A[iel][1] * xp[1]), 2));
        
        
        double norm2 = 0.;
        for(unsigned i = 0; i < N.size(); i++) norm2 += N[i] * N[i];
        for(unsigned i = 0; i < N.size(); i++) N[i] /= sqrt(norm2);

        return N;
      }

    private:

      void CreateMap();
      bool ParallelElementSearch(const std::vector<double> &xp, const unsigned previousElem);

      Solution *_sol;
      unsigned _nMrk;
      std::ofstream _fout;
      std::vector<std::vector<double>> _yp;
      std::vector<std::vector<double>> _N;
      std::vector<double> _kappa;
      std::vector<std::vector<double>> _yi;
      std::vector<unsigned> _elem;
      std::vector<unsigned> _map;
      MyMarker _mrk;
      std::map<unsigned, std::vector<double>> _A;
      std::map<unsigned, unsigned [2] > _elMrkIdx;
      std::map<unsigned, unsigned [2] >::iterator _itElMrkIdx;


  };

  void Cloud::SetNumberOfMarker(const unsigned &nMax) {
    _nMrk = nMax;
  }

  void Cloud::InitEllipse(const std::vector<double> &xc, const std::vector<double> &a, const unsigned &nMax, Solution* sol) {

    _sol = sol;
    SetNumberOfMarker(nMax);
    double dt = 2. * M_PI / _nMrk;
    std::vector<double> xp(2);

    bool elemSearch;
    unsigned previousElem = UINT_MAX;
    unsigned iel;
    unsigned iproc = sol->processor_id();
    unsigned cnt = 0;

    _yp.resize(_nMrk);
    _yi.resize(_nMrk);
    _elem.resize(_nMrk);
    _N.resize(_nMrk);
    _kappa.resize(_nMrk);
    for(unsigned i = 0; i < _nMrk; i++) {
      double t = i * dt;
      xp[0] = xc[0] + a[0] * cos(t);
      xp[1] = xc[1] + a[1] * sin(t);

      elemSearch = ParallelElementSearch(xp, previousElem);
      if(elemSearch) {
        iel = _mrk.GetElement();
        if(_mrk.GetProc() == iproc) {
          double NNorm = sqrt(a[0] * a[0] * cos(t) * cos(t) + a[1] * a[1] * sin(t) * sin(t));
          _yp[cnt] = xp;
          _yi[cnt]  = _mrk.GetIprocLocalCoordinates();
          _N[cnt] = {a[0] * cos(t) / NNorm, a[1] * sin(t) / NNorm};
          _kappa[cnt] = a[0] * a[1] / (pow(sqrt(a[0] * a[0] * sin(t) * sin(t) + a[1] * a[1] * cos(t) * cos(t)), 3));
          _elem[cnt] = iel;
          cnt++;
        }
        previousElem = iel;
      }
      else {
        previousElem = UINT_MAX;
      }
    }
    _yp.resize(cnt);
    _yi.resize(cnt);
    _elem.resize(cnt);
    _N.resize(cnt);
    _kappa.resize(cnt);

    CreateMap();


    _elMrkIdx.clear();
    unsigned i = 0;
    while(i < _elem.size()) {
      unsigned iel = _elem[_map[i]];
      _elMrkIdx[iel][0] = i;
      while(i < _elem.size() && _elem[_map[i]] == iel) {
        i++;
      }
      _elMrkIdx[iel][1] = i;
    }

//     for(_itElMrkIdx = _elMrkIdx.begin(); _itElMrkIdx != _elMrkIdx.end(); _itElMrkIdx++) {
//       std::cout << _itElMrkIdx->first << " " << _itElMrkIdx->second[0] << " " << _itElMrkIdx->second[1] << std::endl;
//     }
  }

  void Cloud::InitCircle(const std::vector<double> &xc, const double &R, const unsigned &nMax, Solution* sol)  {
    InitEllipse(xc, {R, R}, nMax, sol);
  }

  void Cloud::CreateMap() {
    std::vector<unsigned*> vec(_elem.size());
    for(unsigned i = 0; i < _elem.size(); i++) {
      vec[i] = &_elem[i];
    }
    std::sort(vec.begin(), vec.end(), [](const unsigned * a, const unsigned * b) {
      return *a < *b;
    });
    _map.resize(_elem.size());
    for(unsigned i = 0; i < _map.size(); i++) {
      _map[i] =  static_cast<unsigned>(vec[i] - &_elem[0]);
    }
  }

  bool Cloud::ParallelElementSearch(const std::vector<double> &xp, const unsigned previousElem = UINT_MAX) {
    bool elemSearch;
    if(previousElem == UINT_MAX) elemSearch = _mrk.ParallelElementSearchWithInverseMapping(xp, _sol, 2);
    else elemSearch = _mrk.ParallelElementSearchWithInverseMapping(xp, _sol, 2, previousElem);

    return elemSearch;
  }

  void Cloud::PrintNoOrder(const unsigned &t) {
    unsigned iproc = _sol->processor_id();
    unsigned nprocs = _sol->n_processors();
    unsigned dim = _sol->GetMesh()->GetDimension();
    for(unsigned kp = 0; kp < nprocs; kp++) {
      if(kp == iproc) {
        if(kp == 0) _fout.open("markerno.dat", std::fstream::out);
        else _fout.open("markerno.dat", std::fstream::app);
        for(unsigned i = 0; i < _yp.size(); i++) {
          for(unsigned k = 0; k < dim; k++) {
            _fout << _yp[i][k] << " ";
          }
          for(unsigned k = 0; k < dim; k++) {
            _fout << _yi[i][k] << " ";
          }
          for(unsigned k = 0; k < dim; k++) {
            _fout << _N[i][k] << " ";
          }
          _fout << _kappa[i] << " " << iproc << " " << _elem[i] << std::endl;
        }
        _fout.close();
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  }

  void Cloud::PrintWithOrder(const unsigned &t) {
    unsigned iproc = _sol->processor_id();
    unsigned nprocs = _sol->n_processors();
    unsigned dim = _sol->GetMesh()->GetDimension();

    for(unsigned kp = 0; kp < nprocs; kp++) {
      if(kp == iproc) {
        if(kp == 0) _fout.open("markerTest.dat", std::fstream::out);
        else _fout.open("markerTest.dat", std::fstream::app);
        for(unsigned i = 0; i < _yp.size(); i++) {
          for(unsigned k = 0; k < dim; k++) {
            _fout << _yp[_map[i]][k] << " ";
          }
          for(unsigned k = 0; k < dim; k++) {
            _fout << _yi[_map[i]][k] << " ";
          }
          for(unsigned k = 0; k < dim; k++) {
            _fout << _N[_map[i]][k] << " ";
          }
          _fout << _kappa[_map[i]] << " " << iproc << " " << _elem[_map[i]] << std::endl;
        }
        _fout.close();
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  }

  void Cloud::PrintCSV(const unsigned &t) {
    unsigned iproc = _sol->processor_id();
    unsigned nprocs = _sol->n_processors();
    unsigned dim = _sol->GetMesh()->GetDimension();

    for(unsigned kp = 0; kp < nprocs; kp++) {
      if(kp == iproc) {
        std::ostringstream foo(std::ostringstream::ate);
        foo.str("./output/marker");
        foo << t;
        foo << ".csv";
        if(kp == 0) _fout.open(foo.str(), std::fstream::out);
        else _fout.open(foo.str(), std::fstream::app);

        if(kp == 0) {
          _fout << "\"X\",\"Y\",\"Z\",\"xi\",\"eta\",\"zeta\",\"Nx\",\"Ny\",\"Nz\",\"kappa\",\"ipoc\",\"elem\"" << std::endl;
        }
        for(unsigned i = 0; i < _yp.size(); i++) {
          for(unsigned k = 0; k < dim; k++) {
            _fout << _yp[_map[i]][k] << ",";
          }
          _fout << "0.,";
          for(unsigned k = 0; k < dim; k++) {
            _fout << _yi[_map[i]][k] << ",";
          }
          _fout << "0.,";
          for(unsigned k = 0; k < dim; k++) {
            _fout << _N[_map[i]][k] << ",";
          }
          _fout << "0.,";
          _fout << _kappa[_map[i]] << "," << iproc << "," << _elem[_map[i]] << std::endl;
        }
        _fout.close();
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  }

  void Cloud::ComputeQuadraticBestFit() {
    _A.clear();


    map<unsigned, bool> pSerach;

    Mesh *msh = _sol->GetMesh();

    unsigned dim = _sol->GetMesh()->GetDimension();
    std::vector<std::vector<double>> coord;
    coord.reserve(_elem.size());
    std::vector<double> norm;


    for(_itElMrkIdx = _elMrkIdx.begin(); _itElMrkIdx != _elMrkIdx.end(); _itElMrkIdx++) {
      unsigned iel = _itElMrkIdx->first;
      unsigned i0 = _itElMrkIdx->second[0];
      unsigned i1 = _itElMrkIdx->second[1];
      coord.resize(i1 - i0, std::vector<double> (dim));
      norm.assign(dim, 0);
      unsigned cnt = 0;
      for(unsigned i = i0; i < i1; i++, cnt++) {
        for(unsigned k = 0; k < dim; k++) {
          coord[cnt][k] = _yp[_map[i]][k];
          norm[k] += _N[_map[i]][k];
        }
      }

      if(coord.size() < 6) {
        for(unsigned i = 0; i < msh->el->GetElementNearElementSize(iel,1); i++) {
          int jel = msh->el->GetElementNearElement(iel, i); 
          if(_elMrkIdx.find(jel) != _elMrkIdx.end()) { //jel is a cut fem  
            unsigned j0 = _elMrkIdx[jel][0];
            unsigned j1 = _elMrkIdx[jel][1];
            coord.resize(coord.size() + (j1 - j0), std::vector<double> (dim));
            for(unsigned j = j0; j < j1; j++, cnt++) {
              for(unsigned k = 0; k < dim; k++) {
                coord[cnt][k] = _yp[_map[j]][k];
              }
            }
          }
        }
      }

      if(coord.size() < 6) {
        pSerach[iel] = true;
      }
      else femus::FindQuadraticBestFit(coord, boost::none, norm, _A[iel]);

    }

    map<unsigned, bool>::iterator it;

    unsigned iproc = _sol->processor_id();
    unsigned nprocs = _sol->n_processors();

    if(nprocs > 1) {

      for(unsigned kp = 0; kp < nprocs; kp++) {

        unsigned elementStart = msh->_elementOffset[kp];
        unsigned elementEnd = msh->_elementOffset[kp + 1];

        unsigned nel;
        if(iproc == kp) {
          nel = pSerach.size();
        }
        MPI_Bcast(&nel, 1, MPI_UNSIGNED, kp, MPI_COMM_WORLD);

        if(nel > 0) {
          if(iproc == kp) {
            it =  pSerach.begin();
          }
          for(unsigned cntEl = 0; cntEl < nel; cntEl++) {
            unsigned kel;
            unsigned nNgbElms;
            if(iproc == kp) {
              kel = it->first;
              unsigned i0 = _elMrkIdx[kel][0];
              unsigned i1 = _elMrkIdx[kel][1];
              coord.resize(i1 - i0, std::vector<double> (dim));
              norm.assign(dim, 0);
              unsigned cnt = 0;
              for(unsigned i = i0; i < i1; i++, cnt++) {
                for(unsigned k = 0; k < dim; k++) {
                  coord[cnt][k] = _yp[_map[i]][k];
                  norm[k] += _N[_map[i]][k];
                }
              }
              nNgbElms = msh->el->GetElementNearElementSize(kel,1);
            }
            MPI_Bcast(&nNgbElms, 1, MPI_UNSIGNED, kp, PETSC_COMM_WORLD);
        
            for(unsigned i = 0; i < nNgbElms; i++) {

              int jel;
              if(iproc == kp) {
                jel = msh->el->GetElementNearElement(kel, i); 
              }
              MPI_Bcast(&jel, 1, MPI_INT, kp, PETSC_COMM_WORLD);

                unsigned jp = msh->IsdomBisectionSearch(jel, 3);  // return  jproc for piece-wise constant discontinuous type (3)
                std::vector<std::vector<double>> coordJel;
                unsigned cntJel = 0;
                if(iproc == jp) {
                  if(_elMrkIdx.find(jel) != _elMrkIdx.end()) {   // if jel is cut cell
                    unsigned j0 = _elMrkIdx[jel][0];
                    unsigned j1 = _elMrkIdx[jel][1];
                    coordJel.resize(dim, std::vector<double> (j1 - j0));
                    for(unsigned j = j0; j < j1; j++, cntJel++) {
                      for(unsigned k = 0; k < dim; k++) {
                        coordJel[k][cntJel] = _yp[_map[j]][k];
                      }
                    }
                  }
                  if(jp != kp) {
                    MPI_Send(&cntJel, 1, MPI_UNSIGNED, kp, 0, MPI_COMM_WORLD);
                    if(cntJel != 0) {
                      for(unsigned k = 0; k < dim; k++) {
                        MPI_Send(coordJel[k].data(), coordJel[k].size(), MPI_DOUBLE, kp, 1 + k, MPI_COMM_WORLD);
                      }
                    }
                  }
                }

                if(iproc == kp) {
                  if(kp != jp) {
                    MPI_Recv(&cntJel, 1, MPI_UNSIGNED, jp, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if(cntJel != 0) {
                      coordJel.resize(dim, std::vector<double> (cntJel));
                      for(unsigned k = 0; k < dim; k++) {
                        MPI_Recv(coordJel[k].data(), coordJel[k].size(), MPI_DOUBLE, jp, 1 + k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                      }
                    }
                  }
                  if(cntJel != 0) {
                    unsigned size0 = coord.size();
                    coord.resize(coord.size() + cntJel, std::vector<double> (dim));
                    for(unsigned j = 0; j < cntJel; j++) {
                      for(unsigned k = 0; k < dim; k++) {
                        coord[size0 + j][k] = coordJel[k][j];
                      }
                    }
                  } 
              }
            }//face loop
           
            if(iproc == kp) {   
              femus::FindQuadraticBestFit(coord, boost::none, norm, _A[kel]);  
              it++;
            }

          }//element loop
        }
      }

    }


  }
  
  void Cloud::GetCellInt(const std::vector<std::vector<double>> &xv, const unsigned &iel){
    const unsigned dim = xv.size();
    const unsigned nve = xv[0].size();    
    unsigned intMax = 2;
    
    std::vector<double> Cf = _A[iel];//(_A[iel].size());
    
    std::vector<double> A(2, 0.);
    double D = 0.;
    unsigned cnt = 0;
    std::vector<std::vector<double>> xe(dim, std::vector<double>(2*nve));
    
    for(unsigned i = 0; i < nve; i++){
      unsigned ip1 = (i + 1) % nve;
      A[0] = xv[1][ip1] - xv[1][i];
      A[1] = - xv[0][ip1] + xv[0][i];
      D = - A[0] * xv[0][i] - A[1] * xv[1][i];
      std::vector<double> inters(intMax, 0.);
      unsigned dir = (fabs(A[0]) > fabs(A[1])) ? 1 : 0 ;
      unsigned dirp1 = (dir + 1) % 2;
      double iMax = std::max(xv[dir][ip1], xv[dir][i]);
      double iMin = std::min(xv[dir][ip1], xv[dir][i]);
      
      double a =  - A[0] * Cf[1] * A[1] + Cf[0] * A[1] * A[1] + A[0] * A[0] * Cf[2];
      double b = - (A[dirp1] * A[dir] * Cf[4-dir] + A[dirp1] * Cf[1] * D - 2 * Cf[2 * dirp1] * A[dir] * D - A[dirp1] * A[dirp1] * Cf[4-dirp1]);
      double c = - A[dirp1] * Cf[4-dir] * D + Cf[2 * dirp1] * D * D + A[dirp1] * A[dirp1] * Cf[5];
      
      double delta = b * b - 4 * a * c; 
      
      if(delta > 0. && a != 0) {
        inters[0] = (- b + sqrt(delta)) / (2. * a);  
        inters[1] = (- b - sqrt(delta)) / (2. * a);
        
        unsigned nInt = 0;
        unsigned jInt = 2;
        for(unsigned j = 0; j < intMax; j++) {
          if(inters[j] < iMax && inters[j] > iMin) {
            nInt++;
            jInt = j;
            xe[dir][cnt] = inters[jInt];
            xe[dirp1][cnt] = (- D - A[dir] * xe[dir][cnt]) / A[dirp1];
            cnt++;
          }
        }
      }
    }
    for (unsigned k = 0; k < dim; k++){
        xe[k].resize(cnt);
    }
  }

}





//     coord.resize(0);
//     unsigned i = 0;
//     while(i < _elem.size()) {
//       unsigned iel = _elem[_map[i]];
//       unsigned cnt = 0;
//       while (i < _elem.size() && _elem[_map[i]] == iel) {
//         coord.resize(cnt + 1, std::vector<double> (dim));
//         norm.assign(dim, 0);
//         for( unsigned k = 0; k < dim; k++) {
//           coord[cnt][k] = _yp[_map[i]][k];
//           norm[k] += _N[_map[i]][k];
//         }
//         cnt++;
//         i++;
//       }
//       if(coord.size() == 1){} //not yet implemented
//       else if (coord.size() > 1 && coord.size() < 5) { // linear interpolation for nmarker \in [2, 4]
//         _A[iel].resize(coord[cnt].size() + 1);
//         std::vector<double> a(coord[cnt].size());
//         double d =0.;
//         femus::FindBestFit(coord, boost::none, norm, a, d);
//         for(unsigned k = 0; k < coord[cnt].size(); k++) _A[iel][k] = a[k];
//         _A[iel][coord[cnt].size()] = d;
//       }
//       else femus::FindQuadraticBestFit(coord, boost::none, norm, _A[iel]);
//     }
//}

//}



#endif
