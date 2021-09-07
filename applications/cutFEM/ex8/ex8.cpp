
#include <iostream>
#include <iomanip>

#include <boost/math/special_functions/factorials.hpp>

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

using boost::multiprecision::cpp_dec_float_50;
using boost::multiprecision::cpp_dec_float_100;

namespace boost {
  namespace multiprecision {
    typedef number<cpp_dec_float<200> > cpp_dec_float_200;

    typedef number < backends::cpp_bin_float < 24, backends::digit_base_2, void, boost::int16_t, -126, 127 >, et_off >         cpp_bin_float_single;
    typedef number < backends::cpp_bin_float < 53, backends::digit_base_2, void, boost::int16_t, -1022, 1023 >, et_off >       cpp_bin_float_double;
    typedef number < backends::cpp_bin_float < 64, backends::digit_base_2, void, boost::int16_t, -16382, 16383 >, et_off >     cpp_bin_float_double_extended;
    typedef number < backends::cpp_bin_float < 113, backends::digit_base_2, void, boost::int16_t, -16382, 16383 >, et_off >    cpp_bin_float_quad;
    typedef number < backends::cpp_bin_float < 237, backends::digit_base_2, void, boost::int32_t, -262142, 262143 >, et_off >  cpp_bin_float_oct;

    typedef number<cpp_dec_float<7> > cpp_dec_float_7;
    typedef number<cpp_dec_float<14> > cpp_dec_float_14;
    typedef number<cpp_dec_float<21> > cpp_dec_float_21;
    typedef number<cpp_dec_float<28> > cpp_dec_float_28;
    typedef number<cpp_dec_float<35> > cpp_dec_float_35;
    typedef number<cpp_dec_float<42> > cpp_dec_float_42;
    typedef number<cpp_dec_float<49> > cpp_dec_float_49;
    typedef number<cpp_dec_float<56> > cpp_dec_float_56;
    typedef number<cpp_dec_float<63> > cpp_dec_float_63;
    typedef number<cpp_dec_float<63> > cpp_dec_float_70;

  }
} // namespaces

using boost::math::factorial;

template <class Type>
std::vector<std::vector<Type>> ModifiedGramSchmidt(std::vector<std::vector<Type>> M);

template <class Type>
std::vector<std::vector<Type>> Transpose(std::vector<std::vector<Type>> M);

template <class Type>
std::vector<std::vector<Type>> MatrixMatrixMultiply(std::vector<std::vector<Type>> M, std::vector<std::vector<Type>> N);

template <class Type>
std::vector<Type> MatrixVectorMultiply(std::vector<std::vector<Type>> M, std::vector<Type> Nv);

template <class Type>
Type DotProduct(std::vector<Type> M, std::vector<Type> Nv);

template <class Type>
std::vector<Type> ExtractVector(std::vector<std::vector<Type>> M, unsigned n, bool ROW);

template <class Type>
std::vector<std::vector<Type>> GetMassMatrix(Type h, int element, int degree);


template <class Type>
std::vector<std::vector<Type>> ModifiedGramSchmidt(std::vector<std::vector<Type>> M) {

  int size = M.size();
  std::vector<std::vector<Type>> A(size, std::vector<Type>(size, 0));
  std::vector<std::vector<Type>> Atest(size, std::vector<Type>(size, 0));
  //std::cout <<  size << std::endl;
  //std::cout <<  M[0].size() << std::endl;
  std::vector<Type> test(size, 0);
  Type DP = 0;
  unsigned c = 0;

  for(int i = 0; i < size; i++) {


    A[i][i] = Type(1);

    for(int j = 0; j < size; j++) {

      M[i][j] = Type(1) / (i + j + 1);
//       std::cout << A[i][j] << "  ";
//       if(j ==  M[0].size() - 1) {
//         std::cout << std::endl;
//       }

    }

  }



  Type vN = pow((DotProduct(ExtractVector(A, 0, true), MatrixVectorMultiply(M, ExtractVector(A, 0, true)))), 0.5);
  Type vNd = (DotProduct(ExtractVector(A, 0, true), MatrixVectorMultiply(M, ExtractVector(A, 0, true))));

//
//   for(int i = 0; i < size; i++) {
//     std::cout << test[i] << "  ";
//   }
//   std::cout <<  std::endl;
//
//   test = MatrixVectorMultiply(M, ExtractVector(A, 0, true));
//
//   for(int i = 0; i < size; i++) {
//     std::cout << test[i] << "  ";
//   }
//   std::cout <<  std::endl;
//   std::cout << vN << "  Should work..after mults. " << std::endl;


  for(int i = 0; i < size; i++) {


    if(vN != 0) {
      A[0][i] = A[0][i] / vN;
      //std::cout << A[0][i] << "  ";
    }

    else if(vN == 0) {
      std::cout << "division by zero" << std::endl;
    }

  }


  for(unsigned i = 0; i < size - 1; i++) {

    for(unsigned j = i + 1; j < size; j++) {

      DP = DotProduct(ExtractVector(A, j, true), MatrixVectorMultiply(M, ExtractVector(A, i, true)));
      //test = MatrixVectorMultiply(M, ExtractVector(A, i, true));
      test = ExtractVector(A, j, true);
//       for(unsigned h = 0; h < size; h++) {
//
//         std::cout << test[h] << " should be AT " << std::endl;
//
//       }

      //std::cout << DP << "  this is DP  ";
      for(unsigned k = 0; k < size; k++) {

        A[j][k] = A[j][k] - (DP * A[i][k]);

      }


    }
    std::cout << std::endl;
    c = i + 1;
    vNd = (DotProduct(ExtractVector(A, c, true), MatrixVectorMultiply(M, ExtractVector(A, c, true))));
    vN = (vNd > 0) ? pow((DotProduct(ExtractVector(A, c, true), MatrixVectorMultiply(M, ExtractVector(A, c, true)))), 0.5) : Type(-1);

    if(vN == -1) std::cout <<  " negative number under sqrt " << std::endl;

    std::cout << vNd << " vNd " << std::endl;
    for(int l = 0; l < size; l++) {

      //std::cout << vN << " vN " << std::endl;


      A[c][l] = A[c][l] / vN;
      //std::cout << A[c][l] << "  ";


    }

  }

  for(unsigned i = 0; i < size; i++) {

    for(unsigned j = 0; j < size; j++) {

      std::cout << A[i][j] << "  " ;
      if(j ==  A.size() - 1) {
        std::cout << std::endl;
      }

    }
  }

  std::cout << std::endl;
  std::cout << std::endl;

  Atest = MatrixMatrixMultiply(A, MatrixMatrixMultiply(M, Transpose(A)));

  for(unsigned i = 0; i < size; i++) {

    for(unsigned j = 0; j < size; j++) {

      std::cout << Atest[i][j] << "  " ;
      if(j ==  Atest.size() - 1) {
        std::cout << std::endl;
      }

    }
  }

  return A;

}

