
#include "MyEigenFunctions.hpp"
#include <numeric>
#include <iostream>

namespace femus {

  void FindBestFit(const std::vector < std::vector < double > > &xp, boost::optional < const std::vector < double > & > w, const std::vector < double > &N, std::vector < double > &a, double &d) {

    const unsigned& dim = N.size();
    a.resize(dim);

    unsigned np = xp.size();
    Eigen::MatrixXd m(np, dim);

    std::vector < double > xg(dim, 0.);


    if(w) {
      //Calculate centroid
      double wSum = 0;
      for(unsigned i = 0; i < np; i++) {
        wSum += (*w)[i];
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += (*w)[i] * xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= wSum;
      }

      //Fill matrix to be passed to JacobiSVD
      for(unsigned i = 0; i < np; i++) {
        for(unsigned j = 0; j < dim; j++) {
          m(i, j) = sqrt((*w)[i]) * (xp[i][j] - xg[j]);
        }
      }
    }
    else {
      //Calculate centroid
      for(unsigned i = 0; i < np; i++) {
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= np;
      }
      //Fill matrix to be passed to JacobiSVD
      for(unsigned i = 0; i < np; i++) {
        for(unsigned j = 0; j < dim; j++) {
          m(i, j) = (xp[i][j] - xg[j]);
        }
      }
    }


    Eigen::JacobiSVD<Eigen::MatrixXd> svd(m, Eigen::ComputeThinU | Eigen::ComputeThinV);
    const Eigen::MatrixXd &v = svd.matrixV();

    // use singular vector associated with min singular vector
    double aDotN = 0.;
    for(unsigned i = 0; i < dim; i++) {
      a[i] = v(i, dim - 1);
      aDotN += a[i] * N[i];
    }

    //Rotate normal by pi if Normal dot coefficents is less than zero
    if(aDotN < 0) {
      for(unsigned i = 0; i < dim; i++) {
        a[i] *= -1.;
      }
    }

    //Calculate constant d in ax+by+d=0 or ax+by+cz+d=0
    d = 0.;
    for(unsigned i = 0; i < dim; i++) {
      d -= a[i] * xg[i];
    }

// // use singular vector associated with min singular vector
//     for(unsigned i = 0; i < dim; i++) {
//       a[i] = v(i, dim - 1);
//     }
//
//
//     double aDotN = std::inner_product(a.begin(), a.end(), N.begin(), 0);
//
//     //Rotate normal by pi if Normal dot coefficients is less than zero
//     if(aDotN < 0) {
//       for(unsigned i = 0; i < dim; i++) {
//         a[i] = -a[i];
//       }
//     }
//
//     //Calculate constant d in ax+by+d=0 or ax+by+cz+d=0
//     d = -std::inner_product(a.begin(), a.end(), xg.begin(), 0);
  }

