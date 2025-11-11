#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <iostream>
#include <fstream>
#include <unsupported/Eigen/SparseExtra>

Eigen::MatrixXd graphToMat(const std::string& filename);

using std::cout;
using std::endl;
using Eigen::MatrixXd;
using Eigen::SparseMatrix;
using Eigen::VectorXd;

int main() {
  int n = 9;

  // ---------
  // TASK 1
  // ---------

  MatrixXd A_g = graphToMat("graph.txt");
  cout << "The Frobenius norm of the matrix A_g is: " << A_g.norm() << endl;


  // ---------
  // TASK 2
  // ---------

  // Contructing v_g vector
  VectorXd v_g(n);
  for (int i = 0; i < n; i++) {
    int sum = 0;
    for (int j = 0; j < n; j++) {
      sum += A_g(i,j);
    }
    v_g[i] = sum;
  }

  // Computing the graph Laplacian L_g and the product L_g * x
  MatrixXd D_g = v_g.asDiagonal();
  MatrixXd L_g = D_g - A_g;
  VectorXd x = VectorXd::Ones(n);
  VectorXd y = L_g * x;
  cout << "The norm of y is: " << y.norm() << endl;

  
  // ---------
  // TASK 3
  // ---------

  // We use the SelfAdjointEigenSolver to compute the eigenvalues as L_g is symmetric
  Eigen::SelfAdjointEigenSolver<MatrixXd> solver(L_g);
  Eigen::VectorXd eivals = solver.eigenvalues();
  cout << "The eigenvalues of the L_g matrix are:" << endl << eivals << endl;

  double  max_eigenvalue = eivals.maxCoeff();
  cout << "The largest eigenvalue is: " << max_eigenvalue << endl;  

  double  min_eigenvalue = eivals.minCoeff();
  cout << "The smallest eigenvalue is: " << min_eigenvalue << endl;


  /*
  * OPTIONAL
  */

  // Version 2 using Householder transformation
  // Once we computed the smallest eigenvalue I want to compute the corresponding eigenvector
  int index_min_eigenvalue;
  for (int i = 0; i < n; i++) {
    if (eivals[i] == min_eigenvalue) {
      index_min_eigenvalue = i;
      break;
    }
  }

  //cout << "eigenvectors " << endl << solver.eigenvectors() << endl;
  //cout << "The index of the smallest eigenvalue is: " << index_min_eigenvalue << endl;



  VectorXd min_eigenvector = solver.eigenvectors().col(index_min_eigenvalue);
  //cout << "The corresponding eigenvector is: " << endl << min_eigenvector << endl;


  
  MatrixXd H(n,n); 
  MatrixXd I(n,n);
  I.setIdentity();

  VectorXd e1 = VectorXd::Zero(n);
  e1(0) = 1.0;


  VectorXd u = min_eigenvector - min_eigenvector.norm() * e1;   
  if (u.norm() < 1e-12) {
    u = VectorXd::Zero(n);
    u(1) = 1.0;  
  }
  u.normalize();

  // We construct the Householder matrix H by using the Householder transformation
  H = I - 2.0*u*u.transpose();

  // We compute the Matrix S that contains the min eigenvalue 
  // in the first position of the diagonal
  MatrixXd S = H * L_g * H.transpose();
  cout << "The matrix S is: " << endl << S << endl; 

  MatrixXd A_deflated = S.block(1, 1, n - 1, n - 1);

  // We use the SelfAdjointEigenSolver to compute the eigenvalues of the deflated matrix
  Eigen::SelfAdjointEigenSolver<MatrixXd> solver_deflated(A_deflated);
  Eigen::VectorXd eivals_deflated = solver_deflated.eigenvalues();
  cout << "Optional way using Housolder transformation" << endl;
  cout << "The eigenvalues of the deflated matrix are:" << endl << eivals_deflated << endl; 
  double min_deflated_eigenvalue = eivals_deflated.minCoeff();
  cout << "The second smallest eigenvalue is: " << min_deflated_eigenvalue << endl;


  // ---------
  // TASK 4
  // ---------

  // We look for the second smallest eigenvalue by iterating over the eigenvalues
  double min = 100;
  int min_index = 0;
  for (int i = 0; i < eivals.size() ; ++i){
    if (eivals[i] < 1e-16) 
        continue;
    if (eivals[i] < min) {
      min = eivals[i];
      min_index = i;
    }
  }

  VectorXd eigvecs = solver.eigenvectors().col(min_index);
  cout << "The second smallest eigenvalue is: " << min << endl;
  cout << "The corresponding eigenvector is: " << endl << eigvecs << endl;
  cout << L_g << endl;

  // Check for a reasonable clustering choice
  for (int i = 0; i < eigvecs.rows(); ++i) {
    if (i < 4) {
      assert(eigvecs[i] > 0);
    } else {
      assert(eigvecs[i] < 0);
    }
  }

  
  // ---------
  // TASK 5
  // ---------
  
  SparseMatrix<double> A_s;
  loadMarket(A_s, "../data/social.mtx");
  cout << "The Frobenius norm of the matrix A_s is: " << A_s.norm() << endl;


  // ---------
  // TASK 6
  // ---------
  
  
  // Filling D_S as a sparse matrix
  int n_s = A_s.rows();
  SparseMatrix<double> D_s(n_s,n_s);
  for (int i = 0; i < n_s; i++) {
    double sum = 0;
    for (int j = 0; j < n_s; j++) {
      sum += A_s.coeff(i, j);
    }
    D_s.insert(i, i) = sum;
  }

  // Constructing the graph Laplacian L_s from A_s
  SparseMatrix<double> L_s = D_s - A_s;
  cout << "Number of non zero elements in L_s: " << L_s.nonZeros() << endl;
  // Checking if L_s is symmetric
  SparseMatrix<double> L_sT = SparseMatrix<double>(L_s.transpose());
  if((L_s - L_sT).norm() < 1e-16)
    cout << "L_s is symmetric" << endl;
  else
    cout << "L_s is not symmetric" << endl;
    

  // ---------
  // TASK 7
  // ---------

  /* We add a perturbation to the first diagonal entry
   * of L_s, export it to a .mtx file and use a LIS
   * solver to compute the largest eigenvalue of L_s */

  L_s.coeffRef(0, 0) += 0.2;
  SparseMatrix L_sT2 = SparseMatrix<double>(L_s.transpose());
  if((L_s - L_sT2).norm() < 1e-16) {
    cout << "perturbed L_s is symmetric" << endl;
  } else {
    cout << "perturbed L_s is not symmetric" << endl; 
  }
  saveMarket(L_s, "L_s.mtx");
  int ret = system("../data/run_test.sh");


  // ---------
  // TASK 8
  // ---------

  int ret2 = system("../data/run_testshift.sh");


  // ---------
  // TASK 9
  // ---------

  int ret3 = system("../data/run_test_inverse.sh");


  // ---------
  // TASK 9
  // ---------

  SparseMatrix<double> eigenVecs;
  loadMarket(eigenVecs, "eigvec_i.mtx");
  VectorXd eigVec = eigenVecs.col(1);

  int n_p(0), n_n(0);
  std::vector<int> clusterA;
  std::vector<int> clusterB;

  for (int i = 0; i < n_s; i++) {
    if (eigVec[i] < 0) {
      clusterB.emplace_back(i);
      n_n++;
    } else {
      clusterA.emplace_back(i);
      n_p++;
    }
  }

  cout << "Number of positive entries: " << n_p << endl;
  cout << "Number of negative entries: " << n_n << endl;

  
  // ---------
  // TASK 10
  // ---------

  // Constructing the permutation matrix P  
  SparseMatrix<double> P(n_s, n_s);
  for (int i = 0; i < n_p; i++) {
    P.coeffRef(i, clusterA[i]) = 1;
  }
  for (int i = 0; i < n_n; i++) {
    P.coeffRef(i+n_p, clusterB[i]) = 1;
  }
  
  //Constructing the reodered adjacency matrix A_ord
  SparseMatrix<double> A_ord = P * A_s * P.transpose();
  SparseMatrix<double> nonDiagAord = A_ord.topRightCorner(n_p, n_n);
  
  cout << nonDiagAord.nonZeros() << endl;

  SparseMatrix<double> nonDiagAs = A_s.topRightCorner(n_p, n_n);
  cout << nonDiagAs.nonZeros() << endl;

  return 0;
}

/*
 * Takes a graph in edge list format and returns the 
 * adjacency matrix associated to the graph as an Eigen
 * matrix */   
Eigen::MatrixXd graphToMat(const std::string& filename) {
  std::ifstream in(filename);
  if (!in) {
    cout << "Couldn't open the file" << endl;
  }
  
  std::string line;

  int n, edges; // Number of vertices
  in >> n >> edges;
  Eigen::MatrixXd A = Eigen::MatrixXd::Zero(n, n);

  int i, j;
  while(in >> i >> j) {
    A(i-1, j-1) = 1;
    A(j-1, i-1) = 1;
  }

  return A;
}