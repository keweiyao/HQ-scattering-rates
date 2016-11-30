#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

int main(){
	//double M = 1.3;
	//update updater(M);
	//updater.sample_channel(10.0, 0.4);
	

	// Here is a sample code for a medium cell
	double temp=0.3;
	std::vector<double> vcell(3);
	vcell[0] = 0.7; vcell[1] = 0.0; vcell[2] = 0.0;
	
	std::vector<std::vector<double> > ensamble(10000);
	for (auto&& p_lab : ensamble){
		p_lab.resize(4);
		p_lab[0] = 10.0; p_lab[1] = 0.0; p_lab[2] = 0.0; p_lab[3] = std::sqrt(10.0*10.0-M*M);
	}

	for (auto&& p1 : ensamble){
	std::vector<double> p_lab = p1;
	double t = 0.0, dt = 0.04/0.197;
	for (int i=0; i<1500; i++){
	// Step1: boost to cell frame and sample scattering rates
	std::vector<double>  p_cell(4), p_cell_z(4);
	boost_by3(p_lab, p_cell, vcell);
	
	double rQg2Qg = RQg2Qg.interpR(p_cell[0], temp);	
	double rQq2Qq = RQq2Qq.interpR(p_cell[0], temp);
	double rtot = rQg2Qg + rQq2Qq;
	double P = dt*rtot;
	if (P > 0.15) std::cout << " P too large " << P << std::endl;
	double r = std::rand()*1./RAND_MAX;
	if (r > P) {
		print4vec(p_lab);
		continue;
	}
	// sample channel;
	
	// Step2: sample E2, s; given E1, temp within cell rest frame assuming p1 in z-direction
	double s, E2;
	if (r/P > rQg2Qg/rtot) RQg2Qg.sample_initial(p_cell[0], temp, E2, s);
	else  RQq2Qq.sample_initial(p_cell[0], temp, E2, s);

	// Step3: construct (E1, 0, 0, p1) and (E2, p2x, p2y, p2z) assuming incident HQ in z-direction
	
	double E1 = p_cell[0], p1 = std::sqrt(E1*E1-M*M);
	double costheta2 = (M*M + 2.*p_cell[0]*E2 - s)/2./p1/E2;
	double sintheta2 = std::sqrt(1. - costheta2*costheta2);
	double phi = 2.*M_PI*(std::rand()*1./RAND_MAX);
	double cosphi = std::cos(phi), sinphi = std::sin(phi);
	std::vector<double> pQ(4), p2(4);
	pQ[0] = E1; pQ[1] = 0.; pQ[2] = 0.; pQ[3] = p1;
	p2[0] = E2; p2[1] = E2*sintheta2*cosphi; p2[2] = E2*sintheta2*sinphi; p2[3] = E2*costheta2;
	
	// Step4: Boost to center of mass frame of pQ and p2:
	std::vector<double> pQcom(4), vcom(3);
	vcom[0] = (pQ[1] + p2[1])/(pQ[0] + p2[0]);
	vcom[1] = (pQ[2] + p2[2])/(pQ[0] + p2[0]);
	vcom[2] = (pQ[3] + p2[3])/(pQ[0] + p2[0]);
	boost_by3(pQ, pQcom, vcom);
	
	// Step5: Sample final state momentum in CoM, assuming incident HQ in z-direction
	std::vector<std::vector<double> > fs;
	XQg2Qg.sample_dXdPS(s, temp, fs);

	// Step-5: rotate final state momentum back to CoM frame with the original orientation
	std::vector<double> pnew_com(4);
	double alpha1 = std::atan2(pQcom[2], pQcom[1])+M_PI/2.;
	double beta1 = std::atan2(std::sqrt(pQcom[1]*pQcom[1]+pQcom[2]*pQcom[2]), pQcom[3]);
	double gamma1 = 0.0;
	rotate_Euler(fs[0], pnew_com, -gamma1, -beta1, -alpha1);

	// Step-3: boost back to cell rest frame
	std::vector<double> pnew_cell_z(4), ivcom(3);
	ivcom[0] = -vcom[0]; ivcom[1] = -vcom[1]; ivcom[2] = -vcom[2];
	boost_by3(pnew_com, pnew_cell_z, ivcom);

	// Step-2: rotate back to Cell frame with the original orientation;
	std::vector<double> pnew_cell(4);
	double alpha = std::atan2(p_cell[2], p_cell[1])+M_PI/2.;
	double beta = std::atan2(std::sqrt(p_cell[1]*p_cell[1]+p_cell[2]*p_cell[2]), p_cell[3]);
	double gamma = 0.0;
	rotate_Euler(pnew_cell_z, pnew_cell, -gamma, -beta, -alpha);
	
	// Step-1: boost back to lab frame
	std::vector<double> pnew_lab(4), ivcell(3);
	ivcell[0] = -vcell[0]; ivcell[1] = -vcell[1]; ivcell[2] = -vcell[2]; 
	boost_by3(pnew_cell, p_lab, ivcell);

	print4vec(p_lab);}
	}

	return 0;
}