  void FindQuadraticBestFit(const std::vector < std::vector < double > > &xp, boost::optional < const std::vector < double > & > w, const std::vector < double > &N, std::vector < double > &a) {
    const unsigned& dim = N.size();
    unsigned nParam = 4 * dim - 2;

    a.resize(nParam);
    unsigned np = xp.size();
    Eigen::MatrixXd m(np, nParam);

    std::vector < double > xg(dim, 0.);
    double maxD, maxD2;

    if(w) {
      //Calculate centroid
      double wSum = 0;
      for(unsigned i = 0; i < np; i++) {
        wSum += (*w)[i];
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += (*w)[i] * xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= wSum;
      }

      maxD2 = 0.;
      double d2, xk;
      for(unsigned i = 0; i < np; i++) {
        d2 = 0;
        for(unsigned k = 0; k < dim; k++) {
          xk = (xp[i][k] - xg[k]);
          d2 += xk * xk;
        }
        if(d2 > maxD2) maxD2 = d2;
      }
      maxD = sqrt(maxD2);

      //Fill matrix to be passed to JacobiSVD
      for(unsigned i = 0; i < np; i++) {
        double x = (xp[i][0] - xg[0]) / maxD;
        double y = (xp[i][1] - xg[1]) / maxD;
        unsigned cnt = 0;
        for(int o = 2; o >= 0; o--) {
          for(int b = o; b >= 0; b--) {
            m(i, cnt) = sqrt((*w)[i]) * pow(x, b) * pow(y, o - b);
            cnt++;
          }
        }
        if(cnt != nParam) {
          std::cerr << "3D best fit not yet implemented!";
          abort();
        }
      }
    }
    else {
      //Calculate centroid
      for(unsigned i = 0; i < np; i++) {
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= np;
      }

      maxD2 = 0.;
      double d2, xk;
      for(unsigned i = 0; i < np; i++) {
        d2 = 0;
        for(unsigned k = 0; k < dim; k++) {
          xk = (xp[i][k] - xg[k]);
          d2 += xk * xk;
        }
        if(d2 > maxD2) maxD2 = d2;
      }
      maxD = sqrt(maxD2);

      //Fill matrix to be passed to JacobiSVD for Ax2 + Bxy + Cy2+ Dx + Ey + F = 0
      for(unsigned i = 0; i < np; i++) {
        double x = (xp[i][0] - xg[0]) / maxD;
        double y = (xp[i][1] - xg[1]) / maxD;
        unsigned cnt = 0;
        for(int o = 2; o >= 0; o--) {
          for(int b = o; b >= 0; b--) {
            m(i, cnt) = pow(x, b) * pow(y, o - b);
            cnt ++;
          }
        }
        if(cnt != nParam) {
          std::cerr << "3D best fit not yet implemented!";
          abort();
        }
      }
    }

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(m, Eigen::ComputeThinU | Eigen::ComputeThinV);
    const Eigen::MatrixXd &v = svd.matrixV();

    double norm2 = 0;
    for(unsigned i = 0; i < nParam; i++) {
      norm2 += v(i, nParam - 1) * v(i, nParam - 1);
    }
//     while (norm2 == 0) {
//       nParam--;
//       for(unsigned i = 0; i < a.size(); i++) {
//         norm2 += v(i, nParam - 1) * v(i, nParam - 1);
//       }
//     }

    // use singular vector associated with min singular vector

    double norm;
    std::vector<double> N1(dim);


//     if(np == 4) {
//       std::cout << v.cols() <<" ";
//       std::cout << "AAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
//       std::cout << v << std::endl;
//     }

    if(norm2 > 0.1) {
      a[0] = v(0, v.cols() - 1) / maxD2;
      a[1] = v(1, v.cols() - 1) / maxD2;
      a[2] = v(2, v.cols() - 1) / maxD2;
      a[3] = v(3, v.cols() - 1) / maxD;
      a[4] = v(4, v.cols() - 1) / maxD;
      a[5] = v(5, v.cols() - 1) + a[0] * xg[0] * xg[0] +  a[1] * xg[0] * xg[1] + a[2] * xg[1] * xg[1] - a[3] * xg[0] - a[4] * xg[1];

      a[3] -= 2 * a[0] * xg[0] + a[1] * xg[1];
      a[4] -= 2 * a[2] * xg[1] + a[1] * xg[0];

      norm = 0.;
      for(unsigned i = 0; i < a.size(); i++) norm += a[i] * a[i];
      norm = sqrt(norm);
      N1 = {2 * a[0] * xg[0] + a[1] * xg[1] + a[3],  a[1] * xg[0] + 2 * a[2] * xg[1] + a[4]};
    }
    else {

      // std::cout << "AAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;

      nParam = dim + 1;
      a.resize(nParam);
      Eigen::MatrixXd m1(np, nParam);
      for(unsigned j = 0; j < nParam; j++) {
        m1.col(j) = m.col((dim * (dim + 1)) / 2 + j);
      }

      Eigen::JacobiSVD<Eigen::MatrixXd> svd(m1, Eigen::ComputeThinU | Eigen::ComputeThinV);
      const Eigen::MatrixXd &v = svd.matrixV();

      norm2 = 0;
      for(unsigned i = 0; i < nParam; i++) {
        norm2 += v(i, nParam - 1) * v(i, nParam - 1);
      }

      if(norm2 > 0.1) {
        a[nParam - 1] = 0.;
        for(unsigned i = 0; i < dim; i++) {
          a[i] = v(i, nParam - 1);
          a[nParam - 1] -= a[i] * xg[i];
        }
      }
      else {
        a[nParam - 1] = 0.;
        for(unsigned i = 0; i < dim; i++) {
          a[i] = N[i];
          a[nParam - 1] -= a[i] * xg[i];
        }
      }

      norm = 0.;
      for(unsigned i = 0; i < a.size(); i++) norm += a[i] * a[i];
      norm = sqrt(norm);
      N1.insert(N1.begin(), a.begin(), a.begin() + dim);

    }
    double NdotN1 = 0;
    for(unsigned k = 0; k < dim; k++) {
      NdotN1 += N[k] * N1[k];
    }
    if(NdotN1 < 0) {
      for(unsigned i = 0 ; i < a.size(); i++) {
        a[i] = -a[i] / norm;
      }
    }
    else {
      for(unsigned i = 0 ; i < a.size(); i++) {
        a[i] = a[i] / norm;
      }
    }

  }

