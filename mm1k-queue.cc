
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>
#include <math.h> 
#include <vector>
#include <utility>      // std::pair, std::make_pair
#include <queue>          // std::priority_queue


using namespace std;

double exp_generator(double lambda) {
    double y;
	double x;
 	y = ((double)rand()/(double)RAND_MAX);
	x = (double)(-1.0 * log(1.0 - y))/ lambda;
	return x;
}

enum event_type {arrival = 0, departure, observation};

struct sort_func {
    bool operator()(const pair<event_type, double> &left, const pair<event_type, double> &right) {
        return left.second > right.second;
    }
};

int main (int argc, char* argv[]) {

	srand(time(0)); 
	static const double T = 5000.0;
    static const double L = 12000.0;
    static const double C = 1000000.0;
    static const double K = atof(argv[1]); // Size of the finite buffer 

    
    priority_queue<pair<event_type, double>, vector<pair<event_type, double> >, sort_func > DES;
    

    double min_ro = atof(argv[2]);
    double max_ro = atof(argv[3]);
    double step_size = atof(argv[4]); 

    for(double ro = min_ro; ro < max_ro; ro += step_size) { 


	    double Na = 0.0; // Number of arrivals
	    double Nd = 0.0; // Number of departures
	    double No = 0.0; // Number of observations
	    double Ni = 0.0; // Idle counter
	    double Np = 0.0; // Number of packets in the system
	    double Ngen = 0.0; // Number of generated packets during simulation
	    double Nloss = 0.0; // Number of lost packets 

	    double ta = 0.0; // Arrival time
	    double td = 0.0; // Departure time
	    double to = 0.0; // Observation time
	    double avg_num_packets;
	    double p_idle;
	    double p_loss;	// Number of packets that were dropeed due to the buffer being full
        double lambda = ro * C / L;
        double alpha = lambda * 2;
        double packet_len;  // packet length 
        double ts; 	// service time 

	   	while (ta < T) { 
	   		ta += exp_generator(lambda);
	   		DES.push(make_pair<event_type, double>(arrival, ta));
	   	}

	   	while (to < T) { 
	   		to += exp_generator(alpha);
	   		DES.push(make_pair<event_type, double>(observation, to));
	   	} 

	   	double event_time;
	   	event_type event;
	  
		while (!DES.empty()) { 
			pair<event_type, double> event_pair = DES.top(); // Get the first element
			event = event_pair.first;
			event_time = event_pair.second;
			double avg_len = 1.0 / L;
			if (event == arrival) { 
				Ngen = Ngen + 1.0;
				if (Na - Nd < K) { // queue is not full
					packet_len = exp_generator(avg_len);
					ts = packet_len / C;
					
					if (Na - Nd == 0) {
						td = event_time + ts;
					} else {
						td = td + ts;
					}
					Na = Na + 1;
					DES.pop();
					DES.push(make_pair<event_type, double>(departure, td)); // insert the departure event 
				} else { 
					Nloss = Nloss + 1; 
					DES.pop(); 
				}
			} else if (event == departure) { 
				Nd = Nd + 1;
				DES.pop();

			} else if (event == observation) {
				No = No + 1;
				Np += (Na - Nd);
				if (Na - Nd == 0) {
					Ni = Ni + 1; //increment the idle counter
				} 
				DES.pop();	
			}
		}

  		
		avg_num_packets = Np / No;
		p_idle = Ni / No;
		p_loss = Nloss / Ngen;
	 
		cout << "Ro: " << ro << "\n";
		cout << "K: " << K << "\n";
		cout << "Avg_num_packets: " << avg_num_packets << "\n";
		cout << "P_loss: " << p_loss << "\n"; 
	 
	}

}

