////////////////////////////////////////////////////////////////////////
//                                                                    //
// generator.h                                                        //
//                                                                    //
// the definition of the generator class, it provides members for     //
// creating and transforming a random number stream.                  //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __GENERATOR_H
#define __GENERATOR_H

class generator
{
public :
  generator(unsigned int s1,unsigned int s2 = 0,unsigned int s3 = 0);
  void antithetic();
  void reset();
  double uniform(double a,double b);
  double exponential(double a);
  double hyperexponential(double p,double a,double b);
  double laplace(double a);
  double normal(double a,double b);
  double chisquare(unsigned int n);
  double student(unsigned int n);
  double lognormal(double a,double b);
  double erlang(unsigned int n,double a);
  double gamma(double a,double b);
  double beta(double a,double b);
  double fdistribution(unsigned int n,unsigned int m);
  double poisson(double a);
  double geometric(double p);
  double hypergeometric(unsigned int m,unsigned int n,double p);
  double weibull(double a,double b);
  double binomial(double p,unsigned int n);
  double negativebinomial(double p,unsigned int n);
  double triangular(double a);
  int probability(double p);

protected :
  double random();             // the random number stream

private :
  int antithc;     // if set, the reverse of the current stream is used
  unsigned int x;        // the current x value
  unsigned int y;        // the current y value
  unsigned int z;        // the current z value
  unsigned int initial_x;   // the initial x value
  unsigned int initial_y;   // the initial y value
  unsigned int initial_z;   // the initial z value
};

#endif