  void FindCircleBestFit(const std::vector < std::vector < double > > &xp, boost::optional < const std::vector < double > & > w, const std::vector < double > &N, std::vector < double > &a) {
    const unsigned& dim = N.size();
    unsigned nParam = 4;

    a.resize(4 * dim - 2);
    unsigned np = xp.size();
    Eigen::MatrixXd m(np, nParam);

    std::vector < double > xg(dim, 0.);
    double maxD, maxD2;

    if(w) {
      //Calculate centroid
      double wSum = 0;
      for(unsigned i = 0; i < np; i++) {
        wSum += (*w)[i];
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += (*w)[i] * xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= wSum;
      }

      maxD2 = 0.;
      double d2, xk;
      for(unsigned i = 0; i < np; i++) {
        d2 = 0;
        for(unsigned k = 0; k < dim; k++) {
          xk = (xp[i][k] - xg[k]);
          d2 += xk * xk;
        }
        if(d2 > maxD2) maxD2 = d2;
      }
      maxD = sqrt(maxD2);

      //Fill matrix to be passed to JacobiSVD
      for(unsigned i = 0; i < np; i++) {
        double x = (xp[i][0] - xg[0]) / maxD;
        double y = (xp[i][1] - xg[1]) / maxD;
        
        m(i,0) = sqrt((*w)[i]) * (pow(x, 2) + pow(y, 2));
        m(i,1) = sqrt((*w)[i]) * x;
        m(i,2) = sqrt((*w)[i]) * y;
        m(i,3) = sqrt((*w)[i]);
        
      }
    }
    else {
      //Calculate centroid
      for(unsigned i = 0; i < np; i++) {
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= np;
      }

      maxD2 = 0.;
      double d2, xk;
      for(unsigned i = 0; i < np; i++) {
        d2 = 0;
        for(unsigned k = 0; k < dim; k++) {
          xk = (xp[i][k] - xg[k]);
          d2 += xk * xk;
        }
        if(d2 > maxD2) maxD2 = d2;
      }
      maxD = sqrt(maxD2);

      //Fill matrix to be passed to JacobiSVD for Ax2 + Bxy + Cy2+ Dx + Ey + F = 0
      for(unsigned i = 0; i < np; i++) {
        double x = (xp[i][0] - xg[0]) / maxD;
        double y = (xp[i][1] - xg[1]) / maxD;
        m(i,0) = pow(x, 2) + pow(y, 2);
        m(i,1) = x;
        m(i,2) = y;
        m(i,3) = 1.;
      }
    }

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(m, Eigen::ComputeThinU | Eigen::ComputeThinV);
    const Eigen::MatrixXd &v = svd.matrixV();

    double norm2 = 0;
    for(unsigned i = 0; i < nParam; i++) {
      norm2 += v(i, nParam - 1) * v(i, nParam - 1);
    }

    double norm;
    std::vector<double> N1(dim);

    if(norm2 > 0.1) {
      a[0] = v(0, v.cols() - 1) / maxD2;
      a[1] = 0.;
      a[2] = v(0, v.cols() - 1) / maxD2;
      a[3] = v(1, v.cols() - 1) / maxD;
      a[4] = v(2, v.cols() - 1) / maxD;
      a[5] = v(3, v.cols() - 1) + a[0] * xg[0] * xg[0] +  a[1] * xg[0] * xg[1] + a[2] * xg[1] * xg[1] - a[3] * xg[0] - a[4] * xg[1];

      a[3] -= 2 * a[0] * xg[0] + a[1] * xg[1];
      a[4] -= 2 * a[2] * xg[1] + a[1] * xg[0];

      norm = 0.;
      for(unsigned i = 0; i < a.size(); i++) norm += a[i] * a[i];
      norm = sqrt(norm);
      N1 = {2 * a[0] * xg[0] + a[1] * xg[1] + a[3],  a[1] * xg[0] + 2 * a[2] * xg[1] + a[4]};
    }
    else {

      // std::cout << "AAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;

      nParam = dim + 1;
      a.resize(nParam);
      Eigen::MatrixXd m1(np, nParam);
      for(unsigned j = 0; j < nParam; j++) {
        m1.col(j) = m.col((dim * (dim + 1)) / 2 + j);
      }

      Eigen::JacobiSVD<Eigen::MatrixXd> svd(m1, Eigen::ComputeThinU | Eigen::ComputeThinV);
      const Eigen::MatrixXd &v = svd.matrixV();

      norm2 = 0;
      for(unsigned i = 0; i < nParam; i++) {
        norm2 += v(i, nParam - 1) * v(i, nParam - 1);
      }

      if(norm2 > 0.1) {
        a[nParam - 1] = 0.;
        for(unsigned i = 0; i < dim; i++) {
          a[i] = v(i, nParam - 1);
          a[nParam - 1] -= a[i] * xg[i];
        }
      }
      else {
        a[nParam - 1] = 0.;
        for(unsigned i = 0; i < dim; i++) {
          a[i] = N[i];
          a[nParam - 1] -= a[i] * xg[i];
        }
      }

      norm = 0.;
      for(unsigned i = 0; i < a.size(); i++) norm += a[i] * a[i];
      norm = sqrt(norm);
      N1.insert(N1.begin(), a.begin(), a.begin() + dim);

    }
    double NdotN1 = 0;
    for(unsigned k = 0; k < dim; k++) {
      NdotN1 += N[k] * N1[k];
    }
    if(NdotN1 < 0) {
      for(unsigned i = 0 ; i < a.size(); i++) {
        a[i] = -a[i] / norm;
      }
    }
    else {
      for(unsigned i = 0 ; i < a.size(); i++) {
        a[i] = a[i] / norm;
      }
    }

  }