template <class Type>
std::vector<std::vector<Type>> MatrixMatrixMultiply(std::vector<std::vector<Type>> M, std::vector<std::vector<Type>> N) {


  int row = M.size();
  //std::cout << row << std::endl;
  int columnM = M[0].size();
  int rowN = N.size();
  //std::cout << rowN << std::endl;
  int column = N[0].size();
  //std::cout << column << std::endl;
  std::vector<std::vector<Type>> bad(row, std::vector<Type>(column, 0));

  if(rowN == columnM) {
    Type temp = 0;
    std::vector<std::vector<Type>> MN(row, std::vector<Type>(column, 0));

    for(unsigned i = 0; i < row; i++) {

      for(unsigned j = 0; j < column; j++) {

        for(unsigned k = 0; k < rowN; k++) {

          temp += M[i][k] * N[k][j];

        }

        MN[i][j] = temp;
        temp = 0;

      }
    }

    return MN;

  }

  else {
    std::cout << " Dimension mismatch!!!!" << std::endl;
    return  bad;
  }

}

template <class Type>
std::vector<Type> MatrixVectorMultiply(std::vector<std::vector<Type>> M, std::vector<Type> Nv) {

  int row = M.size();
  int column = M[0].size();
  int rowN = Nv.size();
  Type temp = Type(0);
  std::vector<Type> bad(row, Type(0));

  if(column == rowN) {

    std::vector<Type> MNv(row, Type(0));

    for(unsigned i = 0; i < row; i++) {

      for(unsigned j = 0; j < column; j++) {

        temp += M[i][j] * Nv[j];

      }
      MNv[i] = temp;
      temp = Type(0);
    }

    return MNv;
  }


  else {
    std::cout << " Dimension mismatch!!!!" << std::endl;
    return  bad;
  }

}


template <class Type>
Type DotProduct(std::vector<Type> M, std::vector<Type> Nv) {

  int row = M.size();
  int column = M.size();
  int rowN = Nv.size();
  Type temp = Type(0);
  Type bad = Type(0);

  if(column == rowN) {

    Type MNv;

    for(unsigned i = 0; i < column; i++) {
      temp += M[i] * Nv[i];
    }

    MNv = temp;
    //std::cout << MNv << "  ";

    return MNv;
  }


  else {
    std::cout << " Dimension mismatch!!!!" << std::endl;
    return  bad;
  }

}


template <class Type>
std::vector<Type> ExtractVector(std::vector<std::vector<Type>> M, unsigned n, bool ROW) {

  int row = M.size();
  //std::cout << row << "this is row " << std::endl;
  int column = M[0].size();
  //std::cout << n <<"this is coumn " << std::endl;
  std::vector<Type> bad(row, 0);

  if(n >= row || n >= column) {
    std::cout << "index out of bounds" << std::endl;
    return bad;
  }


  if(ROW) {
    std::vector<Type> result(column, Type(0));

    for(int i = 0; i < column; i++) {
      result[i] = M[n][i];


    }
    return result;


  }

  else {

    std::vector<Type> result(row, 0);

    for(int i = 0; i < row; i++) {

      result[i] = M[i][n];

    }
    return result;


  }




}




