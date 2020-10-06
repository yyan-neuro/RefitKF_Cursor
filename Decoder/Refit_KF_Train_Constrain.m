function [A,W,C,Q] = Refit_KF_Train_Constrain(Y,X,dt)
  %This function calculates the parameters of a kalman filter subject
  % to physical constraints. This is done by limiting A, the auto-regressive
  % parameter, to a specific form that obeys the kinematics laws.
  
  %The formulation of KF is the same as Refit_KF_Train1

  %calculate C and Q normally
  D = size(X,2);  %The number of samples
  C = Y*(X.')/(X*X.');
  
  Q = (1/D) *(Y-C*X)*((Y-C*X).'); 
  
  
  %use least square estimator to estimate the optimal acceleration first
  X1 = X(3:4,1:end-1);
  X2 = X(3:4,2:end);
  A_accel = X2*(X1.') / (X1*X1.');
  
  % then incorporate it into A
  A = zeros([5,5]);
  A(1,1) = 1; A(1,3) = dt;
  A(2,2) = 1; A(2,4) = dt;
  A(3:4,3:4) = A_accel;
  A(5,5) = 1;
  
  %apply a similar constraint to the auto-regressive error covariance
  %matrix W
  W_accel = (1/(D-1)) * (X2-A_accel*X1) * ((X2 - A_accel*X1).');
  W = zeros([5,5]);
  W(3:4,3:4) = W_accel;
  
end