  void FindParabolaBestFit(const std::vector < std::vector < double > > &xp, boost::optional < const std::vector < double > & > w, const std::vector < double > &N, const bool &fx, std::vector < double > &a) {
    const unsigned& dim = N.size();
    unsigned nParam = 4;

    a.resize(4 * dim - 2);
    unsigned np = xp.size();
    Eigen::MatrixXd m(np, nParam);

    std::vector < double > xg(dim, 0.);
    double maxD, maxD2;

    if(w) {
      //Calculate centroid
      double wSum = 0;
      for(unsigned i = 0; i < np; i++) {
        wSum += (*w)[i];
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += (*w)[i] * xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= wSum;
      }

      maxD2 = 0.;
      double d2, xk;
      for(unsigned i = 0; i < np; i++) {
        d2 = 0;
        for(unsigned k = 0; k < dim; k++) {
          xk = (xp[i][k] - xg[k]);
          d2 += xk * xk;
        }
        if(d2 > maxD2) maxD2 = d2;
      }
      maxD = sqrt(maxD2);

      //Fill matrix to be passed to JacobiSVD
      for(unsigned i = 0; i < np; i++) {
        double x = (xp[i][0] - xg[0]) / maxD;
        double y = (xp[i][1] - xg[1]) / maxD;
        
        m(i,0) = sqrt((*w)[i]) * (fx * pow(x, 2) + !fx * pow(y, 2));
        m(i,1) = sqrt((*w)[i]) * x;
        m(i,2) = sqrt((*w)[i]) * y;
        m(i,3) = sqrt((*w)[i]);
        
      }
    }
    else {
      //Calculate centroid
      for(unsigned i = 0; i < np; i++) {
        for(unsigned j = 0; j < dim; j++) {
          xg[j] += xp[i][j];
        }
      }
      for(unsigned j = 0; j < dim; j++) {
        xg[j] /= np;
      }

      maxD2 = 0.;
      double d2, xk;
      for(unsigned i = 0; i < np; i++) {
        d2 = 0;
        for(unsigned k = 0; k < dim; k++) {
          xk = (xp[i][k] - xg[k]);
          d2 += xk * xk;
        }
        if(d2 > maxD2) maxD2 = d2;
      }
      maxD = sqrt(maxD2);

      //Fill matrix to be passed to JacobiSVD for Ax2 + Bxy + Cy2+ Dx + Ey + F = 0
      for(unsigned i = 0; i < np; i++) {
        double x = (xp[i][0] - xg[0]) / maxD;
        double y = (xp[i][1] - xg[1]) / maxD;
        m(i,0) = fx * pow(x, 2) + !fx * pow(y, 2);
        m(i,1) = x;
        m(i,2) = y;
        m(i,3) = 1.;
      }
    }

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(m, Eigen::ComputeThinU | Eigen::ComputeThinV);
    const Eigen::MatrixXd &v = svd.matrixV();

    double norm2 = 0;
    for(unsigned i = 0; i < nParam; i++) {
      norm2 += v(i, nParam - 1) * v(i, nParam - 1);
    }

    double norm;
    std::vector<double> N1(dim);

    if(norm2 > 0.1) {
      a[0] = fx * v(0, v.cols() - 1) / maxD2;
      a[1] = 0.;
      a[2] = !fx * v(0, v.cols() - 1) / maxD2;
      a[3] = v(1, v.cols() - 1) / maxD;
      a[4] = v(2, v.cols() - 1) / maxD;
      a[5] = v(3, v.cols() - 1) + a[0] * xg[0] * xg[0] +  a[1] * xg[0] * xg[1] + a[2] * xg[1] * xg[1] - a[3] * xg[0] - a[4] * xg[1];

      a[3] -= 2 * a[0] * xg[0] + a[1] * xg[1];
      a[4] -= 2 * a[2] * xg[1] + a[1] * xg[0];

      norm = 0.;
      for(unsigned i = 0; i < a.size(); i++) norm += a[i] * a[i];
      norm = sqrt(norm);
      N1 = {2 * a[0] * xg[0] + a[1] * xg[1] + a[3],  a[1] * xg[0] + 2 * a[2] * xg[1] + a[4]};
    }
    else {

      nParam = dim + 1;
      a.resize(nParam);
      Eigen::MatrixXd m1(np, nParam);
      for(unsigned j = 0; j < nParam; j++) {
        m1.col(j) = m.col((dim * (dim + 1)) / 2 + j);
      }

      Eigen::JacobiSVD<Eigen::MatrixXd> svd(m1, Eigen::ComputeThinU | Eigen::ComputeThinV);
      const Eigen::MatrixXd &v = svd.matrixV();

      norm2 = 0;
      for(unsigned i = 0; i < nParam; i++) {
        norm2 += v(i, nParam - 1) * v(i, nParam - 1);
      }

      if(norm2 > 0.1) {
        a[nParam - 1] = 0.;
        for(unsigned i = 0; i < dim; i++) {
          a[i] = v(i, nParam - 1);
          a[nParam - 1] -= a[i] * xg[i];
        }
      }
      else {
        a[nParam - 1] = 0.;
        for(unsigned i = 0; i < dim; i++) {
          a[i] = N[i];
          a[nParam - 1] -= a[i] * xg[i];
        }
      }

      norm = 0.;
      for(unsigned i = 0; i < a.size(); i++) norm += a[i] * a[i];
      norm = sqrt(norm);
      N1.insert(N1.begin(), a.begin(), a.begin() + dim);

    }
    double NdotN1 = 0;
    for(unsigned k = 0; k < dim; k++) {
      NdotN1 += N[k] * N1[k];
    }
    if(NdotN1 < 0) {
      for(unsigned i = 0 ; i < a.size(); i++) {
        a[i] = -a[i] / norm;
      }
    }
    else {
      for(unsigned i = 0 ; i < a.size(); i++) {
        a[i] = a[i] / norm;
      }
    }

  }
  

}