template <class Type>
std::vector<std::vector<Type>> Transpose(std::vector<std::vector<Type>> M) {

  unsigned rowsize = M.size();
  unsigned columnsize = M[0].size();
  std::vector<std::vector<Type>> MT(columnsize, std::vector<Type>(rowsize, Type(0)));

  for(unsigned i = 0; i < columnsize; i++) {

    for(unsigned j = 0; j < rowsize; j++) {

      MT[i][j] = M[j][i];

    }
  }

  return MT;

}


template <class Type>
std::vector<std::vector<Type>> GetMassMatrix(Type h, int element, int degree) {


  //TODO hexahedron = 0, tet = 1, wedge =2, quad = 3, tri = 4, line = 5, point = 6

  unsigned size = 1;
  unsigned count = 0;
  std::vector<std::vector<Type>> MM(size, std::vector<Type>(size, Type(0)));




  if(element == 0 || element == 1) {

    size = ((degree + 1) * (degree + 2) * (degree + 3)) / 6;
    std::vector<std::vector<Type>> MM(size, std::vector<Type>(size, Type(0)));



    return MM;
  }

  if(element == 3 || element == 4) {

    size = ((degree + 1) * (degree + 2)) / 2;
    std::vector<std::vector<Type>> MM(size, std::vector<Type>(size, Type(0)));

    std::vector<unsigned> m(size, 0);
    std::vector<unsigned> n(size, 0);


    for(int i = 0; i <= degree; i++) {

      for(int j = i; j >= 0; j--) {


        m[count] = j;
        n[count] = i - j;
        std::cout << "count = " << count << std::endl;
        count++;
        //std::cout << "i - j = " << i - j << " j = " << j <<  std::endl;

      }

    }

    if(element == 3) {

      for(int k = 0; k < size; k++) {
        count = 0;
        for(int i = 0; i <= degree; i++) {

          for(int j = i; j >= 0; j--) {

            MM[k][count] = Type(1) / ((m[k] + m[count] + 1) * (n[k] + n[count] + 1));
            count++;
            //std::cout << "i - j = " << i - j << " j = " << j <<  std::endl;



          }

        }


      }


//         for(int k = count; k >= 0; k--) {
//             std::cout << "m_" << k << " = " << m[k] << "n_" << k << " = " << n[k] << std::endl;
//         }

      return MM;
    }

    if(element == 5) {

      size = degree + 1;
      std::vector<std::vector<Type>> MM(size, std::vector<Type>(size, Type(0)));

      for(int i = 0; i < size; i++) {

        for(int j = 0; j < size; j++) {

          MM[i][j] = Type(1) / (i + j + 1);


        }

      }

      return MM;
    }

    else;

    std::cout << " Not an appropriate element " << std::endl;
    return MM;



  }

}


int main(int, char**) {


  //typedef double myTypeB;
  typedef boost::multiprecision::cpp_bin_float_oct myTypeB;

  std::vector<std::vector<myTypeB>> AA = {{1, 6, 5, 0, 3}, {1, 0, 1, 0, 4}, {4, 0, 0, 1, 4}, {2, 5, 9, 7, 4}, {1, 2, 3, 4, 5}};
  std::vector<myTypeB> Av = {5, 6, 2, 1, 4};
  std::vector<myTypeB> Avv = {5, 6, 2, 1, 4, 4};
  std::vector<myTypeB> Avr(3, 0);
  std::vector<std::vector<myTypeB>> A = {{9, 0, 3}, {1, 4, 0}, {0, 3, 1}, {0, 3, 1}};
  std::vector<std::vector<myTypeB>> AAP;
  AAP.resize(AA.size());
  AAP[0].resize(A[0].size());
  int d = 4;
  int c = ((d + 1) * (d + 2)) / 2;
  myTypeB f = 0;
  std::vector<std::vector<myTypeB>> AT(c, std::vector<myTypeB>(c, myTypeB(0)));
  //DotProduct(Av, Avv);
  //std::vector<myTypeB> R = ExtractVector(AA, 4, 1);
  //AAP = MatrixMatrixMultiply(AA, A);
  //Avr = MatrixVectorMultiply(AA, Av);
  //AA = ModifiedGramSchmidt(AT);
  //AT = Transpose(A);
  AT = GetMassMatrix(f, 3, 2);


//   for(unsigned i = 0; i < R.size(); i++) {
//
//     std::cout << R[i] << "  ";
//
//   }

  std::string space = "  ";
  for(unsigned i = 0; i < AT.size(); i++) {

    for(unsigned j = 0; j < AT[0].size(); j++) {

      std::cout << AT[i][j] << "  ";
      if(j ==  AT[0].size() - 1) {
        std::cout << std::endl;
      }

    }
  }
//
//   for(unsigned i = 0; i < AA.size(); i++) {
//
//     std::cout << Avr[i] << std::endl;
//
//   }


  return 1;


}























